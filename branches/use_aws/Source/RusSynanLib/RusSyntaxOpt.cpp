// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include  "StdSynan.h"
#include  "RusSyntaxOpt.h"
#include  "RusSentence.h"
#include  "RusThesaurus.h"
#include  "RusOborot.h"
#include  "../StructDictLib/TempArticle.h"


const int rSyntaxGroupTypesCount = 48 ;
const char rSyntaxGroupTypes [rSyntaxGroupTypesCount][30] = 
{

					"����_���",				"�����",      
"����_����",		"���_����_���",			"���_����",    
"��",				"�����_����",			"�����_������",	
"���_����",			"���_���",				"�����_�������",	
"���_�����",		"����_���",				"�����_��",			
"�����_���",		"���_����_���",			"�����_���",			     
"���",				"�����_��",				"����_���",				
"��",				"�����_��",				"��_�����",
"�����_����",		"�����_����",			"�������_��",
"���_���_����",		"�������_��",			"����_���",
"����",				"������",				"���������",		
"����������",		"���_���_����",			"���",				
"������_����",		"������",				"���_������",      
"����_�������",     "������_���",			"����_�����",      
"�����_������",     "����_���",				"������_����", 
"�������",			"����_���_���",			"�����_���"

};




CSyntaxOpt*  NewOptionsRussian ()
{
	return new CRusSyntaxOpt(morphRussian);
};

CRusSyntaxOpt :: CRusSyntaxOpt (MorphLanguageEnum langua) : CSyntaxOpt(langua)
{	
	SynDependOnAdv = NULL;
	SynDependOnAdj = NULL;
	m_pCompAdvList = NULL;
	m_pNounNumList = NULL;
	m_pVerbsWithInstrObj = NULL;
	m_pNumberAdverbsList = NULL;
    
	AdvAdj = NULL;
	VerbsThatCanSubdueInfinitive = NULL;
	
	m_lPradigmID_NECHEGO = -1;	

	m_SimilarNPGroupType = SIMILAR_NOUN_GROUPS;
	m_PluralMask = _QM(rPlural);
	m_SingularMask = _QM(rSingular);
	m_IndeclinableMask = _QM(rIndeclinable);
	m_GenNounGroupType = GEN_NOUN;
	m_PrepNounGroupType = PREP_NOUN;
	m_DirObjGroupType = DIR_OBJ;
	m_NPGroupType = NOUN_GR;
	m_NounAdjGroupType= NOUN_ADJUNCT;
	m_NameGroupType = NAMES;
	m_DisruptConjRelation = DISRUPT_CONJ_RELATION;
	m_DisruptConjGroupType = DISRUPT_CONJ;
	m_OborotGroupType = OBOROTS;
	m_WWWGroupType = WEB_ADDR;
	m_KEYBGroupType = KEYB;
	m_SubjRelation = SUBJ;
	m_RusParenthesis = INP;
	m_Preposition = PREP;
	m_Conjunction = CONJ;
	m_InfintiveClauseType = INFINITIVE_T;

	m_SyntaxGroupTypes.clear();
	for (size_t i=0; i < rSyntaxGroupTypesCount; i++)
		m_SyntaxGroupTypes.push_back(rSyntaxGroupTypes[i]);
	
}

CSentence* CRusSyntaxOpt::NewSentence() const {
	return new CRusSentence(this);
};

CAgramtab *CRusSyntaxOpt::NewGramTab() const {
	return new CRusGramTab();
};

CLemmatizer *CRusSyntaxOpt::NewLemmatizer() const {
	return new CLemmatizerRussian();
};

COborDic * CRusSyntaxOpt::NewOborDic(const CSyntaxOpt* opt)  {
	return new CRusOborDic(opt);
};

CThesaurusForSyntax* CRusSyntaxOpt::NewThesaurus(const CSyntaxOpt* opt) {
	return new CRusThesaurusForSyntax(opt);
};

void CRusSyntaxOpt::DestroyOptions ()
{
	CSyntaxOpt::DestroyOptions();
	if(m_pCompAdvList)
		delete m_pCompAdvList;
	if(m_pNounNumList)
		delete m_pNounNumList;
	if(m_pVerbsWithInstrObj)
		delete m_pVerbsWithInstrObj;
	
	if(m_pNumberAdverbsList)
		delete m_pNumberAdverbsList;
	if(AdvAdj)
		delete AdvAdj;
	if(SynDependOnAdj)
		delete SynDependOnAdj;
	if(SynDependOnAdv)
		delete SynDependOnAdv;
	if (VerbsThatCanSubdueInfinitive)
		delete VerbsThatCanSubdueInfinitive;

	
};





