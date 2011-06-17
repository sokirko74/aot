

#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"



TreeVariantValueCoefs::TreeVariantValueCoefs()
{
		ValencyDisorderCoef = 10;
        DirectDisagreeCoef = 10;
	    ConnectedComponentsCountCoef = 100;
	    SemFetDisagreeCoef = 20;
	    ProjectnessCoefCoef = 80;
	    LexFetAgreeCountCoef = 20;
	    AgreeWithSyntaxTopCoefCoef = 20;
	    TopAgreeWithSyntaxCriteriaCoef = -10;
	    RelationsLengthCoef =2;
	    MNAViolationsCountCoef = 50;
	    SemRelPOSViolationsCountCoef = 5;
	    OnlyCommaBetweenViolationsCountCoef  =20;
	    SubjectPredicateViolationsCountCoef = 50;
        CommaBetweenBrothersExceptMNAViolationsCountCoef = 100;
	    OptionalValencyPenaltyCoef = 30;
		CopulViolationsCountCoef = 100;
		InstrAgentRelsCountCoef = 5;
		LexFunctsCountCoef = -30;
	    GramRestrViolationsCountCoef = 100;
		CollocsCountCoef = -15;
		OborotAdverbialCountCoef = -10;
		MaxRelationLeapsCount = 2;
		ObligatoryValencyViolationCoef = 10;
		ColloquialInterpsCoef = 10;
		CorporaGleicheCoef = -2;
		PassiveValencyPenaltyCoef = 1;
		SemFetAgreeMNACoef = -30;
		WordWeightCoef = -1;
		MiscSemAgreeCoef = -10;
		PrichastieCoef = 10;
};



bool TreeVariantValueCoefs::ReadOneCoef (const char * s) 
{
	if (sscanf(s, "ValencyDisorderCoef %i", &ValencyDisorderCoef) == 1)  return true; 
	if (sscanf(s, "DirectDisagreeCoef %i", &DirectDisagreeCoef) == 1)  return true; 
	if (sscanf(s, "ConnectedComponentsCountCoef %i", &ConnectedComponentsCountCoef) == 1)  return true; 
	if (sscanf(s, "SemFetDisagreeCoef %i", &SemFetDisagreeCoef) == 1)  return true; 
	if (sscanf(s, "ProjectnessCoefCoef%i", &ProjectnessCoefCoef) == 1)  return true; 
	if (sscanf(s, "LexFetAgreeCountCoef %i", &LexFetAgreeCountCoef) == 1)  return true; 
	if (sscanf(s, "AgreeWithSyntaxTopCoefCoef %i", &AgreeWithSyntaxTopCoefCoef) == 1)  return true; 
	if (sscanf(s, "TopAgreeWithSyntaxCriteriaCoef %i", &TopAgreeWithSyntaxCriteriaCoef) == 1)  return true; 
	if (sscanf(s, "RelationsLengthCoef %i", &RelationsLengthCoef) == 1)  return true; 
	if (sscanf(s, "MNAViolationsCountCoef %i", &MNAViolationsCountCoef) == 1)  return true; 
	if (sscanf(s, "SemRelPOSViolationsCountCoef %i", &SemRelPOSViolationsCountCoef) == 1)  return true; 
	if (sscanf(s, "OnlyCommaBetweenViolationsCountCoef %i", &OnlyCommaBetweenViolationsCountCoef) == 1)  return true; 
	if (sscanf(s, "SubjectPredicateViolationsCountCoef %i", &SubjectPredicateViolationsCountCoef) == 1)  return true; 
	if (sscanf(s, "CommaBetweenBrothersExceptMNAViolationsCountCoef %i", &CommaBetweenBrothersExceptMNAViolationsCountCoef) == 1)  return true; 
	if (sscanf(s, "OptionalValencyPenaltyCoef %i", &OptionalValencyPenaltyCoef) == 1)  return true; 
	if (sscanf(s, "CopulViolationsCountCoef %i", &CopulViolationsCountCoef) == 1)  return true; 
	if (sscanf(s, "InstrAgentRelsCountCoef %i", &InstrAgentRelsCountCoef) == 1)  return true; 
	if (sscanf(s, "LexFunctsCountCoef %i", &LexFunctsCountCoef) == 1)  return true; 
	if (sscanf(s, "GramRestrViolationsCountCoef %i", &GramRestrViolationsCountCoef) == 1)  return true; 
	if (sscanf(s, "CollocsCountCoef %i", &CollocsCountCoef) == 1)  return true; 
	if (sscanf(s, "OborotAdverbialCountCoef %i", &OborotAdverbialCountCoef) == 1)  return true; 
	if (sscanf(s, "MaxRelationLeapsCount %i", &MaxRelationLeapsCount) == 1)  return true; 
	if (sscanf(s, "ObligatoryValencyViolationCoef %i", &ObligatoryValencyViolationCoef) == 1)  return true; 
	if (sscanf(s, "ColloquialInterpsCoef %i", &ColloquialInterpsCoef) == 1)  return true; ;		
	if (sscanf(s, "CorporaGleicheCoef %i", &CorporaGleicheCoef) == 1)  return true; ;		
	if (sscanf(s, "PassiveValencyPenaltyCoef %i", &PassiveValencyPenaltyCoef) == 1)  return true; ;		
	if (sscanf(s, "SemFetAgreeMNACoef %i", &SemFetAgreeMNACoef) == 1)  return true; ;		
	if (sscanf(s, "WordWeightCoef %i", &WordWeightCoef) == 1)  return true; ;		
	if (sscanf(s, "MiscSemAgreeCoef %i", &MiscSemAgreeCoef) == 1)  return true; ;		
	if (sscanf(s, "PrichastieCoef %i", &PrichastieCoef) == 1)  return true; ;		
	
	
	
	return false;
};


TreeVariantValue& TreeVariantValue :: operator = (const TreeVariantValue& X)
{  
	Panic = X.Panic;
	ValencyDisorder = X.ValencyDisorder;
	ValencyMiss = X.ValencyMiss;
	DirectDisagree = X.DirectDisagree;
	ConnectedComponentsCount = X.ConnectedComponentsCount;
	SemFetDisagree = X.SemFetDisagree;
	ProjectnessCoef = X.ProjectnessCoef;
	LexFetAgreeCount =  X.LexFetAgreeCount;
	AgreeWithSyntaxTopCoef = X.AgreeWithSyntaxTopCoef;
	TopAgreeWithSyntaxCriteria = X.TopAgreeWithSyntaxCriteria;
	RelationsLength = X.RelationsLength;
	MNAViolationsCount = X.MNAViolationsCount;
	SemRelPOSViolationsCount = X.SemRelPOSViolationsCount;
	OnlyCommaBetweenViolationsCount = X.OnlyCommaBetweenViolationsCount;
	SubjectPredicateViolationsCount = X.SubjectPredicateViolationsCount;
	CommaBetweenBrothersExceptMNAViolationsCount = X.CommaBetweenBrothersExceptMNAViolationsCount;
	OptionalValencyCount = X.OptionalValencyCount;
	CopulViolationsCount = X.CopulViolationsCount;
	InstrAgentRelsCount = X.InstrAgentRelsCount;
  	LexFunctsCount = X.LexFunctsCount;
    GramRestrViolationsCount = X.GramRestrViolationsCount;
	CollocsCount = X.CollocsCount;
	SAMNodeViolation = X.SAMNodeViolation;
	OborotAdverbialCount = X.OborotAdverbialCount;
	ObligatoryValencyViolationCount = X.ObligatoryValencyViolationCount;
	ColloquialInterpsCount = X.ColloquialInterpsCount;
	CorporaGleicheCount = X.CorporaGleicheCount;
	PassiveValencyCount = X.PassiveValencyCount;
	SemFetAgreeMNACount = X.SemFetAgreeMNACount;
	WordWeightCount = X.WordWeightCount;
	MiscSemAgreeCount = X.MiscSemAgreeCount;
	PrichastieWithoutActantsCount = X.PrichastieWithoutActantsCount;
	Coefs = X.Coefs;
	
	return *this;
};



