// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)

#include "StdRmlThes.h"
#include "Thesaurus.h"

int CThesaurus::GetModelNoByModelId (long ModelId) const
{
	vector<CInnerModel>::const_iterator It = lower_bound(m_Models.begin(), m_Models.end(),CInnerModel(ModelId));
	if  (  (It != m_Models.end())
		 &&(It->m_ModelId==ModelId)
		)
		return It - m_Models.begin();
	return -1;

};


struct IsLessByTerminId {
	bool  operator() (const CInnerTermin& _T, const int& TerminId) const
	{
		 return _T.m_TerminId < TerminId;
	};
	bool  operator() (const int& TerminId, const CInnerTermin& _T ) const
	{
		 return TerminId < _T.m_TerminId;
	};
	bool  operator() (const CInnerTermin& _T1, const CInnerTermin& _T2) const
	{
		 return _T1.m_TerminId < _T2.m_TerminId ;
	};

};

int  CThesaurus::GetTerminNoByTextEntryId(long TextEntryId) const
{
	vector<CInnerTermin>::const_iterator It = lower_bound (m_Termins.begin(), m_Termins.end(), TextEntryId, IsLessByTerminId());
	if (   ( It !=   m_Termins.end())
		&& (TextEntryId == It->m_TerminId) 
		)
		return It - m_Termins.begin();
	return -1;
};



bool CThesaurus::LoadTermins(string FileName)
{
	m_Termins.clear();
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
	if (Header != "TextEntryId;TextEntryStr;FreqCollocTypeId;AbbrForm;SynonymsCount;")
	{
		fclose(fp);
		return false;
	};

	while   (fgets(buff, 2000, fp))
	{
		int i = 0;
		CInnerTermin T;
		for (char* s = strtok(buff,FieldDelimiter); s; s = strtok(0, FieldDelimiter))
		{
			int len = strlen(s);			
			if (s[0] == '"')
			{
				if (s[len - 1] != '"')
				{
					return false;
					fclose(fp);
				}
			   s[len-1] = ' ';
			   s++;
			   
			};
			if (i==0)
				T.m_TerminId = atoi(s);
			else
		    if (i==1)
			{
				T.m_TerminStr =  s;
				Trim(T.m_TerminStr);
			}
			else
			   if (i==2)
				 T.m_ModelNo =  GetModelNoByModelId(atoi(s));
   			else
			   if (i==3)
				{
				    T.m_AbbrForm[0] = 0;
				    strcpy (T.m_AbbrForm, s);
					rtrim(T.m_AbbrForm);
				}
            i++;
		}
		m_Termins.push_back(T);
		

	};

	for (long i=0; i < m_Termins.size(); i++)
		if (m_Termins[i].m_AbbrForm[0] != 0)
			m_AbbrForms2TerminNo[string(m_Termins[i].m_AbbrForm)] = i;

    fclose(fp);

	return true;
};

