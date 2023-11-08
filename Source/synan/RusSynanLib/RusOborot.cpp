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




static long GetItemNoByItemStr(const CDictionary* piOborDic, const std::string& ItemStr, const char* _DomStr) 
{
	BYTE DomNo = piOborDic->GetDomenNoByDomStr(_DomStr);
    return piOborDic->GetItemNoByItemStr(ItemStr, DomNo);
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
		long ParenthesItemNo = GetItemNoByItemStr(piOborDic,_R("ВВОДН"), "D_PART_OF_SPEECH");
		assert(ParenthesItemNo != -1);
		long AdverbItemNo = GetItemNoByItemStr(piOborDic,_R("НАР"), "D_PART_OF_SPEECH");
		assert(AdverbItemNo != -1);
		long ParticItemNo = GetItemNoByItemStr(piOborDic,_R("ЧАСТ"), "D_PART_OF_SPEECH");
		assert(ParticItemNo != -1);
		long SubordConjItemNo = GetItemNoByItemStr(piOborDic,_R("ПОДЧ_СОЮЗ"), "D_PART_OF_SPEECH");
		assert(SubordConjItemNo != -1);
		long CoordConjItemNo = GetItemNoByItemStr(piOborDic,_R("СОЧ_СОЮЗ"), "D_PART_OF_SPEECH");
		assert(CoordConjItemNo != -1);
		long SimplePrepItemNo = GetItemNoByItemStr(piOborDic,_R("ПРОСТ_ПРЕДЛ"), "D_PART_OF_SPEECH");
		assert(SimplePrepItemNo != -1);
		long CoordDisruptPrepItemNo = GetItemNoByItemStr(piOborDic,_R("СОЧ_РАЗРЫВ_СОЮЗ"), "D_PART_OF_SPEECH");
		assert(CoordDisruptPrepItemNo != -1);
		long SubordDisruptPrepItemNo = GetItemNoByItemStr(piOborDic,_R("ПОДЧ_РАЗРЫВ_СОЮЗ"), "D_PART_OF_SPEECH");
		assert(SubordDisruptPrepItemNo != -1);
		long PrepItemNo = GetItemNoByItemStr(piOborDic,_R("ПРЕДЛ"), "D_PART_OF_SPEECH");
		assert(PrepItemNo != -1);
		// падежи 
		long NominativItemNo = GetItemNoByItemStr(piOborDic,_R("И"), "D_CASE");
		assert(NominativItemNo != -1);
		long GenitivItemNo = GetItemNoByItemStr(piOborDic,_R("Р"), "D_CASE");
		assert(GenitivItemNo != -1);
		long AccusativItemNo = GetItemNoByItemStr(piOborDic,_R("В"), "D_CASE");
		assert(AccusativItemNo != -1);
		long DativItemNo = GetItemNoByItemStr(piOborDic,_R("Д"), "D_CASE");
		assert(DativItemNo != -1);
		long InstrumentalisItemNo = GetItemNoByItemStr(piOborDic, _R("Т"), "D_CASE");
		assert(InstrumentalisItemNo != -1);
		long VocativItemNo = GetItemNoByItemStr(piOborDic, _R("П"), "D_CASE");
		assert(VocativItemNo != -1);
		long NominativPluralItemNo = GetItemNoByItemStr(piOborDic,_R("И_мн"), "D_CASE");

		
		

		// уточнения ГГ
		long InfinitiveItemNo = GetItemNoByItemStr(piOborDic,_R("инф"), "D_VP_SPECIF");
		assert(InfinitiveItemNo != -1);


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
				long Item0 = piOborDic->GetCortegeItem(i,0);
				long Item1 = piOborDic->GetCortegeItem(i,1);
				if (LeafId == 0)
				{
					{
						char OutBuffer[1000];
						piOborDic->CortegeToStr(*piOborDic->GetCortegePtr(i), OutBuffer);
						oborot.m_GrammarFeature = OutBuffer;
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