void TreeVariantValue :: Init() 
{  
	Coefs =0;
	ValencyDisorder = 0;
	ValencyMiss = 0;
	DirectDisagree = 0;
	SemFetDisagree = 0;
	ConnectedComponentsCount = 0;
	AgreeWithSyntaxTopCoef = 0;
	TopAgreeWithSyntaxCriteria = 0;
	RelationsLength = 0;
	MNAViolationsCount = 0;
	SemRelPOSViolationsCount = 0;
	Panic = false;
	OnlyCommaBetweenViolationsCount = 0;
	SubjectPredicateViolationsCount = 0;
	CommaBetweenBrothersExceptMNAViolationsCount = 0;
	OptionalValencyCount = 0;
	LexFetAgreeCount = 0;
	ProjectnessCoef = 0;
	CopulViolationsCount = 0;
	InstrAgentRelsCount = 0;
	LexFunctsCount = 0;
	GramRestrViolationsCount = 0;
	CollocsCount = 0;
	SAMNodeViolation = false;
	OborotAdverbialCount = 0;
	ObligatoryValencyViolationCount = 0;
	ColloquialInterpsCount = 0;
	CorporaGleicheCount = 0;
	PassiveValencyCount = 0;
	SemFetAgreeMNACount = 0;
	WordWeightCount = 0;
	MiscSemAgreeCount = 0;
	PrichastieWithoutActantsCount = 0;
};

TreeVariantValue::TreeVariantValue(const TreeVariantValueCoefs* _Coefs) 
{
	Init();
	Coefs =_Coefs;

};
TreeVariantValue::TreeVariantValue()   
{
	Init();
};

bool TreeVariantValue::operator == ( const TreeVariantValue& X ) const
{
      return  GetTreeWeight() == X.GetTreeWeight();
};

bool TreeVariantValue::operator < ( const TreeVariantValue& X ) const
{
	return  GetTreeWeight() < X.GetTreeWeight();
};





long TreeVariantValue::GetWordWeight()  const
{
	return WordWeightCount*Coefs->WordWeightCoef/100;
}

long TreeVariantValue::GetWeight1(bool  CheckConnect)  const
{ 
	assert (Coefs);
	return   ValencyDisorder*Coefs->ValencyDisorderCoef 
		  +  DirectDisagree*Coefs->DirectDisagreeCoef 
		  +  ProjectnessCoef*Coefs->ProjectnessCoefCoef 
		  +  LexFetAgreeCount*Coefs->LexFetAgreeCountCoef 
		  +  MNAViolationsCount*Coefs->MNAViolationsCountCoef
		  +  SemRelPOSViolationsCount*Coefs->SemRelPOSViolationsCountCoef 
		  +  OnlyCommaBetweenViolationsCount*Coefs->OnlyCommaBetweenViolationsCountCoef
		  +  SubjectPredicateViolationsCount*Coefs->SubjectPredicateViolationsCountCoef
		  +  CommaBetweenBrothersExceptMNAViolationsCount*Coefs->CommaBetweenBrothersExceptMNAViolationsCountCoef
		  +  (Panic ? 1000 : 0)
		  +  ConnectedComponentsCount* ((CheckConnect) ? Coefs->ConnectedComponentsCountCoef  : 0)
		  +  CopulViolationsCount*Coefs->CopulViolationsCountCoef
          +  GramRestrViolationsCount*Coefs->GramRestrViolationsCountCoef
	      +  CollocsCount*Coefs->CollocsCountCoef
		  +  OborotAdverbialCount*Coefs->OborotAdverbialCountCoef
		  +  ColloquialInterpsCount*Coefs->ColloquialInterpsCoef
		  +  SAMNodeViolation*1000
		  +  CorporaGleicheCount*Coefs->CorporaGleicheCoef
		  +  PassiveValencyCount*Coefs->PassiveValencyPenaltyCoef
		  +  SemFetAgreeMNACount*Coefs->SemFetAgreeMNACoef
		  +  GetWordWeight()
		  +  PrichastieWithoutActantsCount*Coefs->PrichastieCoef
		  +  RelationsLength*Coefs->RelationsLengthCoef/100
		  +  MiscSemAgreeCount*Coefs->MiscSemAgreeCoef;
};


long TreeVariantValue::GetWeight2()  const
{ 
	assert (Coefs);
	return 
			 SemFetDisagree*Coefs->SemFetDisagreeCoef/100
		  
		  +  OptionalValencyCount*Coefs->OptionalValencyPenaltyCoef
  	      +  LexFunctsCount*Coefs->LexFunctsCountCoef
		  +  InstrAgentRelsCount*Coefs->InstrAgentRelsCountCoef/100
		  +  ObligatoryValencyViolationCount*Coefs->ObligatoryValencyViolationCoef;
    

};

long TreeVariantValue::GetWeight3(bool  CheckConnect)  const
{ 
	assert (Coefs);
	return
		     AgreeWithSyntaxTopCoef*Coefs->AgreeWithSyntaxTopCoefCoef
		  +  TopAgreeWithSyntaxCriteria*Coefs->TopAgreeWithSyntaxCriteriaCoef;
};

long TreeVariantValue::GetTreeWeight()  const
{ 
	assert (Coefs);
	return   GetWeight1()
          +  GetWeight2()
		  +  GetWeight3();

};

string TreeVariantValue::GetStr()  const
{
    string Q = Format ("ValencyDisorder=%i;\nDirectDisagree=%i;\nSemFetDisagree=%i;\nConnectedComponents=%i;\nProjectnessCoef=%i;\nLexFetAgree=%i;\nAgreeWithSyntaxTop=%i;\nTopAgreeWithSyntaxCriteria=%i\nSemRelPOSViolations=%i;\nMNAViolations=%i;\nOnlyCommaBetweenViolations=%i;\nRelationsLength=%i;\nSubjectPredicateViolations=%i;\nOptionalValencyCount=%i;\nCopulViolations=%i;\nInstrAgentRels=%i;\nLexFuncts=%i;\nGramRestrViolations=%i;\nCollocs=%i\n OborotAdverbial=%i\nObligatoryValencyViolation=%i\nColloquialInterps=%i\nCorporaGleicheCount=%i\nPassiveValencyCount=%i\nSemFetAgreeMNACount=%i WordWeightCount=%i MiscSemAgreeCount=%i",
		ValencyDisorder,  DirectDisagree,   SemFetDisagree,     ConnectedComponentsCount,   ProjectnessCoef,   LexFetAgreeCount,   AgreeWithSyntaxTopCoef, TopAgreeWithSyntaxCriteria, SemRelPOSViolationsCount, MNAViolationsCount, OnlyCommaBetweenViolationsCount, RelationsLength, SubjectPredicateViolationsCount,	OptionalValencyCount, CopulViolationsCount, InstrAgentRelsCount, LexFunctsCount, GramRestrViolationsCount, CollocsCount, OborotAdverbialCount, ObligatoryValencyViolationCount, ColloquialInterpsCount, CorporaGleicheCount,	PassiveValencyCount,         SemFetAgreeMNACount,	WordWeightCount, MiscSemAgreeCount);
	return Q;
};


