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
	RelationsLengthCoef = 2;
	MNAViolationsCountCoef = 50;
	SemRelPOSViolationsCountCoef = 5;
	OnlyCommaBetweenViolationsCountCoef = 20;
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



bool TreeVariantValueCoefs::ReadOneCoef(const char* s)
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

std::string TreeVariantValueCoefs::GetCoefsString() const 
{
	std::string s =
		Format("ConnectedComponentsCount = %f\n", (double)ConnectedComponentsCountCoef) +
		Format("ProjectnessViolation = %f\n", (double)ProjectnessCoefCoef) +
		Format("DirectDisagree = %f\n", (double)DirectDisagreeCoef) +
		Format("SemFetDisagree = %f\n", (double)SemFetDisagreeCoef) +
		Format("LexFetAgreeCount = %f\n", (double)LexFetAgreeCountCoef) +
		Format("RelationsLength = %f\n", (double)RelationsLengthCoef) +
		Format("SemRelPOSViolationsCount = %f\n", (double)SemRelPOSViolationsCountCoef) +
		Format("OptionalValencyPenalty = %f\n", (double)OptionalValencyPenaltyCoef) +
		Format("InstrAgentRelsCount = %f\n", (double)InstrAgentRelsCountCoef) +
		Format("ValencyDisorder = %f\n", (double)ValencyDisorderCoef) +
		Format("CommaBetweenBrothersExceptMNAViolationsCount = %f\n", (double)CommaBetweenBrothersExceptMNAViolationsCountCoef) +
		Format("OnlyCommaBetweenViolationsCount = %f\n", (double)OnlyCommaBetweenViolationsCountCoef) +
		Format("AgreeWithSyntaxTop = %f\n", (double)AgreeWithSyntaxTopCoefCoef) +
		Format("TopAgreeWithSyntaxCriteria = %f\n", (double)TopAgreeWithSyntaxCriteriaCoef) +
		Format("SubjectPredicateViolationsCount = %f\n", (double)SubjectPredicateViolationsCountCoef) +
		Format("GramRestrViolationsCount = %f\n", (double)GramRestrViolationsCountCoef) +
		Format("MNAViolationsCount = %f\n", (double)MNAViolationsCountCoef) +
		Format("CopulViolationsCount = %f\n", (double)CopulViolationsCountCoef) +
		Format("CollocsCount = %f\n", (double)CollocsCountCoef) +
		Format("OborotAdverbialCount = %f\n", (double)OborotAdverbialCountCoef) +
		Format("LexFunctsCount = %f\n", (double)LexFunctsCountCoef) +
		Format("ObligatoryValencyViolation = %f\n", (double)ObligatoryValencyViolationCoef) +
		Format("ColloquialInterps = %f\n", (double)ColloquialInterpsCoef) +
		Format("CorporaGleiche = %f\n", (double)CorporaGleicheCoef) +
		Format("PassiveValencyPenalty = %f\n", (double)PassiveValencyPenaltyCoef) +
		Format("SemFetAgreeMNACount = %f\n", (double)SemFetAgreeMNACoef) +
		Format("WordWeightCount = %f\n", (double)WordWeightCoef) +
		Format("MiscSemAgree = %f\n", (double)MiscSemAgreeCoef) +
		Format("PrichastieWithoutActantsCount = %f\n", (double)PrichastieCoef) +
		Format("SAMNodeViolation = 1000.000000\n") +
		Format("PanicMode = 1000.000000\n");
	return s;
}


