#if !defined(semantic_weight)
#define semantic_weight

#pragma warning (disable : 4786)




struct TreeVariantValueCoefs {
	// =========   ����������� ������ (��, ������� �� ������������ ��� ���������� ��������� ���������)
	// ����� ��������� ����������
	long ConnectedComponentsCountCoef;
	// ���������� ������ ��� ��� 
	long ProjectnessCoefCoef;

	// =========   ������ �������� ���������  (��� ������ ������� �� ����� ���������� ���������)
	// ���������� ���������, ���������� ���������������� ����������� 
    long DirectDisagreeCoef;
	//  ����� ���������, ��������� �������� �� SF
	long SemFetDisagreeCoef;
	//  ������� ��������� ������ ������������ �� LEX
	long LexFetAgreeCountCoef;
	//  ����� ����� ��������� � ������
	long RelationsLengthCoef;
	//	���������� ���������, ������� ��������� � ����, ����� ���� ������� �� ������������ ������� ���������
	long SemRelPOSViolationsCountCoef;
	//  ����� ���������, ������� ����� ���� �� �� �����, ���� �� ���������� ������
	long OptionalValencyPenaltyCoef;
	//  ����� ���������, ������� ������������� ���������� ������������ � ������� �������� �� �� ������, 
	//  � �� ��������� ������������� (����� ��������� ��������� ������� ���� �������)
	long InstrAgentRelsCountCoef;


	// =========   ������� � ������
	// ���������� �����, ������� �������� �������, ��������� � CSemNode::m_ValsLayout
	long ValencyDisorderCoef;
	// ���������,  ����� ����� ����� ��������� ������ ����, ������� �� �������� ���-����������,  ������  ���� �������
    long CommaBetweenBrothersExceptMNAViolationsCountCoef;
	// ���������� ���������, ����� ������ ������� ��������� ������ ���� �������, � ����� ���� �� ������
	long OnlyCommaBetweenViolationsCountCoef;


	// =========   �������������� ��������
	//  ������� �� ������� ��������� � �������������� ���������
	long AgreeWithSyntaxTopCoefCoef;
	//  ����� �� ������� ������� ���� �������� �� ����. �� ���������
	// (��������, ������ ����������� �� �����)
	long TopAgreeWithSyntaxCriteriaCoef;
	// �������� ������������ ����������� � ����������
	long SubjectPredicateViolationsCountCoef;
	// ��������� ������������ � ����� RESTR
	long GramRestrViolationsCountCoef;

	
	// ========   ����������� �������� (��� ���������� �������)
	// ����� ���������� ������������, ������� ����� ������ ���� ������� (��� �����!) 	
	long MNAViolationsCountCoef;
	//  ����� ��������� �����, � ������� ����� ������������ �� ����� ����
	long CopulViolationsCountCoef;
	//  ���������� ��������������
	long CollocsCountCoef;
	//  ���������� ��������� � GF = ����_��
	long OborotAdverbialCountCoef;
	//  ���������� ����������� �������
	long LexFunctsCountCoef;
	//  ������������ ����� � �������������� ��������
	long MaxRelationLeapsCount;
	//  ���������� ������������
	long ObligatoryValencyViolationCoef;
    //  ����� ����� � DOMAIN = ����
	long ColloquialInterpsCoef;
    //  ����� �����, � ������� ���������� DOMAIN =/= ���
	long CorporaGleicheCoef;
    //  ����� �����, � ������� ���������� DOMAIN =/= ���
	long PassiveValencyPenaltyCoef;
//  ����� ���������� ������������� (MUA), � ������� ��� ����� ����� ���� �� ����	// �����  SF
	long SemFetAgreeMNACoef;
    //  ����� WordWeight 
	long WordWeightCoef;
	// ��������� ������������� ��������, ������� �������� ���
	long MiscSemAgreeCoef;
	// ���� � ���������  ��� �������� ����� �������� ��� ���, 
	// ����� ��������� �������������� ��� ��������������� 
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
	

	// ���, ����� ������ �������� ��������� � ObligatoryValencyViolationCount
	long GetWeight1(bool  CheckConnect = true)  const;

	// ������ �������� ��������� + ObligatoryValencyViolationCount
	long GetWeight2()  const;
	// �������������� ������
	long GetWeight3(bool  CheckConnect = true)  const;
	// ����� ������� ������
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
 � ���� ������ ���������� �����  � ������ 
 ���������� ������ ������ ������������ ��������
*/
struct CTreeOfLexVariantWeight {
	// ��������� ��������������
	size_t m_SetCollocHypNo;
	// ����� ������������� � ���������������
	size_t m_LexVariantNo;
	// ������ ������� ������ � ���� ��������
	TreeVariantValue m_BestValue;

    // �������. �����
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
	// =====  ������������ ����������
	// �������������� ����� ������
	long							m_SyntaxClauseNo;
	// ��������������� ������� ������
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