void BuildArticle(CDictionary* piRossDict, string s, CTempArticle& A1)
{
	A1.m_pRoss =  piRossDict;
	A1.SetArticleStr( s.c_str() );
	A1.MarkUp();
	A1.BuildCortegeList();
};


void CRusSyntaxOpt :: LoadFromRoss(CDictionary* piRossDict)
{
	int i;

	try
	{
		int iSize = piRossDict->GetUnitsSize();

		AdvAdj = new SDatItems(_QM(ADV));	
		SynDependOnAdv = new SDatItems(_QM(ADV));	
		SynDependOnAdj = new SDatItems(_QM(ADV));	

		CTempArticle A1;
		BuildArticle(piRossDict,"GF = * ���:���_���", A1);
		
		CTempArticle A2;
		BuildArticle(piRossDict,"GF1 = * X!:���", A2);

		CTempArticle A3;
		BuildArticle(piRossDict,"GF1 = * X!:����", A3);		

		
		CTempArticle A;
		A.m_pRoss = piRossDict;

		for(i = 0 ; i < iSize ; i++ )
		{
			A.ReadFromDictionary (i, false, true);
			
			const string& DebugWord = piRossDict->m_Units[i].m_EntryStr;

			if (A1.IsPartOf(&A, true))
			{
				string dat_item = piRossDict->m_Units[i].m_EntryStr;
				AdvAdj->m_vectorDatItems.push_back(dat_item);
			}

			if (A2.IsPartOf(&A, true))
			{
				string  dat_item = piRossDict->m_Units[i].m_EntryStr;
				SynDependOnAdv->m_vectorDatItems.push_back(dat_item);
			}

			if (A3.IsPartOf(&A, true))
			{
				string  dat_item = piRossDict->m_Units[i].m_EntryStr;
				SynDependOnAdj->m_vectorDatItems.push_back(dat_item);
			}
		}
	}
	catch(...)
	{
		char strMsg[200];
		strcpy(strMsg, "Problems with ROSS, word : \"");
		strcat (strMsg, piRossDict->m_Units[i].m_EntryStr);
		strcat (strMsg, "\"");
		OutputErrorString(strMsg);
	}
	SynDependOnAdj->Sort();
	SynDependOnAdv->Sort();
	AdvAdj->Sort();


}


const char g_strRegRossDicPath[] = "Software\\Dialing\\Ross\\DictPath"; 

static string GetSyntaxFilePath()
{
	return GetRmlVariable()+"/Dicts/SynAn/";
};


