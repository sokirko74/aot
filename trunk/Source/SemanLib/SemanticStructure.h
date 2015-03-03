#if !defined(semantic_str_h)
#define semantic_str_h

#pragma warning (disable : 4786)

#include "../common/utilit.h"
#include "RossHolder.h"
#include "../StructDictLib/Ross.h"
#include "SemanticsHolder.h"
#include "SemPattern.h"

 
typedef vector<long> VectorLong;


// ����� �������� � ������� ���� (���������� ������� � �������, ���� ����������
// ������, ������� ��� ���, ������ ����������� ������ ��������)
// ���� ����� ������������  ������ � ������� CSemanticStructure::HasItem

struct CRossQuery {
	// � �����  �������
	DictTypeEnum	m_TypeEnum;
	// �� ����� ������
    WORD			m_UnitNo;
	// ��� ����� ����
	string			m_FieldStr;
	// ��� ������ ����� �������
	BYTE			m_LeafId;
	// ��� ������ ����� ����������
	BYTE			m_BracketLeafId;
	// ����� ���������
	string			m_ItemStr;
	// �� ������
	string			m_DomStr;
	// ���������, ���������� �� ��������� m_ItemStr � ���� m_FieldStr � �.�.
	bool			m_Result; 

	CRossQuery (DictTypeEnum  TypeEnum,
		    WORD UnitNo,	
			string FieldStr,
			string ItemStr,
			string DomStr,
			BYTE  LeafId,
			BYTE BracketLeafId
			);
    bool operator == ( const CRossQuery& X ) const;

};


class CSemWord  
{

	// ��������, ������� ��������� �����
	QWORD          m_FormGrammems;
	// ����� ��������, ������� ��������� �����
	QWORD          m_TypeGrammems;

public:	 
	// ��������� ��� �������� �����
	RegisterEnum   m_CharCase;
	// ������� ����� ���������� �������
	string        m_Word;
	// �����
	string        m_Lemma;
	string        m_GramCodes;
	// ����� ��������� � ����. ������� 
	long           m_ParadigmId;
	// ���������� ����� ��������� � ����. ������� (��� ���������� ���� "���-���������")
	long           m_AdditParadigmId;
	// ����� ����, ������� ��������� ����� 
	poses_mask_t   m_Poses;
	// ����� ����� � �������������� �������������
	long           m_WordNo;
	// ������ ����������� �����, ����� ������ ����� 
	// ��������� ������� ������� (�����������, ������������� ����)
	StringVector m_WordEquals; 
	// ������������ � ������ ���� "�����������", ������� ����������� � "3-fenestral"	
	string		m_NumeralPrefix;

	long			m_WordWeight;
	// �������� �� ��� ����� �������� "��-���������", "��-��������"...
	bool			m_bAdverbFromAdjective;	

	// ��������� �� ��� ������� ������� 
	const CSemanticsHolder* m_pData;
	// ��� ������� ��� ������� �� CSemanticsHolder
	const CRossHolder* GetRossHolder (DictTypeEnum Type)   const;  
	const CDictionary* GetRoss (DictTypeEnum Type) const;

	

	CSemWord();
	CSemWord (long WordNo, string Lemma );

	void Init();
	// �������� �� ������ ����� ��������
	bool IsQuoteMark () const;
	// ����������� �� �������� ����� 
	bool HasOneGrammem(int grammem) const;
	bool operator == ( const long& X ) const;
	// ��������, ��� ����� ���� POS �����������  ������� ����� 
	virtual bool   HasPOS (size_t POS) const = 0;
	bool		 IsRusSubstPronounP () const;
	QWORD		GetAllGrammems() const;
	QWORD		GetFormGrammems() const;
	void		SetFormGrammems(QWORD);
	QWORD		GetTypeGrammems() const;
	void		SetTypeGrammems(QWORD);
	void		AddFormGrammem(int g);
};


typedef pair<string,string> PairOfString;



struct CSynRealization   
{
	// ��������  
	vector<CRossInterp>		m_Preps;
	// ����� (��� ���������)
    CRossInterp				m_Conj;

	// �������� ��������� ��� ����
	QWORD                   m_Grammems;

	// ������� ��� ����, ������� �� ��� ������ � ��������, �� �� ��� �������� � ������.
	//  ����� ������� ��� ���� ������� ����� � ������.
	string					m_Other;

