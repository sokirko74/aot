// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#pragma warning(disable:4786) 
#include "StdSynan.h"
#include "RusOborot.h"



COborDic* NewOborDicRussian (CSyntaxOpt* Opt)
{
	return new CRusOborDic(Opt);
};

static const int SubConjsCount = 21;
static const std::string strSubConjs[SubConjsCount] = {_R("ЧТО"),_R("ЧТОБЫ"),_R("ГДЕ"),_R("КОГДА"),_R("ЕСЛИ"),_R("ЗАЧЕМ"),_R("ОТЧЕГО"),
														 _R("ПОЧЕМУ"),_R("СКОЛЬКО"),_R("НАСКОЛЬКО"),_R("ЧЕМ"),
														 _R("ХОТЯ"),_R("КТО"),_R("ПОСКОЛЬКУ"),_R("ПОКА"),_R("КАК"), _R("ОТКУДА"),_R("КУДА"),_R("КОТОРЫЙ"),_R("ЧЕЙ"), _R("ДАБЫ")};


static const size_t CoordConjTypesCount = 5;
static const CCoordConjType CoordConjTypes[CoordConjTypesCount] = {
	{_R("И"), false},
    {_R("ИЛИ"), false},
	{_R("ЛИБО"),false},
	{_R("НО"), true},
	{_R("А"), true}
};


CRusOborDic::CRusOborDic(const CSyntaxOpt* Opt) : COborDic(Opt)
{
	m_SimpleSubConj.clear();
	size_t i ;
	for (i=0; i < SubConjsCount;i++)
		m_SimpleSubConj.push_back(strSubConjs[i]);

	m_SimpleCoordConj.clear();
	for (i=0; i < CoordConjTypesCount;i++)
		m_SimpleCoordConj.push_back(CoordConjTypes[i]);
};




static dom_item_id_t GetItemNoByItemStr(const CDictionary* piOborDic, const std::string& ItemStr, const char* domStr)
{
	BYTE DomNo = piOborDic->GetDomenNoByDomStr(domStr);
	if (DomNo == ErrUChar) {
		throw CExpc("cannot find item s domain %s", domStr);
	}
	dom_item_id_t item_id = piOborDic->GetItemIdByItemStr(ItemStr, DomNo);
	if (is_null(item_id)) {
		throw CExpc("cannot find %s in domain %s", ItemStr.c_str(), domStr);
	}
    return item_id;
}