bool CRusSyntaxOpt :: InitOptionsLanguageSpecific()
{
	
	//loading ross
	try
	{			
		CDictionary piRossDict;	
		string strPath = GetRegistryString( g_strRegRossDicPath );
		if (!piRossDict.Load(strPath.c_str())) return false;
		LoadFromRoss(&piRossDict);
	
	}
	catch(CExpc& )
	{
		char strMsg[400];
		sprintf(strMsg, "Failed to find registry entry %s", g_strRegRossDicPath);
		OutputErrorString("Clause Builder");
		return false;			
	}
	catch(...)
	{			
		OutputErrorString("Failed to load \"StructDict.dll\" ( Ross)");	
		return false;
	}

	

	
	string Path = GetSyntaxFilePath();
  	

	m_pCompAdvList = new StringVector;
	if (!ReadListFile (Path+"comp_adv.dat",*(m_pCompAdvList)))
		return false;
	
	m_pNounNumList = new StringVector;
	if (!ReadListFile (Path+"noun_num.dat",*(m_pNounNumList)))
		return false;


	m_pNumberAdverbsList = new StringVector;
	if (!ReadListFile (Path+"num_pr.dat",*(m_pNumberAdverbsList)))
		return false;

	
	VerbsThatCanSubdueInfinitive = new SDatItems(_QM(VERB) | _QM(INFINITIVE) | _QM(ADVERB_PARTICIPLE) | _QM(PARTICIPLE_SHORT) | _QM(PARTICIPLE));	
	if (!ReadListFile (Path+"verbs_with_inf.txt", VerbsThatCanSubdueInfinitive->m_vectorDatItems))
		return false;
	VerbsThatCanSubdueInfinitive->Sort();

	m_pVerbsWithInstrObj = new SDatItems(_QM(VERB) | _QM(INFINITIVE) | _QM(ADVERB_PARTICIPLE) | _QM(PARTICIPLE_SHORT) | _QM(PARTICIPLE));	
	if (!ReadListFile (Path+"verbs_with_inf.txt", m_pVerbsWithInstrObj->m_vectorDatItems))
		return false;
	m_pVerbsWithInstrObj->Sort();
	
    if (m_pProfessions->m_vectorDatItems.empty())
    {
        // read it from file if thesaurus is disabled
	    if (!ReadListFile (Path+"profes.txt", m_pProfessions->m_vectorDatItems))
		    return false;
	    m_pProfessions->Sort();
    }


	try
	{
		vector<CFormInfo> Paradigms;
		string h = "������";
		GetLemmatizer()->CreateParadigmCollection(true, h, false, false, Paradigms);

		for (long k=0; k < Paradigms.size(); k++)
		{
		  string AnCode = Paradigms[k].GetAncode(0);
		  BYTE POS = GetGramTab()->GetPartOfSpeech(AnCode.c_str() );
		  if  (POS == PRONOUN_PREDK)
		  {
			m_lPradigmID_NECHEGO = Paradigms[k].GetParadigmId();
			m_Gramcode_NECHEGO = Paradigms[k].GetAncode(0);
			break;
		  };
		};


	}
	catch (...)
	{

	}


	return true;
}


bool CRusSyntaxOpt::is_firm_group(int GroupType) const
{
	return (		(GroupType == WEB_ADDR ) 
				||	(GroupType == NUMERALS ) 
				||	(GroupType == C_NUMERALS ) 
				||	(GroupType == NAMES )	
				||	(GroupType == KEYB )  
				||	(GroupType == FOREIGN_TERM )
				||	(GroupType == OBOROTS ) 
			);
}


bool CRusSyntaxOpt::IsGroupWithoutWeight(int GroupType, const char* cause) const
{
	//	������ ����_��� �� ����� ��������� ��� �������� ����, ��������� 
	//	����� �� ����. �������� ��������� �������� �������:
	//		��� � "������" �� ���� ������� ������ ���������� "���"
	//	��� ����� "� ������ ��� ������� ��������." ��������� 
	//	�������������  ������� ������� "���".

	
	if (GroupType == DIR_OBJ) return true;
	
	//  ���� ��� ��, � �������� ������� ����� - ��������������, ����� 
	//  ����� ����� �� ������ ��������� (���� ��� ����) � ������� �� ���
	//  ��������: "� ������ ��������" - ����� ����� ���������  ��� ������ 
	//  "������ ��������"
	
	if (    cause
			&& !strcmp(cause,"���������� ������ c ��������������")
		)
		return true;


	//	������ ���_����� �� ����� ��������� ��� �������� ����, ��������� 
	//	����� �� ����. �������� ��������� �������� �������:
	//		"�� ���� ���� ������ � ���"
	if (GroupType == NEG_VERB) return true;


	// ������ ��� ������ ���_���� "���� ������� ������� ����� ����������� ������� �����."
	if (GroupType == ADV_VERB) return  true;

	// "����� �����"
	if (GroupType == GEN_NOUN) return  true;


	// "��� �� ������ ���?"
	if (GroupType == FROM_COMPAR) return  true;

	// ���� �� ���� �������� ���:
	//if (GroupType == INSTR_OBJ) return  true;

	// "����� ������ ��� �������� �� ����� ���� , �������� ������ �������"
	if (GroupType == NOUN_PARTICIPLE) return  true;
	
	return false;

};

bool CRusSyntaxOpt::IsSimilarGroup (int type) const
{
	return   (type  == SIMILAR_ADJS)
		   ||(type  == SIMILAR_NOUN_GROUPS)
		   ||(type  == SIMILAR_INFS)
		   ||(type  == SIMILAR_ADVS)
		   ||(type  == SIMILAR_NUMERALS) ;
};