	// ����� ������������� ������� � ������� CSemPattern::m_GramCorteges 
	long                    m_CortegeNo;

	// ����������� ������  CSemPattern::m_GramCorteges[m_CortegeNo]
	TCortege                m_Cortege;

	// ��������� ��� ������ "�� �����", � ������� ������������ ����������� ��������� �1(��)
	string                  m_AlgStr; 

	void SetEmpty();
    // ���������, �������� �� ���� ������� PrepNo
	bool HasThePrep (WORD UnitNo) const;

};

enum NodeTypeEnum  { SimpleNode, MNA, Copul,  ModalCopul, SJA, Situat};
/*
  MNA - �������� ������������
  Copul - ��������� ���� ("� ������ ��� ������� ������")
  ModalCopul - �������� ��������� ���� ("���� �� � ��� �����������")
  SJA - 	����, ������� ���������� �� ���������� ������� �� �� �  ��������� ��� ������ �����������
*/


enum MNATypeEnum  { NotMNA, SimpleMNA, RepeatMNA, CHEMOborot, KAK_MNA};
/*
  NotMNA - �� �������� ���������� ������������ 
  Simple - ������� �������� ������������ (���������� ��� � ����� ������)
  RepeatMNA - ������������� ��� ������� ����
  CHEMOborot - ������������� ������ (������ "���" � ������������� �������)
*/

/*
 ���������� �� ����, ������� �������� �� ��������� ������
*/
enum SemCategoryEnum  {scSituation=0, scObject=1, scLabel=2, scRelation=3};
extern const char* GetStrByCategory (SemCategoryEnum t);

struct  CInterpNodeInfo  {
	// ���. ��������� ( LABL, LABL.OBJ, LABL.SIT, REL)	
	SemCategoryEnum	m_SemCategory;
	//SF ����
	vector<QWORD>	m_NodeSemFets;
	// ���������� ������� ����
	StringVector  m_POs;
	bool			m_ThesAbbrForm;

	// �������� �� ���� ��������� �����������	
	bool			m_bOrgName;
	// �������� �� ���� ������ �����������
	bool			m_bProper;


    // ����������� ������� ��� ������� ��������� ������������� ���� 
	vector<CLexicalFunctionField>	m_LexFunctFields;

	StringVector	m_HigherConcepts;

	// ��������, ������� ���� ��������� � ����������� ��������, ���������� �������
	// �������� ������ ����
	// ��� ����� "�������"  ����� ����� "�������"
	// ��� ����� "�����"  ����� ����� "������"
	// ��������� Oper1(�������) = �������, Real(������)= �����
	vector<CDictReference> m_ConnectedSits;

	// ��������� ��������� ���� � ������� ��������� �������������
	vector<CValency>  m_Vals;
};


class CSemNode : public CInterpNodeInfo
{
	// ������� �������� ������ 
	QWORD			m_Grammems;

protected:
	// ��� ��������� ������������� ����  
    CDictUnitInterpVector   m_Interps;
    // ������� �������������
    long                    m_CurrInterp;

public:
    // ����������� �������, ������� ���������������� � ������� CRusSemNode � CEngSemNode
	virtual const CSemWord&	GetWord(int WordNo) const = 0;
	virtual		  int		GetWordsSize() const = 0;

	// ������� ����� ���� (��� ����������� ����� m_MainWordNo=0)
	long			m_MainWordNo;

	// ���� ���� ��� ��������� �� ������, �� ����� �������� ��� ������ 
	string			m_SynGroupTypeStr;

    // ����������� ���� ������
    string			m_GramCodes;
    // type ancode 
    string			m_TypeAncode;
	// ����� ������, � ������� �������� ������  ����
	long			m_ClauseNo;
    // ����� ������, � ������� �������� ������  ����
	CSynRealization	m_SynReal;


	//==========           ����������� ���������  =====================
    // �������� ����, ���� �� ���� ����� ��� ������ � �������
	bool					m_bReached;
	// �������� ����, ���� ���� ���� ����� �������, �� ���� ��� ������� ������, ��������� ���-�� 
	// ����� ������� �������
	bool					m_bToDelete;	
	//  ����� ������������, � ������� ������ ������ ����, �� ���������� ��������� 
	long					m_Tag;

    // ������, ���� ������ � ����� �� ������ ��  �������� ���� ��������� �������
	bool					m_bQuoteMarks;

