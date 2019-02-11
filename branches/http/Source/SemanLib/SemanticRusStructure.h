#if !defined(semantic_rus_str)
#define semantic_rus_str

#pragma warning (disable : 4786)

#include "SemanticStructure.h"
#include "SemanticWeight.h"
#include "../SynCommonLib/RelationsIterator.h"




// ==== ����� ������� �����
class CRusSemWord  : public CSemWord
{
public:	 
  // ��������������� ������ ���
  bool           m_IsPunct;
  // ��������������� ������ ���
  bool           m_ILE;
  // ����� ���������, ������� ����� � ������ ����� ����� ����� 
  string         m_PostPuncts;
  string		 m_GraphDescrs;
  //  ����� ��������������� ������������ �������, ������� ���� �������� �� ������������� �������
  string		 m_SupAdjLemma;
  bool			 m_bRomanNumber;
  long			 m_SynWordNo;

  void			Init();
  CRusSemWord();
  CRusSemWord   ( long WordNo, string Lemma );

  // ���������� - ���������� �����, ������� ������������� �� "��", "��" 
  bool IsReflexiveVerb () const;
  UINT GetRusRichPoses () const;
  bool   HasPOS (size_t POS) const;
  bool IsEqualMorph  (const CRusSemWord& W) const;
};

class CRusMorphHomonym  
{
  public:	 
  long			 m_SynWordNo;
  long		    m_ParadigmId;
  
  CRusMorphHomonym();
  CRusMorphHomonym(const CRusSemWord& X) ;
};


typedef pair<long,long> PairOfLong;
typedef vector<int>		VectorInt;


struct  CRusInterpNodeInfo  {
	//  ���� � GF-������� ����� ����� ��������� �����
	bool			m_bSaveIsParenthesis;
	// ���������������� ������� ���������� ������������ � ������, ������� ����������� �� ��������� ������
	vector<long>      m_ValsLayout;
	// ������������ ����������� (����� �� ���� INCONS)
	vector<PairOfLong>      ProhibitedPairOfVals;
	// ��������� �������� ������������ ���������� ���� � ������

	vector<VectorInt>     m_NotProhibitedValSets;
	//  ���� � GF-������� ����� ����� ��������� c_��� (��������, ����:�_���),
    // �� ��������� ���� - ������
	bool			m_bFullGleiche;

	// ���� ���� - ���������������, ����� ����� ����� �������, 
	// ��� ������� ��� ��������������� �������� S0
	StringVector    m_VerbsOfS0;

	bool			  m_bConjWord;

		/* ��� ��� ����� ������������ �������, � ������� �����
	  ������ ������� ������� ����, ��������,
	   "��� ����, ��� � ������"
	   ����� ������� "����" ����� ���� ������ 
	   ����� "���" � "���"
	   ���� m_MinActantWordNo	 == -1, �����
	   ������� ����� ������ �� ���� ������
	*/
	long			m_MinActantWordNo;
	long			m_MaxActantWordNo;

};

// ==== ����� "������� ����"
class CRusSemNode : public CSemNode, public CRusInterpNodeInfo
{
public:
	vector<CRusSemWord>		m_Words;
	const CSemWord&	GetWord(int WordNo) const { return m_Words[WordNo];};
	int		GetWordsSize() const { return m_Words.size();};

	//  ����� ������, �� ������� ��� ��������� ���� ���� (� ������ IRelationsIteratorPtr::FirmGroups)
	int						m_SyntaxGroupNo; 
	// �������� ����, ���� ���� ���� ����� ����
	bool					m_bInCycle;
	// ����� �����, �� ������� ����� �������, ���� ���� ������� ���
	long			m_PrepWordNo;
	// ���� �������� �������������� ��������������, ��� �������� �� ���� ������ ������� ������� 
	// ��������, "�������� ����, ������"
	bool			m_bCompWithOneActant;
	// ������, ���� ���� ���� �������� ������ �� ������ ����� � �������
	bool			m_bFirstInTermin;
	// ���� �������� �������������� ��������������
	bool			m_bCompAdj;

	//  ����������� ������ ��������� � �������� ������������� ��������� 
	// (������������ �� �������� ���������, ��� ��������� ��������� �� ��������)
    vector<long>	m_InRels;
	vector<long>	m_OutRels;

	long 			m_MemNodeNo;

	BYTE			m_IncomRelsCount; 


	UINT			m_RichPoses;

	bool			m_bQuestionWord;
	string			m_AntecedentStr;


	CRusSemNode();

	bool IsILENode() const;
	// ���������, ����� �� ����� ���������� ����� ���� ���� ����������
	bool HasPostPunct(BYTE punct_char) const;
    // ���������, ����� �� ����� �� ����� ���������� ����� ���� �������
	bool HaveCommaAfter() const;
    // ����  ����� ������ ��� ������� ����������� ��� �������� SF
	WORD         GetSemCorrectUnitNo() const;
	// ���������, ��� ����������� �� �������� �������������
	// (����� ������������ ���� m_RelationId, � ������� �������� ���������� ����� �����������
	// � �����, ������� ������ ������� �RusSemStructure::IndexRelations)
	bool RelsCanBeTogether(const long RelationId1,  const long RelationId2) const;
	// �������� �� ������ ���� ��������
	bool   IsOneQuoteMark () const;
	// �������� �� ������ ���� ����
	bool   IsOneDash () const;
	bool IsMNA_ButNotCHEM () const;
    // ������� ����� IsWordContainer(),����� IsAbstract(), � ��� ������ ������ ��������
    bool operator < (const CRusSemNode& Node) const;
	bool IsAnaphoricPronoun () const;
	QWORD GetInnerGrammems() const;
	bool IsLemma(string Lemma) const;
	bool IsWordForm(string WordForm) const;
	bool IsLemmaList( const char*  FirstLemma, ... ) const;
	bool IsEqualMorph (const CRusSemNode& N) 	const;
	void SetMainWordNo (long WordNo);
	void ModifyGramCodes(string GramCodes, int mode, const CRusGramTab *R);

};

class CMemNode : public CInterpNodeInfo, public CRusInterpNodeInfo
{
public:
	vector<CRusMorphHomonym> m_Words;
	CDictUnitInterp			 m_Interp;
	long					 m_PrepsCount;
	long					 m_FirstPrepUnitNo;
	QWORD					 m_Grammems;
	//������, ����� ���� ���� ���������� � ���������� ���������
	set<long>				 m_Cases;
	CMemNode () {
		m_PrepsCount = 0;
		m_FirstPrepUnitNo = ErrUnitNo;
		m_Grammems = 0;
	};
};

// ==== ����� "������� ���������"
class CRusSemRelation : public CSemRelation {
public:
	// ������, ��� ��������� �� ����� ��������� ������ ��� ����� ������� (��� ������ ��������� ��� ��� ���-��)
    bool				m_CannotHaveOnlyCommaBetween;
	// ������, ���� ��������� ����������� �� SF (��� �������� ���������� ��������)
	bool				m_bSemFetAgree;
	// ����� ���� ��������������, ������� �������� ���������, ��������� �� ������ ���� � �������������� ���� ����� 
	long				m_EqualFieldId;
	//����� ��������� � "�������" �������������� ��������
	long			m_LeapsCount;
	//����������� ����: �������� ���������, ������� ����� ��������� � ��������� ��������
	long			m_bUniqueRelation;
	// ����� ������������ �������, �� �������� ���� ��������� ��� ����������� �����
	long			m_ClauseRuleNo;

	// ������, ���� �������� ��������� ����������� � ������ ���� 
	// ����, � ������� ��� ����
	// ��������, PROPERT ������ ���� � ��������������// � SUB � ���������������	
	bool			m_bSemRelPOS;


    CRusSemRelation (const CValency& Valency, 
	             long SourceNodeNo, 
				 long TargetNodeNo, 
				 string SyntacticRelation) : CSemRelation(Valency,SourceNodeNo,TargetNodeNo,SyntacticRelation)
	{
 	  m_CannotHaveOnlyCommaBetween = false;
	  m_bSemFetAgree = false;
	  m_bReverseRel = false;
 	  m_LeapsCount = 0;
 	  m_bUniqueRelation = false;
	  m_ClauseRuleNo = -1;
	  m_bSemRelPOS = true;

	};
};


// ������, ���� ����� ����� ������ ����� ������ � ������ �����
const BYTE HaveFiniteVerbBetween = 1;
	// ������, ���� ����� ����� ������ ����� ������ ���� c ������� ��� ��� ���
	// ( � ������ ������).
	// �������� 1. ���� � ����, 
	//			2.������, ��� � ������
	// ��������������, ��� � ���� ������ ����� ����� ����� ������ ������ �������� 
	// ������� ��������� �������.
