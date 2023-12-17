// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include "GraphmatFile.h"
#include "abbrev.h"
#include "GraphanDicts.h"
#include "morph_dict/common/util_classes.h"

struct CStrToCompare {
	MorphLanguageEnum	m_Language;
	const char* m_Str;
	size_t				m_StrLen;

	CStrToCompare(MorphLanguageEnum	 Language, const char* Str, size_t StrLen)
	{
		m_Str = Str;
		m_StrLen = StrLen;
		m_Language = Language;
	};

	CStrToCompare(const CGraphmatFile* pParent, size_t LineNo, bool bCaseSensitive)
	{
		if (bCaseSensitive)
			m_Str = pParent->GetUnits()[LineNo].GetToken().c_str();
		else
			m_Str = pParent->GetUpperString(LineNo).c_str();


		m_StrLen = pParent->GetUnits()[LineNo].GetTokenLength();
		m_Language = pParent->m_Language;
	};

};

bool CAbbrevItem::operator == (const CAbbrevItem X) const
{
	return		(m_ItemStr == X.m_ItemStr);
};


bool CAbbrevItem::operator < (const CAbbrevItem X) const
{
	return m_ItemStr < X.m_ItemStr;
};

inline bool AbbrevIsEqualToString(const CAbbrevItem& X, const CGraLine& token)
{
	switch (X.m_Type)
	{
	case abString: return X.m_ItemStr == token.GetTokenUpper();
	case abNumber: return token.HasDes(ODigits);
	case abUpperCase: return token.HasDes(OUp) || token.HasDes(OUpLw);
	case abAny: return !token.IsSoft();
	};
	assert(false);
	return true;
};

bool AbbrevIsGreaterThanString(const CAbbrevItem& X, const CStrToCompare& Str)
{
	size_t min_len = (X.m_ItemStr.length() < Str.m_StrLen) ? X.m_ItemStr.length() : Str.m_StrLen;

	int t = strncmp(X.m_ItemStr.c_str(), Str.m_Str, min_len);

	return		(t > 0)
		|| ((t == 0)
			&& (X.m_ItemStr.length() > Str.m_StrLen)
			);
};


std::string NumberPlace = "/:D";

static void ReadAbbrevationsFromOneFile(std::string path, std::unordered_multimap<std::string, CAbbrev>& V, MorphLanguageEnum langua)
{
	std::ifstream inp(path);
	if (!inp.good()) {
		throw CExpc("cannot open file %s", path.c_str());
	}
	std::string line;
	while (std::getline(inp, line))
	{
		Trim(line);
		{
			int index = line.find("//");
			if (index != std::string::npos)
				line.erase(index);
		};
		if (line.empty()) continue;
		auto s = convert_from_utf8(MakeUpperUtf8(line).c_str(), langua);

		CAbbrev Abbrev;
		for (auto& item_str : split_string(s, ' '))
		{
			CAbbrevItem item;
			item.m_ItemStr = item_str;
			if (item.m_ItemStr == NumberPlace)
				item.m_Type = abNumber;
			else
				if (item.m_ItemStr == "/:U")
					item.m_Type = abUpperCase;
				else
					if (item.m_ItemStr == "/:A")
						item.m_Type = abAny;
					else
						item.m_Type = abString;
			Abbrev.m_AbbrevItems.push_back(item);
		};
		if (Abbrev.m_AbbrevItems.empty()) {
			throw CExpc("empty abbreviation in file %s", path.c_str());
		}
		auto& item0 = Abbrev.m_AbbrevItems[0];
		if (item0.m_Type != abString && item0.m_Type != abNumber) {
			throw CExpc("first abbreviation item must be a string or digits, file %s, abbrev=%s", 
				path.c_str(), line.c_str());
		}
		V.insert({Abbrev.m_AbbrevItems[0].m_ItemStr, Abbrev});
	};
	inp.close();
};


void CGraphanDicts::ReadAbbrevations()
{
	std::string FileName = GetRegistryString("Software\\Dialing\\Graphan\\AbbrFile");

	m_Abbrevs.clear();

	// English abbreviation are common part
	ReadAbbrevationsFromOneFile(MakeFName(FileName, "eng"), m_Abbrevs, morphEnglish);

	if (m_Language == morphGerman)
		ReadAbbrevationsFromOneFile(MakeFName(FileName, "ger"), m_Abbrevs, morphGerman);
	else if (m_Language == morphRussian)
		ReadAbbrevationsFromOneFile(MakeFName(FileName, "rus"), m_Abbrevs, morphRussian);
};



bool CGraphmatFile::DealAbbrev(size_t  StartPos, size_t EndPos)
{
	//  no intersection  of abbreviations is possible, since 
	//  there is no way to distinct abbrevations aftrewards using fork graphematical 
	//  descriptors, if they have an intersection

	if (GetUnits()[StartPos].IsAbbreviation()) return false;
	if (GetUnits()[StartPos].IsSoft()) return false;

	const auto* first_token = &GetUnits()[StartPos].GetTokenUpper();
	if (GetUnits()[StartPos].HasDes(ODigits)) {
		first_token = &NumberPlace;
	}
	auto [start, end] = m_pDicts->m_Abbrevs.equal_range(*first_token);
	int max_token_index = -1;
	for (auto abbrev_it = start; abbrev_it != end; ++abbrev_it)
	{
		const CAbbrev& a = abbrev_it->second;
		size_t token_index = StartPos;
		bool found = true;
		for (auto& i: a.m_AbbrevItems)
		{
			token_index = PSoft(token_index, EndPos);
			if (token_index == EndPos || !AbbrevIsEqualToString(i, GetUnits()[token_index])) {
				found = false;
				break;
			}
			token_index++;
		};
		if (found && (int)token_index > max_token_index)
		{
			// found an abbreviation
			max_token_index = token_index;
		}
	};
	if (max_token_index != -1) {
		SetDes(StartPos, OAbbr1);
		SetDes(max_token_index - 1, OAbbr2);
		SetState(StartPos, max_token_index, stAbbreviation);
		return true;
	}
	return false;
};