TreeVariantValue& TreeVariantValue :: operator = (const TreeVariantValue& X)
{
	Panic = X.Panic;
	ValencyDisorder = X.ValencyDisorder;
	ValencyMiss = X.ValencyMiss;
	DirectDisagree = X.DirectDisagree;
	ConnectedComponentsCount = X.ConnectedComponentsCount;
	SemFetDisagree = X.SemFetDisagree;
	ProjectnessCoef = X.ProjectnessCoef;
	LexFetAgreeCount = X.LexFetAgreeCount;
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



void TreeVariantValue::Init()
{
	Coefs = 0;
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
	Coefs = _Coefs;

};
TreeVariantValue::TreeVariantValue()
{
	Init();
};

bool TreeVariantValue::operator == (const TreeVariantValue& X) const
{
	return  GetTreeWeight() == X.GetTreeWeight();
};

bool TreeVariantValue::operator < (const TreeVariantValue& X) const
{
	return  GetTreeWeight() < X.GetTreeWeight();
};





long TreeVariantValue::GetWordWeight()  const
{
	return WordWeightCount * Coefs->WordWeightCoef / 100;
}

long TreeVariantValue::GetWeight1(bool  CheckConnect)  const
{
	assert(Coefs);
	long s1 = ValencyDisorder * Coefs->ValencyDisorderCoef
		+ DirectDisagree * Coefs->DirectDisagreeCoef
		+ ProjectnessCoef * Coefs->ProjectnessCoefCoef
		+ LexFetAgreeCount * Coefs->LexFetAgreeCountCoef
		+ MNAViolationsCount * Coefs->MNAViolationsCountCoef
		+ SemRelPOSViolationsCount * Coefs->SemRelPOSViolationsCountCoef
		+ OnlyCommaBetweenViolationsCount * Coefs->OnlyCommaBetweenViolationsCountCoef
		+ SubjectPredicateViolationsCount * Coefs->SubjectPredicateViolationsCountCoef;
	long s2 = CommaBetweenBrothersExceptMNAViolationsCount * Coefs->CommaBetweenBrothersExceptMNAViolationsCountCoef
		+ (Panic ? 1000 : 0)
		+ ConnectedComponentsCount * ((CheckConnect) ? Coefs->ConnectedComponentsCountCoef : 0)
		+ CopulViolationsCount * Coefs->CopulViolationsCountCoef
		+ GramRestrViolationsCount * Coefs->GramRestrViolationsCountCoef
		+ CollocsCount * Coefs->CollocsCountCoef
		+ OborotAdverbialCount * Coefs->OborotAdverbialCountCoef
		+ ColloquialInterpsCount * Coefs->ColloquialInterpsCoef;
	long s3 = SAMNodeViolation * 1000
		+ CorporaGleicheCount * Coefs->CorporaGleicheCoef
		+ PassiveValencyCount * Coefs->PassiveValencyPenaltyCoef
		+ SemFetAgreeMNACount * Coefs->SemFetAgreeMNACoef
		+ GetWordWeight()
		+ PrichastieWithoutActantsCount * Coefs->PrichastieCoef
		+ RelationsLength * Coefs->RelationsLengthCoef / 100
		+ MiscSemAgreeCount * Coefs->MiscSemAgreeCoef;
	return s1 + s2 + s3;
};


long TreeVariantValue::GetWeight2()  const
{
	assert(Coefs);
	return
		SemFetDisagree * Coefs->SemFetDisagreeCoef / 100

		+ OptionalValencyCount * Coefs->OptionalValencyPenaltyCoef
		+ LexFunctsCount * Coefs->LexFunctsCountCoef
		+ InstrAgentRelsCount * Coefs->InstrAgentRelsCountCoef / 100
		+ ObligatoryValencyViolationCount * Coefs->ObligatoryValencyViolationCoef;


};

long TreeVariantValue::GetWeight3(bool  CheckConnect)  const
{
	assert(Coefs);
	return
		AgreeWithSyntaxTopCoef * Coefs->AgreeWithSyntaxTopCoefCoef
		+ TopAgreeWithSyntaxCriteria * Coefs->TopAgreeWithSyntaxCriteriaCoef;
};

long TreeVariantValue::GetTreeWeight()  const
{
	assert(Coefs);
	return   GetWeight1()
		+ GetWeight2()
		+ GetWeight3();

};

std::string TreeVariantValue::GetStr()  const
{
	std::string Q = Format("ValencyDisorder=%i;\nDirectDisagree=%i;\nSemFetDisagree=%i;\nConnectedComponents=%i;\nProjectnessCoef=%i;\nLexFetAgree=%i;\nAgreeWithSyntaxTop=%i;\nTopAgreeWithSyntaxCriteria=%i\nSemRelPOSViolations=%i;\nMNAViolations=%i;\nOnlyCommaBetweenViolations=%i;\nRelationsLength=%i;\nSubjectPredicateViolations=%i;\nOptionalValencyCount=%i;\nCopulViolations=%i;\nInstrAgentRels=%i;\nLexFuncts=%i;\nGramRestrViolations=%i;\nCollocs=%i\n OborotAdverbial=%i\nObligatoryValencyViolation=%i\nColloquialInterps=%i\nCorporaGleicheCount=%i\nPassiveValencyCount=%i\nSemFetAgreeMNACount=%i WordWeightCount=%i MiscSemAgreeCount=%i",
		ValencyDisorder, DirectDisagree, SemFetDisagree, ConnectedComponentsCount, ProjectnessCoef, LexFetAgreeCount, AgreeWithSyntaxTopCoef, TopAgreeWithSyntaxCriteria, SemRelPOSViolationsCount, MNAViolationsCount, OnlyCommaBetweenViolationsCount, RelationsLength, SubjectPredicateViolationsCount, OptionalValencyCount, CopulViolationsCount, InstrAgentRelsCount, LexFunctsCount, GramRestrViolationsCount, CollocsCount, OborotAdverbialCount, ObligatoryValencyViolationCount, ColloquialInterpsCount, CorporaGleicheCount, PassiveValencyCount, SemFetAgreeMNACount, WordWeightCount, MiscSemAgreeCount);
	return Q;
};


std::string TreeVariantValue::GetStrOfNotNull()  const
{
	std::string Result;
	if (ValencyDisorder > 0)  Result += Format("ValencyDisorder=%i\n", ValencyDisorder);
	if (DirectDisagree > 0)  Result += Format("DirectDisagree=%i\n", DirectDisagree);
	if (SemFetDisagree > 0)  Result += Format("SemFetDisagree=%i\n", SemFetDisagree);

	if (ConnectedComponentsCount > 0)  Result += Format("ConnectedComponentsCount=%i\n", ConnectedComponentsCount);
	if (ProjectnessCoef > 0)  Result += Format("ProjectnessCoef=%i\n", ProjectnessCoef);
	if (LexFetAgreeCount > 0)  Result += Format("LexFetAgreeCount=%i\n", LexFetAgreeCount);


	if (AgreeWithSyntaxTopCoef > 0)  Result += Format("AgreeWithSyntaxTopCoef=%i\n", AgreeWithSyntaxTopCoef);
	if (TopAgreeWithSyntaxCriteria > 0)  Result += Format("TopAgreeWithSyntaxCriteria=%i\n", TopAgreeWithSyntaxCriteria);
	if (SemRelPOSViolationsCount > 0)  Result += Format("SemRelPOSViolationsCount=%i\n", SemRelPOSViolationsCount);


	if (MNAViolationsCount > 0)  Result += Format("MNAViolationsCount=%i\n", MNAViolationsCount);
	if (OnlyCommaBetweenViolationsCount > 0)  Result += Format("OnlyCommaBetweenViolationsCount=%i\n", OnlyCommaBetweenViolationsCount);
	if (RelationsLength > 0)  Result += Format("RelationsLength=%i\n", RelationsLength);

	if (SubjectPredicateViolationsCount > 0)  Result += Format("SubjectPredicateViolationsCount=%i\n", SubjectPredicateViolationsCount);
	if (OptionalValencyCount > 0)  Result += Format("OptionalValencyCount=%i\n", OptionalValencyCount);
	if (CopulViolationsCount > 0)  Result += Format("CopulViolationsCount=%i\n", CopulViolationsCount);

	if (InstrAgentRelsCount > 0)  Result += Format("InstrAgentRelsCount=%i\n", InstrAgentRelsCount);
	if (LexFunctsCount > 0)  Result += Format("LexFunctsCount=%i\n", LexFunctsCount);
	if (GramRestrViolationsCount > 0)  Result += Format("GramRestrViolationsCount=%i\n", GramRestrViolationsCount);


	if (CollocsCount > 0)  Result += Format("CollocsCount=%i\n", CollocsCount);
	if (OborotAdverbialCount > 0)  Result += Format("OborotAdverbialCount=%i\n", OborotAdverbialCount);
	if (ObligatoryValencyViolationCount > 0)  Result += Format("ObligatoryValencyViolationCount=%i\n", ObligatoryValencyViolationCount);

	if (ColloquialInterpsCount > 0)  Result += Format("ColloquialInterpsCount=%i\n", ColloquialInterpsCount);
	if (CorporaGleicheCount > 0)  Result += Format("CorporaGleicheCount=%i\n", CorporaGleicheCount);
	if (PassiveValencyCount > 0)  Result += Format("PassiveValencyCount=%i\n", PassiveValencyCount);

	if (SemFetAgreeMNACount > 0)  Result += Format("SemFetAgreeMNACount=%i\n", SemFetAgreeMNACount);
	if (WordWeightCount > 0)  Result += Format("WordWeightCount=%i\n", WordWeightCount);
	if (CommaBetweenBrothersExceptMNAViolationsCount > 0)  Result += Format("CommaBetweenBrothersExceptMNAViolationsCount=%i\n", CommaBetweenBrothersExceptMNAViolationsCount);

	if (MiscSemAgreeCount > 0)  Result += Format("MiscSemAgreeCount=%i\n", MiscSemAgreeCount);

	if (PrichastieWithoutActantsCount > 0)  Result += Format("PrichastieWithoutActantsCount=%i\n", PrichastieWithoutActantsCount);

	return Result;
};


std::string TreeVariantValue::GetDifference(const TreeVariantValue& X)  const
{
	std::string Result;
	if (X.ValencyDisorder != ValencyDisorder)  Result += Format("ValencyDisorder=%i (%i)\n", ValencyDisorder, X.ValencyDisorder);
	if (X.DirectDisagree != DirectDisagree)  Result += Format("DirectDisagree=%i (%i)\n", DirectDisagree, X.DirectDisagree);
	if (X.SemFetDisagree != SemFetDisagree)  Result += Format("SemFetDisagree=%i (%i)\n", SemFetDisagree, X.SemFetDisagree);

	if (X.ConnectedComponentsCount != ConnectedComponentsCount)  Result += Format("ConnectedComponentsCount=%i (%i)\n", ConnectedComponentsCount, X.ConnectedComponentsCount);
	if (X.ProjectnessCoef != ProjectnessCoef)  Result += Format("ProjectnessCoef=%i (%i)\n", ProjectnessCoef, X.ProjectnessCoef);
	if (X.LexFetAgreeCount != LexFetAgreeCount)  Result += Format("LexFetAgreeCount=%i (%i)\n", LexFetAgreeCount, X.LexFetAgreeCount);


	if (X.AgreeWithSyntaxTopCoef != X.AgreeWithSyntaxTopCoef)  Result += Format("AgreeWithSyntaxTopCoef=%i (%i)\n", AgreeWithSyntaxTopCoef, X.AgreeWithSyntaxTopCoef);
	if (X.TopAgreeWithSyntaxCriteria != TopAgreeWithSyntaxCriteria)  Result += Format("TopAgreeWithSyntaxCriteria=%i (%i)\n", TopAgreeWithSyntaxCriteria, X.TopAgreeWithSyntaxCriteria);
	if (X.SemRelPOSViolationsCount != X.SemRelPOSViolationsCount)  Result += Format("SemRelPOSViolationsCount=%i (%i)\n", SemRelPOSViolationsCount, X.SemRelPOSViolationsCount);


	if (X.MNAViolationsCount != X.MNAViolationsCount)  Result += Format("MNAViolationsCount=%i (%i)\n", MNAViolationsCount, X.MNAViolationsCount);
	if (X.OnlyCommaBetweenViolationsCount != X.OnlyCommaBetweenViolationsCount)  Result += Format("OnlyCommaBetweenViolationsCount=%i (%i)\n", OnlyCommaBetweenViolationsCount, X.OnlyCommaBetweenViolationsCount);
	if (X.RelationsLength != RelationsLength)  Result += Format("RelationsLength=%i (%i)\n", RelationsLength, X.RelationsLength);

	if (X.SubjectPredicateViolationsCount != SubjectPredicateViolationsCount)  Result += Format("SubjectPredicateViolationsCount=%i (%i)\n", SubjectPredicateViolationsCount, X.SubjectPredicateViolationsCount);
	if (X.OptionalValencyCount != OptionalValencyCount)  Result += Format("OptionalValencyCount=%i (%i)\n", OptionalValencyCount, X.OptionalValencyCount);
	if (X.CopulViolationsCount != CopulViolationsCount)  Result += Format("CopulViolationsCount=%i (%i)\n", CopulViolationsCount, X.CopulViolationsCount);

	if (X.InstrAgentRelsCount != InstrAgentRelsCount)  Result += Format("InstrAgentRelsCount=%i (%i)\n", InstrAgentRelsCount, X.InstrAgentRelsCount);
	if (X.LexFunctsCount != LexFunctsCount)  Result += Format("LexFunctsCount=%i (%i)\n", LexFunctsCount, X.LexFunctsCount);
	if (X.GramRestrViolationsCount != X.GramRestrViolationsCount)  Result += Format("GramRestrViolationsCount=%i (%i)\n", GramRestrViolationsCount, X.GramRestrViolationsCount);


	if (X.CollocsCount != CollocsCount)  Result += Format("CollocsCount=%i (%i)\n", CollocsCount, X.CollocsCount);
	if (X.OborotAdverbialCount != OborotAdverbialCount)  Result += Format("OborotAdverbialCount=%i (%i)\n", OborotAdverbialCount, X.OborotAdverbialCount);
	if (X.ObligatoryValencyViolationCount != X.ObligatoryValencyViolationCount)  Result += Format("ObligatoryValencyViolationCount=%i (%i)\n", ObligatoryValencyViolationCount, X.ObligatoryValencyViolationCount);

	if (X.ColloquialInterpsCount != ColloquialInterpsCount)  Result += Format("ColloquialInterpsCount=%i (%i)\n", ColloquialInterpsCount, X.ColloquialInterpsCount);
	if (X.CorporaGleicheCount != CorporaGleicheCount)  Result += Format("CorporaGleicheCount=%i (%i)\n", CorporaGleicheCount, X.CorporaGleicheCount);
	if (X.PassiveValencyCount != PassiveValencyCount)  Result += Format("PassiveValencyCount=%i (%i)\n", PassiveValencyCount, X.PassiveValencyCount);

	if (X.SemFetAgreeMNACount != SemFetAgreeMNACount)  Result += Format("SemFetAgreeMNACount=%i (%i)\n", SemFetAgreeMNACount, X.SemFetAgreeMNACount);
	if (X.WordWeightCount != WordWeightCount)  Result += Format("WordWeightCount=%i (%i)\n", WordWeightCount, X.WordWeightCount);
	if (X.CommaBetweenBrothersExceptMNAViolationsCount != X.CommaBetweenBrothersExceptMNAViolationsCount)  Result += Format("CommaBetweenBrothersExceptMNAViolationsCount=%i (%i)\n", CommaBetweenBrothersExceptMNAViolationsCount, X.CommaBetweenBrothersExceptMNAViolationsCount);

	if (X.MiscSemAgreeCount != MiscSemAgreeCount)  Result += Format("MiscSemAgreeCount=%i (%i)\n", MiscSemAgreeCount, X.MiscSemAgreeCount);

	if (X.PrichastieWithoutActantsCount != PrichastieWithoutActantsCount)  Result += Format("PrichastieWithoutActantsCount=%i (%i)\n", PrichastieWithoutActantsCount, X.PrichastieWithoutActantsCount);

	return Result;
};


//===================

void CTreeVariant::AddRel(WORD RelNo)
{
	m_TreeRels.push_back(RelNo);
};

void CTreeVariant::SetRel(WORD RelNo, WORD Value)
{
	m_TreeRels[RelNo] = Value;
};

void CTreeVariant::DeleteLast()
{
	m_TreeRels.erase(m_TreeRels.begin() + m_TreeRels.size() - 1);
};

WORD CTreeVariant::GetRel(WORD RelNo) const
{
	return m_TreeRels[RelNo];
};

size_t CTreeVariant::GetRelsCount() const
{
	return m_TreeRels.size();
};


//=================
long CTreeOfLexVariantWeight::GetBestTreeWeight() const {
	if (TreeVariantCount == 0) return 1000;
	return m_BestValue.GetTreeWeight();
}

long CTreeOfLexVariantWeight::GetBestTreeWeight1(bool CheckConnected) const
{
	if (TreeVariantCount == 0) return 1000;
	return    m_BestValue.GetWeight1(CheckConnected);


};
void  CTreeOfLexVariantWeight::CopyTreeOfLexVariantWeight(const CTreeOfLexVariantWeight& X)
{
	m_SetCollocHypNo = X.m_SetCollocHypNo;
	m_LexVariantNo = X.m_LexVariantNo;
	m_BestValue = X.m_BestValue;
	TreeVariantCount = X.TreeVariantCount;
	AllRelationsCount = X.AllRelationsCount;
};


bool CTreeOfLexVariantWeight::operator == (const CTreeOfLexVariantWeight& X) const
{
	return GetBestTreeWeight() == X.GetBestTreeWeight();
};


bool CTreeOfLexVariantWeight::operator < (const CTreeOfLexVariantWeight& X) const
{
	return GetBestTreeWeight() < X.GetBestTreeWeight();
};