const BYTE HaveOnlyConjBetween = 2;
	// ������, ���� ��� ���� ����� ������ ���� ������� ����� �����
	// � ������ ������
const BYTE HaveOnlyCommaBetween = 4;
	// ������, ���� ��� ���� ����� ������� ����� �����
	// ����� ����� ���-�� ���
const BYTE HaveCommaBetween = 8;
    // ��. ����������� � ������� AreOldNodesOfTheSameCase
const BYTE OldNodesOfTheSameCase= 16;


// ==== ����� "������� ������"
class CRusSemClause : public CSemClause 
{
	// �������, � ������� ��������� �������� ��������� �����������  ������ ����� 
	vector < vector<BYTE> > m_GrammarMatrix;
public:
	// ======= ����� �������������� ����������
	// �������������� ����� ������
	long						m_SyntaxClauseNo;
	// ����� �����������  (�� ��������������� ������� - �������������� ����� ������ � VisualSynan) 
	long						m_SubjectWordNo;
	// ����� ���������� (�� ���������� - �������������� ����� ������� � VisualSynan)
	long						m_PredicateWordNo;
	// ����� ������� ������
	long						m_RootWordNo;
    // ��� ������ 
	long						m_ClauseType;
	// ����� ���������������� �������� ������
	long						m_ClauseVariantNo;

	// =======  ��������� ������������� 
	// ������� ����� ��������� �������������
	long						m_CurrLexVariantNo;
	// ����� � ������� ����������� ������� ���� ����� (��� ���������� ��������������� �������� 
	// ����� ������ ������������� ������ ���� ���)
	long						m_AlreadyBuiltClauseVariantNo;

	// ����� ����, ������� ��������� �������� ������ (�� ��������������� ��������)
	// ���� ����� �������� ����� ����� ����� �������� m_RootWordNo,
	// �� ���� m_PredicateWordNo = -1, ����� ��������� ���� ���� �������
	long						m_ClauseSyntaxTop;
	// ����� ������, ������� ��������� ������ ������ � ������� ������������ ���������
	long						m_HostClause;
	// ����� �������, �� �������� ���� ���������� ����������� ���������
	long						m_ClauseRuleNo;
	// ���������� ����������� ��������������� ��������� �����
	vector<CSyntaxClauseVariant> m_AddedClauses;
	// ����� ������� ���� ������
	long						m_BeginNodeNo;
	// ����� ���������� ���� ������
	long						m_EndNodeNo;
	// ����, ��� ���������� �������� ������� �����
	stack<long>			m_NodeRef;
	// ���� � ������ ���� "������"  ��� "������",  �� ������ ���������� ����� ������ � �����������  
	bool						m_bHasNumeralComp; // ������ ��� ������

	// ���� �� � ������������� ���� "���"
	bool						m_bHasCHEM;
	// ���-�� �����, ������������� ���������� ������� �����������
	int						    m_BestPONodesCount;
    // ������ � ������  ������ � ����������
	bool						m_bSaveClauseHasNegationInVerb;



	// ����������� ����� ����� WordNo (�� ��� ����� ����� ���������� ����� �����)
	void FreeWordNo (long WordNo);
	CRusSemClause();
	// ������ ����� ����� ������
	long GetNodesCount() const;
	
	void InitGramMatrix ();
	void AddFlagToGramMatrix (long NodeNo1, long NodeNo2, BYTE Value);
	// ������ �������� ����������� ������ ��� ���� ����� 
	const BYTE GetGramMatrix (long NodeNo1, long NodeNo2) const;
	void UpdateSizeOfGramMatrix();
	

	// ��������� � ��������������� ������ �� ���� 
	void SaveNodesReferences();
	void RestoreNodesReferences();
	void IncrementNodesReferences(long Delta);
	void PopNodesReferences();
};



enum DirectEnum {
	LeftToRight,
    RightToLeft,
	Indifferent
};
struct CRossInterpAndLemma : public CRossInterp {
	string m_Lemma;

	CRossInterpAndLemma() {
		m_DictType = NoneRoss;
		m_UnitNo = ErrUnitNo;
	};

	bool operator == ( const CRossInterpAndLemma& X ) const
	{
		return	   (X.m_DictType == m_DictType) 
				&& (X.m_UnitNo == m_UnitNo)
				&& (X.m_Lemma == m_Lemma)
				;
	};
	
};


class CRusSemStructure;
// ���� �������� �������, ������� �������� ���� �������� ���������
typedef bool (CRusSemStructure::*TClauseRule)(long, long, long);	  
struct CSemanClauseRule {
	// ����������, ���� �������
	TClauseRule		m_Rule;
	// ����������� ���������� ����� �������
	DirectEnum		m_Direction;
	// ��� �������
	string			m_Name;

	CSemanClauseRule()
	{
		m_Rule = 0;
	}
	CSemanClauseRule (TClauseRule Rule, DirectEnum Direction, string Name)
	{
		m_Rule = Rule;
		m_Direction = Direction;
		m_Name = Name;
	};
};

typedef vector<CRusSemNode>  CRusSemNodeVec;


struct CConj {
	const char* conj;
	const char* antecedent;
	const char* RelationStr;
};

// ���� ����� ������������ ���� ������ ��� ���������� ����� �������
struct CNodeHypot {
	// ����� ����
	long m_NodeNo;
	// ���� � ���� LEXi
	long m_LexFetNo;
	// ���� � ���� GFi
	long m_GramCortegeNo;
	// ����� �������� � ������� ��������
	long m_PrepNo;
	// ����� �������� � ���� PREP
	long m_PrepNoInArticle;
	// �����, ����  GF = ��_������ ��� ����� ����
	bool m_bShouldBeNumeral_P; 

	CNodeHypot (long NodeNo, long LexFetNo, long GramCortegeNo, long PrepNo, long PrepNoInArticle, bool bShouldBeNumeral_P)
	{
		m_NodeNo = NodeNo;
		m_LexFetNo = LexFetNo;
		m_GramCortegeNo = GramCortegeNo;
		m_PrepNo = PrepNo;
		m_PrepNoInArticle = PrepNoInArticle;
		m_bShouldBeNumeral_P = bShouldBeNumeral_P;
	};

	CNodeHypot (long NodeNo, long GramCortegeNo)
	{
		m_NodeNo = NodeNo;
		m_GramCortegeNo = GramCortegeNo;
		m_LexFetNo = -1;
		m_PrepNo = -1;
		m_PrepNoInArticle =  -1;
		m_bShouldBeNumeral_P = false;
	};

	CNodeHypot ()
	{
		m_NodeNo = -1;
		m_GramCortegeNo = -1;
		m_LexFetNo = -1;
		m_PrepNo = -1;
		m_PrepNoInArticle = -1;
		m_bShouldBeNumeral_P = false;
	};
};

typedef vector<CNodeHypot>  CNodeHypotVector;

enum ClauseRulesTypesEnum
{
	ProminentClauseRules,
	AllPossibleClauseRules
};


template<class T>
 void IncrementIndexBinaryRelation (vector<T>& Vec,  long StartNodeNo, long Delta)
	{
	 for (long i=0; i < Vec.size(); i++)
	 {
		if (Vec[i].m_TargetNodeNo >=  StartNodeNo) 
			Vec[i].m_TargetNodeNo += Delta;

		if (Vec[i].m_SourceNodeNo >=  StartNodeNo) 
			Vec[i].m_SourceNodeNo += Delta;
	 };
	};

/*
 � ����  ������ ���������� ������� � ���������� � ������
 ������ ����� ��������
*/
class CLexVariant : public CTreeOfLexVariantWeight
{
	public:
	// �������� ���� ����
	vector<CRusSemNode>			m_Nodes;
	// �������� ���� ����
	vector<CRusSemRelation>		m_Relations;

	// �������� ���� �������������� ����
	vector<CRusSemRelation>		m_DopRelations;

	// ������ �����������
	vector<CRusSemClause>		m_Clauses; 


	// �������� ���� ���� (��� �� �������� ���������)
	vector<CSynRelation>		m_SynRelations;

	
	// �������� ���� LF, ��������� � �����������
	vector<CLexFunctRel>		m_LexFuncts;

	// �������� ���� �������������� ���������, ������� ������
	// �� ���� AUX ��������, ������� ����� ��������
	vector<CSemThesRelation>	m_ThesSemRelations;

