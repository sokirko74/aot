// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)


#include "StdRmlThes.h"
#include "Thesaurus.h"

int CThesaurus::GetConceptNoByConceptId (long ConceptId) const
{
	vector<CConcept>::const_iterator It = lower_bound(m_Concepts.begin(), m_Concepts.end(),CConcept(ConceptId));
	if  (  (It != m_Concepts.end())
		 &&(It->m_ConceptId==ConceptId)
		)
		return It - m_Concepts.begin();
	return -1;

};
int CThesaurus::GetConceptNoByConceptStr (string ConceptStr) const
{
 for (int i=0; i < m_Concepts.size(); i++)
  if (m_Concepts[i].m_ConceptStr == ConceptStr)
   return i;

  return -1;
};

bool CThesaurus::LoadConcepts(string FileName)
{
	m_Concepts.clear();
	FILE* fp = fopen (FileName.c_str(), "r");
	if (!fp) return false;
	char buff[2000];
	if ( !fgets (buff, 2000, fp) )
	{
		fclose(fp);
		return false;
	}	
	string Header = buff;
	Trim(Header);
	if (Header != "ConceptId;ConceptStr;ShortComments;")
	{
		fclose(fp);
		return false;
	};

	while   (fgets(buff, 2000, fp))
	{
		int i = 0;
		CConcept C;
		for (char* s = strtok(buff,FieldDelimiter); s; s = strtok(0, FieldDelimiter))
		{
			int len = strlen(s);			
			if (s[0] == '"')
			{
				if (s[len - 1] != '"')
				{
					fclose(fp);
					return false;
				}
			   s[len-1] = ' ';
			   s++;
			   
			};
			if (i==0)
				C.m_ConceptId = atoi(s);
			else
		    if (i==1)
			{
				C.m_ConceptStr =  s;
				Trim (C.m_ConceptStr);
			};
			
            i++;
		}

		m_Concepts.push_back(C);
	};
	fclose(fp);
	return true;
};
