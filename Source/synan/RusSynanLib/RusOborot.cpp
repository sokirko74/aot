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
static const std::string strSubConjs[SubConjsCount] = {"ЧТО","ЧТОБЫ","ГДЕ","КОГДА","ЕСЛИ","ЗАЧЕМ","ОТЧЕГО",
														 "ПОЧЕМУ","СКОЛЬКО","НАСКОЛЬКО","ЧЕМ",
														 "ХОТЯ","КТО","ПОСКОЛЬКУ","ПОКА","КАК", "ОТКУДА","КУДА","КОТОРЫЙ","ЧЕЙ", "ДАБЫ"};


static const size_t CoordConjTypesCount = 5;
static const CCoordConjType CoordConjTypes[CoordConjTypesCount] = {
	{"И", false},
    {"ИЛИ", false},
	{"ЛИБО",false},
	{"НО", true},
	{"А", true}
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
		auto CoordDisruptPrepItemNo = GetItemNoByItemStr(piOborDic,"СОЧ_РАЗРЫВ_СОЮЗ", "D_PART_OF_SPEECH");		
		auto SubordDisruptPrepItemNo = GetItemNoByItemStr(piOborDic, "ПОДЧ_РАЗРЫВ_СОЮЗ", "D_PART_OF_SPEECH");
		auto PrepItemNo = GetItemNoByItemStr(piOborDic,"ПРЕДЛ", "D_PART_OF_SPEECH");

		BYTE case_dom_no = piOborDic->GetDomenNoByDomStr("D_CASE");
		auto NominativItemNo = piOborDic->GetItemIdByItemStr("И", case_dom_no);
		auto GenitivItemNo = piOborDic->GetItemIdByItemStr("Р", case_dom_no);
		auto AccusativItemNo = piOborDic->GetItemIdByItemStr("В", case_dom_no);
		auto DativItemNo = piOborDic->GetItemIdByItemStr("Д", case_dom_no);
		auto InstrumentalisItemNo = piOborDic->GetItemIdByItemStr("Т", case_dom_no);
		auto VocativItemNo = piOborDic->GetItemIdByItemStr("П", case_dom_no);
		auto NominativPluralItemNo = piOborDic->GetItemIdByItemStr("И_мн", case_dom_no);
		assert(!is_null(NominativPluralItemNo));

		
		

		// уточнения ГГ
		auto InfinitiveItemNo = GetItemNoByItemStr(piOborDic,"инф", "D_VP_SPECIF");
		assert(!is_null(InfinitiveItemNo));

		for(int UnitNo = 0 ; UnitNo < piOborDic->GetUnitsSize(); UnitNo++ )
		{
			COborotForSyntax oborot;
			oborot.m_OborotEntryStr = piOborDic->GetEntryStr(UnitNo);
			MakeUpperUtf8(oborot.m_OborotEntryStr);
			oborot.m_AllPossibleDependCases = 0;
			if (piOborDic->IsEmptyArticle(UnitNo)) {
				m_Entries.push_back(oborot);
				continue;
			}

			for( int i = piOborDic->GetUnitStartPos(UnitNo) ; i <= piOborDic->GetUnitLastPos(UnitNo) ; i++)
			{
				const TCortege& c = piOborDic->GetCortege(i);
				if (GramFetFieldNo != c.m_FieldNo) 
					continue;

				BYTE LeafId = c.m_LeafId;
				dom_item_id_t Item0 = c.GetItem(0);
				dom_item_id_t Item1 = c.GetItem(1);
				if (LeafId == 0)
				{
					{
						oborot.m_GrammarFeature = piOborDic->WriteToString(c);
						Trim(oborot.m_GrammarFeature);
						if (oborot.m_GrammarFeature == "НАР")
							oborot.m_GrammarFeature = "Н";

					};
					if ( ParenthesItemNo == Item0)
					{
						oborot.AddPartOfSpeech(INP);
						oborot.m_bParenthesis = true;

					};

					//nim : наречие ("заграницей"), проверить GF=НАР
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

