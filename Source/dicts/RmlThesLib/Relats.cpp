// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)

#include "StdRmlThes.h"
#include "Thesaurus.h"


bool CThesaurus::LoadRelats(std::string FileName)
{
	m_Relats.clear();
	FILE* fp = fopen (FileName.c_str(), "r");
	if (!fp) return false;
	char buff[2000];
	if ( !fgets (buff, 2000, fp) )
	{
		fclose(fp);
		return false;
	};
	std::string Header = buff;
	Trim(Header);
	if (Header != "Concept1Id;Concept2Id;RelationId;Concept2CutStr;")
	{
		fclose(fp);
		return false;
	};

	while   (fgets(buff, 2000, fp))
	{
		int i = 0;
		CRelat R;
		for (const char* s = strtok(buff,FieldDelimiter); s; s = strtok(0, FieldDelimiter))
		{
			if (i==0)
				R.m_Concept1Id = atoi(s);
			else
		    if (i==1)
				R.m_Concept2Id = atoi(s);
			else
		    if (i==2)
				R.m_RelationType =  atoi(s);
            i++;
		}

		if (   (R.m_Concept1Id == UnknownConceptId)
			||  (R.m_Concept2Id == UnknownConceptId)
		   )
		{
			ErrorMessage("Thesaurus ("+m_Name+") integrity violations (cannot load thesaurus relations)");
			fclose(fp);
			return  false;
		};
		m_Relats.push_back(R);
	};

	fclose(fp);
	return true;
};