	CLexVariant ( )	 {}
	void CopyLexVar ( const CLexVariant& X)
	{
		m_Nodes = X.m_Nodes;
		m_Relations = X.m_Relations;
		m_DopRelations = X.m_DopRelations;
		m_Clauses = X.m_Clauses;
		m_SynRelations = X.m_SynRelations;
		m_LexFuncts = X.m_LexFuncts;
		m_ThesSemRelations = X.m_ThesSemRelations;
	};
	void IncrementNodeRef (long StartNodeNo, long Delta) 
	{
	    IncrementIndexBinaryRelation(m_Relations, StartNodeNo, Delta);
		IncrementIndexBinaryRelation(m_SynRelations, StartNodeNo, Delta);
		IncrementIndexBinaryRelation(m_LexFuncts, StartNodeNo, Delta);
		IncrementIndexBinaryRelation(m_ThesSemRelations, StartNodeNo, Delta);
		IncrementIndexBinaryRelation(m_DopRelations, StartNodeNo, Delta);
	
	};

};

struct  CSemClauseVariantResult : public CSyntaxClauseVariant {

	// ���� ���� ����������, ��� ��������� ���� �������, 
	// ��  �� �������������������
	bool m_bNew;
	// =====  ������������ ����������
	// ����������� ������
	vector<CSyntaxClauseVariant>	m_AddedClauses;

	//======  ���������� � ����� ������ ����������� ��������� 
	vector<CLexVariant> m_BestLexVariants;

	//======  ��������������� ���������� ��� ����������
	long m_NodesCount;
	long m_PanicRelationsCount;
	/*
	 ����� ������, ��� ������� ��� �������� CSemClauseVariantResult ���,
	 ��� �� ����������� �� � CSemNode::m_ClauseNo �� �����
	 m_BestLexVariants
	*/
	long m_BuiltClauseNo;

	
	

    CSemClauseVariantResult (long SyntaxClauseNo,
	                         long ClauseVariantNo,
							 const vector<CSyntaxClauseVariant>&          AddedClauses
							 )  :  CSyntaxClauseVariant(SyntaxClauseNo,ClauseVariantNo)
	{
		m_AddedClauses = AddedClauses;
		m_NodesCount =0;
		m_PanicRelationsCount = 0;
		m_bNew = true;
	};

    bool operator == ( const CSemClauseVariantResult& X ) const
	{
		return    (X.m_SyntaxClauseNo == m_SyntaxClauseNo) 
			   && (X.m_ClauseVariantNo == m_ClauseVariantNo)
			   && (X.m_AddedClauses == m_AddedClauses);
	};
	string  GetStr();


};
const int MaxRelSet = 100;
struct CRelSet {
 WORD	 m_Rels[MaxRelSet];
 BYTE	 m_RelsCount;
 CRelSet () 
 {
	 m_RelsCount = 0;
 }
 void Add (WORD RelNo)
 {
	 assert(m_RelsCount < MaxRelSet-1);
	 // � Release ������ �� ����� ��������� ���������, ���� �� ������� �����
	 if (m_RelsCount >= MaxRelSet-1)
		 return;
	 m_Rels[m_RelsCount++] = RelNo;
 };
 void Insert (WORD No, WORD RelNo)
 {
	 //assert(m_RelsCount < MaxRelSet-1);
	 // � Release ������ �� ����� ��������� ���������, ���� �� ������� �����
 	 if (m_RelsCount >= MaxRelSet-1)
		 return;

	 for (long i = m_RelsCount; i > No ; i--)
		 m_Rels[i] = m_Rels[i-1];
	 m_RelsCount++;
	 m_Rels[No] = RelNo;
 };
 void  Erase (WORD No)
 {
	 //assert (No < m_RelsCount);
	 for (long i = No; i < m_RelsCount-1 ; i++)
		 m_Rels[i] = m_Rels[i+1];
	 m_RelsCount--;
 };
};




typedef pair<long, long> PairOfLong;

struct CSemCollocHyp 
{	
	struct CCollocItemNoAndWordNo 
	{
		// an index to a colloc item in Content field
		long  m_CollocItemNo;
		// an index of a word in the input sentence
		long  m_WordNo;
	};
	long							m_CollocNo;
	vector<CCollocItemNoAndWordNo>  m_Coords;  

	CSemCollocHyp (long  CollocNo)
	{
		m_CollocNo = CollocNo;
	}
};
typedef vector<CSemCollocHyp> CSemCollocHypVector;
typedef vector<CSemCollocHypVector>  CSemCollocHypVectorVector;