	// =============     ���������  �������������  ==================
	//������������� ��������� ��������������
	COpenCollocInterp		m_Colloc;
	// ���������� ����� ��������������, � ������� ������ ������ ����
	long                    m_CollocId;

	// ������������� � ���������( ������� ������� �� ���������� ��� �� �����������)
	long			m_TerminId;
	long			m_ThesaurusId;





	// ��� ����  
	NodeTypeEnum	m_NodeType;

	// ���� m_NodeType == MNA, �� � ���� ����� ����  �������������, ��� ������� ���������� � ��������� ����� 
	MNATypeEnum		m_MNAType;


	// ��������� ��������� (��, ����, ��)
	StringVector   m_RelOperators;

    // ����� ������������ ����
	long			m_AbstractNodeId;



	CSemNode();

	// ����������� ���� - ��� ����� ����, ������� ������� ������ �� ������ �����
	bool	IsPrimitive() const;
	// �������� �� ������ ���� ����������� (�� ���������� ����)
    bool	IsAbstract () const; 	
    // IsWordContainer = "�������� �� ������ ���� �����"
	// ������ ������� IsWordContainer � GetWordsSize() 
	// ����� ��������, �� ����� ��������,
	// ������� ����� ������������ ��� ����
	bool	IsWordContainer () const;
	// ������ ����� ������ ������ ����� ������� ����
	long	GetMaxWordNo () const;
	// ������ ����� ������ ������� ����� ������� ����
	long	GetMinWordNo () const;
	// ��������� ������� ��������� �����  RelOperators
	bool	HasRelOperator (string oper) const;
	// ������� �� �������� ��  m_RelOperators
	void	DelRelOperator(const string& oper);
    // ���������, �������� �� ���� ���� �� ���� �������
	bool	HasSomePrep () const;
    // ���������, �������� �� ���� ������� PrepNo
	bool	HasThePrep (WORD UnitNo) const;

	bool	IsTimeRossNode() const;
	bool	IsMainTimeRossNode() const;
	bool	IsThesNode() const;
	bool	IsTrueLocNode() const;
	bool	HasPOS (size_t POS) const;
	bool	IsComma() const;
	bool	IsLemma(string Lemma) const;
	// ���������, ��� ������ ���� �������� ��������� ��������
	bool	IsPassiveVerb () const;

    // ���� ��� ������� ��� ������� �������������
	DictTypeEnum		GetType()   const;
	// ����  ������� �����������
	CDictUnitInterp*	GetInterp();
	// ����  ����������� ������� �����������
	const CDictUnitInterp*  GetInterp() const;
	void	DelAllInterps();
	void	AddInterp(const CDictUnitInterp& X);
	void	SetInterp(const CDictUnitInterp& X);
	void	CopyInterps(const CSemNode& X);
	void	ResetCurrInterp(); 	
	const CDictUnitInterpVector GetInterps () const;
	bool	SetCurrInterp (const CDictUnitInterp& X);
	void	DeleteInterp (size_t i);
	long	GetCurrInterpNo() const;

	// ����  ����� ������ ��� ������� �����������
    WORD	GetUnitNo() const;
	QWORD	GetGrammems() const;
	void	SetGrammems(QWORD g);
	void	ModifyGramCodes(string GramCodes, bool andwords, const CRusGramTab *R);
	void	AddOneGrammem(int g);
	bool	HasOneGrammem(int g) const;
	bool	HasGrammems(QWORD g) const;
	void	AddGrammems(QWORD  grammems);
	void	DeleteGrammems(QWORD grammems);

	// ������ ����� ����, ������� ��������� �������� ����� ����
	poses_mask_t GetNodePoses() const;

};

enum PositionTypeEnum {
	UnknownPosType,
	FromArticlePosType,
	FromAlgorithmPosType
};

struct CSemRelation : public CSimpleBinaryRelation{
	// �����������, ������� ��������� ����� �����
	CValency		m_Valency;
	// �������������� ���������, ������� ����� � ������ ����� �������������� ���������
	// ���� ������ ���� ������ �� ������� �������������� ���������, �� ����� ����� ����, 
	// ������ ��������������� �� ����������, ����� ����� ����� ���������, ������� �������� � ���� GFi
	string			m_SyntacticRelation;
    //����������� ����: �������� �� ���������, ������� ����� ����� �������
	bool			m_bToDelete;
	//����� ������������ ���������� � ���, ��� ����������� ���������� � ������������� ��� ���������
	CSynRealization	m_SynReal;
	//����������� ����: ������������ ��� ��������� ������ ������������ ��������� ����� ���������� ���������
	long			m_bRelUse;
	// ������ ����, ��� GFi ��� ������� ����������� ���������� ("X!")
	bool				m_bReverseRel;

