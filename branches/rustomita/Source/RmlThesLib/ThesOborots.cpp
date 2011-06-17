// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)

#include "StdRmlThes.h"
#include "Thesaurus.h"


bool CThesaurus::LoadOborots (string FileName)
{
	m_Oborots.clear();
	FILE* fp = fopen (FileName.c_str(), "r");
	if (!fp) return false;
	char buff[2000];
	if ( !fgets (buff, 2000, fp) )
		return false;
	string Header = buff;
	Trim(Header);
	if (Header != "OborotId;OborotStr;OborotNo;PartOfSpeech;SubGrammems;")
		return false;

	if (!m_pOborDic) return false;

    while   (fgets(buff, 2000, fp))
	{
		COborot O;
		int i = 0;
		string OborotStr;
   		for (const char* s = strtok(buff,FieldDelimiter); s; s = strtok(0, FieldDelimiter))
		{
			string Field =  s;
			if (Field[0] == '"')
			{
			   if (Field[Field.length() - 1] != '"')
				   return false;
			   Field.erase(0,1);
			   Field.erase(Field.length() - 1,1);

			};


			if (i==0)
				O.m_OborotId = atoi(Field.c_str());
			else
			if (i==1)
			{
				OborotStr = Field;
			}
            i++;
		};

		if (OborotStr.empty()) return false;
        O.m_UnitNo = m_pOborDic->LocateUnit(OborotStr.c_str(), 1);
		if ( O.m_UnitNo == ErrUnitNo)
		{
			//ErrorMessage ("Cannot find oborot; This oborot was probably deleted! "+OborotStr);
		};

		m_Oborots.push_back(O);
	};

	sort(m_Oborots.begin(), m_Oborots.end());
	fclose(fp);

 return true;
};

