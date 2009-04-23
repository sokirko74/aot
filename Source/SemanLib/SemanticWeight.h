#if !defined(semantic_weight)
#define semantic_weight

#pragma warning (disable : 4786)




struct TreeVariantValueCoefs {
	// =========   безусловные оценки (те, которые не используются для сортировки древесных вариантов)
	// число компонент связностей
	long ConnectedComponentsCountCoef;
	// проективно дерево или нет 
	long ProjectnessCoefCoef;

	// =========   оценки качества отношений  (эти оценки делятся на общее количество отношений)
	// количество отношений, нарущающих предпочтительное направление 
    long DirectDisagreeCoef;
	//  число отношений, прошедших проверку на SF
	long SemFetDisagreeCoef;
	//  сколько отношений прощло согласование по LEX
	long LexFetAgreeCountCoef;
	//  общая длина отношений в словах
	long RelationsLengthCoef;
	//	количество отношений, которые втыкаются в узлы, часть речи которых не соответсвует природе отношений
	long SemRelPOSViolationsCountCoef;
	//  число отношений, которые взяты либо не из РОССа, либо из добавочных статей
	long OptionalValencyPenaltyCoef;
	//  число отношений, которые синтаксически выражаются творительным и которые получены не из статей, 
	//  а из пассивной трансформации (такие отношения считаются немного хуже обычных)
	long InstrAgentRelsCountCoef;


	// =========   порядок в тексте
	// количество узлов, которые нарушают порядок, указанный в CSemNode::m_ValsLayout
	long ValencyDisorderCoef;
	// запрещено,  чтобы между двумя сыновьями одного узла, которые не является МНА-оператором,  стояла  одна запятая
    long CommaBetweenBrothersExceptMNAViolationsCountCoef;
	// количество отношений, между узлами которых находится только одна запятая, а этого быть не должно
	long OnlyCommaBetweenViolationsCountCoef;


	// =========   синтаксические критерии
	//  совпала ли вершина структуры с синтаксическим сказуемым
	long AgreeWithSyntaxTopCoefCoef;
	//  может ли текущая вершина быть вершиной по синт. ее свойствам
	// (например, личные местоимения не могут)
	long TopAgreeWithSyntaxCriteriaCoef;
	// проверка согласования подлежащего и сказуемого
	long SubjectPredicateViolationsCountCoef;
	// нарушения согласования с полем RESTR
	long GramRestrViolationsCountCoef;

	
	// ========   лексические критерии (для конкретных случаев)
	// число операторов однородности, которые имеют только одну стрелку (это плохо!) 	
	long MNAViolationsCountCoef;
	//  число копульных узлов, у которых число валентностей не равно двум
	long CopulViolationsCountCoef;
	//  количество словосочетаний
	long CollocsCountCoef;
	//  количество оборотоов с GF = ОБСТ_ГР
	long OborotAdverbialCountCoef;
	//  количество лексических функций
	long LexFunctsCountCoef;
	//  максимальная длина в синтаксических деревьях
	long MaxRelationLeapsCount;
	//  отсутствие обязательной
	long ObligatoryValencyViolationCoef;
    //  число узлов с DOMAIN = разг
	long ColloquialInterpsCoef;
    //  число узлов, у которых одинаковые DOMAIN =/= общ
	long CorporaGleicheCoef;
    //  число узлов, у которых одинаковые DOMAIN =/= общ
	long PassiveValencyPenaltyCoef;
//  число операторов однородностей (MUA), у которых все члены имеют хотя бы одну	// общую  SF
	long SemFetAgreeMNACoef;
    //  Сумма WordWeight 
	long WordWeightCoef;
	// некоторые семантические проверки, которые повышают вес
	long MiscSemAgreeCoef;
	// если у причастия  нет актантов будем понижать ему вес, 
	// чтобы выбралось прилагательное или существительное 
	long PrichastieCoef;


	TreeVariantValueCoefs();
	
    bool ReadOneCoef (const char * s);


};