string TreeVariantValue::GetStrOfNotNull()  const
{
	string Result;
	if (ValencyDisorder >0)  Result += Format("ValencyDisorder=%i\n",ValencyDisorder);
	if (DirectDisagree >0)  Result += Format("DirectDisagree=%i\n",DirectDisagree);
	if (SemFetDisagree >0)  Result += Format("SemFetDisagree=%i\n",SemFetDisagree);

	if (ConnectedComponentsCount >0)  Result += Format("ConnectedComponentsCount=%i\n",ConnectedComponentsCount);
	if (ProjectnessCoef >0)  Result += Format("ProjectnessCoef=%i\n",ProjectnessCoef);
	if (LexFetAgreeCount >0)  Result += Format("LexFetAgreeCount=%i\n",LexFetAgreeCount);


	if (AgreeWithSyntaxTopCoef >0)  Result += Format("AgreeWithSyntaxTopCoef=%i\n",AgreeWithSyntaxTopCoef);
	if (TopAgreeWithSyntaxCriteria >0)  Result += Format("TopAgreeWithSyntaxCriteria=%i\n",TopAgreeWithSyntaxCriteria);
	if (SemRelPOSViolationsCount>0)  Result += Format("SemRelPOSViolationsCount=%i\n",SemRelPOSViolationsCount);


	if (MNAViolationsCount>0)  Result += Format("MNAViolationsCount=%i\n",MNAViolationsCount);
	if (OnlyCommaBetweenViolationsCount>0)  Result += Format("OnlyCommaBetweenViolationsCount=%i\n",OnlyCommaBetweenViolationsCount);
	if (RelationsLength>0)  Result += Format("RelationsLength=%i\n",RelationsLength);

	if (SubjectPredicateViolationsCount>0)  Result += Format("SubjectPredicateViolationsCount=%i\n",SubjectPredicateViolationsCount);
	if (OptionalValencyCount>0)  Result += Format("OptionalValencyCount=%i\n",OptionalValencyCount);
	if (CopulViolationsCount>0)  Result += Format("CopulViolationsCount=%i\n",CopulViolationsCount);

	if (InstrAgentRelsCount>0)  Result += Format("InstrAgentRelsCount=%i\n",InstrAgentRelsCount);
	if (LexFunctsCount>0)  Result += Format("LexFunctsCount=%i\n",LexFunctsCount);
	if (GramRestrViolationsCount>0)  Result += Format("GramRestrViolationsCount=%i\n",GramRestrViolationsCount);


	if (CollocsCount>0)  Result += Format("CollocsCount=%i\n",CollocsCount);
	if (OborotAdverbialCount>0)  Result += Format("OborotAdverbialCount=%i\n",OborotAdverbialCount);
	if (ObligatoryValencyViolationCount>0)  Result += Format("ObligatoryValencyViolationCount=%i\n",ObligatoryValencyViolationCount);

	if (ColloquialInterpsCount>0)  Result += Format("ColloquialInterpsCount=%i\n",ColloquialInterpsCount);
	if (CorporaGleicheCount>0)  Result += Format("CorporaGleicheCount=%i\n",CorporaGleicheCount);
	if (PassiveValencyCount>0)  Result += Format("PassiveValencyCount=%i\n",PassiveValencyCount);

	if (SemFetAgreeMNACount>0)  Result += Format("SemFetAgreeMNACount=%i\n",SemFetAgreeMNACount);
	if (WordWeightCount>0)  Result += Format("WordWeightCount=%i\n",WordWeightCount);
	if (CommaBetweenBrothersExceptMNAViolationsCount>0)  Result += Format("CommaBetweenBrothersExceptMNAViolationsCount=%i\n",CommaBetweenBrothersExceptMNAViolationsCount);
	
	if (MiscSemAgreeCount>0)  Result += Format("MiscSemAgreeCount=%i\n",MiscSemAgreeCount);
	
	if (PrichastieWithoutActantsCount>0)  Result += Format("PrichastieWithoutActantsCount=%i\n",PrichastieWithoutActantsCount);
    
	return Result;
};


string TreeVariantValue::GetDifference(const TreeVariantValue& X)  const
{
	string Result;
	if (X.ValencyDisorder != ValencyDisorder)  Result += Format("ValencyDisorder=%i (%i)\n",ValencyDisorder, X.ValencyDisorder);
	if (X.DirectDisagree != DirectDisagree)  Result += Format("DirectDisagree=%i (%i)\n",DirectDisagree, X.DirectDisagree);
	if (X.SemFetDisagree != SemFetDisagree)  Result += Format("SemFetDisagree=%i (%i)\n",SemFetDisagree, X.SemFetDisagree);

	if (X.ConnectedComponentsCount != ConnectedComponentsCount)  Result += Format("ConnectedComponentsCount=%i (%i)\n",ConnectedComponentsCount, X.ConnectedComponentsCount);
	if (X.ProjectnessCoef != ProjectnessCoef)  Result += Format("ProjectnessCoef=%i (%i)\n",ProjectnessCoef, X.ProjectnessCoef);
	if (X.LexFetAgreeCount != LexFetAgreeCount)  Result += Format("LexFetAgreeCount=%i (%i)\n",LexFetAgreeCount, X.LexFetAgreeCount);


	if (X.AgreeWithSyntaxTopCoef != X.AgreeWithSyntaxTopCoef)  Result += Format("AgreeWithSyntaxTopCoef=%i (%i)\n",AgreeWithSyntaxTopCoef, X.AgreeWithSyntaxTopCoef);
	if (X.TopAgreeWithSyntaxCriteria != TopAgreeWithSyntaxCriteria)  Result += Format("TopAgreeWithSyntaxCriteria=%i (%i)\n",TopAgreeWithSyntaxCriteria, X.TopAgreeWithSyntaxCriteria);
	if (X.SemRelPOSViolationsCount != X.SemRelPOSViolationsCount)  Result += Format("SemRelPOSViolationsCount=%i (%i)\n",SemRelPOSViolationsCount, X.SemRelPOSViolationsCount);


	if (X.MNAViolationsCount != X.MNAViolationsCount)  Result += Format("MNAViolationsCount=%i (%i)\n",MNAViolationsCount, X.MNAViolationsCount);
	if (X.OnlyCommaBetweenViolationsCount != X.OnlyCommaBetweenViolationsCount)  Result += Format("OnlyCommaBetweenViolationsCount=%i (%i)\n",OnlyCommaBetweenViolationsCount, X.OnlyCommaBetweenViolationsCount);
	if (X.RelationsLength != RelationsLength)  Result += Format("RelationsLength=%i (%i)\n",RelationsLength, X.RelationsLength);

	if (X.SubjectPredicateViolationsCount != SubjectPredicateViolationsCount)  Result += Format("SubjectPredicateViolationsCount=%i (%i)\n",SubjectPredicateViolationsCount, X.SubjectPredicateViolationsCount);
	if (X.OptionalValencyCount != OptionalValencyCount)  Result += Format("OptionalValencyCount=%i (%i)\n",OptionalValencyCount, X.OptionalValencyCount);
	if (X.CopulViolationsCount != CopulViolationsCount)  Result += Format("CopulViolationsCount=%i (%i)\n",CopulViolationsCount, X.CopulViolationsCount);

	if (X.InstrAgentRelsCount != InstrAgentRelsCount)  Result += Format("InstrAgentRelsCount=%i (%i)\n",InstrAgentRelsCount, X.InstrAgentRelsCount);
	if (X.LexFunctsCount != LexFunctsCount)  Result += Format("LexFunctsCount=%i (%i)\n",LexFunctsCount, X.LexFunctsCount);
	if (X.GramRestrViolationsCount != X.GramRestrViolationsCount)  Result += Format("GramRestrViolationsCount=%i (%i)\n",GramRestrViolationsCount, X.GramRestrViolationsCount);


	if (X.CollocsCount!=CollocsCount)  Result += Format("CollocsCount=%i (%i)\n",CollocsCount, X.CollocsCount);
	if (X.OborotAdverbialCount!=OborotAdverbialCount)  Result += Format("OborotAdverbialCount=%i (%i)\n",OborotAdverbialCount, X.OborotAdverbialCount);
	if (X.ObligatoryValencyViolationCount!=X.ObligatoryValencyViolationCount)  Result += Format("ObligatoryValencyViolationCount=%i (%i)\n",ObligatoryValencyViolationCount, X.ObligatoryValencyViolationCount);

	if (X.ColloquialInterpsCount!=ColloquialInterpsCount)  Result += Format("ColloquialInterpsCount=%i (%i)\n",ColloquialInterpsCount, X.ColloquialInterpsCount);
	if (X.CorporaGleicheCount!=CorporaGleicheCount)  Result += Format("CorporaGleicheCount=%i (%i)\n",CorporaGleicheCount, X.CorporaGleicheCount);
	if (X.PassiveValencyCount!=PassiveValencyCount)  Result += Format("PassiveValencyCount=%i (%i)\n",PassiveValencyCount, X.PassiveValencyCount);

	if (X.SemFetAgreeMNACount!=SemFetAgreeMNACount)  Result += Format("SemFetAgreeMNACount=%i (%i)\n",SemFetAgreeMNACount, X.SemFetAgreeMNACount);
	if (X.WordWeightCount!=WordWeightCount)  Result += Format("WordWeightCount=%i (%i)\n",WordWeightCount, X.WordWeightCount);
	if (X.CommaBetweenBrothersExceptMNAViolationsCount!=X.CommaBetweenBrothersExceptMNAViolationsCount)  Result += Format("CommaBetweenBrothersExceptMNAViolationsCount=%i (%i)\n",CommaBetweenBrothersExceptMNAViolationsCount, X.CommaBetweenBrothersExceptMNAViolationsCount);
	
	if (X.MiscSemAgreeCount!=MiscSemAgreeCount)  Result += Format("MiscSemAgreeCount=%i (%i)\n",MiscSemAgreeCount, X.MiscSemAgreeCount);
	
	if (X.PrichastieWithoutActantsCount!=PrichastieWithoutActantsCount)  Result += Format("PrichastieWithoutActantsCount=%i (%i)\n",PrichastieWithoutActantsCount, X.PrichastieWithoutActantsCount);
    
	return Result;
};




