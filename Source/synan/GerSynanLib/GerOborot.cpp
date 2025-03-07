#pragma warning(disable:4786) 
#include "GerSynan.h"
#include "GerOborot.h"

static const size_t CoordConjTypesCount = 3;
static const CCoordConjType CoordConjTypes[CoordConjTypesCount] = {
	{"UND", false},
    {"ODER", false},
	{"SONDERN",true}
	
	};

CGerOborDic::CGerOborDic(const CSyntaxOpt* Opt) : COborDic(Opt)
{
	m_SimpleCoordConj.clear();
	for (size_t i=0; i < CoordConjTypesCount;i++)
		m_SimpleCoordConj.push_back(CoordConjTypes[i]);

}

static long GetItemNoByItemStr(const CDictionary* piOborDic, const std::string& ItemStr, const char* _DomStr) 
{
	BYTE DomNo = piOborDic->GetDomenNoByDomStr(_DomStr);
    return piOborDic->GetItemIdByItemStr(ItemStr, DomNo);
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
		

		BYTE case_dom_no = piOborDic->GetDomenNoByDomStr("D_CASE");
		// cases
		auto NominativItemNo = piOborDic->GetItemIdByItemStr("N", case_dom_no);
		assert(NominativItemNo != -1);
		auto GenitivItemNo = piOborDic->GetItemIdByItemStr("G", case_dom_no);
		assert(GenitivItemNo != -1);
		auto AccusativItemNo = piOborDic->GetItemIdByItemStr("A", case_dom_no);
		assert(AccusativItemNo != -1);
		auto DativItemNo = piOborDic->GetItemIdByItemStr("D", case_dom_no);
		assert(DativItemNo != -1);

		for(int UnitNo = 0 ; UnitNo < iSize ; UnitNo++ )
		{
			
			COborotForSyntax oborot;
			
			oborot.m_OborotEntryStr = piOborDic->GetEntryStr(UnitNo);
			MakeUpperUtf8(oborot.m_OborotEntryStr);

			oborot.m_AllPossibleDependCases = 0;
		    if (piOborDic->IsEmptyArticle(UnitNo) == false)
			for( int i = piOborDic->GetUnitStartPos(UnitNo) ; i <= piOborDic->GetUnitLastPos(UnitNo) ; i++)
			{
				if (GramFetFieldNo != piOborDic->GetCortege(i).m_FieldNo) 
					continue;
				BYTE LeafId = piOborDic->GetCortege(i).m_LeafId;
				dom_item_id_t Item0 = piOborDic->GetCortege(i).GetItem(0);
				dom_item_id_t Item1 = piOborDic->GetCortege(i).GetItem(1);
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





