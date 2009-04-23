#pragma warning(disable:4786) 
#include "GerSynan.h"
#include "GerOborot.h"



COborDic* NewOborDicGerman (CSyntaxOpt* Opt)
{
	return new CGerOborDic(Opt);
};

static const size_t CoordConjTypesCount = 3;
static const CCoordConjType CoordConjTypes[CoordConjTypesCount] = {
	{"UND", false},
    {"ODER", false},
	{"SONDERN",true}
	
	};

CGerOborDic::CGerOborDic(CSyntaxOpt* Opt) : COborDic(Opt)
{
	m_SimpleCoordConj.clear();
	for (size_t i=0; i < CoordConjTypesCount;i++)
		m_SimpleCoordConj.push_back(CoordConjTypes[i]);

}

static long GetItemNoByItemStr(const CDictionary* piOborDic, const char* ItemStr, const char* _DomStr) 
{
	BYTE DomNo = piOborDic->GetDomenNoByDomStr(_DomStr);
    return piOborDic->GetItemNoByItemStr(ItemStr, DomNo);
}







bool CGerOborDic::ReadOborDic (const CDictionary* piOborDic)
{
	try
	{
		if(piOborDic == NULL )
			return false;

		//for (i=0; i < SubConjsCount;i++)
		//	m_SimpleSubConj.push_back(strSubConjs[i]);

		m_SimpleSubConj.clear();
		m_SimpleSubConj.push_back("WAS");
		m_SimpleSubConj.push_back("WER");
		m_SimpleSubConj.push_back("WO");
		m_SimpleSubConj.push_back("WARUM");
		m_SimpleSubConj.push_back("WESHALB");
		m_SimpleSubConj.push_back("WESWEGEN");
		m_SimpleSubConj.push_back("WIESO");
		m_SimpleSubConj.push_back("WIEVIEL");
		m_SimpleSubConj.push_back("WOBEI");
		m_SimpleSubConj.push_back("WANN");
		m_SimpleSubConj.push_back("WOHIN");
		m_SimpleSubConj.push_back("WOHER");
		m_SimpleSubConj.push_back("WODURCH");
		m_SimpleSubConj.push_back("WOFÜR");
		m_SimpleSubConj.push_back("WOHINTER");
		m_SimpleSubConj.push_back("WOMIT");
		m_SimpleSubConj.push_back("WONACH");
		m_SimpleSubConj.push_back("WONEBEN");
		m_SimpleSubConj.push_back("WORAN");
		m_SimpleSubConj.push_back("WORAUF");
		m_SimpleSubConj.push_back("WORAUS");
		m_SimpleSubConj.push_back("WORIN");
		m_SimpleSubConj.push_back("WORUM");
		m_SimpleSubConj.push_back("WORUNTER");
		m_SimpleSubConj.push_back("WORÜBER");
		m_SimpleSubConj.push_back("WOVON");
		m_SimpleSubConj.push_back("WOVOR");
		m_SimpleSubConj.push_back("WOZU");
		m_SimpleSubConj.push_back("WOZWISCHEN");

		int iSize = piOborDic->GetUnitsSize();
		m_Entries.reserve(iSize + 1);

		BYTE GramFetFieldNo = piOborDic->GetFieldNoByFieldStr ("GF");
		assert (GramFetFieldNo != ErrUChar);
		long SimplePrepItemNo = GetItemNoByItemStr(piOborDic,"PRP", "D_PART_OF_SPEECH");
		long KonjUntItemNo = GetItemNoByItemStr(piOborDic,"KON_unt", "D_PART_OF_SPEECH");
		long KonjNebItemNo = GetItemNoByItemStr(piOborDic,"KON_neb", "D_PART_OF_SPEECH");
		long DisruptKonItemNo = GetItemNoByItemStr(piOborDic,"DISKONT_KON", "D_PART_OF_SPEECH");
		long SchaltwortItemNo = GetItemNoByItemStr(piOborDic,"SCHALTWORT", "D_PART_OF_SPEECH");
		long ZahlwortItemNo = GetItemNoByItemStr(piOborDic,"ZAL", "D_PART_OF_SPEECH");
		

		// cases
		long NominativItemNo = GetItemNoByItemStr(piOborDic,"N", "D_CASE");
		assert(NominativItemNo != -1);
		long GenitivItemNo = GetItemNoByItemStr(piOborDic,"G", "D_CASE");
		assert(GenitivItemNo != -1);
		long AccusativItemNo = GetItemNoByItemStr(piOborDic,"A", "D_CASE");
		assert(AccusativItemNo != -1);
		long DativItemNo = GetItemNoByItemStr(piOborDic,"D", "D_CASE");
		assert(DativItemNo != -1);



		for(int UnitNo = 0 ; UnitNo < iSize ; UnitNo++ )
		{
			
			COborotForSyntax oborot;
			
			oborot.m_OborotEntryStr = piOborDic->GetEntryStr(UnitNo);
			GerMakeUpper(oborot.m_OborotEntryStr);

			oborot.m_AllPossibleDependCases = 0;
		    if (piOborDic->IsEmptyArticle(UnitNo) == false)
			for( int i = piOborDic->GetUnitStartPos(UnitNo) ; i <= piOborDic->GetUnitEndPos(UnitNo) ; i++)
			{
				if (GramFetFieldNo != piOborDic->GetCortegeFieldNo(i)) 
					continue;
				BYTE LeafId = piOborDic->GetCortegeLeafId(i);
				long Item0 = piOborDic->GetCortegeItem(i,0);
				long Item1 = piOborDic->GetCortegeItem(i,1);
				if (LeafId == 0)
				{
					if ( SimplePrepItemNo == Item0)
					{
						oborot.AddPartOfSpeech(gPRP);
						BuildOborots(UnitNo, piOborDic, &COborDic::WriteSimplePrep );
					}

					if	(		( KonjUntItemNo == Item0) 
							||	( KonjNebItemNo== Item0)
						)
					{
						oborot.AddPartOfSpeech(gKON);
						if	( KonjUntItemNo == Item0) 
						{
							BuildOborots(UnitNo, piOborDic, &COborDic::WriteSimpleSubConj );
							oborot.m_ConjType = sub_conj;
						};
					};
					if(    DisruptKonItemNo  == Item0 )
					{
						oborot.m_ConjType = disrupt_conj;
						oborot.AddPartOfSpeech(gKON);
						BuildOborots(UnitNo, piOborDic, &COborDic::TokenizeDoubleConj );
					}		

					if(    SchaltwortItemNo  == Item0 )
					{
						oborot.m_bParenthesis = true;
						oborot.AddPartOfSpeech(gADV);
					}	

					if(    ZahlwortItemNo  == Item0 )
					{
						oborot.AddPartOfSpeech(gZAL);
					};
				};

				if (LeafId == 2)
				{

					if (NominativItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM( gNominativ);
					}
					if (GenitivItemNo == Item1 )
					{
					 oborot.m_DependCases |= _QM( gGenitiv );
					}
					if (AccusativItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM( gAkkusativ);
					}
					if (DativItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM(gDativ);
					}
				};

			}


			m_Entries.push_back(oborot);
		}    
		
	}
	catch(...)
	{		
		return false;
	}
	return true; 	
};





