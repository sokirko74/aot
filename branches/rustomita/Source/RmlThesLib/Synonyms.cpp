// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)


#include "StdRmlThes.h"
#include "Thesaurus.h"

inline bool IsLessByTextEntry (const CSynonym& _X1, const CSynonym& _X2) 
{
		return _X1.m_TextEntryId < _X2.m_TextEntryId;
};

inline bool IsLessByConcept (const CSynonym& _X1, const CSynonym& _X2) 
{
		return _X1.m_ConceptId < _X2.m_ConceptId;
};



void  CThesaurus::GetConceptsByTextEntryId(long TextEntryId, vector<long>& Concepts) const
{
 Concepts.clear();
 CSynonym S;
 S.m_TextEntryId = TextEntryId;
 for (
	 vector<CSynonym>::const_iterator It = lower_bound(m_SynonymsByTextEntry.begin(), m_SynonymsByTextEntry.end(),S, IsLessByTextEntry);
	  !(It == m_SynonymsByTextEntry.end()) && (It->m_TextEntryId==TextEntryId);
	  It++
	 )
 {
	 Concepts.push_back(It->m_ConceptId);
 }
};

void  CThesaurus::GetTextEntriesByConcept(long ConceptId, vector<long>& TextEntries) const
{
 TextEntries.clear();
 CSynonym S;
 S.m_ConceptId = ConceptId;
 for (
	 vector<CSynonym>::const_iterator It = lower_bound(m_SynonymsByConcept.begin(), m_SynonymsByConcept.end(),S, IsLessByConcept);
	  !(It == m_SynonymsByConcept.end()) && (It->m_ConceptId==ConceptId);
	  It++
	 )
 {
	 TextEntries.push_back(It->m_TextEntryId);
 }
};



bool CThesaurus::LoadSynonyms(string FileName)
{
	m_SynonymsByConcept.clear();
	m_SynonymsByTextEntry.clear();
	FILE* fp = fopen (FileName.c_str(), "r");
	if (!fp) return false;
	char buff[2000];
	if ( !fgets (buff, 2000, fp) )
	{
		fclose(fp);
		return false;
	};
	string Header = buff;
	Trim(Header);
	if (Header != "ConceptId;TextEntryId;TextEntryCutStr;")
	{
		fclose(fp);
		return false;
	};

	while   (fgets(buff, 2000, fp))
	{
		int i = 0;
		CSynonym S;
		for (const char* s = strtok(buff,FieldDelimiter); s; s = strtok(0, FieldDelimiter))
		{
			if (i==0)
				S.m_ConceptId = atoi(s);
			else
		    if (i==1)
				S.m_TextEntryId =  atoi(s);
            i++;
		}

		m_SynonymsByConcept.push_back(S);
	};
    fclose(fp);

	m_SynonymsByTextEntry = m_SynonymsByConcept;
	sort(m_SynonymsByTextEntry.begin(), m_SynonymsByTextEntry.end(), IsLessByTextEntry );

	return true;
};
