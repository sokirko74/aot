// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Posmorphological Module (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Alexey Sokirko (1999-2002)

#include "MAPostMain.h"


CPostMorphInteface* NewRussianPostMorph(const CLemmatizer* RusLemmatizer, const CAgramtab* RusGramTab)
{
	CMAPost* M = new CMAPost;
	if (!M) return 0;
	if (!M->Init(RusLemmatizer,RusGramTab))
	{

		delete M;
		return 0;
	};
	return M;
}

CMAPost::CMAPost()
{
    m_bUseTrigrams = false;
	m_LogFileName = "";
	/*try {
		
		string log_path  = GetRegistryString( "Software\\Dialing\\Logs\\Main" );
		m_LogFileName = log_path + "mapost.log";
	}
	catch (...) {
	};*/
	m_bCanChangeInputText = true;
    m_bHumanFriendlyOutput = false;

}

CMAPost::~CMAPost()
{
	m_Words.clear();
	m_pRusLemmatizer = 0;
}





CFixedColloc::CFixedColloc(string LemmaStr,	long MainWordNo,	string InterfaceString, const CAgramtab* m_piRusGramTab)
{
		m_LemmaStr = LemmaStr;
		m_MainWordNo = MainWordNo;
		m_InterfaceString = InterfaceString;
		StringTokenizer tok(LemmaStr.c_str(), " ");
		while (tok()) 
		{
		    CLemmaAndPOS L;
			string Item = tok.val();
			int i = Item.find("[");
			if (i != string::npos )
			{
				int k = Item.find("]");
				string POS = Item.substr(i+1, k - i - 1);
				QWORD dummy;
				m_piRusGramTab->ProcessPOSAndGrammems(POS.c_str(),L.m_POS, dummy);
				L.m_Lemma = Item.substr(0, i);
			}
			else
			{
				L.m_Lemma = Item;
				L.m_POS = 255;
			};
			m_Lemmas.push_back(L);
		};
		assert (m_MainWordNo<m_Lemmas.size());
		assert (m_MainWordNo>=0);
};

bool CMAPost::ReadCollocs()
{
	try
		{
		
			string FileName =  GetRegistryString("Software\\Dialing\\Mapost\\Collocs");
			FILE* fp = fopen(FileName.c_str(), "rb");
			if (!fp)
			{
				ErrorMessage ("Cannot Load Collocations in Mapost");
				return false;
			};
			char s[1000];
			while(fgets(s, 1000, fp))
			{
				string		   LemmaStr;;
				int			   MainWordNo;
				string		   InterfaceString;
				StringTokenizer tok(s,"#");
				int i =0;
				while (tok())
				{
					string q = tok.val();
					Trim(q);
					if ( i ==0)
						LemmaStr = q;
					else
						if (i ==1)
							MainWordNo = atoi(q.c_str());
						else
							if (i==2)
								InterfaceString = q;
					i++;
				};

			   m_FixedCollocs.push_back(CFixedColloc(LemmaStr, MainWordNo, InterfaceString, m_pRusGramTab));
			};
			fclose(fp);
		}
		catch(...)
		{
			return false;
		}
   return true;
};



bool	CMAPost::Init(const CLemmatizer* RusLemmatizer, const CAgramtab* RusGramTab)
{
	try {
		m_pRusGramTab = RusGramTab;
		m_pRusLemmatizer = RusLemmatizer;
		if (!ReadCollocs())
		{
			return false;			
		};

	
		//получаем из морфологии код Ќ≈”Ѕ»¬ј…ћ≈Ќя (неизмен€еммое существительное всех родов)
		BYTE POS;
		QWORD Grammems;
		if (    !m_pRusGramTab->ProcessPOSAndGrammemsIfCan(" — мр,жр,ср,мн,ед,им,рд,дт,вн,тв,пр",&POS, &Grammems) 
			 || !m_pRusGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan (POS, Grammems, m_DURNOVOGramCode)
		   )
		{
			ErrorMessage ("MAPOST", "Cannot parse string for Ќ≈”Ѕ»¬ј…ћ≈Ќя");
			return false;
		};
	    
        string TrigramConfig = GetRegistryString("TrigramConfig");
        if (!TrigramConfig.empty())
        {
            m_bUseTrigrams = true;    
            m_TrigramModel.InitModelFromConfigAndBuildTagset(BuildRMLPath(TrigramConfig.c_str()), m_pRusLemmatizer, m_pRusGramTab, true);
            m_TrigramModel.ReadBinary();
        }
	}
	catch(CExpc c)
	{
        ErrorMessage ("MAPOST", Format("%s",c.m_strCause.c_str()).c_str());
		return false;		
	}
	catch(...)
	{
		return false;		
	}
	return true;
}

