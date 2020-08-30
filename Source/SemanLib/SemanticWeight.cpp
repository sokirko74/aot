#include "stdafx.h"
#include "SemanticRusStructure.h"
#include "LexFuncts.h"
#include "SemanticWeight.h"


struct TreeVariantDefaultValue {
	SemantiWeightComponentEnum Type;
	std::string Name;
	double DefaultValue;
	WeightType WeightType;
};

//static const std::vector<size_t> dummy = { 1,2,3 };

static const std::vector<TreeVariantDefaultValue> AllComponents = {
	{ConnectedComponentsCount, "ConnectedComponentsCount", 10000, WeightType::Weight1},
	{ProjectnessViolation, "ProjectnessViolation", 400, WeightType::Weight1},
	{DirectDisagree, "DirectDisagree", 10, WeightType::Weight1},
	{SemFetDisagree, "SemFetDisagree", 0.15, WeightType::Weight2},
	{LexFetAgreeCount, "LexFetAgreeCount", -20, WeightType::Weight1},
	{RelationsLength, "RelationsLength", 0.01, WeightType::Weight1},
	{SemRelPOSViolationsCount, "SemRelPOSViolationsCount", 5, WeightType::Weight1},
	{OptionalValencyPenalty, "OptionalValencyPenalty", 90, WeightType::Weight2},
	{InstrAgentRelsCount, "InstrAgentRelsCount", 0.05, WeightType::Weight2},
	{ValencyDisorder, "ValencyDisorder", 7, WeightType::Weight1},  
	{CommaBetweenBrothersExceptMNAViolationsCount, "CommaBetweenBrothersExceptMNAViolationsCount", 100, WeightType::Weight1},
	{OnlyCommaBetweenViolationsCount, "OnlyCommaBetweenViolationsCount", 60, WeightType::Weight1},
	{AgreeWithSyntaxTop, "AgreeWithSyntaxTop", -20, WeightType::Weight3},
	{TopAgreeWithSyntaxCriteria, "TopAgreeWithSyntaxCriteria", -10, WeightType::Weight3},
	{SubjectPredicateViolationsCount, "SubjectPredicateViolationsCount", 100, WeightType::Weight1},
	{GramRestrViolationsCount, "GramRestrViolationsCount", 100, WeightType::Weight1},
	{MNAViolationsCount, "MNAViolationsCount", 200, WeightType::Weight1},
	{CopulViolationsCount, "CopulViolationsCount", 100, WeightType::Weight1},
	{CollocsCount, "CollocsCount", -15, WeightType::Weight1},
	{OborotAdverbialCount, "OborotAdverbialCount", -10, WeightType::Weight1},
	{LexFunctsCount, "LexFunctsCount", -50, WeightType::Weight2},
	{ObligatoryValencyViolation, "ObligatoryValencyViolation", 50, WeightType::Weight1},
	{ColloquialInterps, "ColloquialInterps", 50, WeightType::Weight1},
	{CorporaGleiche, "CorporaGleiche", -2, WeightType::Weight1},
	{PassiveValencyPenalty, "PassiveValencyPenalty", 1, WeightType::Weight1},
	{SemFetAgreeMNACount, "SemFetAgreeMNACount", -30, WeightType::Weight1},
	{WordWeightCount, "WordWeightCount", -0.01, WeightType::Weight1},
	{MiscSemAgree, "MiscSemAgree", -20, WeightType::Weight1},
	{PrichastieWithoutActantsCount, "PrichastieWithoutActantsCount", 10, WeightType::Weight1},
	{SAMNodeViolation, "SAMNodeViolation", 1000, WeightType::Weight1},
	{PanicMode, "PanicMode", 1000, WeightType::Weight1},
};



const std::string& GetStringBySemantiWeightComponent(SemantiWeightComponentEnum t) {
	return AllComponents[t].Name;
}

SemantiWeightComponentEnum GetSemantiWeightComponentByString(const std::string& s) {
	for (auto i : AllComponents) {
		if (i.Name == s) {
			return i.Type;
		}
	}
	throw CExpc("bad semantic weight component %s ", s.c_str());
}