struct TreeVariantValue {
	bool Panic;
	long ValencyDisorder;
    long ValencyMiss;
    long DirectDisagree;
	long ConnectedComponentsCount;
	long SemFetDisagree;
	long ProjectnessCoef;
	long LexFetAgreeCount;
	long AgreeWithSyntaxTopCoef;
	long TopAgreeWithSyntaxCriteria;
	long RelationsLength;
	long MNAViolationsCount;
	long SemRelPOSViolationsCount;
	long OnlyCommaBetweenViolationsCount;
	long SubjectPredicateViolationsCount;
    long CommaBetweenBrothersExceptMNAViolationsCount;
	long OptionalValencyCount;
	long CopulViolationsCount;
	long InstrAgentRelsCount;
	long LexFunctsCount;
	long GramRestrViolationsCount;
	long CollocsCount;
	bool SAMNodeViolation;
	long OborotAdverbialCount;
	long ObligatoryValencyViolationCount;
	long ColloquialInterpsCount;
	long CorporaGleicheCount;
	long PassiveValencyCount;
	long SemFetAgreeMNACount;
	long WordWeightCount;
	long MiscSemAgreeCount;
	long PrichastieWithoutActantsCount;
	const TreeVariantValueCoefs*  Coefs;


	TreeVariantValue& operator = (const TreeVariantValue& X);
	void Init();
	TreeVariantValue(const TreeVariantValueCoefs* _Coefs);
	TreeVariantValue();
	

	// все, кроме оценок качества отношений и ObligatoryValencyViolationCount
	long GetWeight1(bool  CheckConnect = true)  const;

	// оценки качества отношений + ObligatoryValencyViolationCount
	long GetWeight2()  const;
	// синтаксические оценки
	long GetWeight3(bool  CheckConnect = true)  const;
	// Общая функция оценок
	long GetTreeWeight()  const;

	long	GetWordWeight()  const;

    bool operator == ( const TreeVariantValue& X ) const;
	bool operator < ( const TreeVariantValue& X ) const;
	string	GetStr() const;
	string	GetStrOfNotNull() const;
	string	GetDifference(const TreeVariantValue& X) const;
	
};

const WORD Unhosted = 0xffff;
const WORD MustBeIgnored = 0xffff-1;
class CTreeVariant 
{
	// the size of m_TreeRels can really be more than 256 on some sentences
	vector<WORD>	m_TreeRels;
	size_t			m_Size;	
public:
	CTreeVariant()
	{ 
		m_TreeRels.reserve(100);
		m_Size = 0;
	};
	void AddRel(WORD RelNo)
	{
		m_TreeRels.push_back(RelNo);
		m_Size++;
	};
	void SetRel(WORD RelNo, WORD Value)
	{
		assert (RelNo < m_Size);
		m_TreeRels[RelNo] = Value;
	};
	void DeleteLast()
	{
		assert (m_Size > 0);
		m_TreeRels.erase(m_TreeRels.begin() +m_Size-1);
		m_Size--;
	};

	WORD GetRel(WORD RelNo) const
	{
		return m_TreeRels[RelNo];
	};

	size_t GetRelsCount() const 
	{
		return m_Size; // for the speed 
	};
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

	long GetBestTreeWeight() const 
	{
		if ( TreeVariantCount == 0) return 1000;
		return m_BestValue.GetTreeWeight(); 
			    
	};

    long GetBestTreeWeight1(bool CheckConnected = true) const 
	{
		if ( TreeVariantCount == 0) return 1000;
		return    m_BestValue.GetWeight1(CheckConnected);
			    

	};
	void  CopyTreeOfLexVariantWeight(const CTreeOfLexVariantWeight& X)
	{
		m_SetCollocHypNo = X.m_SetCollocHypNo;
		m_LexVariantNo = X.m_LexVariantNo;
	    m_BestValue = X.m_BestValue;
		TreeVariantCount = X.TreeVariantCount;
	    AllRelationsCount = X.AllRelationsCount;
	};




    bool operator == ( const CTreeOfLexVariantWeight& X ) const
    {
		return GetBestTreeWeight() == X.GetBestTreeWeight();
	};

	bool operator < ( const CTreeOfLexVariantWeight& X ) const
    {
		return GetBestTreeWeight() < X.GetBestTreeWeight();
	};
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





typedef vector<CTreeOfLexVariantWeight> CLexVariantWeightVector;
typedef pair<CTreeVariant, TreeVariantValue> TreeAndValue;
typedef vector<TreeAndValue> TreeAndValueVector;


#endif
