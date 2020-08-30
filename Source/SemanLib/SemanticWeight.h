#pragma once 

const long MaxRelationLeapsCount = 4;

// semantic weight: the less the better, the min is the best
enum SemantiWeightComponentEnum : long {
	ConnectedComponentsCount = 0,
	// проективно дерево или нет 
	ProjectnessViolation,
	// =========   оценки качества отношений  (эти оценки делятся на общее количество отношений)
	// количество отношений, нарущающих предпочтительное направление 
	DirectDisagree,
	//  число отношений, прошедших проверку на SF
	SemFetDisagree,
	//  сколько отношений прощло согласование по LEX
	LexFetAgreeCount,
	//  общая длина отношений в словах
	RelationsLength,
	//	количество отношений, которые втыкаются в узлы, часть речи которых не соответсвует природе отношений
	SemRelPOSViolationsCount,
	//  число отношений, которые взяты либо не из РОССа, либо из добавочных статей
	OptionalValencyPenalty,
	//  число отношений, которые синтаксически выражаются творительным и которые получены не из статей, 
	//  а из пассивной трансформации (такие отношения считаются немного хуже обычных)
	InstrAgentRelsCount,
	// =========   порядок в тексте
	// количество узлов, которые нарушают порядок, указанный в CSemNode::m_ValsLayout
	ValencyDisorder,
	// запрещено,  чтобы между двумя сыновьями одного узла, которые не является МНА-оператором,  стояла  одна запятая
	CommaBetweenBrothersExceptMNAViolationsCount,
	// количество отношений, между узлами которых находится только одна запятая, а этого быть не должно
	OnlyCommaBetweenViolationsCount,
	// =========   синтаксические критерии
	//  совпала ли вершина структуры с синтаксическим сказуемым
	AgreeWithSyntaxTop,
	//  может ли текущая вершина быть вершиной по синт. ее свойствам
	// (например, личные местоимения не могут)
	TopAgreeWithSyntaxCriteria,
	// проверка согласования подлежащего и сказуемого
	SubjectPredicateViolationsCount,
	// нарушения согласования с полем RESTR
	GramRestrViolationsCount,
	// ========   лексические критерии (для конкретных случаев)
	// число операторов однородности, которые имеют только одну стрелку (это плохо!) 	
	MNAViolationsCount,
	//  число копульных узлов, у которых число валентностей не равно двум
	CopulViolationsCount,
	//  количество словосочетаний
	CollocsCount,
	//  количество оборотоов с GF = ОБСТ_ГР
	OborotAdverbialCount,
	//  количество лексических функций
	LexFunctsCount,
	//  отсутствие обязательной
	ObligatoryValencyViolation,
	//  число узлов с DOMAIN = разг
	ColloquialInterps,
	//  число узлов, у которых одинаковые DOMAIN =/= общ
	CorporaGleiche,
	//  число узлов, у которых одинаковые DOMAIN =/= общ
	PassiveValencyPenalty,
	//  число операторов однородностей (MUA), у которых все члены имеют хотя бы одну	// общую  SF
	SemFetAgreeMNACount,
	//  Сумма WordWeight 
	WordWeightCount,
	// некоторые семантические проверки, которые повышают вес
	MiscSemAgree,
	// если у причастия  нет актантов будем понижать ему вес, 
	// чтобы выбралось прилагательное или существительное 
	PrichastieWithoutActantsCount,
	//
	SAMNodeViolation,
	//
	PanicMode,
	//
	SemantiWeightComponentSize
};
extern const std::string& GetStringBySemantiWeightComponent(SemantiWeightComponentEnum t);
extern SemantiWeightComponentEnum GetSemantiWeightComponentByString(const std::string& s);

struct TreeVariantValueCoefs {
	std::vector<double> Coefs;
	TreeVariantValueCoefs();
    void ReadOneCoef (std::string s);
	std::string GetCoefsString() const;
};


enum struct WeightType {
	Weight1,
	Weight2,
	Weight3
};

class TreeVariantValue {
	const TreeVariantValueCoefs* Coefs;
	std::vector<long> Weights;
	long GetWeightByType(bool  CheckConnect, WeightType weightType)  const;
public:
	TreeVariantValue& operator = (const TreeVariantValue& X);
	void Init();
	TreeVariantValue(const TreeVariantValueCoefs* coefs);
	TreeVariantValue();
	void SetCoefs(const TreeVariantValueCoefs* coefs);
	

	// все, кроме оценок качества отношений и ObligatoryValencyViolationCount
	long GetWeight1(bool  CheckConnect = true)  const;

	// оценки качества отношений + ObligatoryValencyViolationCount
	long GetWeight2()  const;
	// синтаксические оценки
	long GetWeight3(bool  CheckConnect = true)  const;
	// Общая функция оценок
	long GetTreeWeight()  const;
	bool operator == ( const TreeVariantValue& X ) const;
	bool operator < ( const TreeVariantValue& X ) const;
	std::string	GetStr() const;
	std::string	GetStrOfNotNull() const;
	std::string	GetDifference(const TreeVariantValue& X) const;
	void SetWeight(SemantiWeightComponentEnum w, long value);
	long GetSingleWeight(SemantiWeightComponentEnum w) const;
	long GetWeightCoef(SemantiWeightComponentEnum w) const;
	bool IsPanic() const;
	void SetPanic();
};

const WORD Unhosted = 0xffff;
const WORD MustBeIgnored = 0xffff-1;

class CTreeVariant 
{
	// the size of m_TreeRels can really be more than 256 on some sentences
	std::vector<WORD>	m_TreeRels;
public:
	void AddRel(WORD RelNo);
	void SetRel(WORD RelNo, WORD Value);
	void DeleteLast();
	WORD GetRel(WORD RelNo) const;
	size_t GetRelsCount() const;
};

/*
 в этом классе содержится адрес  и оценка 
 некоторого дерева одного лексического варианта
*/
struct CTreeOfLexVariantWeight {
	// множество словосочетаний
	size_t m_SetCollocHypNo;
	// номер интерпретации в словосочетаниях
	size_t m_LexVariantNo;
	// оценка лучшего дерева в этом варианте
	TreeVariantValue m_BestValue;

    // вспомог. слоты
	long   TreeVariantCount;
	long   AllRelationsCount;

	long GetBestTreeWeight() const;
	long GetBestTreeWeight1(bool CheckConnected = true) const;
	void  CopyTreeOfLexVariantWeight(const CTreeOfLexVariantWeight& X);
	bool operator == (const CTreeOfLexVariantWeight& X) const;
	bool operator < (const CTreeOfLexVariantWeight& X) const;
    
};

struct  CSyntaxClauseVariant {
	// =====  определяющая информация
	// синтаксический номер клаузы
	long							m_SyntaxClauseNo;
	// морфологический вариант клаузы
	long							m_ClauseVariantNo;

	CSyntaxClauseVariant (long SyntaxClauseNo,long ClauseVariantNo) 
	{
		m_SyntaxClauseNo = SyntaxClauseNo;
	    m_ClauseVariantNo = ClauseVariantNo;
	};
    bool operator == ( const CSyntaxClauseVariant& X ) const
	{
		return    (X.m_SyntaxClauseNo == m_SyntaxClauseNo) 
			   && (X.m_ClauseVariantNo == m_ClauseVariantNo)
			   ;
	};

};



typedef std::vector<CTreeOfLexVariantWeight> CLexVariantWeightVector;
typedef std::pair<CTreeVariant, TreeVariantValue> TreeAndValue;
typedef std::vector<TreeAndValue> TreeAndValueVector;


