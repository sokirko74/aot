// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)


#include "StdRmlThes.h"
#include "Thesaurus.h"

int CThesaurus::GetConceptNoByConceptId(long ConceptId) const
{
	std::vector<CConcept>::const_iterator It = lower_bound(m_Concepts.begin(), m_Concepts.end(), CConcept(ConceptId));
	if ((It != m_Concepts.end())
		&& (It->m_ConceptId == ConceptId)
		)
		return It - m_Concepts.begin();
	return -1;

};
int CThesaurus::GetConceptNoByConceptStr(std::string ConceptStr) const
{
	for (int i = 0; i < m_Concepts.size(); i++)
		if (m_Concepts[i].m_ConceptStr == ConceptStr)
			return i;

	return -1;
};

bool CThesaurus::LoadConcepts(std::string FileName)
{
	m_Concepts.clear();
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
		CConcept C;
		C.m_ConceptId = atoi(items[0].c_str());
		C.m_ConceptStr = trim_quotes(items[1]);
		m_Concepts.push_back(C);
	};
	return true;
};