	// �������� LEX, ������� ������ ������������� ������ ���������
	StringVector		m_LexFets;

	//SF ���������
	vector<QWORD>	m_SemFets;

	bool			m_bDopRelation;

	// �������, ������� ����������� ��� ������� 
	string				m_Position;
	PositionTypeEnum    m_PosType; 

// ���������, ������� �� ����� ����������� ������ ����������� � ���� SIT , � ���� �������������	// � ������� ������
	bool				m_bSituatRelation;

	void      Init();
    CSemRelation ();
    CSemRelation (const CValency& Valency, 
	             long SourceNodeNo, 
				 long TargetNodeNo, 
				 string SyntacticRelation);
	
    // ������, ���� ��������� ��������� ��������
	bool  HasSomePrep() const;
};



// ���������� ����������� (�������������, ���������������, ��������������)
enum SentenceMoodEnum {Indicative, Exclamative, Interrogative};



// ==== ����� 
struct CSemClause 
{
	// ���� �� � ������ ������� "��"
	bool						m_HasParticleBY;
	// ���������� ������� ������
	string						m_BestPO;
		// �������� �� ������ �������������� 
	bool						m_bQuestion;
	// �������� �� ������ ����, ������������ ��� �������
	// ��������, "c ������ �������������� �� � �������"
	bool						m_bHasParticleConjAnd;

	//��������, ��� ��� ������ ���������� (������ ����)
	bool						m_bQuoted;
	//��������, ��� ��� ��������� � ������
	bool						m_bBracketed;
	CSemClause();
	void  Copy(const CSemClause& C);
};



class CSemanticStructure   
{ 
public:
	// ��������� �� ��� ������� ������� 
	CSemanticsHolder* m_pData;
    // ��� ������� ��� ������� �� CSemanticsHolder
	CRossHolder* GetRossHolder (DictTypeEnum Type)   const;
	const CDictionary* GetRoss (DictTypeEnum Type) const;

	virtual const vector<CSynRelation>&		GetSynRels() const = 0;
	virtual		  vector<CSynRelation>&		GetSynRels() = 0;
	virtual const vector<CLexFunctRel>&		GetLexFuncts() const = 0;
	virtual		  vector<CLexFunctRel>&		GetLexFuncts() = 0;
	virtual const CSemClause&				GetSemClause(long ClauseNo) const = 0;
	virtual const long						GetSemClausesCount() const = 0;

	// ���������� �������, ������� ������ �����
	string						m_PO;


	// ���������� �����������
    SentenceMoodEnum		m_SentenceMood;


	// ������ ���������, � �������������� �������������� ��������
	// ������������ ���������� � ������� 
	string                 m_ClausePropertiesProtocol;
	// �������� ���� �������������(�����) ��� ���� ����� 
	string                 m_WeightProtocol;
	// ������� ������� ����� �� ��� ���� ������������
	string                 m_TimeStatictics;
	//����� �������� ����� ���� ������� 
	string                 m_ClauseVariantsStatistics;
	// ���������� �� ��� ����������� �� ����� "��"
	bool					m_bHasConjBut;


    // ��� �������, ������� ��������� ����� ������� HasItem	
	vector<CRossQuery> Queries;


	vector<string>   m_IndexedSemFets;


	//===========    ������ � ��������������� �������������� ����������� 
	virtual const CSemRelation*	GetDopRelation(int RelNo) const = 0;
	virtual		  CSemRelation*	GetDopRelation(int RelNo)		= 0;
	virtual int					GetDopRelationsSize() const	 = 0;
	virtual void				EraseDopRelation(int RelNo) 	= 0;
	void						GetIncomingDopRelations (long NodeNo, vector<long>& Relations) const;
	// ������ ��������� � ���� NodeNo AUX-���������.
	void						GetOutcomingDopRelations (long NodeNo, vector<long>& Relations) const;
	// ������ ����, �� ������� ��������� ������������� �����������
	long						GetAnaphoraTarget(long NodeNo) const;
	// ������ ����, � ������� ���� �������������� ��������� THESAME	
	long						GetEquNode(long NodeNo) const;
	

	