bool CRusOborDic::ReadOborDic (const CDictionary* piOborDic)
{
	try
	{
		if(piOborDic == NULL )
			return false;

		int iSize = piOborDic->GetUnitsSize();
		m_Entries.reserve(iSize + 1);
		int fl_conj_t = 0;
		// части речи

		BYTE GramFetFieldNo = piOborDic->GetFieldNoByFieldStr ("GF");
		assert (GramFetFieldNo != ErrUChar);
		auto ParenthesItemNo = GetItemNoByItemStr(piOborDic, "ВВОДН", "D_PART_OF_SPEECH");
		auto AdverbItemNo = GetItemNoByItemStr(piOborDic, "НАР", "D_PART_OF_SPEECH");
		auto ParticItemNo = GetItemNoByItemStr(piOborDic, "ЧАСТ", "D_PART_OF_SPEECH");
		auto SubordConjItemNo = GetItemNoByItemStr(piOborDic, "ПОДЧ_СОЮЗ", "D_PART_OF_SPEECH");
		auto CoordConjItemNo = GetItemNoByItemStr(piOborDic, "СОЧ_СОЮЗ", "D_PART_OF_SPEECH");
		auto SimplePrepItemNo = GetItemNoByItemStr(piOborDic, "ПРОСТ_ПРЕДЛ", "D_PART_OF_SPEECH");
		auto CoordDisruptPrepItemNo = GetItemNoByItemStr(piOborDic,"СОЧ_РАЗРЫВ_СОЮЗ", "D_PART_OF_SPEECH");		long SubordDisruptPrepItemNo = GetItemNoByItemStr(piOborDic,_R("ПОДЧ_РАЗРЫВ_СОЮЗ"), "D_PART_OF_SPEECH");
		auto PrepItemNo = GetItemNoByItemStr(piOborDic,"ПРЕДЛ", "D_PART_OF_SPEECH");

		BYTE case_dom_no = piOborDic->GetDomenNoByDomStr("D_CASE");
		auto NominativItemNo = piOborDic->GetItemIdByItemStr("И", case_dom_no);
		auto GenitivItemNo = piOborDic->GetItemIdByItemStr("Р", case_dom_no);
		auto AccusativItemNo = piOborDic->GetItemIdByItemStr("В", case_dom_no);
		auto DativItemNo = piOborDic->GetItemIdByItemStr("Д", case_dom_no);
		auto InstrumentalisItemNo = piOborDic->GetItemIdByItemStr("Т", case_dom_no);
		auto VocativItemNo = piOborDic->GetItemIdByItemStr("П", case_dom_no);
		auto NominativPluralItemNo = piOborDic->GetItemIdByItemStr("И_мн", piOborDic->GetDomenNoByDomStr("D_CASE_NUMBER"));

		
		

		// уточнения ГГ
		auto InfinitiveItemNo = GetItemNoByItemStr(piOborDic,_R("инф"), "D_VP_SPECIF");


		for(int UnitNo = 0 ; UnitNo < iSize ; UnitNo++ )
		{
			
			COborotForSyntax oborot;
			
			oborot.m_OborotEntryStr = piOborDic->GetEntryStr(UnitNo);
			EngRusMakeUpper(oborot.m_OborotEntryStr);

			oborot.m_AllPossibleDependCases = 0;
		    if (piOborDic->IsEmptyArticle(UnitNo) == false)
			for( int i = piOborDic->GetUnitStartPos(UnitNo) ; i <= piOborDic->GetUnitEndPos(UnitNo) ; i++)
			{
				if (GramFetFieldNo != piOborDic->GetCortegeFieldNo(i)) 
					continue;

				BYTE LeafId = piOborDic->GetCortegeLeafId(i);
				dom_item_id_t Item0 = piOborDic->GetCortege(i).GetItem(0);
				dom_item_id_t Item1 = piOborDic->GetCortege(i).GetItem(1);
				if (LeafId == 0)
				{
					{
						oborot.m_GrammarFeature = piOborDic->WriteToString(piOborDic->GetCortege(i));
						Trim(oborot.m_GrammarFeature);
						if (oborot.m_GrammarFeature == _R("НАР"))
							oborot.m_GrammarFeature = _R("Н");

					};
					//nim : вводный оборот, проверить GF=ВВОДН
					if ( ParenthesItemNo == Item0)
					{
						oborot.AddPartOfSpeech(INP);
						oborot.m_bParenthesis = true;

					};

					//nim : наречие (_R("заграницей")), проверить GF=НАР
					if ( AdverbItemNo == Item0)

						oborot.AddPartOfSpeech(ADV);

					if ( ParticItemNo == Item0)
						oborot.AddPartOfSpeech(PARTICLE);

					if ( SubordConjItemNo == Item0)
					{
						oborot.m_ConjType = sub_conj;
						oborot.AddPartOfSpeech(CONJ);


					}
					else if( CoordConjItemNo == Item0)
					{
						oborot.m_ConjType = comp_conj;
						oborot.AddPartOfSpeech(CONJ);
					}

					if ( SimplePrepItemNo == Item0)
					{
						oborot.AddPartOfSpeech(PREP);
						BuildOborots(UnitNo, piOborDic, &COborDic::WriteSimplePrep );
					}
			
					if(    CoordDisruptPrepItemNo  == Item0
						|| SubordDisruptPrepItemNo == Item0
					  )
					{
						oborot.m_ConjType = disrupt_conj;
						oborot.AddPartOfSpeech(CONJ);
						BuildOborots(UnitNo, piOborDic, &COborDic::TokenizeDoubleConj );
					}			

					if( PrepItemNo == Item0 )
					{
						oborot.AddPartOfSpeech(PREP);
					}

				};

				if (LeafId == 2)
				{
					if (InfinitiveItemNo == Item1 )
					  if (oborot.m_ConjType == sub_conj)
				 	  	oborot.m_bConjWithInfinitiveSubord= true;

					if (NominativItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM(rNominativ);
					}
					if (NominativPluralItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM(rNominativ);
					}
					if (GenitivItemNo == Item1 )
					{
					 oborot.m_DependCases |= _QM(rGenitiv);
					}
					if (AccusativItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM(rAccusativ);
					}
					if (DativItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM(rDativ);
					}
					if (InstrumentalisItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM(rInstrumentalis);
					}
					if (VocativItemNo == Item1 )
					{
					  oborot.m_DependCases |= _QM(rLocativ);
					}

				};

			}
			m_Entries.push_back(oborot);
			const COborotForSyntax& debug = m_Entries.back();
		}    
		
	}
	catch(...)
	{		
		return false;
	}
	return true; 	
};