TreeVariantValueCoefs::TreeVariantValueCoefs() {
	size_t s = AllComponents.size();
	assert(s == SemantiWeightComponentSize);
	long i = 0; 
	for (const auto& c : AllComponents) {
		Coefs.push_back(c.DefaultValue);
		assert((long)c.Type == i);
		++i;
	}
}



void TreeVariantValueCoefs::ReadOneCoef(std::string  s)
{
	Trim(s);
	size_t i = s.find(" ");
	if (i == string::npos) {
		throw CExpc("cannot find space in \"%s\" while reading semantic coefficient", s.c_str());
	}
	auto t = GetSemantiWeightComponentByString(s.substr(0, i));
	s = s.substr(i);
	Trim(s);
	Coefs[t] = atof(s.c_str());

	// just not to store junk values
	assert(Coefs[t] > -1000);
	assert(Coefs[t] <= 20000);
};

std::string TreeVariantValueCoefs::GetCoefsString() const {
	std::vector<std::string> items;
	for (const auto& a : AllComponents) {
		auto s = Format("%s=%f", a.Name.c_str(), Coefs[a.Type]);
		items.push_back(s);
	}
	return join_string(items, "\n");
}

//============================


void TreeVariantValue::SetCoefs(const TreeVariantValueCoefs* coefs) {
	Coefs = coefs;
}

TreeVariantValue& TreeVariantValue :: operator = (const TreeVariantValue& X)
{
	Coefs = X.Coefs;
	Weights = X.Weights;
	return *this;
};

void TreeVariantValue::Init()
{
	Coefs = nullptr;
	Weights.resize(AllComponents.size(), 0);
};

void TreeVariantValue::SetWeight(SemantiWeightComponentEnum w, long value) {
	Weights[w] = value;
}

long TreeVariantValue::GetSingleWeight(SemantiWeightComponentEnum w) const {
	return Weights[w];
}

bool TreeVariantValue::IsPanic() const {
	return Weights[PanicMode] > 0;
}

void TreeVariantValue::SetPanic() {
	Weights[PanicMode] = 1;
}

long TreeVariantValue::GetWeightCoef(SemantiWeightComponentEnum w) const {
	return Coefs->Coefs[w];
}

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



long TreeVariantValue::GetWeightByType(bool  CheckConnect, WeightType weightType)  const {
	assert(Coefs);
	double weight = 0.0;
	for (const auto& a : AllComponents) {
		if (CheckConnect || a.Type != ConnectedComponentsCount) {
			if (a.WeightType == weightType) {
				weight += (double)Weights[a.Type] * Coefs->Coefs[a.Type];
			}
		}
	}
	return (long)weight;

}

long TreeVariantValue::GetWeight1(bool  CheckConnect)  const
{
	return GetWeightByType(CheckConnect, WeightType::Weight1);
};


long TreeVariantValue::GetWeight2()  const
{
	return GetWeightByType(true, WeightType::Weight2);
};

long TreeVariantValue::GetWeight3(bool  CheckConnect)  const
{
	return GetWeightByType(true, WeightType::Weight3);
};

long TreeVariantValue::GetTreeWeight()  const
{
	return   GetWeight1() + GetWeight2() + GetWeight3();
};

std::string TreeVariantValue::GetStr()  const
{
	std::vector<std::string> items;
	for (const auto& a : AllComponents) {
		items.push_back(Format("%s=%ld;", a.Name.c_str(), Weights[a.Type]));
	}
	return join_string(items, "\n");
};


std::string TreeVariantValue::GetStrOfNotNull()  const
{
	std::vector<std::string> items;
	for (const auto& a : AllComponents) {
		if (Weights[a.Type] != 0) {
			items.push_back(Format("%s=%ld;", a.Name.c_str(), Weights[a.Type]));
		}
	}
	return join_string(items, "\n");
};


std::string TreeVariantValue::GetDifference(const TreeVariantValue& _X)  const
{
	std::vector<std::string> items;
	for (const auto& a : AllComponents) {
		if (Weights[a.Type] != _X.Weights[a.Type]) {
			items.push_back(Format("%s: %ld != (%ld);", a.Name.c_str(), Weights[a.Type], _X.Weights[a.Type]));
		}
	}
	return join_string(items, "\n");
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