	//===========    ������ � �������������� ����������� 
	// ����������� ������� ��� ������ � �������� ���� ����
    virtual const CSemRelation*	GetRelation(int RelNo) const  = 0;
	virtual		  CSemRelation*	GetRelation(int RelNo)		  = 0;
	virtual int					GetRelationsSize()		const = 0;
	void				ReverseRelation(int iRel)   { swap(GetRelation(iRel)->m_TargetNodeNo, GetRelation(iRel)->m_SourceNodeNo); }
	virtual void				EraseRelation(int RelNo)		=0; 
	virtual void				GetColorAndWidthOfRelation(int RelNo, float& Width, string& Color) = 0;

	
	// ������ ��� ���� �� ������ ClauseNo, � ������� �� ������ �� ������ ���� �� ������� ���� ���� �� ������
	//(����������� ��������� ����� �������!)
	virtual	void				GetClauseRoots (size_t ClauseNo, vector<long>& Roots) const;

	// ������ �������� �� ���� NodeNo ���������. ����   UseUse = true, �� ������ ������ �� ���������,
	// ��� ������� CSemRelation::m_RelUse==true
	void			GetIncomingRelations  (long NodeNo, vector<long>& Relations, bool UseUse = false) const;
	// ������ �������� �� ���� NodeNo �������������� ���������. 
	void			GetIncomingInClauseRelations (long NodeNo, vector<long>& Relations) const;
	// ������ �������� �� ���� NodeNo ����������� ���������. 
	void			GetIncomingClauseRelations (long ClauseNo, vector<long>& Relations) const;
	// ������ ����������� ���� NodeNo ���������. 
	void			GetRelations  (long NodeNo, vector<long>& Relations, bool UseUse= false) const;
	// ������ ����� �������� � ���� NodeNo ���������.
	long			GetIncomingRelationsCount (long NodeNo, bool UseUse = false) const;
	// ������ ��������� � ���� NodeNo ���������.
	void			GetOutcomingRelations (long NodeNo, vector<long>& Relations, bool UseUse= false) const;
	// ������ ����� ��������� � ���� NodeNo ���������.
	long			GetOutcomingRelationsCount (long NodeNo,  bool UseUse = false) const;
	// ������ ����, � ������� ������ ��������� � ���� NodeNo ���������.
	void			GetOutcomingNodes (long NodeNo, vector<long>& Nodes, bool UseUse = false) const;
	// ������ ����, � ������� ������ ��������� �� �����, �� ������� ���� ��������� � ���� NodeNo
	void			GetBrothers (long NodeNo, vector<long>& Nodes, bool UseUse = false) const;
	// ����������� �� ������ ���������, ��� ����� - ��� ���������, ������� ���� �� ���� �� � ���� ��, ���
	// ��������� ������ ���������
	void			DeleteDubleRelations();
	// �������� ��������� � Debug
	void			PrintRelations() const;
	// ���������, ��� ���������� ���� �� ���� ���������, ������� ���� ����� ClauseNo1 � ClauseNo2
	bool			AreConnectedClauses (long ClauseNo1, long ClauseNo2) const;
	// ������ ��� ��������� ����� NodeNo1 � NodeNo2
	void			FindRelations (long NodeNo1, long NodeNo2, vector<long>& Rels) const;
	// ������ ������ ��������� (���������) ����� NodeNo1 � NodeNo2
	long			FindFirstRelation (long NodeNo1, long NodeNo2) const;
	// ������ ������ �������������� ��������� (���������) ����� NodeNo1 � NodeNo2
	long			FindDopFirstRelation (long NodeNo1, long NodeNo2) const;
	// ������� ��������� Rels
	void			DeleteRelations(vector<long>& Rels);
	// ��������� ��� ���������, ����������� ���� FromNode,  � ���� ToNode
	void			MoveRelations(long FromNode, long ToNode);		
	// ��������� ��� �������� ��������� � ���� FromNode  � ���� ToNode
	void			MoveIncomingRelations(long FromNode, long ToNode);
	// ��������� ��� �������� �������������� ��������� � ���� FromNode  � ���� ToNode
	void			MoveIncomingDopRelations(long FromNode, long ToNode);
	// ��������� ��� ��������� �� ���� ������, ����������� ���� FromNode,  � ���� ToNode
	void			MoveAllRelations(long FromNode, long ToNode);
	// ������ ��� ����, � ������� �� ������ �� ������ ����
	void			GetRoots(vector<long>& Roots) const;
	//���������, ��� ������ ��������� - ��������� �����  ���������� � ���������   
	bool			IsRusSubj(int iRel) const;
	// ������ ������ �������������� ���������, ������ � ����� �������� ��������� � ������� � ������ iSemRelNo
	long			GetSynRelBySemRel(long iSemRelNo) const;

	

	
	