//========================================================
//========================================================
const int NounSemRelCount = 11;
string NounSemRel[NounSemRelCount] = { "SUB","OBJ", "ADR", "PACIEN","AUTHOR", "AGENT", "SRC-PNT", "C-AGENT", "TRG-PNT", "LOK","MEANS" };

void CRusSemStructure::MarkSemRelPOSViolations()
{

  for (size_t i = 0;  i < m_Relations.size(); i++)
  {
		  CRusSemNode& N = m_Nodes[m_Relations[i].m_TargetNodeNo];
		  if (   !HasRichPOS (N,NOUN)
			  && !HasRichPOS (N,PRONOUN)  // "я пришел"
			  && !(    (N.m_MainWordNo != -1 ) // "каждый должен это сделать"
			        && N.m_Words[N.m_MainWordNo].IsRusSubstPronounP()
			      )
				&& !(     HasRichPOS (N,PRONOUN_P)  // "на наш взгляд" (SUB - отношение)			         
				&& HasRichPOS (m_Relations[i].m_SourceNodeNo, NOUN)
			      )
			 )
		  {
			  for (size_t k=0; k < NounSemRelCount; k++)
				if (m_Relations[i].m_Valency.m_RelationStr == NounSemRel[k])
				{	
				  m_Relations[i].m_bSemRelPOS = false;
				  break;
				};
		  };
  };

};

long CRusSemStructure::GetSemRelPOSViolationsCount(long Tag)
{
  size_t Result =0;
  long Count = m_Relations.size();
  for (size_t i = 0;  i < Count; i++)
	 if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
      if (m_Relations[i].m_bRelUse)
		  if (!m_Relations[i].m_bSemRelPOS)
			  Result++;

  return Result;
};



//========================================================
//========================================================

long CRusSemStructure::GetOnlyCommaBetweenViolationsCount (long Tag)
{
  size_t Result =0;

  for (size_t i = 0;  i < m_Relations.size(); i++)
   if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
      if (m_Relations[i].m_bRelUse)
	   if (m_Relations[i].m_CannotHaveOnlyCommaBetween)
	    if ( IsWordContainerRel(i) )
		  if (m_Nodes[m_Relations[i].m_TargetNodeNo].m_NodeType == MNA)
		  {
			  // если стрелка идет в однородный ряд, то 
			  // пусть Х=GetRelation(i)->m_SourceNodeNo, а M1, ..., Mn - члены однородного ряда,
			  // упорядоченные по тексту. Если X < M1, то проверим отрицание HaveOnlyPunctBetween
			  // между X и M1, если Х > Мn, то - между X и Mn. Таким образом мы считаем, что
			  // однородный ряд, как бы, является одной группой, которая начинается с М1 и заканчивается Мn.
			  // это нужно для того, чтобы член однородного ряда не мог управлять всем
			  // однородным рядом.
			  
			  long MnaNodeNo = m_Relations[i].m_TargetNodeNo;
			  const CRusSemClause& C = m_Clauses[m_Nodes[MnaNodeNo].m_ClauseNo];
			  long StartNodeNo = -1;
			  long EndNodeNo = -1;
			  long NodesCount  = m_Nodes.size();
			  for (long k=0; k < NodesCount; k++)
			    if (HasTag(k, Tag))
				{
					long Count  = m_Nodes[k].m_InRels.size();
					for (long j =0; j < Count; j++)
						if (   (m_Relations[m_Nodes[k].m_InRels[j]].m_SourceNodeNo == MnaNodeNo)
						    && m_Relations[m_Nodes[k].m_InRels[j]].m_bRelUse   
						   )
						{
							if (StartNodeNo == -1) StartNodeNo = k;
							EndNodeNo = k;
						};
				};
/*если у MUA нет ни одного актанта?*/			   if(StartNodeNo == -1) continue;;
				  

			  if (m_Relations[i].m_SourceNodeNo < m_Nodes[StartNodeNo].GetMinWordNo())
			  {
				 if ( C.GetGramMatrix(m_Relations[i].m_SourceNodeNo, StartNodeNo) & HaveOnlyCommaBetween  )
					 Result++;
			  }
			  else
			   if (m_Relations[i].m_SourceNodeNo > EndNodeNo)
				   if (C.GetGramMatrix(m_Relations[i].m_SourceNodeNo, EndNodeNo) & HaveOnlyCommaBetween)
					 Result++;
		  }
          else
		  {
			 const CRusSemClause& C = m_Clauses[m_Nodes[m_Relations[i].m_TargetNodeNo].m_ClauseNo];

			if (C.GetGramMatrix(m_Relations[i].m_TargetNodeNo, m_Relations[i].m_SourceNodeNo) & HaveOnlyCommaBetween)
				  Result++;
		  };

  return Result; 
};

//========================================================
//========================================================
// проверка того, что найденное семантикой  подлежащее равно синтаксическому подлежащему 
bool CRusSemStructure::AgreeWithSyntaxTop (long Tag) const 
{
	long NodesCount = m_Nodes.size();
	size_t i = 0;
	for (;  i < NodesCount; i++)
		if (HasTag(i, Tag))
			break;
	if (i == m_Nodes.size()) return true;
	long ClauseNo = m_Nodes[i].m_ClauseNo;
	long SubjectNodeNo = (m_Clauses[ClauseNo].m_SubjectWordNo == -1) ? -1 : FindNodeByWordNo(m_Clauses[ClauseNo].m_SubjectWordNo, ClauseNo); 


	if (SubjectNodeNo != -1) 
		for (i = 0;  i < m_Nodes[SubjectNodeNo].m_InRels.size(); i++)
		{
			long  RelNo = m_Nodes[SubjectNodeNo].m_InRels[i];
			if (HasTag(m_Relations[RelNo].m_SourceNodeNo, Tag))
				if (m_Relations[RelNo].m_bRelUse)
					if (m_Relations[RelNo].m_SyntacticRelation != "подл") 
						return false;
		};

	return true;
};

//========================================================
//========================================================
/*
функция  берет узел Х. Если Х является MUA, пропускаем его.  Берем все  потомки Х. Если два потомка Х разделяет 
  только запятая, то функция увеличивает результирующий штраф.
  Например, по этому правилу будет дан один штраф предложению:
   "Мама, папу любит"
*/
long CRusSemStructure::GetCommaBetweenBrothersExceptMNAViolationsCount(long Tag) const
{
	long Result = 0; 

	for (size_t i=0; i< m_Nodes.size(); i++)
		if (   HasTag(i, Tag) 
			&& (m_Nodes[i].m_NodeType != MNA)
		 )
		{
			size_t ClauseNo = m_Nodes[i].m_ClauseNo;
			const CRusSemClause& C = m_Clauses[ClauseNo];
			const vector<long>& Rels = m_Nodes[i].m_OutRels;

			for (long  k=0; k+1 < Rels.size(); k++)
				if (m_Relations[Rels[k]].m_bRelUse)
				{
					long  NodeNo1 = m_Relations[Rels[k]].m_TargetNodeNo;
					if (!m_Nodes[NodeNo1].IsWordContainer()) continue;
					if (!IsInClause(NodeNo1,ClauseNo)) continue; // from BuildAnaphoricRels
					size_t j=k+1;
					for (; j < Rels.size(); j++)
						if (m_Relations[Rels[j]].m_bRelUse)
							break;
					if (j == Rels.size()) break;

					long  NodeNo2 = m_Relations[Rels[j]].m_TargetNodeNo;
					if (!m_Nodes[NodeNo2].IsWordContainer()) continue;
					if (!IsInClause(NodeNo2,ClauseNo)) continue; // from BuildAnaphoricRels

					if (C.GetGramMatrix(NodeNo1, NodeNo2) & HaveOnlyCommaBetween)
						Result++;
				}
		};

	return Result; 
};


