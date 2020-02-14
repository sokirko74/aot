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
static const string strSubConjs[SubConjsCount] = {"���","�����","���","�����","����","�����","������",
														 "������","�������","���������","���",
														 "����","���","���������","����","���", "������","����","�������","���", "����"};


static const size_t CoordConjTypesCount = 5;
static const CCoordConjType CoordConjTypes[CoordConjTypesCount] = {
	{"�", false},
    {"���", false},
	{"����",false},
	{"��", true},
	{"�", true}};


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




static long GetItemNoByItemStr(const CDictionary* piOborDic, const char* ItemStr, const char* _DomStr) 
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
		// ����� ����

		BYTE GramFetFieldNo = piOborDic->GetFieldNoByFieldStr ("GF");
		assert (GramFetFieldNo != ErrUChar);
		long ParenthesItemNo = GetItemNoByItemStr(piOborDic,"�����", "D_PART_OF_SPEECH");
		assert(ParenthesItemNo != -1);
		long AdverbItemNo = GetItemNoByItemStr(piOborDic,"���", "D_PART_OF_SPEECH");
		assert(AdverbItemNo != -1);
		long ParticItemNo = GetItemNoByItemStr(piOborDic,"����", "D_PART_OF_SPEECH");
		assert(ParticItemNo != -1);
		long SubordConjItemNo = GetItemNoByItemStr(piOborDic,"����_����", "D_PART_OF_SPEECH");
		assert(SubordConjItemNo != -1);
		long CoordConjItemNo = GetItemNoByItemStr(piOborDic,"���_����", "D_PART_OF_SPEECH");
		assert(CoordConjItemNo != -1);
		long SimplePrepItemNo = GetItemNoByItemStr(piOborDic,"�����_�����", "D_PART_OF_SPEECH");
		assert(SimplePrepItemNo != -1);
		long CoordDisruptPrepItemNo = GetItemNoByItemStr(piOborDic,"���_������_����", "D_PART_OF_SPEECH");
		assert(CoordDisruptPrepItemNo != -1);
		long SubordDisruptPrepItemNo = GetItemNoByItemStr(piOborDic,"����_������_����", "D_PART_OF_SPEECH");
		assert(SubordDisruptPrepItemNo != -1);
		long PrepItemNo = GetItemNoByItemStr(piOborDic,"�����", "D_PART_OF_SPEECH");
		assert(PrepItemNo != -1);
		// ������ 
		long NominativItemNo = GetItemNoByItemStr(piOborDic,"�", "D_CASE");
		assert(NominativItemNo != -1);
		long GenitivItemNo = GetItemNoByItemStr(piOborDic,"�", "D_CASE");
		assert(GenitivItemNo != -1);
		long AccusativItemNo = GetItemNoByItemStr(piOborDic,"�", "D_CASE");
		assert(AccusativItemNo != -1);
		long DativItemNo = GetItemNoByItemStr(piOborDic,"�", "D_CASE");
		assert(DativItemNo != -1);
		long InstrumentalisItemNo = GetItemNoByItemStr(piOborDic,"�", "D_CASE");
		assert(InstrumentalisItemNo != -1);
		long VocativItemNo = GetItemNoByItemStr(piOborDic,"�", "D_CASE");
		assert(VocativItemNo != -1);
		long NominativPluralItemNo = GetItemNoByItemStr(piOborDic,"�_��", "D_CASE");

		
		

		// ��������� ��
		long InfinitiveItemNo = GetItemNoByItemStr(piOborDic,"���", "D_VP_SPECIF");
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
						piOborDic->CortegeToStr(*piOborDic->GetCortege(i), OutBuffer);
						oborot.m_GrammarFeature = OutBuffer;
						Trim(oborot.m_GrammarFeature);
						if (oborot.m_GrammarFeature == "���")
							oborot.m_GrammarFeature = "�";

					};
					//nim : ������� ������, ��������� GF=�����
					if ( ParenthesItemNo == Item0)
					{
						oborot.AddPartOfSpeech(INP);
						oborot.m_bParenthesis = true;

					};

					//nim : ������� ("����������"), ��������� GF=���
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