	//======================    ������ � ��������������  ������ (����)
    // ����������� ������� ��� ������� � �������� ���� ����
    virtual const CSemNode&		GetNode(int NodeNo)							const = 0;
	virtual		  CSemNode*		GetNodePtr(int NodeNo)							  = 0;
	virtual int					GetNodesSize()								const = 0;
	virtual string				GetInterfaceWordStr(const CSemNode* pNode, int WordNo) const = 0; 
    virtual void				EraseNode(int NodeNo) = 0;

	// ���������, ��� ���� NodeNo  �������� Tag 
	bool			HasTag (long NodeNo, long Tag) const {return GetNode(NodeNo).m_Tag == Tag;};
	// ������ ��������� ������������� ����
	string			GetNodeStr(size_t NodeNo, size_t MaxLength = 65000) const;
	string			GetNodeStr(const CSemNode& N, size_t MaxLength = 65000) const;
	// ������ ��������� ������������� ����, ��������� ������� ����������� ����
	string			GetNodeStr1(size_t NodeNo, size_t MaxLength = 65000) const;
	// ������ ����� ����, ������� ��������� ����
	string			GetNodePosesStr(long NodeNo) const;
	// ������ ��������������� ������������� ���� (����� � ��������)
	virtual string	GetMorphologyOfNode(long NodeNo) const = 0;
	// ������ ������������� ������������� ����
	string			GetNodeLemStr(size_t NodeNo) const;
	// �������� ���� � Debug
	void			PrintNodes() const;
	// �������� �������������  ������������� �����  � Debug
	void			PrintLemNodes() const;
	// ������� ���� 
	virtual void	DelNode(long NodeNo);
    // ���������, ��� ���� �������� �������������� ���  ���������� ������������, ������� ��������� ����������������
	bool			IsNounOrMNANoun(int iNode)  const;
	// ���������, ��� ���� �������� �������� ���  ���������� ������������, ������� ��������� ���������
	bool			IsVerbOrMNAVerb(int iNode) const;
	// ���������, ��� ���� �������� ��������� ���  ���������� ������������, ������� ��������� ������������
	bool			IsInfinitiveOrMNAInfinitive(int iNode) const;

	// ��������� ����� ���� ����
	bool	HasPOS (const CSemNode& N, size_t POS) const;
    bool	HasPOS (size_t NodeNo, size_t POS) const;
	bool	IsVerbForm(const CSemNode& Node) const;
	// ������ ���� ����� m_bToDelete = false
	void			SetNodeToDeleteFalse ();
	// ������� ��� ����, ���������� m_bToDelete==true
	void			DelNodesToDelete();

	// �� �� � �����������
	void			SetRelsToDeleteFalse ();
	void			DelRelsToDelete();

	// ������ ����� ��������� ���� ����� (������� ������� �� �����, ��� �� ������ ����)
	long			FindLeftClosestNode(size_t NodeNo) const;