//========================================================
//========================================================
/*
Здесь используется порядок на узлах
*/
bool CRusSemStructure::IsProjectedNew(long Tag) const
{
  long Count = m_Relations.size();
  // здесь неважно направление,  поэтому n*n/2
  for (long i=0; i<Count; i++)
   for (long k=i+1; k<Count; k++)
    if (    m_Relations[i].m_bRelUse
         && m_Relations[k].m_bRelUse
		 && IsWordContainerRel(i)
		 && IsWordContainerRel(k)
	   )
   {
		long X1 = m_Relations[i].m_SourceNodeNo;
		long X2 = m_Relations[i].m_TargetNodeNo;
		long Y1 = m_Relations[k].m_SourceNodeNo;
		long Y2 = m_Relations[k].m_TargetNodeNo;

		// =================================================================
		// проверка условия слабой проективности: все узлы, которые лежат под некоторой 
		// стрелкой, должны быть подчинены главному узлу этой стрелки, то есть
		// запрещено следующее X1<X2<X3 и (X2->X3, X3->X1). Здесь  вершина Х2 не подчинена Х3,
		// а это должно быть
		// инфинитив в качестве промежуточной вершины не проверяется, например:
		// "на него стоит настроиться"
		// "с ним нелегко иметь дело"

		// 
		// сравнительный оборот не будем проверять на проективность
		// например: "в 2000 году  мы построим больше школ, чем в прошлом году

		// InterMediaRel - отношение, в выходящий узел которого взодит другое отношение
		long InterMediaRel = (X1 == Y2) ?  i : k ;
		if ( !HasRichPOS (m_Relations[InterMediaRel].m_SourceNodeNo, INFINITIVE) )
		if ( m_Nodes[m_Relations[InterMediaRel].m_SourceNodeNo].m_MNAType != CHEMOborot)
		if   (   (    (X1 == Y2)
			       && (X2 < Y1)
				   && (Y1 < X1) 
				 )
			  || (    (X1 == Y2)
			       && (X1 < Y1)
				   && (Y1 < X2) 
				 )
			  || (    (Y1 == X2)
			       && (Y2 < X1)
				   && (X1 < Y1) 
				 )
			  || (    (Y1 == X2)
			       && (Y1 < X1)
				   && (X1 < Y2) 
				 )
			 )
			 return false;


		// ==========================================================
		// проверка сильной проективности (стрелки не должны пересекаться)

		if (X1 > X2) swap (X1, X2);
		if (Y1 > Y2) swap (Y1, Y2);
		if (X1 > Y1)
		{
			swap (X1, Y1);
			swap (X2, Y2);
		};
		if  ((X1 < Y1) && (Y1 < X2) && (X2 < Y2))
		{
		  // приходится обрабатывать отдельно конструкцию с инфинитивом типа:
		  // "это собирались сделать враги"
		  // будем считать, что следующий if - достаточнре условие для обнаружения этого случая	
			if (   HasRichPOS (X2,INFINITIVE) 
				&& (Y1+1 ==  X2)
				&& HasRichPOS (Y1,VERB) 
			   )
			continue;

			return false;
		};
	}
  return true;

};

//========================================================
//========================================================
//  число валентностей у Copul должно быть равно 0 или двум (П_АКТ, S-ACT).// Если у Copul нет валетностей, то он  будет удален  на этапе Идеализации.
// к копулам приравниваются словарные статьи, у которых в SF стоит Copul, но
// не стоит CAUS и NEG. (SF(выращивать) = CAUS,COpul - это не настоящий копул)

long CRusSemStructure::GetCopulViolationsCount (long Tag) const
{
	long Result =0;
	long Count = m_Nodes.size();
	for (size_t i = 0;  i < Count; i++)
		if (HasTag(i, Tag))
			if (   (m_Nodes[i].m_NodeType == Copul)
				|| HasSemFetPro(m_Nodes[i], "Copul")
				)
			{
				const vector<long>& Rels = m_Nodes[i].m_OutRels;
				long CopulRelsCount = 0;

				for (long k=0; k <  Rels.size(); k++)
					if (m_Relations[Rels[k]].m_bRelUse)
						if (  (m_Relations[Rels[k]].m_Valency.m_RelationStr == "F-ACT")||(m_Relations[Rels[k]].m_Valency.m_RelationStr == "S-ACT")				  )
							CopulRelsCount++;

				if ((CopulRelsCount != 0) && (CopulRelsCount != 2)) Result++;
			};
	return Result;
};

