// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)

#include "StdRmlThes.h"
#include "Thesaurus.h"


bool CThesaurus::LoadOborots(std::string FileName)
{
	m_Oborots.clear();
	std::ifstream inp(FileName);
	if (!inp.is_open()) {
		return false;
	}
	std::string line;
	if (!std::getline(inp, line)) {
		return false;
	}
	if (line != "OborotId;OborotStr;OborotNo;PartOfSpeech;SubGrammems;") {
		throw CExpc("bad header in %s", FileName.c_str());
	}

	if (!m_pOborDictionary) return false;

	while (std::getline(inp, line))
	{
		line = convert_from_utf8(line.c_str(), m_MainLanguage);
		StringTokenizer tok(line.c_str(), FieldDelimiter);
		std::vector<std::string> items;
		while (tok()) {
			items.push_back(tok.val());
		}
		if (items.size() < 2) {
			throw CExpc("bad number columns");
		}
		COborot O;
		O.m_OborotId = atoi(items[0].c_str());
		auto oborotStr = trim_quotes(items[1]);
		if (oborotStr.empty()) return false;
		O.m_UnitNo = m_pOborDictionary->LocateUnit(oborotStr.c_str(), 1);
		if (O.m_UnitNo == ErrUnitNo)
		{
			//ErrorMessage ("Cannot find oborot; This oborot was probably deleted! "+OborotStr);
		};
		m_Oborots.push_back(O);
	};

	sort(m_Oborots.begin(), m_Oborots.end());
	return true;
};