    //======================    ������ �� ���������� ���������������
    // ������ ����� ������ ���� �� GF-�������� ������ UnitNo
	poses_mask_t    GetPosesFromRusArticle(CRossHolder& Ross,WORD UnitNo) const;
	// ��������� ������������ ��������� ������ UnitNo �� ������ W �� ������ ���� 
	bool			GramFetAgreeWithPoses (CRossHolder& Ross,WORD UnitNo, const CSemWord& W ) const;
	// ���������, ��� � ����� �� �������� ���� CAT ����� ��������� Type		(������������� ���������)
	bool			HasSemType (const CSemNode& Node, string Type) const;
	// ���������, ��� � ����� �� �������� ���� SF-������� ����� ��������� SemFet (������������� �-��)
	bool			HasSemFet (const CSemNode& Node, const string& SemFet) const;
	// ���������, ����������� �� ���� ������ SF ��� �����-������ SF ���� �� ��������	
	bool			HasSemFetOrLower (const CSemNode& Node, const string& SemFet) const;
	// �������� �� ���� ����������, ���� � ��������� ������������ CAUS � NEG,    
	// � ������������ SemFet, ����� ������ ������
	bool			HasSemFetPro (const vector<QWORD>& SemFets, const string& SemFet) const;
    bool			HasSemFetPro (const CSemNode& Node, const string& SemFet) const;
	// ���������, ��� � ����� �� �������� ���� FieldStr ����� ��������� ItemStr �� ������ DomStr
	bool HasItem (DictTypeEnum DictTy,  WORD UnitNo, const string& FieldStr, const string& ItemStr, const string& DomStr, BYTE LeafId, BYTE BracketLeafId)  const;
	// �������  ��������� ������������� � ��������� �������������
	string			InterpToStr(vector<CDictUnitInterp>::const_iterator I)  const;
	// �������  ������������� ��������� �������������� � ��������� �������������
	string			OpenCollocInterpToStr(const COpenCollocInterp& I)  const;
	//������ ��������, ���������� �� ���� RESTR ��� ������� ��������� �������������
	vector<QWORD>	GetGramRestr(const CSemNode& W);
	// ��������� ������������ �� SF
	bool			GleicheSemFet(const vector<QWORD>& SemFets1, const vector<QWORD>& SemFets2, bool bInclusion) const;
	// ����������� ���� ���� (���� ��������)
	QWORD			GetOneIndexedSemFet (const vector<string>& SemFet, bool bInlcudeLowerHierarcy,  bool	bInlcudeHigherHierarcy = false);
	// ����������� SF
	vector<QWORD> GetIndexedSemFets (const CSemPattern& P, bool bInlcudeLowerHierarcy,  bool	bInlcudeHigherHierarcy);
	// ������ ������������� SF ��� ������ �����
	vector<string>	GetSemFetStr (QWORD SemFet) const;
	// ������ ��������� ������������� SF
	string			GetSemFetsInOneStr (const vector<QWORD>& SemFets) const;
	// ���������, ���� �� � ������ AL1, ���� ����, �� ������ ��� 
	SEngEquiv		GetAL1Value(int NodeNo) const;
   // �� ��������� ������ �������� ��� ����� ������ ������������� ���������, ������� ��� ��������
    CValency		GetSemRelOfPrepOrConj(const CDictUnitInterp& Unit) const;

		



	//======================    ������ � ������ �� �� ���������� ������� � �������������� ������
	// ���������, ��� ���� NodeNo ����������� ������ ClauseNo
	bool			IsInClause (size_t NodeNo, size_t ClauseNo) const;
	virtual long	GetClauseFirstWordNo(long ClauseNo) const = 0;
	virtual long	GetClauseLastWordNo(long ClauseNo) const = 0;
	virtual string			GetClausePO(long ClauseNo) const  {return "";};
	
	

	//======================    ������ � ������������ ��������� (LF)
    // ������ ������ LF, ������ � ����� ������� ��������� � ������� � ������ iSemRelNo
	long			FindLexFunctBySemRel(long iSemRelNo) const;
    // ������ ������ LF, ������ � ����� ������� ��������� � iW1 � iW2 ��������������
	long			FindLexFunct(int iW1, int iW2) const;
	// ������ ������ LF, ������� ������ � NodeNo (�����-��������)
	long			FindLexFunctBySituationNodeNo(long NodeNo) const;
	// ������ ������ LF, ������� ������� �� NodeNo 
	long			FindLexFunctByParameterNodeNo(long NodeNo) const;

	// ====================   ����� ������� ������������� 
	// ������� �������������� ���������� � ������
	string			SynRealToStr(const CSynRealization&  SynReal, string Delimiter) const;
	// ������� ����� � ��������� �������������
	string			GetTxtGraph();
	// ������� ����� � TCL-�������������
	string			GetTclGraph(bool ShowUnusedValencies, bool UseIsTop);
	// ������ ���������, ������� �� ����� ������������� � ������ (��� �� �������� ���������)
	string	GetOtherRelations();
	// ������ TCL-������������� ��� �������������� ���������
	string			GetTclSyntaxGraph();
	// ������ ������ 
	void			ErrorMessage (string Mess) const { m_pData->ErrorMessage(Mess); };
	// �������� ���������� ���� ��������
	void			AssertValidGraph();
	
	
        