//========================================================
//========================================================
/*
 считает число узлов-оборотов, которым приписано GF=ОБСТ_ГР  
*/
long CRusSemStructure::GetOborAdverbialCount (long Tag) const
{
	long Result =0;

	for (size_t i = 0;  i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
			if (m_Nodes[i].GetType() == OborRoss)
				if (m_pData->Oborottos[m_Nodes[i].GetUnitNo()].m_bRusOborAdverbial)
					Result++;

	return Result;
};



//========================================================
//========================================================

// будем считать, что у слова моежт быть только 10 обязательных валентностей
struct CObligatoryVal{
	BYTE ValsCount;
	char ValsNo[10]; //must be unsigned
	/*
	  Если  первая валентнсть из любого несовместного 
	  множества заполнена,  то все остальные первые валентности
	  уже не считаются обязательными.
	  Например, "я заключил договор" (заключить 1)
	  Слот FirstValencyWasFilled равен истине, если
	  у узла хотя бы одна первая валентнсть заполнена.	
	*/
	bool FirstValencyWasFilled;
	CObligatoryVal ()
	{
		ValsCount = 0;
		FirstValencyWasFilled = false;
	};
};
/*
 слот FirstValencyWasFilled решает только частный случай,
 на самом деле, здесь нужно программировать по-другому.
 Нужно искать все несовместные пары валентностей, которые 
 обе являются обязательными. Тогда если заполнена одна из них 
 (а обе никак не могут быть заполнены), тогда  то, что вторая валенетность
 не заполнена, не надо считать за ошибку.
 Если обе несовместные обязательные валентности не заполнены, тогда
 это нужно считать за одну ошибку.
 Если же из двух несовместных валентностей  только одна обязательна, 
 тогда незаполненность обязательной валентности всегда ошибка.
 Встает вопрос, что делать, когда у тебя много пар несовместных валентностей.
 Как подсчитать количество незаполненных обязательных валентностей?
*/

// считает число узлов, у которых не заполнена первая валентность (или валентность,
// которая входит в поле ОБЯЗ_ВАЛ)
// рассматриваются только глаголы, предикативы, краткие формы (для всех остальных
// правило первой валентности не проверяется)
long CRusSemStructure::GetObligatoryValencyViolation (long Tag)
{
	long NodesCount = m_Nodes.size();
	vector<CObligatoryVal> X(NodesCount);

	long StartNodeOfTag = -1;
	for (long i=0; i < NodesCount; i++)
		if ( HasTag(i, Tag))
		{
			if (StartNodeOfTag == -1) StartNodeOfTag = i;
		};

	if (StartNodeOfTag == -1)	  return  0;
	for (long i=StartNodeOfTag; i < NodesCount; i++)
		if ( HasTag(i, Tag))
			for (long j=0; j < m_Nodes[i].m_Vals.size(); j++)
				if (m_Nodes[i].m_Vals[j].m_bObligatory)
					X[i-StartNodeOfTag].ValsNo[X[i-StartNodeOfTag].ValsCount++] = j;


	long RelsCount =  m_Relations.size();
	/*
	будем рассматривать только активные и уникальные отношения,
	а то получается, что во фразе "сколько времени это у тебя займет "
	"это" может заполнять 1 и 2 валентности глагола "займет". Первая валнтеность - обязательная.
	В гипотетическом графе одновременно сосуществуют обе стрелки к "это": как 1 и как 2 валлентности.
	Если убрать условие об активности или уникальности в варианте, где "это" - 2 валентность
	мы будем считать, что первая вале=нтность заполнена.
	*/
	for (long i=0; i < RelsCount; i++)
		if (m_Relations[i].m_bRelUse || m_Relations[i].m_bUniqueRelation)
		{
			long NodeNo = m_Relations[i].m_SourceNodeNo;
			if(NodeNo>= StartNodeOfTag)
				for (int j=0; j < X[NodeNo-StartNodeOfTag].ValsCount; j++)
				{
					char&  ValNo = X[NodeNo-StartNodeOfTag].ValsNo[j];
					if (ValNo != -1) 
						if (m_Relations[i].m_Valency ==  m_Nodes[NodeNo].m_Vals[ValNo])
						{
							if (m_Nodes[NodeNo].m_Vals[ValNo].m_bObligatoryBecauseFirst)
								X[NodeNo-StartNodeOfTag].FirstValencyWasFilled = true;
							ValNo  = -1;

						};
				}


		};
	// поищем в LF, если в слово входит LF тита Oper1, тогда не нужно искать заполнение  
	// первой валентности, поскольку она заполняется с помощью присвязочного глагола
	RelsCount =  m_LexFuncts.size();
	for (long i=0; i < RelsCount; i++)
	{
		long NodeNo = m_LexFuncts[i].m_SourceNodeNo;
		if (IsOper1(m_LexFuncts[i].m_LexFunctName))
			if (m_Nodes[NodeNo].m_Vals.size() > 0)
				if(NodeNo>= StartNodeOfTag)
					for (int j=0; j < X[NodeNo-StartNodeOfTag].ValsCount; j++)
					{
						char&  ValNo = X[NodeNo-StartNodeOfTag].ValsNo[j];
						if (ValNo != -1) 
							if (m_Nodes[NodeNo].m_Vals[0] ==  m_Nodes[NodeNo].m_Vals[ValNo])
							{
								if (m_Nodes[NodeNo].m_Vals[ValNo].m_bObligatoryBecauseFirst)
									X[NodeNo-StartNodeOfTag].FirstValencyWasFilled = true;
								ValNo  = -1;
							};
					};
	};

	// поищем в дополнительных отношениях, которые, правда почти всегда пусты, но
	// при обработке  межклаузных связей могут заполниться
	RelsCount =  m_DopRelations.size();
	for (long i=0; i < RelsCount; i++)
	{
		long NodeNo =  m_DopRelations[i].m_SourceNodeNo;
		if(NodeNo>= StartNodeOfTag)
			for (int j=0; j < X[NodeNo-StartNodeOfTag].ValsCount; j++)
			{
				char&  ValNo = X[NodeNo-StartNodeOfTag].ValsNo[j];
				if (ValNo != -1) 
					if (m_DopRelations[i].m_Valency ==  m_Nodes[NodeNo].m_Vals[ValNo])
					{
						if (m_Nodes[NodeNo].m_Vals[ValNo].m_bObligatoryBecauseFirst)
							X[NodeNo-StartNodeOfTag].FirstValencyWasFilled = true;
						ValNo  = -1;

					};
			};
	};



	long Result = 0;
	for (long i=StartNodeOfTag; i < NodesCount; i++)
		for (int j=0; j < X[i-StartNodeOfTag].ValsCount; j++)
			if (X[i-StartNodeOfTag].ValsNo[j] != -1)
			{
				char&  ValNo = X[i-StartNodeOfTag].ValsNo[j];

				if (X[i-StartNodeOfTag].FirstValencyWasFilled)
					if (m_Nodes[i].m_Vals[ValNo].m_bObligatoryBecauseFirst)
						continue;
				Result++;
			};
	return Result;
};

//========================================================
//========================================================
/*
 выдает число пассивных валентностей. Пассивная валентность
 имеет меньший приоритет, чем активная.
  Можно, наверно, объявить, что все пассивные валентности 
  являются  факультативными, но по сути своей они, конечно, 
  не факультаивны, а синтаксически обязательны.
*/
long CRusSemStructure::GetPassiveValencyCount(long Tag) const 
{
	size_t Result =0;

	long Count  = m_Relations.size();

	for (size_t i = 0;  i < Count; i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
				if ( m_Relations[i].m_bReverseRel )
					Result++;

	return Result; 
};


//===========================

// проверка согласования SF  (кроме сравнительного оборота)
void CRusSemStructure::FindSemFetDisagree(long Tag)
{
	for (size_t i = 0; i<m_Relations.size(); i++)
		if (    HasTag(m_Relations[i].m_SourceNodeNo, Tag)
			&& HasTag(m_Relations[i].m_TargetNodeNo, Tag) 

			&& m_Relations[i].m_bRelUse
			) 
		{
			m_Relations[i].m_bSemFetAgree = false;
			// для сравнительного оборота нужно проводить специальную проверку (см. файл 
			// сравнительные конструкции), которая, на самом деле, равна стандартной,
			// поэтому  здесь считаем, что CHEMOborot - не является разновидностью MNA
			//if (  m_Nodes[m_Relations[i].m_SourceNodeNo].IsMNA_ButNotCHEM() ) continue;

			bool HasOneContradiction = false;
			bool NeighboursAreGleicheSemFet = true;
			long LastNeighbour = -1;
			long NodesCount = 0;
			long Rels[200];
			long RelsCount = 0;
			for (long k =0; k < m_Relations.size(); k++)
				if  (  (		(m_Nodes[m_Relations[i].m_TargetNodeNo].m_NodeType == MNA)
							&&	(m_Relations[k].m_SourceNodeNo == m_Relations[i].m_TargetNodeNo)
							&&	m_Relations[k].m_Valency.m_RelationStr.empty()
							&&	m_Relations[k].m_bRelUse
						)
						||	(		(m_Nodes[m_Relations[i].m_TargetNodeNo].m_NodeType != MNA)
								&&	(i==k)
							)
					)
				{
					Rels[RelsCount++] = k;
					if (LastNeighbour != -1) 
						NeighboursAreGleicheSemFet =  NeighboursAreGleicheSemFet 
						&& GleicheSemFet(m_Nodes[LastNeighbour].m_NodeSemFets, m_Nodes[m_Relations[k].m_TargetNodeNo].m_NodeSemFets, false);
					LastNeighbour = m_Relations[k].m_TargetNodeNo;
					NodesCount++;
					size_t HostUnitNo = m_Relations[k].m_Valency.m_UnitNo; 
					long TargetNodeNo = m_Relations[k].m_TargetNodeNo;

					// проверка временных валентностей (до учета m_bReverseRel)
					if  (      (m_Relations[i].m_Valency.m_RelationStr == "TIME" )		
							&&	IsTimeAdverbial(TargetNodeNo)
						)
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};

					if (m_Relations[i].m_bReverseRel) 
						TargetNodeNo = m_Relations[i].m_SourceNodeNo;
					size_t SlaveUnitNo = m_Nodes[TargetNodeNo].GetSemCorrectUnitNo();

					m_Relations[k].m_bSemFetAgree = false;

					if  (   (m_Relations[k].m_SyntacticRelation == "врем_группа")
						|| (m_Relations[k].m_Valency.m_RelationStr == "THESAME")		   )
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};


					/*
					считаем, что вводные обороты семантически согласованы 	 с любой ситуацией 
					*/
					if (m_Nodes[m_Relations[k].m_TargetNodeNo].m_bSaveIsParenthesis)
						if (HasSemFetPro(m_Nodes[m_Relations[i].m_SourceNodeNo], "SITUAT") )		{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};





					if (find(m_Relations[k].m_LexFets.begin(), m_Relations[k].m_LexFets.end(), "KEY") != m_Relations[k].m_LexFets.end() )	      
						if (m_Nodes[TargetNodeNo].m_SynGroupTypeStr == KEYB)
						{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};

					if (m_Nodes[TargetNodeNo].m_NodeType == Copul)
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};

					if (m_Nodes[TargetNodeNo].m_CollocId != -1)
						if (m_Nodes[TargetNodeNo].m_CollocId == m_Nodes[m_Relations[i].m_SourceNodeNo].m_CollocId)
						{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};




					if (m_Relations[i].m_Valency.m_RelationStr == "VALUE")	    
						if (m_Nodes[TargetNodeNo].m_SynGroupTypeStr == NUMERAL_NOUN)
						{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};

					/*Этот оператор введен для примера "сорок человек", где "сорок" 
					интерпретируется как "сорока"	 (птица)
					*/
					if (m_Relations[i].m_Valency.m_RelationStr == "QUANTIT")	    
						if (m_Relations[i].m_SyntacticRelation == "ЧИСЛ_СУЩ")
						{
							m_Relations[k].m_bSemFetAgree = true;
							continue;
						};



					// проверка местоимений
					if  (     HasRichPOS (TargetNodeNo,PRONOUN) 		   )
					{
						// местоимения, которые стоят в среднем роде, не должны 
						// заполнять одушевленную валентность
						if ( m_Nodes[TargetNodeNo].HasOneGrammem(rNeutrum) )
						{
							if	(		HasSemFetPro(m_Relations[i].m_SemFets, "ANIM")				   
								&& (m_Relations[i].m_SemFets.size() == 1)
								)
							{
								HasOneContradiction = true;
								continue;
							};
						};

						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};


					// проверка локативных валентностей
					if  (       IsLocSemRel(m_Relations[i].m_Valency.m_RelationStr ) 
						&& (    (m_Nodes[TargetNodeNo].GetType() == LocRoss)
						|| (m_Nodes[TargetNodeNo].m_ThesaurusId == LocThes)
						)
						) 
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};

					string LocSemRel[2] = {"SRC-PNT","TRG-PNT"};	  
					size_t j=0;
					for (; j < 2; j++)
						if  (		 (m_Relations[i].m_Valency.m_RelationStr == LocSemRel[j] ) 
								&& (m_Nodes[TargetNodeNo].GetType() == LocRoss)
								&&  HasItem(LocRoss, SlaveUnitNo, "SF", LocSemRel[j],"D_SEM_REL",0, 0)
							) 
							break;

					if ( j < 2)
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};


					if  (    (m_Relations[i].m_Valency.m_RelationStr == "IDENT" )  			     
							&& (m_Nodes[TargetNodeNo].m_MainWordNo != -1)
							&& (		(m_Nodes[TargetNodeNo].m_Words[m_Nodes[TargetNodeNo].m_MainWordNo].m_CharCase != LowLow)
									||	m_Nodes[TargetNodeNo].m_bQuoteMarks
									||	m_Nodes[TargetNodeNo].m_Words[m_Nodes[TargetNodeNo].m_MainWordNo].m_ILE
								)
						) 
					{
						m_Relations[k].m_bSemFetAgree = true;
						continue;
					};



					if  (     (HostUnitNo  == ErrUnitNo) 
						|| (SlaveUnitNo  == ErrUnitNo) )
					{
						HasOneContradiction = true;
						continue;
					};


					//  главная проверка по словарным статьям
					if (m_Relations[i].m_Valency.m_RossHolder != 0)
					{
						// когда прописаны LEX (здесь же "вышестоящие концепты"), но нет SFi, тогда будем считать, что
						// по SF не согласовалось, поскольку у проверка на LEX есть своя собственная процедура
						if (   !m_Relations[i].m_SemFets.empty() 
							|| m_Relations[i].m_LexFets.empty()
							)
						{
							m_Relations[k].m_bSemFetAgree = (    m_Relations[k].m_bRelUse  && GleicheSemFet (m_Relations[i].m_SemFets, m_Nodes[TargetNodeNo].m_NodeSemFets, true) );
						}
					};

					if (!m_Relations[k].m_bSemFetAgree)
						HasOneContradiction = true;
				};

			// будем считать, что узел MUA удовлетворяет SFi,	 
			// если все его члены удовлетворяют SFi или между собой согласованы попарно
			// ближайшие соседи.
			m_Relations[i].m_bSemFetAgree =    !HasOneContradiction 
				|| (     NeighboursAreGleicheSemFet 
				&& (NodesCount > 1)
				);

			for (long k=0; k < RelsCount; k++)
				if (Rels[k] != i)
					m_Relations[Rels[k]].m_bSemFetAgree =    ( NeighboursAreGleicheSemFet 
					&& (RelsCount > 1)
					);



		};


};


