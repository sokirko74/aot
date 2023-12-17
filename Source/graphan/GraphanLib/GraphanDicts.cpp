// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include "GraphanDicts.h"


CGraphanDicts::CGraphanDicts(MorphLanguageEnum Language)
{
	m_Language = Language;
}

CGraphanDicts::~CGraphanDicts()
{
	m_pOborDictionary.FreePointer();
};

void CGraphanDicts::FreeData()
{
	m_pOborDictionary.FreePointer();
	m_EnglishNames.clear();
	m_Oborottos.clear();
	m_Abbrevs.clear();
};

void CGraphanDicts::ReadENames(std::string path)
{
	assert(m_Language != morphUnknown);
	std::ifstream inp(path);
	if (!inp.good()) {
		throw CExpc("cannot open file %s", path.c_str());
	}
	std::string s;
	while (std::getline(inp, s))
	{
		Trim(s);
		if (s.empty()) continue;
		m_EnglishNames.insert(convert_from_utf8(s.c_str(), m_Language));
	};
	inp.close();
};

void CGraphanDicts::ReadSpacedWords(std::string path)
{
	std::ifstream inp(path);
	if (!inp.good()) {
		throw CExpc("cannot open %s", path.c_str());
	}
	m_SpacedWords.clear();
	std::string line;
	while (std::getline(inp, line))
	{
		Trim(line);
		m_SpacedWords.push_back(convert_from_utf8(line.c_str(), m_Language));
	};
}



bool CGraphanDicts::IsFileExtension(const std::string& s) const
{
	return m_Extensions.find(s) != m_Extensions.end();
};


bool CGraphanDicts::IsRegisteredKeyModifier(const char* Str, size_t Len) const
{
	for (long i = 0; i < m_KeyModifiers.size(); i++)
		if (    (m_KeyModifiers[i].length() == Len)
			&& !strncmp(m_KeyModifiers[i].c_str(), Str, Len)
			)
			return true;

	return  false;
};


static bool StrSpacingCompare(MorphLanguageEnum langua, const std::string& word, const char* text, BYTE& spaced_length)
{
	int spaces_count = 1;

	// после первой буквы должна идти  разрядка
	if (!is_spc_fill((BYTE)text[1])) return false;

	//  разрядка может быть в два пробела 
	if (is_spc_fill((BYTE)text[2])) {
		spaces_count = 2;
	}

	// пошла проверка слов
	for (int i = 0; i < word.length(); i++)
	{
		if (i > 0)
			for (size_t k = 0; k < spaces_count; ++k) {
				if (!is_spc_fill((BYTE)text[i   + spaces_count * (i - 1) + k ])) 
					return false;
			}
		if (!IsSuperEqualChar((BYTE)word[i], (BYTE)text[i + spaces_count * i], langua))
			return false;
	}

	spaced_length = word.length()  + spaces_count*(word.length() - 1);

	return true;

};


const std::string empty_string;
const std::string& CGraphanDicts::SearchSpacedWords(const char* In, BYTE& len) const
{
	for (auto& s: m_SpacedWords)
		if (StrSpacingCompare(m_Language, s, In, len))
		{
			return s;
		};
	len = 0;
	return empty_string;
}

bool CGraphanDicts::ReadKeyboard(std::string FileName)
{
	m_Keys.clear();
	m_KeyModifiers.clear();
	FILE* fp = fopen(FileName.c_str(), "r");

	if (fp == 0) return true;
	char s[100];

	fgets(s, 100, fp);
	rtrim(s);
	const char* q = s + strspn(s, " \t");
	if (strcmp(q, "[modifiers]")) return false;

	bool ModifiersSect = true;
	while (fgets(s, 100, fp))
	{
		rtrim(s);
		if (s[0] == 0) continue;


		char* q = s + strspn(s, " \t");
		if (!strcmp(q, "[keys]"))
		{
			ModifiersSect = false;
			continue;
		};

		if (ModifiersSect)
		{
			m_KeyModifiers.push_back(q);
		}
		else
		{
			m_Keys.push_back(q);
		};
	};
	fclose(fp);
	return true;
};

void CGraphanDicts::ReadExtensions(std::string path)
{
	m_Extensions.clear();
	std::ifstream inp(path);
	if (!inp.good()) {
		throw CExpc("cannot open file %s", path.c_str());
	}
	std::string s;
	while (std::getline(inp, s))
	{
		Trim(s);
		MakeUpperUtf8(s);
		if (s.empty()) {
			continue;
		}
		if (!CheckEnglishUtf8(s)) {
			throw CExpc("only English file extensions are enabled");
		}
		m_Extensions.insert(s);
	};
	inp.close();

};