	// ====================   ������ � ��������������� �����������
	// �������� ����, ��� ���� �����c�� �������
	bool			CheckGroupBeginAndCase(string ItemStr, size_t NodeNo, long& PrepNo) const;
	// ������ SF �� ��������� �� �������� ���������
    void			InitThesSemFet (CSemNode& OutNode, const CSemNode& InNode);
	// ��������� SF � ����
	void			AddSemFet (CSemNode& Node, const string& SemFet);


		
		
		
			
		
		
	// ====================   ������ � �������������
	//������������� ������������ �� ������� ��������� �������������
	void		InitVals(CSemNode& Node);
	long        FindAbstractPlugArticle (DictTypeEnum type, QWORD Grammems, poses_mask_t Poses, long ClauseType) const;
	void        FindAbstractAdditionArticle (DictTypeEnum type, const CSemNode& Node, vector<long>& Articles,  bool IsClauseSyntaxRoot, long ClauseType);
	void		AddAbstractAdditionVals (DictTypeEnum type, CSemNode& Node, const vector<long>& Articles);


	//============ ��������
	long			GetLocPrepFromBegining (size_t NodeNo) const; 
	bool			HasLocPrepInBegining (size_t NodeNo) const;

	virtual bool IsQuestionClause(long ClauseNo) const = 0;
	// ������� ���� �� ������� ������� (�� �������� � ������ �������) � �������� ����������� ������� 
	// (������������ ������� �������� ���������)
	// ���� Tag != -1, �� ����� �� ���� �� ���������� ����������
	virtual void			dfs(size_t NodeNo, bool ConsiderUseOfNodes,long Tag = -1) = 0;
	// ���������, ��� ��� ����������� �������
	bool			IsConnected();

	//  ������ ��������� ���� ���������� �����������  ��� �������
	void SetLastSentencePunctuationMark(string& str) const;

	bool IsRelBetweenClauses(const CSemRelation& rel) const;
};


 
	



class IsLessByMinWordNo : public binary_function<bool,const long, const long>
{
 public:  
  const CSemanticStructure* m_SemStr;

  IsLessByMinWordNo(const CSemanticStructure* SemStr)
	{m_SemStr = SemStr;}  

  result_type operator()(const long& NodeNo1, const long& NodeNo2)
  {
	  return m_SemStr->GetNode(NodeNo1).GetMinWordNo() < m_SemStr->GetNode(NodeNo2).GetMinWordNo();
  }
};


template<class T>
void FreeWordNo (T& Node, long WordNo)
{
	   if (Node.IsWordContainer())
		 for (long i=0;i < Node.m_Words.size();i++)
			 if (Node.m_Words[i].m_WordNo >=  WordNo)
				 Node.m_Words[i].m_WordNo++;

};

template<class T>
void	ResetAllReachedFlags(T& SemStr)
{
	for(int i = 0 ; i < SemStr.m_Nodes.size(); i++ )
		SemStr.m_Nodes[i].m_bReached = false;
}




// ������� ��������� ���������, ���������� m_bDopRelation, � ������ m_DopRelations
// (�������������� ��������� ���������� � ������� ��������, ������ ��� ��� �������������
// ��� ������������� ��������� ��������)
template <class T>
void MoveDopRelationsBack(T& SemStr)
{
    SemStr.m_DopRelations.clear();
	for(int i = 0 ; i < SemStr.m_Relations.size() ; i++ )
		if(SemStr.m_Relations[i].m_bDopRelation) 
		{
			SemStr.m_DopRelations.push_back(SemStr.m_Relations[i]);
			SemStr.EraseRelation(i);
			i--;
		}		
	
}
extern bool IsLocSemRel (const string& S);
extern void SetSpacesAndRegisterInSentence (string& str, MorphLanguageEnum Langua);

const  string SIMILAR_NUMERALS = "�����_����";
const  string NUMERALS = "�����";
const  string C_NUMERALS = "����_����";
const  string KEYB = "���";
const  string WEB_ADDR = "��_�����";
const  string NAMES = "���";
const  string NUMERAL_NOUN = "����_���";
const  string NOUN_ADJ = "����_���";
const  string NOUN_NUMERAL = "���_����";
const  string NUMERAL_ADVERB = "���_����_���";
const  string SELECTIVE_GR = "�����_��";
const size_t MaxValsCount = 15;




#endif