class CRusSemStructure  : 
	public CSemanticStructure, 
	public CMyTimeSpanHolder, 
	public CLexVariant
{ 
public:
	// ������� ���� ������ �����������
	const CSentence*			m_piSent;
   // ����� �������� ���������� ������ 
	long						m_ClauseVariantsCombinationNo;
	// ����� �������� ���������� ������, ������� ������� ������������
	long						m_UserClauseVariantsCombinationNo;
	// ����� �������� ������, ������� ������� ������������
	long						m_UserTreeVariantNo;

	// ��������� �������� �������� ������� � ��������������� ��� ������ ������ 
	vector<CSemCollocHypVectorVector>	m_ClauseSetCollocHyps;


	//��� ���� � ��� ���������, ������� ���� ��������� �� ���� ���������
	vector<CMemNode>			m_MemNodes;
	vector<CRusSemRelation>     m_MemRelations;
	long						m_MemoryCaseNo;
	long						m_MemSaveRelationCount;
	size_t						m_SentenceCount;
	map<pair<int,CRusSemNode>, int>		m_PriorNounNodes2Distance; 
	// ����������� ����������� ���-�� ��������� ��������
	long						m_PanicTreeVariantCount;
	// ��������� ���� ������ �������������� ��������� (������� ����� �������� �� ���������)
	StringVector				m_WeakSynRels;
	// �����������  ����������� �������� (��������������� �������������)
	vector<CRossInterpAndLemma>			m_UserProhibitedLexVars;
	// ����� ��������(���������������) ���������			
	long						m_ClauseCombinationVariantsCount; 
	// ����������, � ����� ������ ����� ��������� ����������� �������, ���� 
	// m_ClauseConnectionType != AllPossibleClauseRules, ����� ��������� ������� �� ��������
	ClauseRulesTypesEnum        m_ClauseConnectionType;
	/* �� ����� ��������� ����� � �������  ����� m_bLastTry ����������� ��������� ��������
       �� ��������� �������
	*/
	bool						m_bLastTry;
	// ������� � ���, ��� ������������ ����� ���������� ���� �������
	bool						m_bShouldBeStopped;
	// ���������� ��� ProgressBar
	long						m_InterfaceClauseNo;
	long						m_AllClausesVariants;

	//  ���������� ��� ������� � ���, ����� ����� ���� �������� �� ������
	map<string,string>			m_SynthLemmaToReplace;

	// �������� ���� ����
    const CSemNode&		GetNode(int NodeNo) const;
	CSemNode*			GetNodePtr(int NodeNo);
	int					GetNodesSize() const;
	string				GetInterfaceWordStr(const CSemNode* pNode, int WordNo) const;
	void				EraseNode(int NodeNo);
	

    // �������� ���� ����
    const CSemRelation*	GetRelation(int RelNo) const;
	CSemRelation*		GetRelation(int RelNo);
	int					GetRelationsSize() const;
	void				EraseRelation(int RelNo);
	void				AddRelation(const CRusSemRelation& R);
	void				GetColorAndWidthOfRelation(int RelNo, float& Width, string& Color);
	// �������� ���� �������������� ���������, ��������� � �����������
    const CSemRelation*	GetDopRelation(int RelNo) const;
	CSemRelation*		GetDopRelation(int RelNo);
	int					GetDopRelationsSize() const;
	void				EraseDopRelation(int RelNo);

	size_t				GetSynWordsCount() const;


	// ���������, ��� ����, ������� ��������� ��������� �� �������� ������������
	bool						IsWordContainerRel(int iRel)  const;
	const vector<CSynRelation>&	GetSynRels() const;
	vector<CSynRelation>&		GetSynRels();
	const vector<CLexFunctRel>&	GetLexFuncts() const;
	vector<CLexFunctRel>&		GetLexFuncts() ;


	const CSemClause&			GetSemClause(long ClauseNo) const;
	const long					GetSemClausesCount() const;
	void						FindPossibleAntecedents();

	// ����������� �������������� ���������
	vector<CSynRelation>		m_SynClauseRels;
	
    // ������������, � ������� ������� ���������� �������������� ���������
	TreeVariantValueCoefs	m_SemCoefs; 


	// ����������� ��������������� �������� �����
	vector<CSemClauseVariantResult>  m_AlreadyBuiltClauseVariants;    

	//������ ���� ������ ��� ����������� ������
	vector <CSemanClauseRule>  m_ClauseRules;



    CRusSemStructure();
	//=====================   �������� �������
	// �������� ������� ���������� ����������� 
	long						FindSituations(size_t SentNo);
	// ���������� ����� �� ��������������� ������� � ������ ���� (������������� ����������)
	void						BuildSemNodesBySyntax();
	// �������� ������� ��� ���������� ���� ��� ������ ���������������� �������� ����� ������
	long						FindSituationsForClauseVariantCombination();
	// ���������� ������������� ��������� 
    void						BuildAnaphoricRels();
    // ���������� ������ ������������ �������� ����� ������
	CLexVariant					BuildTheVariant(long ClauseNo);
	// ���������� ����� �������������� ������  ��� ������ ������������ ��������
	bool						BuildHypotRelationsGraph(size_t ClauseNo);
	// ��������� ��������� ���������� ����������� ������
	void						ConnectClausesForLexVariantCombination();
	// ���������� LF-���������� ��� ���� ������, �������� ������� ������ - ������ ("������� �������")
	void						BuildVerbLexFunctParameter(long ClauseNo);
	// ���������� LF-����������, �������� ������� ������ - ������, ��� ���������� ����� 
	void						BuildVerbLexFunctParameterForTheNodes (long  SitNodeNo, long ParamNodeNo);
	// ���������� LF-����������, �������� ������� ������ - �������������� ��� ������� ("��������� �����")
	void						BuildAdjOrAdvLexFunctParamNodes(long ClauseNo);
	// ������ ����� LF � ����� ������
	size_t						CountLexFunctsInOneClause(long ClauseNo);
	// ������ ������ ���������
	long						GetStructureWeight ();


	//======================    ������ � ��������������� �����������  (����)

	// ������ ��� �������� � ���� NodeNo ����
	void			GetIncomingSynRelations  (long NodeNo, vector<long>& Relations) const;
	// ������ ����� ����������� ���� NodeNo ����
    long			GetSynRelationsCount (long NodeNo) const;
	// ������ ����� ����, �� �������� ������� ���������, ������ � NodeNo, ���� ����� ���������, �������� � NodeNo,
	// ����� 1, � ��������� ������ ������ -1
	long			GetSynHost (long NodeNo) const ;
	// ������ ����� �������� � ���� NodeNo ����
	long			GetIncomingSynRelationsCount (long NodeNo) const;
	// ������ ����� ��������� �� ���� NodeNo ����
	long			GetOutcomingSynRelationsCount (long NodeNo) const;
	// ������ ��� ��������� �� ���� NodeNo ����
	void			GetOutcomingSynRelations (long NodeNo, vector<long>& Relations) const;
	// ������� �������������� ���������
	void			DeleteSynRelations(vector<long>& Rels);
	// ������� �������� � ��������� ��������� ����, ������ �������� rName
	void			DeleteSynRelationsByName(long NodeNo, string Name);
	// ��������� �������� �������������� ��������� �� FromNode � ToNode
	void			MoveIncomingSynRelations(long FromNode, long ToNode);
	// ��������� ��� �������������� ��������� �� FromNode � ToNode
	void			MoveSynRelations(long FromNode, long ToNode);
	// ������ ������, ���� � ���� ������ ���� �� ���� "�������" �������������� ���������
	bool			HasIncomingNotWeakSynRelation (long NodeNo) const;
	// ������ ����� ������ ���� � �����, ��� �� X � NodeNo ���� ���� �� "�������" ��������������
	// ���������.
	long			GetDefiniteRightHostByNotWeakSynRelation (long NodeNo) const;
	// ���������, ���  � ���� NodeNo ���� �������� ��� ��������� ��������� RelationStr
	bool			HasSynRelation(long NodeNo, string RelationStr) const; 
	// ���������, ���  � ���� NodeNo ���� ��������� ��������� RelationStr
	bool			HasOutcomingSynRelation(long NodeNo, string RelationStr, long NodeNo2 = -1) const; 
	// ��������� ���������� �����������
	void			HeuristicsLocative(long ClauseNo); 
	void			DeleteSynRelationsOfClause(long ClauseNo);
	
	

	//=====================   �������� ���������
	
	//������ �� ����  ��� ����������
	long						GetRusSubj(int NodeNo);
	// ���������, ��� ������ ���� - ������������� ����
	bool						IsCoordConj(long NodeNo)  const;
	// ���������, ��� ������ ���� - ������� �������������� ����
	bool						IsSimpleSubordConj(long NodeNo)  const;
	// ���������, ��� ������ ���� - ������� �����
	bool						IsConjWord (long NodeNo) const;
    // ���������, ��� ������ ����  �������� ������� ������ ��� ��������
	bool	IsParenthesis (long NodeNo) const;
	// �������� ��� ���� �� ������ ClauseNo, ������� �������������  ������� Fun2
	// � �� �������� m_bToDelete
	void          FindIf(long ClauseNo,bool (CRusSemStructure::*Fun)(long) const,vector <long>& Nodes) const;
	// ������ ������ �����  WordNo ��� ������� ��������������� ������������� ������ ClauseNo
	bool  GetHomonym (long ClauseNo, long WordNo, CSynHomonym& Homonym) const;
	// ��������� ����� ���� ����
	bool   HasRichPOS (const CRusSemNode& N, size_t POS) const;
	bool   HasRichPOS (size_t NodeNo, size_t POS) const;
	bool IsVerbForm(const CRusSemNode& Node) const;
	bool IsInfinitive(long NodeNo) const;
	bool IsFiniteVerb(long NodeNo) const;
	bool IsModalCopul(long NodeNo) const;
	bool IsImperativeVerb(long NodeNo) const;
	bool IsPredicative(long NodeNo) const;
	bool IsCompAdj (long NodeNo) const;
	bool IsPrep(long NodeNo) const;
	bool IsAdjFull(long NodeNo) const;
	bool IsSimpleNounGroupUnderPrep(long NodeNo) const;





	CRusSemNode					CreatePronounByLemma(string Lemma);
    CRusSemNode					CreateAnaphoricPronoun (long PrototypeNode); 
	CRusSemNode					CreateCopulNode(long ClauseNo);
    CRusSemNode					CreateModalCopulNode(long ClauseNo);

	//=====================   ����������� �����  � ������ � ��������
	// ��������������� ������� ��� ������ ����������� ������
	// �� �������� �� ��������� ��������� ����� NodeNo1 � NodeNo2 ������ �������������
	bool			CanHaveRightClauseRelation (long NodeNo1, long NodeNo2) const;
	// ���� ������ ������ ����������� ���� � ������ "�������" ��� "�����"
	long			GetKOTORYJorKAKOJ (long ClauseNo) const;
	// ������� ��������� ����� �������� 
	bool            CreateClauseRelation(long ClauseRuleNo, long ClauseNo1, long ClauseNo2, CValency V, long TargetNodeNo, CDictUnitInterp Conj, long SourceNodeNo = -1);
	// ���������, � ������ ������ ����� �������������� ���� � ������� ���
	const CDictUnitInterp*    GetSubordConjFromTheBegining (long ClauseNo) const;
	// ���������, ��� � ������ ������ ����� ������� ������������� ���� � ������� ���
	long            GetCoordSimpleConj (long ClauseNo) const;
	// ���������, ��� � ���� ���� ������������� ����������� � ���������� ���������������� 
	bool            GetFreeActantPattern (long NodeNo, CSemPattern& P, CSynRealization& SynReal, string SynRel, bool CheckSynRel,  string SynFet, bool CheckSynFet);

	// ����������, ����������� ������� (��. ������� � ������������)
	bool			TryClauseCHTOBY_GG (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseConditionalImperative (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseCHTO_WITH_ANIMAT (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseKOTORYJ (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseZeroSubordWithoutAntecedent(long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseSubordConj(long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseCHTO_TOT_Dop(long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseSimpleComma (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	long			GetClauseCoordSimpleConj (long ClauseNo1) const;
	// ������ �������� ����� � ���� �� ���������� � ����������� (���� ��������� ����)
	QWORD			GetPersonAndNumberGrammemsOfPredicate (long NodeNo);
	bool            TryClauseCoordSimpleConj (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	// ���� ������� ����� � ������ ������, ���� �������, ����� ������ ����� ����
	long			GetConjWordFromClause (long ClauseNo) const;
	bool            TryClauseConjWord(long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseSubordDoubleConj (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseAnaphoricSubordWithoutAntecedent (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClausePrichastie (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseDeeprichastie (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseParenthesis (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryBracketClause (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseNSO (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	
	// ��������� ��� ����������� �������
	void            ApplyClauseRulesOnly(vector<PairOfLong>& ClausePairs);
	// ������������� ��� ����������� ������� � �������� ApplyClauseRulesOnly
	void            ApplyClauseRules(long GapSize);

	void            FindMotherLandForPustycha();
    // ���� ����� ������ ������ ���������� � ����� '�', �� ����� ������� ��� 
	void            ProcessParticipleAndInTheFirstClause();
	// ���� � ������ ����  ���� '�', ������� �� � ���� �� �������������
	void            ProcessIsolatedParticipleAnd();
	// ������, ���� ������ �������� ���� �� ���� �������������� � ������������� �������
	bool            ClauseHasCompAdj (long ClauseNo) const;
	// ������, ���� ������ �������� ������������� ���� "���"
	bool            	ClauseHasChem (long ClauseNo) const;
	// ������, ���� ������ �������� ���� �� ���� ���������
	bool            ClauseHasNegation (long ClauseNo) const;
	// ������, ���� ������ �������� ���� �� ���� ��������� ��� �������
	bool            ClauseHasNegationInVerb(long ClauseNo) const;
	// ���������  ������ �� �������
	bool			IsEmptyClause(long ClauseNo);
	// ������� ������ ������
	void			DeleteEmptyClauses();
	// ������� ������
	void			DelClause(long ClauseNo);
	// ������ ����� ������� ����� ������
	long	GetClauseFirstWordNo(long ClauseNo) const;
	// ������ ����� �����������  ����� ������
	long	GetClauseLastWordNo(long ClauseNo) const;
	


	//=====================   ��������������
	// ������� ��� �������� ��������������
	void			FindCollocsHyps(long ClauseNo);
	// ��������� ����������� ����������� ������������� ��-��� ������� �� ���� StartNodeNo
	void			AddCollocHyp (long StartNodeNo,vector<long>& RefCollocItems,long ItemNo, CSemCollocHyp& Hyp,vector<CSemCollocHyp>& Hyps);
	// ��������� �������������� ����������� �������� ��������������  CollocNo ��� ���� NodeNo
	bool			CheckCollocItemGramFet(long CollocNo, long ItemNo, long NodeNo);
	// ������� ���� �������� �������������� 
	void			BuildBlackBoxCollocs(long ClauseNo, size_t SetNo);
	

	
    


	// ===========     ���������� ����

	long			GetLocInterp(string UnitStr, bool& NegativeForm) const;
	void			BuildParataxNodes(long ClauseNo);
	void			PrintAllLocNodes(); 
	bool			ReadLocProperNames();
	bool			GetLocSemRel(long NodeNo, string& RelationStr, long& PrepNo) const;
	long			FindLocHost (long NodeNo, long ClauseNo);
	void			PutLocRelations(long ClauseNo);
	
	// ===========  ��������� ������������ MNA
	// ���������, ��� NodeNo - ���. ����. ������
	long			GetRepeatConj (long NodeNo);
	// ������������ ���. �����, ������� �� �������� ���� MUA, � MUA � ������� �������� ������ ������	
	void			ConvertFreeCoordConjToMNA (long ClauseNo);
	// ���������, ��� � ������� ���-��������� �� ������� ���� �� ���  �������
	long			GetMNAViolationsCount(long Tag);
//  ������ ����� ���������� ������������� (MUA), � ������� ��� ����� ����� ���� �� ����	// �����  SF
	long			GetSemFetAgreeMNACount(long Tag);
	// ������ �������������� ������ ����� ����������� ���� �� ������ ������� �������
	void			GetMNAPattern(size_t NodeNo, long& Poses, QWORD& Grammems) const;
	// ������ �������������� ������ ����� ����������� ����, ����������� �� ������ ������� �������
	long			GetFirstMNAMemberIfHas(long NodeNo) const;
	// ����� ���� ���������, ������ �� ��������� ������������ NodeNo
	void			GetMNARelations(long NodeNo, vector<long>& Rels, bool UseUse) const;
	// ����� ���� �����, � ������� ���� ��������� �� ��������� ������������ NodeNo
	void			GetMNAMembers(long NodeNo, vector<long>& Nodes, bool UseUse) const;
	// �������������� �������� ������������, ����������� ����������
	void			InterpretSimilarNounGroups (long ClauseNo);
	// ������ �������� ������������ ��� �������� ���� ���� � ������
	bool			DealWithNodePrepS(long NodeNo);
	//"� �� ���� �������� ������, ��� ����"
	void			BuildOutcomingOf_CHEM_Obor(long ClauseNo);
	// "� �� ���� �������� ������ ����"
	void			BuildOutcomingOf_COMP_Obor(long ClauseNo);
	// "�������� ������������ ���������� ��� ����������� �������"
	void			BuildMNA_KAK(long ClauseNo);
	// ������� ��� ��������� ������������ � ����� ��������
	void          DeleteMNAWithOneChild(long ClauseNo);
	// ������ ��������� ��������� ��� ��������� ������������
	void          BuildMNAOutcoming(long ClauseNo);
	// ������ �������� ��������� ��� ��������� ������������
	void          BuildMNAIncoming(long ClauseNo);
		
	// ======= ������ �������
	// ������ ��� ������ ������� ��� ��������� ������
	void            BuildTimeNodes(long ClauseNo);
	// ���������, ��� ������� �������� � ������ ������� ����������� � ����� SYNREP
	bool            TimeHypotIsSyntaxAgree (CNodeHypotVector& V, CTimeUnit& U);
	// ���������, ��� ���� ��������� SF = TIME	
	bool            IsIntervalNode(const CSemNode& N) const;
	// ��������� ����������� ������������ ���� �������� ���� CONTENT
	bool            CheckOneTimeWord(CTimeUnit& TimeUnit,  CNodeHypotVector& Hypots, BYTE PlaceNo, long& AbbrFunctNo);
	// ��������� �������������� ������������ ���� �������� ���� CONTENT
	bool            CheckTimeGramFet(CNodeHypot&  Hypot, CTimeUnit& TimeUnit, BYTE PlaceNo, bool IsHole, long Numbers);
    // ��������� �������������  ������������ ���� �������� ���� CONTENT
	bool            CheckTimeSemFet(long NodeNo, long UnitNo, BYTE PlaceNo);
	// �������, ����, ������� ����� ��������� ����. ���� "�����"
	long            FindTimeHost (long NodeNo, long ClauseNo);
	// ��������� SF �� ���������
	bool            HasTimeSemFet(const CSemNode& W, string SemFet) const;
	// ��������� ����. ����. DictLemma �� ������ W (� ������ Abbr)
	bool            IsEqualTimeWord(string DictLemma, CRusSemNode& N, long& AbbrFunctNo);
	// ������ ��������� ������������� ��� ����� Lemma
    void			GetTimeInterps(string Lemma, string WordStr,  string PostPuncts, CRusSemNode& N) const;
	// ���������, ��� ���� NodeNo ����� ��������� GFi = ��_x
	bool			CheckTimeNumeral(long NodeNo, string GramFet) const;

	// ===== ������������� ���������� � ��������� ��������������� �������� 
	// ������������� ���������� ������ �����
    void						InitWordFeatures(long WordNo, CRusSemWord& SemWord) ;
	// �������� ������������ ���� �� �����
	CRusSemNode					CreatePrimitiveNode(size_t WordNo);
	// �������� ������������ ���� �� ������������� ������
	CRusSemNode					CreateNode(const CRelationsIterator* RelIt, long GroupNo);
	// ������������� ���������
	void			InterpretPrepNouns(long ClauseNo) ;
	// ������������� �����_��
	void			InterpretSelectiveRelations(long ClauseNo) ;
	// ������������� ������� ������
	void			InterpretRepeatConj(long ClauseNo) ;
	// ������������� ����_��� (� ����� ������������� ������)
	void			InterpretAdjNounGroupsAfter ();
	// ������ ��������� ����� ����� "���" � ���-�� ���
	void			BuildIZCHORelation(long ClauseNo);
	// ������������ ������� ���� "��� ��� ���"
	void			BuildPustychaWithETO(long ClauseNo);
	// ��������� �������������� ������� ������
	long			GetClauseSyntaxTop(long ClauseNo) const;
	// ������ ��������� ��������� 
	void			InitRelationOperators (long ClauseNo);



	// ======= ���� �������������� ������ � ���������� ��������� �� ����������
	// ��������� ���� ��������� ������������� 
	void			InitInterps (CRusSemWord& W, bool PassiveForm, long ClauseNo, CRusSemNode& N, bool UsePlugArticle = true) const;
	// ��������� ��������� ������������� �� ���������
    void			GetThesInterps(string UnitStr, const CRusSemWord& W, const CThesaurus* Thes, DictTypeEnum	 DictType, CRusSemNode& N) const;
	// ������ ����� ������ � ������������ ����� ��  ������ �������
	WORD			GetArticleByModel (long TerminId, int ThesaurusId) const;
	// ������ ��������� ������������� � ����� ��� ���������
	WORD			GetInterpWithoutPrefix (CRusSemWord& W) const;
	// ������������� ������������ � ������� ����. �������������
	bool			InitValsRussian(long NodeNo);
	// ������������� DOMAIN
	void			InitPOs(long ClauseNo);
    // ���������� SF ��� ����������� �����������
	void			InterpretOrganisations (long ClauseNo);
	// �����������  ���� ����������� ����� � ����, � ������� �� ������ ��������  Grammems,
	// � �������. ����������� �����  m_Grammems � ����.
	void			DeleteAllGramCodesWithoutTheGrammems (CRusSemNode& N, QWORD Grammems);
	// �������� ��������������� ���������� � �����. � ������ RESTR ������� ��������� �������������
	bool			ApplyGramRestr (size_t ClauseNo);
	//  ������������� ������� ����������� �������
	void            InitGrammarMatrix1 (long ClauseNo);
	//  ������������� �������������� ������� � ��� �� ������� 
	void            InitGrammarMatrix2 (long ClauseNo);
	// ���������� ���� �����. ��������� �� ���� NodeNo
	void			FindActants(size_t NodeNo);
	// ������ �� ������� ������ ����������� ValencyNo
	void			GetActantPattern(size_t NodeNo, BYTE ValencyNo, CSemPattern& P);
	// ���������� ���� �����. ��������� �� ���� NodeNo �� ������ P
	void			TryPatternOnNodes(size_t NodeNo, size_t ClauseNo, CSemPattern& P);
	// �������� ����, ��� ������� ��������������� ���������� RelationStr
	bool			CheckSemanOfPrep(string RelationStr, bool IsAdditionValency, long NodeNo, long& PrepNo);
	// �������� ����, ��� ���� NodeNo ������������� ������ �������� ���� GF
	bool			CheckPatternGramFetLine (CSemPattern& P,  CSynRealization& SynRealization, size_t NodeNo);
	// �������� ����, ��� ���� NodeNo ������������� ������ �������� ���� GF, ������ ��i = X!:*
	bool			CheckPatternReverseGramFetLine (CSemPattern& P,  CSynRealization& SynRealization, size_t NodeNo);
	// �������� ����, ��� ����� ����� ������ ���� ���� �� ���� ���������������
	bool			HaveNounNodeBetweenInOneClause (size_t NodeNo1, size_t NodeNo2) const;
	// �������� �� ���� ��������� GFi � ��������� CheckPatternGramFetLine
	bool			IsPattern (CSemPattern& P, size_t NodeNo, string& SyntacticRelation, CSynRealization& SynRealization);
	// ��������� ��������� ����������� ����� (� ������ ������ ����� ("���"))
	bool          LoadFromDictForPassive(WORD UnitNo, BYTE LeafId, BYTE BracketLeafId, CSemPattern& P);
	// �������� ������� ������ 
	void			DeleteLongRelations (long ClauseNo, long MaxRelationLeapsCount, bool bOnlyNouns);
	
	

	// ========  ������ ��������
	// �������� ������� - ������ ������ ������� �� ����� �������������� ������  (����������� ���� ����� ���� �� ������)
 	long			GetTreeByConnectedComponents (size_t ClauseNo, TreeVariantValue& ResultValue);
	// �������� ��� ���������� ��������� ������� ������ (CSemNode::m_TagId)
	long			MarkConnectedComponentsWithoutUniqueRelations(long ClauseNo);
	// ������� ����� ���������  ���������� 
	long			GetConnectedComponentsCount(long Tag);
	// ���������, ��� ������ ClauseNo �������
   	bool			IsConnectedClause(size_t ClauseNo);
	// ������� ���� �� ������� ������� (�� �������� � ������ �������) � �������� ����������� �������  
	// (������������ ������������� ���������)
	void			dfs_quick_for_connected_component(size_t NodeNo);
	// ������� ���� �� ������� ������� (�� �������� � ������ �������) � �������� ����������� ������� 
	// (������������ ������� �������� ���������)
	// ���� Tag != -1, �� ����� �� ���� �� ���������� ����������
	void			dfs(size_t NodeNo, bool ConsiderUseOfNodes,long Tag = -1);
	// �������� ������� ����� � ���������� ����� ����� (����, �������� � ���� ���������� m_bInCycle)
	bool			FindCycle (long Tag);
	// ����� ����� � ������ ������ 
   	bool			dfs_for_cycle_search (size_t NodeNo, size_t GoalNodeNo);
     // ������ ����� ������� ��� ����� ���������� ���������� 
	void			GetTree (long Tag, TreeAndValueVector& VarAndVals);
	// ��������� ���� ������ �� ������������� ���������
	void			ValueTreeVariant (TreeVariantValue& Value, long ClauseNo);
	// ��������� ���� ������ �� ������������� ���������, ������� ���������� ���� ������
	// �.�. ������� �� ����, ������� ����� ��� ���� �����������,
	// ��������������� �������� GetTree
	void			ValueTreeVariantAdditional (TreeVariantValue& Value, long ClauseNo);
    // �������� ���� ��������� �� SF
	void			FindSemFetDisagree(long Tag);
	// ������ ���������� ���������, �� ���������  �������� ���� ��������� �� SF
	long			GetSemFetDisagreeCount (long Tag);
	// ���������, ��� ������� ������� ��������  �� �������������� ��������� 
	bool			TopAgreeWithSyntaxCriteria (long Tag);
	// �������� ���������, ������� �� ����������� � ������ ���� ����, � ������� ��� ������
	void			MarkSemRelPOSViolations();
	// ������ ����� ���������, ������� �� ����������� � ������ ���� ����, � ������� ��� ������ 
	long			GetSemRelPOSViolationsCount(long Tag);
	// ������ ���� WordWeight ���� ����
	long			GetWordWeightSumma (long Tag) const;
	// ������ ����� ���������, ������� �� ���� �������� �� ��������� �������
	long			GetOptionalValenciesCount(long Tag);
	// ������ ����� �����, � ������� �� ��������� ������ �����������
	long			GetObligatoryValencyViolation (long Tag);
	// ������ ����� ������������� ���������  (��� ������, ��� �����)
	long			GetAnaphoricRelationsCount(long Tag);
	//�������, ����� �������������, ������� ����� �� �� ������ �����, ��������� � CSemNode::m_ValsLayout
	long            GetValencyDisorderCount(long Tag);
	// ������ ����� ����������� (���������) CSemRelation::m_ReverseRel ���������
	long			GetPassiveValencyCount(long Tag) const;
	// ������ ����� ��������� � ������� ��� �������� 
	long			GetPrichastieWithoutActants(long Tag) const;
	// ��������� ��������� ������������� ������������
	long			GetMiscSemAgreeCount(long Tag) const;
	// ������� ����������� ������������� ����������� �������������
	void			DelProhibitedInterps (string UnitStr, CRusSemNode& N) const;

	
	

	// =====    ��������� ���� � ������ "���"
	void			DeleteSAMNode(long ClauseNo, CRusSemNode& SamNode);
	bool			CheckSAMNode(long ClauseNo) const;
	bool			InsertSAMNode(long ClauseNo, CRusSemNode& SamNode);


	// ======  ��������� ������ ������ ������
	// ����������� 
	long			Idealize ();
	// �������� ����. ������������� � �����. � GFi ��������
	void			ApplySubordinationCases ();
	// �������� SF-�������  � �����. � ��i 
	void			ApplySubordinationSemfets ();
	// ���������� �������� ����������
	void			ApplyDopInfinitive ();
	// ���������� �������������� ��������� ��� LF
	void			ApplyDopLexFuncts ();
	
	// =======  ��������������� �������
	// ������� ���������������� (m_bUsed) ���������  � ������  ClauseNo
	void			DelUnusedRelations (long ClauseNo);
	// ������ ����� ��� ������ ClauseNo
	long			SetTagToClause(long ClauseNo);
	// ������ ����� ��� ������ ����� �����������
	long			SetTagToSentence();
	// ���� ����, ������� ����������� �� ������  ��� ������� SyntaxGroupNo
	long			FindNodeBySyntaxGroupNo (long SyntaxGroupNo) const;

   	// ������ ��� ��������� �����, ������������� ���� NodeNo1 � NodeNo2
	void			FindClosestWords(size_t NodeNo1, size_t NodeNo2, size_t& StartWordNo, size_t& EndWordNo) const;
	// ������ ���������� ����� ������ � ������ 
	long			GetDistance(size_t NodeNo1, size_t NodeNo2) const;
	// ������ ����� ��������� ���� ������ (������� ������� �� �����, ��� �� ������ ����)
	long			FindRightClosestNode(size_t NodeNo) const;
    // ������ ����, �������� ����������� ����� WordNo
	long			FindNodeByWordNo(long WordNo) const;
    // ������ ����, �������� ����������� ����� WordNo
	long			FindNodeByWordNo(long WordNo, long ClauseNo) const;
	// ������ ����, ������������� �� ����, ��� ��� ���� � ������ ������
	void			GetAllTextOrderedNodes(vector <long>& Nodes) const;

		// ������ ����� (� ������) ��� ���������, ������� ������� �� �����, ���������� Tag.
	long			GetRelationsLength(long Tag) const;


		bool			CheckAllIncomingRelations(long NodeNo, const string& RelationStr)  const;
        void          FreeWordNo (long WordNo); 		
		void          InitLexVariant(size_t ClauseNo);
		bool		  SetLexVariant(size_t ClauseNo, size_t& LexVariantInCurrSetCollocNo, size_t& CurrSetCollocHypNo);
		void          FindParticleBY(long ClauseNo);
		void          GetClauseVariantCombinations(vector<VectorLong>& Variants) const;
		void          ApplyTerminSemStrForOneRel (string SemRel, long Word1, long Word2, const CRossHolder* RossHolder);
		bool          GetClauseVariantCombination();
   		
		
		bool          ReadDopField(long ClauseNo, long NodeNo, const CRossHolder* Dict, long UnitNo, long CollocId);
		void          ReadDopFieldForClause(long ClauseNo);
		void          ReadThesInterps(long ClauseNo);
		void          ApplyTerminSemStr(long ClauseNo);
		void          IndexRelations ();
		bool		  TryTestTree(string Graph);
	  long FindAlreadyBuiltClauseVariant(long ClauseNo) const 
	  {
		  vector<CSemClauseVariantResult>::const_iterator It = find(m_AlreadyBuiltClauseVariants.begin(), m_AlreadyBuiltClauseVariants.end() ,CSemClauseVariantResult (m_Clauses[ClauseNo].m_SyntaxClauseNo, m_Clauses[ClauseNo].m_ClauseVariantNo, m_Clauses[ClauseNo].m_AddedClauses));
		  if (It == m_AlreadyBuiltClauseVariants.end()) return -1;
		  return It - m_AlreadyBuiltClauseVariants.begin();
	  };
		bool          AgreeWithSyntaxTop (long Tag)  const;

		long          IsLexFetAgree(long NodeNo) const;
		long          GetLexFetAgreeCount(long Tag);
		long          GetColloquialInterpsCount(long Tag) const;
		long          GetCorporaGleicheCount(long Tag) const;
		bool          CanBePostfixRelationOperator (long NodeNo) const;
		bool          CanBePrefixRelationOperator (long NodeNo) const;

		bool          CheckSubjectPredicateRelation (long SubjNodeNo, long PredNodeNo);
		long          GetSubjectPredicateViolationsCount (long Tag);
		long          GetOnlyCommaBetweenViolationsCount (long Tag);
		void          ApplySynStr (long ClauseNo);
		bool          IsOptional(long RelNo) const;
		bool          SplitVariantWithRepeats(size_t  VarNo,  vector<CTreeVariant>& Variants);
		long		  SetEqualFieldId(const CTreeVariant& V);
		void          UncycleVariant(size_t  VarNo, vector<CTreeVariant>& Variants, long Tag);
		long          GetCopulViolationsCount(long Tag)  const;
		long          GetInstrAgentRelsCount (long Tag) const;
		void          AddTreeVariant (const CTreeVariant& Variant);
		long            GetSimpleConj (long NodeNo, const CConj* Conj, long ConjCount) const;
		void          SetTreeVariant (const CTreeVariant& Variant);
		string       GetClauseComplexitiesStr();
		void          GetValencyMisses(long NodeNo, vector<CValency>& ValencyMisses)  const;
		// lexical varinats
		bool          IsInLexVariant(long ClauseNo, long NodeNo);
		void		  UpdateBlockedRelations();
		void          InitPassivePattern(size_t NodeNo, BYTE ValencyNo, CSemPattern& P);
		bool          IsPostSpecifAdjNode(long NodeNo) const;
	bool          CheckCollocItemPosition(long SourceNodeNo, long  TargetNodeNo, long Position);
	// elementary methods
	bool          IsUnderPrep(long NodeNo) const;
		long          GetOborAdverbialCount (long Tag) const;
	
		long			GetCommaBetweenBrothersExceptMNAViolationsCount(long Tag) const;
		long			InsertNode (CRusSemNode& N) ;
		void			InsertNodeAsSubject (long RootNodeNo, CRusSemNode SubjNode);
		// �������� ���������� ���� ��������
		void			AssertValidGraph();
	  	// ������ ������ �� ������ ���� �� ������
		long			FindFirstNode(long ClauseNo) const;
		// ���������, ��� ����� ������ ����� ���� �� ���� �������
		bool			HaveCommaBetweenInOneClause	(size_t NodeNo1, size_t NodeNo2) const;
		// ���������, ��� ����� ������ ����� ���� ������ ���� ���������� "ch", � ������ ������
		bool			HaveOnlyPunctBetween (size_t NodeNo1, size_t NodeNo2, int punct_char) const;
		// ������������� CSemRelation::m_LeapsCount ��� ���� ��������� ������
		void			BuildRelationLeapsCount(long ClauseNo);
		// ������� CSemRelation::m_LeapsCount ��� ������ ���������
		long			GetRelationLeapsCount (long RelNo);
		void			SaveClausesNodeReferences();
		void			RestoreClausesNodeReferences();
		void			PopClausesNodeReferences();
	
			// ������������� ���� ���������� CSemRelation::m_RelUse = false
		void			SetUseFalse ();
		// ������������� ���� ���������� CSemRelation::m_RelUse = true
		void			SetUseTrue ();
		// ������ ����� ���������, ��� ������� CSemRelation::m_RelUse == true
		long			GetUseRelationsCount() const;
		// ������ ����� ���������, ��� ������� CSemRelation::m_RelUse == true
		long			GetSemanticVolume(long Tag) const;
		// ������ ����� ����������� ���������
		long			GetClauseRelationCount ();
		// ���������, ��� ���� NodeNo1 � NodeNo2 ��������� � ��������� ������������ (<,<<,>>,>,^)
		bool			CheckDirection (long NodeNo1, long NodeNo2, string Direction) const;
	
		
		bool			IsProjectedNew(long Tag) const;	
		// ������������ ��� ������ ������������ � �������� ������
		void			ConvertRusNumeralsToArabic();
		string			ConvertArabictoRusNumeral(const CRusSemNode& Node); //� �������
		// ��������� ����� ���� "����� ������ � ���"
		void			DealInvitatoryMoodBeforeTree (long ClauseNo);
		void			DealInvitatoryMoodAfterTree ();
		// ������� ���� � ������ ������, ���� �� �� �����  � ������ �������������� �����
		void			DeleteConjInTheBegining(long ClauseNo, CRusSemNode& FirstConjNode);
		// �������� ��� ���������, ������� ��������� ��� ���������� ����������, ��� ���� 
		// ������� ������ ��������� �� ��������� ��� ��� ����������
		void			MarkUniqueRelations();
		// ���������, ��� ����� ������ ��������� � ��� ��������� ��� ������ �����, ����� RelNo
		bool			dfs_to_test_unique_relation (size_t NodeNo, size_t RelNo);
		string			GetClauseTreeForTcl();
		// �������� SF1 ���� NodeNo		
		vector<QWORD> GetSemFetsOfFirstValency(long NodeNo);
		// ���������, ��� ����� ����� ���� ����� �������
	    bool			HasCommaBefore (size_t NodeNo) const;
        // ������ ������������� ������������ �������
		bool			BuildAnalyticalSupelative();
		// ������ ����������� (X!) ��������� �� ���������� �����, ���� 
		// � ���������� ������ �� ������ �� ������ ���������
		void			FindReverseActantForPreps(size_t ClauseNo);
	   // ������ ��������������� ������������� ���� (����� � ��������)
	    string	GetMorphologyOfNode(long NodeNo) const ;
		// �������� Copul, ������� ������ �� ����� �����
		void			DelChildlessCopul();
		// �������� ���  �������������� ���������, ����� ������, � ���������� ���������
		void			CopyDopRelationsExceptAnaphor();
		// ����������� ���. LEX (������ ������ � ConceptStrs) �����, ���� �� �������� 
		// �������������� ���� LEX
		void			AscribeFindConceptFets(long ClauseNo, const StringVector& ConceptStrs);
		//  ���  ������  ���. LEX ��  ������ ��������� 
		//  ������ �� ������ ���� � �������, ������������ LEX.
		void			FindConceptFetsFromArticles(long ClauseNo);
		//  ���������, ��� ���������� ����� ������ � ������� ��� ������� �������� �� 
		bool			VerbCanBeWithoutReflexiveSuffix(string Lemma) const;
		// ���������� ��� "�������"  � ������� ��������� ����,
		void			DeleteWordNoGaps();
		// ���� ��������  �� ��� ��������
		bool			IsAdverbial (long NodeNo) const;
		// ��������� ���������� ������� �����������
		void			CalculateBestCorporaLabel(size_t ClauseNo);
		// ������� ������� ��� ������� ���� "���������"
		void			NumeralAdverbRule();
		bool            IsTimeAdverbial(long NodeNo) const
		{
			return    m_Nodes[NodeNo].IsMainTimeRossNode()
				 ||	 (   HasRichPOS (NodeNo, ADV) 
				&& HasSemFet(m_Nodes[NodeNo], "TIME")					 );
		};
		// ������ ��������������� ������ _���� � _weak_syn_rel	
		bool			ReadAuxiliaryArticles();
		bool		    IsWeakSynRel (const string& S) const
		{
			return binary_search (m_WeakSynRels.begin(),m_WeakSynRels.end(),S);
		};
		// ���������, ����� �� �������� ��������������� ���������
		bool			IsPragmaticallyStrongSynRel (long RelNo) const;
		// ������� ��������������  ���������, ������� ����� ��������
		void			FindPragmaticallyStrongSynRel ();
		// ������� ��� �������������� ������  � �������������� �����������
		void		    FindQuestionClauses ();
		// ����������  �������� ������ 
		string			GetClauseProperiesStr(long ClauseNo) const;
		// �������      �������, �������� ������� ����� �����������
		void			FindDividedTermins();
		// �������� ������� GetDopFields, ������ �� ����������� m_ThesSemRelations
		long			AddThesSemRelations(const CRossHolder* Dict, long UnitNo, long StartNodeNo);
		// ������ ����� ��������� ������ �� ID �������
		WORD			GetUnitNoByTerminId(DictTypeEnum   DictType,	long TerminId) const;
		// ��������� ���� � ������ ���� "����" � ������ "����"
		void			ProcessDashClauses();
		bool	IsQuestionClause(long ClauseNo) const 
		{
			return m_Clauses[ClauseNo].m_bQuestion;
		};

		// ������ ����� ����������� ��� ����� ������������ 
		long GetValencyNoWithouProhibited (long NodeNo, long ValencyNo) const;

		// ������� ��������� � ������ ������ �������� �� LF
		void			FindConnectedSituations(long NodeNo);
		// ��� ���� ��������������� ������� �������, ��� ������� ��� ��������������� �������� S0
		void			FindVerbsOfS0(long NodeNo);
		// ���������, ��� ������ ����� ���� �� ���� ����� �������� (������ ����������� ������� ������)
		bool			ClausesHaveCommonSituation (long ClauseNo1, long ClauseNo2) const;
		string	GetClausePO(long ClauseNo) const  {return m_Clauses[ClauseNo].m_BestPO;};
		// ����������� "������" � ModalCopul, ���� � "������" �� ������ �� ����� �����
		void			DealUnboundModalCopul(long ClauseNo);
		// ������� ���������� ��� ������ ��� ����������� �� ����. ������
		long			CreateDefaultSubjectFromPreviousClause();
		// ���� ������������ ��������
		void			FindAbbrTermins();
		// ������� ������������� ��� ���������� ������������ �������� ���� Adobe Photoshop
		void			FindEnglishTermins();
		// ���� IDENT ����  "��� N 8"		
		void			ProcessEveryIdents(long ClauseNo);
	// ���������, ��� ������ ����� ���������� �������� (����������� ����������� NAME)		
		bool			IsHumanName (const CRusSemNode& N)  const;
		// ���������, ��� ����� ����� ���� ��������� ����������
		bool			CanBeSubjectForImperative (long NodeNo, long SourceNodeNo)  const;

		/*
		��������� ������, ����� � ������ ���� ������������ �������������� � ������������� 
		��������, "���� ������, � ���� �� ����� �����, �� ����� �� �����."
		*/
		void			CorrectClauseRelations1();
		/*
		 ������� �� ������ ���������� � 
		 ������������ �������������� �������������  ������� � �������  
		*/
		bool			FindComparWithNoActantAndCheck(long ClauseNo);
		// ���������� ����������� �������������� �������������  ������� � �������  
		void			ConvertComparAdjToComparAdv(long NodeNo);
		// ������� ������������� ����������, ���� ���������� ��������� 
		// �� ��������� �����������
		void			ClearInterpsForFreeTimeGroups();
		// ��. �������� ����� � ����� ��������� 
		long			MovePrepNodeToRelationForMainTimeGroups();
		// ���� � ������ ���� ���� ��� � ����  ����, ����� �������  ���� ���
		void			ProcessSAM_SEBJA();
		void			ConvertVSE2_toOperator();

		void			ProcessAllowableLexVars (string LexVarsStr);
		// ���� ����� �� �������� �������,��� ��� ���
		bool			FindEqualClauseVariant();
		void			ZaplataCompar1(long ClauseNo);
		void			ZaplataCompar2(long ClauseNo);
		/*�������� ����� ���� � "������"*/
		void			CopyToMemory(long ClauseNo);
		void			FindOldNodes(long ClauseNo);
		void            RestoreMemoryRelations(long ClauseNo); 
		/*
		 ���������� ������, ���� ��� ���� �����-�� �������� 
		 � ��������� ���������� FindActants ������
		*/
		bool			AreOldNodesOfTheSameCase (long NodeNo1, long NodeNo2) const; 
		

		/*
		 ���������� ������, ���� ���� ����������� �� ����, �����, ������ � ��������������
		*/
		bool			FullGleiche(long NounNodeNo, long AdjNodeNo) const;
		
		void			CreateVerbAnalyticalForm(long AuxVerbNodeNo );

		void			SolveImperativeHomonymy();
		// �������, ������� ������ �������� ��������� ��� ������������� new
		CRelSet			GetIncomingRelations (long NodeNo, bool UseUse) const;
		// ������� ��������� Rels
		void			DeleteRelSet(CRelSet& R);
		// ���������, ��� � ����� �� �������� GF-�������� ����� ��������� ����� ��������� ItemStr (���. ����)
		bool			HasGramFetAfterColon (long NodeNo, string ItemStr)  const;
		//������ ����, ������� ��������� ����������� ����  NodeNo � �������, �������� � ��������� ���������
		CRelSet         GetValencyPlaces(long NodeNo, CRelSet& OutRelations)  const;
		//�������� �� ���� ���� ����������
		bool			IsLocNode(size_t NodeNo) const;
		// ������ ��� ���� ������, � ������� �� ������ �� ������ ���������, �� ����������� ��� �����, ��� �������� CSemNode::m_bToDelete
		void			GetClauseRootsWithoutDeleted (size_t ClauseNo, vector<long>& Nodes) const;
		void			DelNode(long NodeNo);
		void			PrintMemRelations() const;
		// ������ ���������� ����� ��������� (��, ��)
		string			GetNormOfParticiple (const CRusSemWord& W) const;

		// ���������� ��������� � ��� ��� ��������� � ��������� ������, ���� ��� ��� �������� ��������� ������� 
		void			ConvertParticipleTreeToClause();

		// ������ ��� ����,  ��������� �� ������� ����
		void			dfs_in_clause(size_t NodeNo, vector<long>& Nodes) const;
		// ������ ��������� ����� ���(������) ������, ������� ���� ���� �� ������
		void			BuildCircumsChains(long ClauseNo);
		// ������       ����������� -> �������
		bool			BuildWordWithoutFemineSuffix (string& Word) const;

		bool			RussianSynthesis(string& Result) const;
		string			RussianSynthesisOfNode(int NodeNo);
		string			GetPrepOrConjOfNode (const CRusSemNode&  Node) const;
		bool			SemanticAnswer(string& Result, const vector<CRusSemStructure>&	SavedSentences) const;

		bool			ContainsSemCopul(long NodeNo) const;
		void			GetMaxWordWeightByLemma(string Lemma, CRusSemWord& SemWord) const;
		bool			HasAuxiliaryPOS(string Lemma) const;

		bool			GetSyntaxTreeByText(string text, int ClauseVarNo, string& Graph);
		bool			SetLemmasToReplace(string LemmasToReplace);
		string			GetPrepOrConjFromSynReal (const CSynRealization&  SynReal) const ;
		bool			CanBeDeleted(long NodeNo) const;

};

extern bool HasReflexiveSuffix (const string& s);
extern bool IsPossesivePronoun(const CSemNode& N);
extern void GetCommonVariants(const vector<VectorLong>&  Parents, 
									   VectorLong&   V,
							  vector<VectorLong>&  Variants, 
								long       Position);

extern bool IsBetween (const CRusSemNode& Node, const CRusSemNode& LowerBound, const CRusSemNode& UpperBound);
extern TCortege GetSubjCortege (const CRossHolder* RossDoc);
extern TCortege GetInstrObj (const CRossHolder* RossDoc);

// ����������� ���������� �� ������ ������, �� ������� ����� ����������  ������� �����,
// ��������, "� ���� ���, � ������� �������� ���  �����"
const int ConjWordDistance = 5;



#endif