long CRusSemStructure::GetSemFetDisagreeCount(long Tag)
{

	FindSemFetDisagree(Tag);

	long Result = 0;

	for (size_t i = 0; i<m_Relations.size(); i++)
	{
	 if (!HasTag(m_Relations[i].m_SourceNodeNo, Tag) ) continue;
	 if (!HasTag(m_Relations[i].m_TargetNodeNo, Tag) ) continue;
 
	 if (    m_Relations[i].m_bRelUse
		 &&  !m_Relations[i].m_bSemFetAgree)
			 Result++;

	};

    return Result;
};

//=======================================
// проверка того, что семантическая вершина клаузы удовлетворяет синтаксическим критериям 
bool CRusSemStructure::TopAgreeWithSyntaxCriteria (long Tag)
{
	for (size_t i = 0;  i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
			if (m_Nodes[i].IsWordContainer())
			{
				CRelSet R = GetIncomingRelations(i, true);

				if (R.m_RelsCount == 0) // i - корень дерева
					if ( !IsCoordConj (i)  && !HasRichPOS (i,CONJ) )
						return    HasRichPOS (i, INFINITIVE) 
						|| HasRichPOS  (i, VERB) 
						|| HasRichPOS (i, PARTICIPLE)
						|| HasRichPOS (i, PARTICIPLE_SHORT)
						|| HasRichPOS (i, PREDK)
						|| (   HasRichPOS (i, NOUN) 
						&& m_Nodes[i].HasOneGrammem(rNominativ)
						)
						|| HasRichPOS (i, ADVERB_PARTICIPLE )
						|| HasRichPOS (i, ADJ_SHORT );
			};

	return false;
};



//=======================================
typedef short_string<40> string40;
long CRusSemStructure::IsLexFetAgree(long NodeNo) const 
{
	long ParentNodeNo = NodeNo;

	CRelSet R1 = GetIncomingRelations(NodeNo, true);

	if (R1.m_RelsCount != 1) return false;


	if (m_Nodes[m_Relations[R1.m_Rels[0]].m_SourceNodeNo].m_NodeType ==MNA)
		ParentNodeNo = m_Relations[R1.m_Rels[0]].m_SourceNodeNo;



	CRelSet R2 = GetIncomingRelations(ParentNodeNo, true);

	// ищем в вышестоящих концептах	
	for (long i=-0; i < m_Nodes[NodeNo].m_HigherConcepts.size();i++)
		if (    _find(m_Relations[R1.m_Rels[0]].m_LexFets, m_Nodes[NodeNo].m_HigherConcepts[i]) 
			||  ( (R2.m_RelsCount== 1) && _find(m_Relations[R2.m_Rels[0]].m_LexFets, m_Nodes[NodeNo].m_HigherConcepts[i])) 
			)
			return true;

	// ищем как простую лемму 
	if (m_Nodes[NodeNo].m_MainWordNo ==  -1) return false;
	string40 Lemmas[10];
	int LemmasCount = 0;
	Lemmas[LemmasCount++] = m_Nodes[NodeNo].m_Words[m_Nodes[NodeNo].m_MainWordNo].m_Lemma;
	if (m_Nodes[NodeNo].GetType() == CollocRoss)
	{
		Lemmas[LemmasCount++] = string40( GetRoss(CollocRoss)->GetEntryStr(m_Nodes[NodeNo].GetUnitNo()).c_str()  );
		EngRusMakeUpper((char *)Lemmas[LemmasCount - 1]);
	};

	if (m_Nodes[NodeNo].IsPrimitive())
		for (long j=0; j < m_Nodes[NodeNo].m_Words[0].m_WordEquals.size(); j++)
		{
			Lemmas[LemmasCount++] = m_Nodes[NodeNo].m_Words[0].m_WordEquals[j];
			EngRusMakeUpper((char *)Lemmas[LemmasCount - 1]);
		};

	for (long i=0; i < LemmasCount;i++)
	{
		string lem ((const char*)Lemmas[i]);
		if (    _find(m_Relations[R1.m_Rels[0]].m_LexFets, lem) 
			||  (   (R2.m_RelsCount== 1) 
			&& _find(m_Relations[R2.m_Rels[0]].m_LexFets, lem)
			) 
		 )
		 return true;
	}
	return false;
};


long CRusSemStructure::GetLexFetAgreeCount(long Tag)
{
  size_t Result =0;

  for (size_t i = 0;  i < m_Relations.size(); i++)
   if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
      if   (m_Relations[i].m_bRelUse)
	     if (IsLexFetAgree (m_Relations[i].m_TargetNodeNo))
	     Result++;

  return Result; 
};



//=======================================
bool CRusSemStructure::CheckSubjectPredicateRelation (long SubjNodeNo, long PredNodeNo)
{
    if (m_Nodes[SubjNodeNo].m_NodeType == MNA)  return true;
    if (m_Nodes[PredNodeNo].m_NodeType == Copul) return true;;
	if (m_Nodes[PredNodeNo].m_NodeType == ModalCopul) return true;
	if (m_Nodes[SubjNodeNo].IsPrimitive())
	{
		// "ничего не произошло"
		if (m_Nodes[SubjNodeNo].m_Words[0].m_Word == "НИЧЕГО") return true;
		// "никто не пришел"
		if (m_Nodes[SubjNodeNo].m_Words[0].m_Word == "НИКТО") return true;
	};
	/*
	  Разрешаем обе формы: "два мальчика пришло" и "два мальчика пришли", т.е 
	  если у субъекта есть числительное>1, тогда выходим из проверки.
	*/


  	for (size_t i=0; i < m_Nodes[SubjNodeNo].m_OutRels.size(); i++)
	{
		  long RelNo = m_Nodes[SubjNodeNo].m_OutRels[i];
		  if (!m_Relations[RelNo].m_bRelUse) continue;
		  long NodeNo = m_Relations[RelNo].m_TargetNodeNo;
		  if (     HasRichPOS (NodeNo, NUMERAL)
			   &&  m_Nodes[NodeNo].IsPrimitive()
			   &&  (atoi(m_Nodes[NodeNo].m_Words[0].m_Word.c_str()) > 1)
			 )
			 return true;
	};


	return m_pData->GetRusGramTab()->GleicheSubjectPredicate(m_Nodes[SubjNodeNo].m_GramCodes.c_str(), m_Nodes[PredNodeNo].m_GramCodes.c_str());

};

long CRusSemStructure::GetSubjectPredicateViolationsCount (long Tag)
{
  size_t Result =0;

  for (size_t i = 0;  i < m_Relations.size(); i++)
   if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
    if (m_Relations[i].m_bRelUse)
     if (m_Relations[i].m_SyntacticRelation == "подл")
		 if (!CheckSubjectPredicateRelation(m_Relations[i].m_TargetNodeNo, m_Relations[i].m_SourceNodeNo))
			 Result ++;

  return Result; 
};

//=======================================
long CRusSemStructure::GetOptionalValenciesCount(long Tag)
{
  size_t Result =0;

  for (size_t i = 0;  i < m_Relations.size(); i++)
   if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
    if (m_Relations[i].m_bRelUse)
 	  if (m_Nodes[m_Relations[i].m_SourceNodeNo].m_NodeType == MNA)
		{
				CRelSet R = GetIncomingRelations(m_Relations[i].m_SourceNodeNo, true);
				if (   (R.m_RelsCount >  0)
					&& IsOptional(R.m_Rels[0]) 
				   )
					Result++;
		}
       else
        if (    IsOptional(i) )
	 		Result++;

  return Result; 
};

//=======================================
//=======================================
/*
 пока эта функция обрабатывает следующие случаи:
1. Она ищет отношение, идущее от UNIT (метр, день, век) в чилительное и повышает   вес такому отношению.
   Это нужно для примера "два века", где вместо "век" берется  "веко". "Веко" 
не является UNIT, поэтому оно должно уйти.*/
long CRusSemStructure::GetMiscSemAgreeCount(long Tag) const
{
	long Result = 0;

	for (size_t i = 0;  i < m_Relations.size(); i++)
		if (HasTag(m_Relations[i].m_SourceNodeNo, Tag))
			if (m_Relations[i].m_bRelUse)
			{
				if (HasSemFetPro (m_Nodes[m_Relations[i].m_SourceNodeNo].m_NodeSemFets, "UNIT"))			
					if	(		HasPOS(m_Nodes[m_Relations[i].m_TargetNodeNo], NUMERAL) 
							||	HasPOS(m_Nodes[m_Relations[i].m_TargetNodeNo], NUMERAL_P) 
						)
					Result++;

			}
			return Result;

}

//=====================================
//=====================================
// проверка уровня беспорядка в валентностях
long CRusSemStructure::GetValencyDisorderCount(long Tag)
{
	long Result  = 0;
	for (size_t NodeNo=0; NodeNo < m_Nodes.size(); NodeNo++)
		if (HasTag(NodeNo, Tag))
		{  
			CRelSet OutRels;
			CRelSet ValencyPlaces = GetValencyPlaces(NodeNo, OutRels);
			if (ValencyPlaces.m_RelsCount < 2) continue;

			// установить порядок такой, как указан в диатезе
			for (long i = 0; i < ValencyPlaces.m_RelsCount; i++)
				for (long j = 0; j < m_Nodes[NodeNo].m_ValsLayout.size(); j++)
					if (ValencyPlaces.m_Rels[i] == m_Nodes[NodeNo].m_ValsLayout[j])
					{
						ValencyPlaces.m_Rels[i] = j;
						break;
					};

			/*
			Факультатитвные валентности на место и время учитывать не будем
			*/
			/*
			если в начале клаузы стоит союзное слово, тогда его учитывать не надо
			например "я знаю, где ты жил."
			Здесь формально локатив стоит перед субъектом, но нарушения 
			порядка следования нет.
			*/


			long ValTextPos = 0;
			long PrevValRelNo = -1; 
			for (long i = 0; i < ValencyPlaces.m_RelsCount; i++)
				if	(		!m_Relations[OutRels.m_Rels[i]].m_Valency.m_bOptional
						&&	!m_Nodes[m_Relations[OutRels.m_Rels[i]].m_TargetNodeNo].m_bConjWord
					)
				{
					
					if  (ValencyPlaces.m_Rels[i] < PrevValRelNo)
							 Result++;
						
					PrevValRelNo = ValencyPlaces.m_Rels[i];
				};

		};

	return Result;
};

//=======================================

long CRusSemStructure::GetWordWeightSumma (long Tag) const
{
	long Result =0;

	for (size_t i = 0;  i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
		{
			if (m_Nodes[i].GetType() == OborRoss)
				Result += 30; // добавляем вес оборотам
			
			for (size_t j = 0;  j < m_Nodes[i].m_Words.size(); j++)
				Result += m_Nodes[i].m_Words[j].m_WordWeight;
		}

	return Result;	
};

//=======================================
/*
 считаем число полных причастий, у которых нет ни одного актанта. Это
 считается плохо, тогда лучше выбирать прилагательные или существительные.
 Например: "предстоящие выборы" -> "forthcoming elections" (лучше так!)
*/
long CRusSemStructure::GetPrichastieWithoutActants (long Tag) const
{
	long Result =	0;

	for (size_t i = 0;  i < m_Nodes.size(); i++)
		if (HasTag(i, Tag))
			if (HasRichPOS (i, PARTICIPLE))
			{
				long j=0;
				for (; j < m_Nodes[i].m_OutRels.size(); j++)
					if (m_Relations[m_Nodes[i].m_OutRels[j]].m_bRelUse)
						break;
				if (j ==    m_Nodes[i].m_OutRels.size())
					Result++;
			};

	return Result;
};


