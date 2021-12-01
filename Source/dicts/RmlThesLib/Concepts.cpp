// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)
#include "StdRmlThes.h"
#include "Thesaurus.h"


thesaurus_concept_t CThesaurus::GetConceptIdByConceptStr(const std::string& conceptStr) const
{
	auto it = m_ConceptName2Id.find(conceptStr);
	if (it == m_ConceptName2Id.end()) {
		return -1;
	}
	return it->second;
};

std::string CThesaurus::GetConceptStrById(const thesaurus_concept_t& id) const
{
	auto it = m_ConceptId2Name.find(id);
	if (it == m_ConceptId2Name.end()) {
		return "";
	}
	return it->second;
};

bool CThesaurus::LoadConcepts(std::string FileName)
{
	m_ConceptId2Name.clear();
	m_ConceptName2Id.clear();
	std::ifstream inp(FileName);
	if (!inp.is_open()) {
		return false;
	}
	std::string line;
	if (!std::getline(inp, line)) {
		return false;
	}
	if (line != "ConceptId;ConceptStr;ShortComments;")
	{
		throw CExpc("bad header in %s", FileName.c_str());
	};

	while (std::getline(inp, line)) {
		line = convert_from_utf8(line.c_str(), m_MainLanguage);
		StringTokenizer tok(line.c_str(), FieldDelimiter);
		std::vector<std::string> items;
		while (tok()) {
			items.push_back(tok.val());
		}
		if (items.size() < 2) {
			throw CExpc("bad columns number"); 
		}
		auto conceptId = atoi(items[0].c_str());
		auto conceptStr = trim_quotes(items[1]);
		m_ConceptId2Name[conceptId] = conceptStr;
		m_ConceptName2Id[conceptStr] = conceptId;
	};
	return true;
};
