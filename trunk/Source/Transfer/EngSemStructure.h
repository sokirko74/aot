#if !defined(eng_semantic_str)
	#define eng_semantic_str


#include "../SemanLib/SemanticStructure.h"
#include "../AgramtabLib/eng_consts.h"
#include "EngBinaryTranslate.h"

enum EngVerbTenseEnum
{ 
		zero_tn,
        present_smp_tn,		 // he works
		past_smp_tn,		 // he worked
		future_smp_tn,		 // he will work
		present_prf_tn,		 // he has worked
		past_prf_tn,		 // he had worked
		future_prf_tn,		 // he will have worked
		present_cnt_tn,		 // he is working
		past_cnt_tn,		 // he was working
		future_cnt_tn,		 // he will be working
		present_prf_cnt_tn,  // he has been working
		past_prf_cnt_tn,     // he had been working
		future_prf_cnt_tn,   // he will have been working
		would_smp_tn,        // he would work
		would_prf_tn,        // he would have worked
		would_cnt_tn,        // he would be working
		would_prf_cnt_tn,    // he would have been working    
		gerund_tn,			 // working
		gerund_prf_tn,		 // having  worked
		pp_tn,		 // worked	
		present_inf_tn,		 // to work
		present_cont_inf_tn, // to be working
		prf_inf_tn,			 // to have worked
		prf_cont_inf_tn,	 // to have been working 
		pp_cont_tn,			 // been working
		ing_tn				 // working (�������������� ��������� � gerund_tn)
};


const QWORD eAllPersons = _QM(eFirstPerson) | _QM(eSecondPerson) | _QM(eThirdPerson);
const QWORD	eAllNumbers = _QM(eSingular) | _QM(ePlural);
const QWORD eAllGenders   = _QM(eMasculinum) | _QM(eFeminum);
const QWORD eAllVerbForms   = _QM(eInfinitive) | _QM(ePastIndef)  | _QM(ePastParticiple) | _QM(eGerund);
const QWORD eAllDegrees   = _QM(eComparativ) | _QM(eSupremum);



enum EGramCortegeType {Subj, InfinitiveOrGerundGram, PrepNp, SubClause, PossNp, Obj, NP_attr, Nothing, UnknownGram};

enum EClauseRelsType { Kotoryj,ChtoKtoAsConjWords,Kak,Kakoj,SubConj,Participle,AdvParticiple,Li,CoordConj,UndefType,NotBetweenClauses };

enum EDicSource {FromRoss, FromLocRoss, FromTimeRoss, NotFound} ;




int FindConj(const char* arrConjs, int iLen, const char* word);

struct SPosleLog
{
	SPosleLog()
	{
		m_Position = ">>";
	};

	CRossInterp   m_PosleLogPrep;
	string        m_PosleLog;
	// ����� D_POSITION (<,<<,>>,>,^, "dir_obj _")
	string	      m_Position;

};


struct CTenseHistory {
	EngVerbTenseEnum	m_Tense;
	string				m_Maker;
	CTenseHistory(EngVerbTenseEnum	Tense, 	string Maker)
	{
		m_Tense = Tense;
		m_Maker = Maker;
	};
	CTenseHistory()
	{
		m_Tense = zero_tn;
		m_Maker = "";
	};
};




class CEngSemWord : public CSemWord {
	 // ����� ��� ���������� ���������� �����, ���� ���� �� �������� ��. ������, ��
	 // m_Tense = zero_tn;
	 EngVerbTenseEnum	m_Tense;

public:
    
     SPosleLog		 m_PosleLog;
     // ���� ��� �������, ����� �� �� ������� ���������  ����� � �����, ����������� � ���������
     bool            m_bDoNotChangeForm;
 

	 //  ���������� ��� ���������� ���. ������� be ����� �������� �������� � ����������� �������  � P2
	 // ���������������  ������ ����� �������� �������� BuildAuxiliaryVerbs
	 bool			 m_bMorphologicalPassiveForm;
	 bool			 m_bImperative;
	 string			 m_OneselfStr;


	 
     vector<CTenseHistory>  m_TenseHistory;
	 
	 void SetTense(EngVerbTenseEnum	NewTense, string Maker);
	 EngVerbTenseEnum GetTense() const;
	 void Init();
	 CEngSemWord();
	 CEngSemWord (const CSemWord& X);
	// ����������� �� ������ ����� ���� ������ ������ ����, ������� �������� �����?
	 virtual bool   HasPOS (size_t POS) const;
};

enum ArticleCauseEnum {	ArticleFromDict, ArticleFromOrdNum, ZeroArticleForProperNames, 
						ZeroArticleBecauseOfPossessive, DefArticleBeforeClausePredicate,
						DefArticleBecauseOfNominalSupplement, DefArticleForAbstractLocal,
						NoIndefArticleForMassNouns, IndefArticleAfterAs, DefArticleBecauseDefiniteClause, 
						DefArticleForSingleRanks, OverwriteArticleForTimeNodes, OverwriteArticleForTerminNodes};

extern string GetArticleCauseHistory (const vector<ArticleCauseEnum>& t);
extern BYTE GetOnePOS(long poses);


class CEngSemNode : public CSemNode
{
public:
	vector<CEngSemWord> m_Words;
 	virtual const CSemWord& GetWord(int WordNo) const { return m_Words[WordNo];};
	virtual		  int		GetWordsSize() const { return m_Words.size();};
 
	int					RusNode;
	vector<CSemPattern> m_Patterns;

	vector<CSemPattern> m_CopulPatterns;
	

	bool				m_bNotUseTo;
	string				m_ArticleStr;
	vector<ArticleCauseEnum>	m_ArticleCauseHistory;

	void SetGrammemsRich(QWORD g);
	QWORD GetGrammemsRich() const;
	void AddOneGrammemRich(int g);
	void AddGrammemsRich(QWORD  grammems);
	bool HasGrammemRich(int g) const;
	void DeleteGrammemsRich(QWORD g);
	

	CEngSemNode();
	CEngSemNode (const CSemNode& X);
	void Init ();

	EngVerbTenseEnum GetTense() const;
	BYTE GetPos() const;
	bool IsLemma(string Lemma) const;
};


class CEngSemRelation : public CSemRelation 
{
public:
	int  m_RusRel;
	bool m_bInterpreted;
	EClauseRelsType m_Type;	

	CSemPattern  m_Pattern; 
	int	m_PatternNoInArticle;

	string m_engPrep;
	bool m_bSourceClauseIsMain;
	EGramCortegeType m_GramType; //�� ������ ���� ����������

	int  m_iHoleNum;
	bool m_bAuxFlag;

	
	CEngSemRelation (CValency Valency,long SourceNodeNo,long TargetNodeNo, string SyntacticRelation);
    CEngSemRelation (const CSemRelation& X);
	void Init();

};

class CEngSemClause : public CSemClause {
public :
	CEngSemClause() : CSemClause()
	{
	};

};

struct SWeightToCortege
{
	SWeightToCortege()
	{	m_Weight = 0; }
	TCortege m_GramCortege;
	int m_Weight;

	bool operator< (const SWeightToCortege& X) const
	{
		return  m_Weight > X.m_Weight;
	}
};

 struct SGramCortegesAndType
 {
	vector<SWeightToCortege> m_WeightGramCorteges;
	EGramCortegeType m_Type;

	vector<TCortege> GetCorteges()
	{
		vector<TCortege> corteges;
		for( int i = 0 ; i < m_WeightGramCorteges.size() ; i++ )
			corteges.push_back(m_WeightGramCorteges[i].m_GramCortege);
		return corteges;
	}
 };



struct CEngInterp : public CRossInterp, public CThesInterp {
	CEngInterp(const CRossInterp& X)
	{
		m_DictType = X.m_DictType;
		m_UnitNo = X.m_UnitNo;
	};
	CEngInterp(const CThesInterp& X)
	{
		 m_ThesaurusId = X.m_ThesaurusId;
		 m_TerminId = X.m_TerminId;
	};
	CEngInterp() : CRossInterp(), CThesInterp() {};
	
};
typedef map<int,vector<CEngInterp> >  CEnglishEquivMap;	



class CEngSemStructure : public CSemanticStructure
{
public:

	const CSemanticStructure&	RusStr;
	vector<CEngSemClause>		m_Clauses;

	/*
	  ������� ��������� �������e���� ������� ����� 
	  (�� ����, ��� ������� ��������� ����. ����)
	*/
	vector<long>			m_InterpretedRusNodes;

	vector<CEngSemNode>  m_Nodes;
	
	
	vector<CLexFunctRel>		m_LexFuncts;
	virtual const vector<CLexFunctRel>&		GetLexFuncts()	const {return m_LexFuncts;};
	virtual		  vector<CLexFunctRel>&		GetLexFuncts()	{return m_LexFuncts;};
	vector<CSynRelation>		m_SynRelations;
	virtual const vector<CSynRelation>&		GetSynRels()  	const {return m_SynRelations;};
	virtual		  vector<CSynRelation>&		GetSynRels()  	{return m_SynRelations;};

    virtual const CSemNode&	GetNode(int NodeNo) const {return m_Nodes[NodeNo];}
	virtual		  CSemNode*	GetNodePtr(int NodeNo)  {return &(m_Nodes[NodeNo]);}
	virtual int				GetNodesSize() const {return m_Nodes.size();};
	virtual string GetInterfaceWordStr(const CSemNode* pNode, int WordNo) const;
	    // �������� ���� ����
	vector<CEngSemRelation>	m_Relations;
    virtual const CSemRelation*	GetRelation(int RelNo) const {return &(m_Relations[RelNo]);}
	virtual		  CSemRelation*	GetRelation(int RelNo)		 {return &(m_Relations[RelNo]);}
	virtual int					GetRelationsSize() const {return m_Relations.size();};
	virtual void				EraseRelation(int RelNo) {m_Relations.erase(m_Relations.begin() +RelNo);};

	virtual void GetColorAndWidthOfRelation(int RelNo,float& Width,string& Color);

	//===========    ������ � ��������������� �������������� ����������� 
	vector<CEngSemRelation>		m_DopRelations;
    virtual const CSemRelation*	GetDopRelation(int RelNo) const {return &(m_DopRelations[RelNo]);}
	virtual		  CSemRelation*	GetDopRelation(int RelNo)		 {return &(m_DopRelations[RelNo]);}
	virtual int					GetDopRelationsSize() const	 {return m_DopRelations.size();};
	virtual void				EraseDopRelation(int RelNo) 	 {m_DopRelations.erase(m_DopRelations.begin() +RelNo);};


	virtual const CSemClause&				GetSemClause(long ClauseNo) const {return m_Clauses[ClauseNo];};
	virtual const long						GetSemClausesCount() const { return m_Clauses.size();};

	

	CEngSemStructure(CSemanticStructure& _RusStr)
		:RusStr(_RusStr)
	{
		
		m_pData = _RusStr.m_pData;
	};


	

	 void CreateSimpleEnglNodeWithInterp(CRossInterp UnitInterp, CEngSemNode& engNode);
  	 void CreateSimpleEnglNode(string lemma, CEngSemNode& engNode, int POS, bool bDontChangeForm, DictTypeEnum DictType= Aoss, int iMean = 1 );
	 void CreateSimpleEnglNodeByOldNode(string lemma, CEngSemNode& engNode, int POS, bool bDontChangeForm , CEngSemNode& oldNode);

	int FindEngVal(string strRusVal, CEngSemNode& engNode,  vector<long>& iBadVals, bool bUseHierarchy = true);
	bool ValencyEq(string strRusVal,string strEngVal);

	CEngInterp FindBestEnglishNodes( int iRusNode , vector<CEngInterp>& vectorEngEquivs, vector<long>& bestRels, int& iSubjRel);
	bool HasThisLexFunc(string LexFunctName,  CRossInterp  UnitInterp, CLexicalFunctionField& LexicalFunct);
	int GetEnglishNodeBadWeight( int iRusNode, CEngInterp& UnitInterp, vector<long>& goodRels, int& iSubjRels);
	int GetReverseRelValNo(const CSemRelation& rusRel,CEngSemNode& reverseNode);
	void CreateEnglNode(CEngInterp UnitInterp, CEngSemNode& engNode, const CSemNode& rusNode);
	void CreateEnglNodeForEngColloc(long UnitNo, CEngSemNode& engNode) const;
	string GetItemStr(long lItemNo, DictTypeEnum type /*= Aoss*/) const ;
	void TranslateOneActant( int iRel,  int iEngNode);
	void TranslateActants( int iEngNode, int iClause);
	string GetPrep(const TCortege& cortege, DictTypeEnum type);
	string HasParticularPrepInField( CRossHolder* pLocRossDoc,int  iRusActant,CEngSemNode&  engActantm);
	

	bool Rule_TranslatePoss( int iRusActant, long RelationNo, const vector<TCortege>& GramCorteges, int iEngNode);
	bool Rule_TranslateObj( int iRusActant, long RelationNo, const vector<TCortege>& GramCorteges, int iEngNode);
	bool Rule_TranslateSubj( int iRusActant, long RelationNo, const vector<TCortege>& GramCorteges, int iEngNode);
	bool Rule_TranslatePrepNounGroup( int iRusActant, long RelationNo, const vector<TCortege>& GramCorteges, int iEngNode);
	bool Rule_TranslateRelWithPrepField(int iRusActant,long RelationNo, int iEngNode);

	int  NumPrepPhr(const vector<TCortege>& GramCorteges, DictTypeEnum type);
	void ReadMorphFromMainGF(WORD UnitNo, DictTypeEnum type, CSemWord& Word) const;
	void InitEngWordAndLemma(CEngSemWord& semWord, string strEngLemma) const;

	typedef map< string,vector<SGramCortegesAndType> > SemRelToGramCortegeMap_t;
	EGramCortegeType GetGramCortegeType(TCortege& cortege, DictTypeEnum type);
	
	SemRelToGramCortegeMap_t m_SemRelToGramCortegeMap;
	void FillSemRelToGramCortegeMap();
	typedef pair<string, string> StringPair;
	void FillVectorOfGramCortegeAndType(vector<SGramCortegesAndType>& GramCortegesAndTypeV,vector<TCortege>& GramCortegesm, DictTypeEnum type );
	string BuildSentence();

	vector< pair<string, string> > m_SemRelEquivs;
	void FillSemRelEquivs();

	bool Rule_TranslateInfinitive( int iRusActant, long RelationNo, const vector<TCortege>& GramCorteges, int iEngNode);
	bool MakeDeverbative( int iRusActant, int iEngActant, CEngSemNode& engNode);
	bool HasGerund(const vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type );
	bool HasToPlusInf(const vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type );
	bool HasInf(const vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type );
	bool HasItem(const vector<TCortege>& GramCorteges, long& CortegeNo, int& ItemNo, DictTypeEnum type, string strItem);
	bool HasItem(const vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type, string strItem);
	string GetGerundPrep(const TCortege& cortege, DictTypeEnum type );


	bool IsSubj (const CEngSemRelation& Rel) const ;
	// � ������� IsSubj ������ �������� ����� ��������� �������, ������ ����� ���� ��������
	long GetSubj(long NodeNo) const;

	void InitEngVals(CEngSemNode& Node);

	bool AddGXiFromLexFunOperWord(long LexFunctWordUnitNo, CSemPattern& semPattern1, int iValNum, int iOperNum);
	UINT EngPOSByRusPOS(UINT rus_pos, string lemma);
	int InterpretRusNodeAndItsChildren( CEnglishEquivMap& mapRNodeToENode, int iRusNode);
	int InterpretOneNode( CEnglishEquivMap& mapRNodeToENode, int iRusNode, vector<long>& RusRelsToEngRels, int& iSubjRel);

    void FindEnglishEquivMain(CEnglishEquivMap& mapRNodeToENode );	 	 
	void FindEnglishEquiv(vector<CEngInterp>& engEquivs, const string& rus_lemma, BYTE MeanNo, DictTypeEnum   EngDictType);
	bool FindEnglishEquivHelper(vector<CEngInterp>& engEquivs, const CEngUnitNoToRusUnit RusEquivToEngArticleNo, DictTypeEnum type );
    bool InterpretWithPlugArticles (long RusNodeNo, CEnglishEquivMap& mapRNodeToENode);

//��
	void CreateLexFunNodeAnRel(CLexicalFunctionField& LexicalFunct, int iRusLexFunNode,CSemRelation& rusRel, int iEngNode, int iRusRel);
	bool AddGXiFromLexFunFuncWord(long LexFunctWordUnitNo, CSemPattern& semPattern1, int iValNum, int iFuncNum );
	void CreateEngLexFunDummyRel(CLexicalFunctionField& LexicalFunct, int iLexFunNodeDummy, int iEngNode);
	enum ELexFunType { Oper, Func, Magn, NoneFun};
	bool HasCopul(vector<CValency>& Vals, string strRusVal);
	int InterpretCopulNodeAndChildren( CEnglishEquivMap& mapRNodeToENode, int iRusCopulNode, int iEngCopulParentNode, int iValNum);	
	void BuildCopulNodeSentence(int iEngNode,bool& bSubjFound, string& sent1, string& sent2 );
	void IndexSemFets();


	translate_helper helper;
	bool init_helper();
	void CreateEngClauses();
	TCortege CreateCortege(string strGX, string strDomen);	
	void AddSemRelToGramCortege(string semRel, TCortege cortege);
	EGramCortegeType GetGramCortegeType(string& gram_str);
	void GetGramCortegesAndTypeFromRel(vector<SGramCortegesAndType>& GramCortegesAndTypeV, CEngSemRelation& semRel);
	void GetIncomingRelationsInThisClause(int iNode, vector<long>& IncomeRels) const;
	
	void TranslateClauseRels();
	EClauseRelsType GetClauseRelType(int i);
	bool IsChtoOrKto(int iNode);
	void ClauseRelRule_Participle(int iRel);
	void ClauseRelRule_Kakoj(int iRelNum);
	void ClauseRelRule_Kotoryj(int i);
	void ClauseRelRule_Chto(int iRelNum);
	void ClauseRelRule_ChtoKtoAsConjWords(int iRelNum);
	void ClauseRelRule_Kak(int iRelNum);
	void ClauseRelRule_SubConj(int iRelNum);	

	void GetEngEquivsFromRusArticle(vector< SEngEquiv >& vectorEngEquivs, int RusUnitNo, DictTypeEnum   DictType  = Ross, int iRusNode = -1) const; 
	void GetEngEquivsFromRusArticle_test(vector< SEngEquiv >& vectorEngEquivs, int RusUnitNo, DictTypeEnum   DictType) const; 
	void IntersectEngEquivs(vector< SEngEquiv >& vectorEngEquivsFromRusArticle, vector<CEngInterp>& vectorEngEquivsFromAoss );
	bool HasThisGX(const vector<TCortege>& gramCorteges, string value, DictTypeEnum type) const;
	void ChangeClauseNo(int oldClauseNo, int newClauseNo);

	bool Init();
	bool TranslateToEnglish( );

	bool HasNeg(long UnitNo, DictTypeEnum type );
	bool HasQuest(long UnitNo, DictTypeEnum type );

	void GetChildNodes(int iNode,vector<long> &nodes) const;
	void GetAlgNodes(vector<long> &nodes) const;
	void ChangeNegWord(int iNode);
	void MakeEngInfinitive(int iNode);
	void ChangeGrammemsForDirObj(int iNode);

	void MoveMeaningRelsToNewNode(int iOldNode,int iNewNode);
	int  MakeBeNodeForEngNode(int iEngNode,string sTenseHistory,bool bCheckOtherVerb = true);
	
	void RefineEngCollocPreps();
	void RefineComparativeMNAwithPreps();
	void RefineUnknownRelsSynRel();
	void CorrectNodeNumByRelNum();
	void AddFixedGrammemsToNode();
	
	bool CompareCortegeItems(const CRossHolder* RossHolder,
		const TCortege &X,const TCortege &Y) const;

	bool HasALG(DictTypeEnum type, long UnitNo, string strAlg);
	
	void ApplyModalCopulRule(int iEngNode);
	void ApplyNoRule(int iEngNode);
	void ApplyBeRule(int iEngNode);
	void ApplyKeepRule(int iEngNode);

	void ApplyALG_AL1(int iEngNode);
	void ApplyALG_AL3(int iEngNode);
	void ApplyALG_it(int iEngNode);
	void ApplyALG_compl_obj(int iEngNode);

	void ApplyComparativeRule(int iEngNode);
	void ApplyAdjShortRule(int iEngNode);
	void ApplyPredicativeRule(int iEngNode);
	void ApplyTwoNegationsRule(int iEngNode);

	void ImpersonalVerb(int iEngNode);
	void InfinitiveAndIf(int iEngNode);

	void RefineSynRealMarks(int iEngNode);
	void RefineByNumeralMarks(int iEngNode);
	void AddLexFuncNode(int iEngNode);

	void ApplyVerbToAdjRule(int iEngNode);
	void ApplyModalVerbTenseRule();

	void ApplyInvitatoryRule(int iEngNode);

	bool GetSINO(DictTypeEnum type,long unit,vector<TCortege>& vecSINO) const;
	void GetSPrp(DictTypeEnum type,long unit,vector<TCortege>& vecSPrp) const;
	void GetSArt(DictTypeEnum type,long unit,vector<TCortege>& vecSArt) const;
	void GetSGxi(DictTypeEnum type,long unit,vector<TCortege>& vecSGxi) const;
	void ConvertClosedCollocToOpen();

	bool IsQuestionClause(long ClauseNo) const { assert(false); return false; }
	bool CheckQuestionClause(int iEngRoot,int &iQueRel,int &iSubRel);
	void HandleQuestionClause(int iEngRoot);
	bool HandleQuestionNode();

	int GetEngRelByRusRel(int iRusRel);
	void FindEngEquivForRusArticle(CEngInterp rusUnit, vector<CEngInterp>& vectorEngEquivsFrom, DictTypeEnum   EngDictType);
	void GetEngEquivsFromVector(CEnglishEquivMap& mapRNodeToENode, int iRusNode, vector< SEngEquiv >& vectorEngEquivs, vector<DictTypeEnum> EngDictTypes);
	SEngEquiv GetV0Value(const CSemNode& rusActant);
	void SetPositionOfActantInColloc(CEngSemRelation& engRel, CSemPattern& semPattern, int iEngNode);
	void CreateBeSemNode(CEngSemNode& semNode, int& iSubjPattern , int& iObjPattern );
	bool CheckDomensForCortege(StringVector& domens, TCortege& cortege, DictTypeEnum   DictType ) const; 
	BYTE GetPosFromPosBit(size_t Pos) const;
	bool IsSubjPattern(const CEngSemRelation& Rel ) const ;
	void CreateEnglNodeForEngObor(long UnitNo, CEngSemNode& engNode) const;
	bool GetDictUnitInterpForObor(string oborStr, CRossInterp& interp);	
	bool IsSubConj(CRossInterp interp);
	bool SetSimpleEngPrep(string prep, int iNode, int iRel);

	void GetAFieldVector(string FieldStr, DictTypeEnum type, vector<TCortege>& vectorAGX, long UnitNo) const;
	
	
	
	string GetCortegeStr(DictTypeEnum type, const TCortege& cortege) const;
	bool IsGramFet(const CEngSemRelation& Rel , const string strPattern) const;
	bool HasGramFet(const CEngSemRelation& Rel, const string strPattern) const;
	bool IsObjPattern(const CEngSemRelation& Rel ) const;
	string GetConj(const vector<TCortege>& GramCorteges, DictTypeEnum type);
	void HideCopul();
	void FindEngWords(vector<CEngInterp>& resEngUnits, vector< SEngEquiv >& vectorEngEquivs, vector<DictTypeEnum> EngDictTypes);
	void CreateHaveSemNode(CEngSemNode& newNode, int& iSubjPattern , int& iObjPattern, int iMeanNum=1 );
	
	void ReverseClauseRel();
	void FindEquivForLexFunct(CEnglishEquivMap& mapRNodeToENode);
	void FilLexFunctRel();
	bool TryToAddNewSemPatternFromLexFunWord(int iRel);

	virtual void            EraseNode(int NodeNo) { m_Nodes.erase(m_Nodes.begin() +NodeNo);};

	// ������  ����� ����������� ���� �� ������ �������� 
    long					GetEngNodeByRusNode(long RusNodeNo) const;
	// ���������, ��� ������ ������� ���� ��� ��� ��������
	bool					WasInterpreted (long RusNodeNo) const;
	// ��������, ���  ������ ������� ���� ��� ��� ��������
	void					SetInterpreted (long RusNodeNo);

	// �������� �� ������ UnitNo �����������
	bool					IsPlugArticle(const CRossHolder* RossHolder, WORD UnitNo) const;
	bool					IsValFromRossArticle(const CSemRelation& semRel) const;

    // ��������� ����-������ (�� ���� �������� ������� ���� ������)
    bool					translate_time_node ( int MainTimeNodeNo);
	// ��������� ������� ����-������ � ������� ������� SR
	string					time_tr_by_ross(long RusNodeNo, string &brack);
    // ������� ��������. �� ���� �������� ������� ����� �������
	bool					translate_termin_node(int MainNodeNo);
	// ������� �������� �������� 
	bool					translate_binary(long NodeNo);
	// ������ ����� ������� ����� ������
	virtual long	GetClauseFirstWordNo(long ClauseNo) const;  
	// ������ ����� �����������  ����� ������
	virtual long	GetClauseLastWordNo(long ClauseNo) const;
    // ��������� ������� �������� � ���������� � ������� ��������  
	CSynRealization TranslateRelization (const CSynRealization& RusSynReal,  const CSemNode& RusNode)  const;
	// ������� ���� oneself
	void			CreateOneselfNodes();	

	// ����������� ����� ����� ��� ����������� �������������
	void			FreeWordNo(long WordNo);	

     // ������ ��������������� ������������� ���� (����� � ��������)
	virtual string	GetMorphologyOfNode(long NodeNo) const;

	// �������� �� �������� ���� ��������� ���������
	// � �������, ���� �� � ������� ���� RUSETENSE, � ������� 
	// ��������� ����� ��������� ����������� ������� �� ��������
	EngVerbTenseEnum handle_AVREM_field(long RelNodeNo,bool bEngStr = false, long TimNodeNo = -1) const;

	//  ������� ������������ ������
	void			ApplySequenceOfTenseRule();
	// ��������� �� ���� EngVerbTenseEnum ���������� �������������
	string			GetTenseString(EngVerbTenseEnum Tense) const;
	// ��������� ���������������� �������
	void			BuildAuxiliaryVerbs();
	// �������� �� ������ ���� ���������������  ��������
	bool			IsAuxVerb(long NodeNo) const;
	// �����  �� ������ ���� ���������������  ������
	bool			HasBeVerb(long NodeNo) const;
	// �������� ����� �������, ���� � ���� RESTR ������ ����� ������� ����� �����. ������
	// ��������, ������ "not_cont".
	bool			CorrectTenseByDictVerbFeatures(CEngSemNode& VerbEngNode);
	// �������� ����� �������, ���� � ���� RESTR ������ ����� ����� �����. ������,
	// ��������, ������ "not_fut". ��������������, ��� ���� ��������� ������.
	bool			CorrectTenseByDictConjFeatures(int iNode,CDictUnitInterp ConjInterp);
	// ��������� �������������� ��������� � �������������� ������
	void			MoveDopRelations();
	// �������� ���������� ���� ��������
	void			AssertValidGraph();
	// ������ ���� ������� ����������, ���� ������� ����
	long 			FindDirObj(long VerbNodeNo) const;
	// ������������� ������� ������� 
	void			HandleAdverbPositions(long VerbNodeNo);
	// ���������, ��� ������ ���������� �������� "�������" �����������
	bool			dir_obj_is_long(int NodeNo) const;
	// ���������  ����� �������� ������	
	long			GetLeftClauseTop(long ClauseNo) const;
	// ���� �������� ����� ����� �������� ������	
	bool			IsLeftClauseTop(long NodeNo) const;
	// ���� �������� ����� ����� �������� ������	
	long			GetMainClauseRoot(long ClauseNo) const;
	// ������ ��� ����, ������� ������� �� ������� 	
	void			dfs_out(size_t NodeNo, vector<long>& Nodes) const;
	// ���������, ��� ���� ������� ����� ����� ����� � ������
	
	bool			IsTheVeryLeftNodeOfClause(long NodeNo) const;

	// �������������� ���� CSemRelation::m_SyntacticRelation ��� ���������� ������;
	// �������� ����� ������� �� ��������� ������
	void			InitSyntaxRelations ();
	// �������������� ���� CEngSemNode::m_Article �� ��������� ������
	void			InitArticleField();
	// ���������, ��� �  ���� ���� ��������� ����������� RelationStr 
	bool			HasOutRelationByName(long NodeNo, string RelationStr) const;
	// ������ ��������� �� ���� NodeNo �������������� ���������. 
	void			GetOutcomingInClauseRelations (long NodeNo, vector<long>& Relations) const;
	// ���������, ���  ��� ��������� ��������� ����� �������� RelationStr
	bool			CheckAllOutcomingRelationsIfAnyExists (long NodeNo, string RelationStr) const;
	// ���������, ��� �  ���� ���� ��������� ����������� � ���� � ������������� Grammems
	bool			HasOutRelationByGrammems(long NodeNo, QWORD Grammems) const;
	// ���������, ��� �  ���� ���� ��������� ���������, ������ � ����������� ����, ������� �������� �����  Word
	int				GetOutRelationByWord(long NodeNo, string Word) const;
	// ���������, ��� �  ���� ���� ��������� ���������, ������ � ����, �������� ���������  ����� ���� POS
	bool			HasOutRelationByPoses(long NodeNo, poses_mask_t Pos) const;

	// ��������� �������������� ����� ��������
	void			translate_abbr_termin_node(int MainNodeNo);	
    // ������ ������� � ���� 
	bool			set_multiword_termin(int NodeNo, const CInnerTermin* eng_termin, int ThesId);
	// ������� ���� �� ������� ������� (�� �������� � ������ �������) � �������� ����������� ������� 
	// (������������ ������� �������� ���������)
	// ���� Tag != -1, �� ����� �� ���� �� ���������� ����������
	virtual void			dfs(size_t NodeNo, bool ConsiderUseOfNodes,long Tag = -1);

	bool				TranslateCasesIfNeed (long NodeNo);
	bool				has_plural_rel(long NodeNo) const;
	/*
	��� �������, ������� ������������ ������� �������. �������������� � �����������
	"the sooner, the better". ����� ������������ ���� POSi, ������� 
	������������� ������� �������� i-�� �������.
	*/
	void				SetPositionOfChildrenByGrammems (long NodeNo, QWORD Grammems, string Position);
	void				SetPositionsFromConj ();
	void				SetSelectiveRelations();
	long				FindNodeByLeafId (long NodeNo, int LeafId) const;
	/*
		���������, ��� � ������� ���������� Lemma ����� ���� ��������
	*/
	bool				CanBeRusVerb(const string& Lemma)  const;

	/*
	������������� ������� ��� ����������, ������� ��������� ���������������� ������������
	*/
	void				SetLongRelations();

	/*
		������������� � GF ��������� ��������� subj
	*/
	bool SetSubjPattern(CEngSemRelation& semRel);

	/*
		���������, ��� � ���������  ������  ���� ���� ������ GF1 = 1 subj	*/
	bool HasSubjAsFirstValency(const CEngSemNode& N);

	/*
	 ������� �����������, ������� ��������� ��i = 1 subj, ���� �����  ����������� ���, 
	 ���������� -1.
	*/
	bool FindSubjPattern(const CEngSemNode& N, CSemPattern& Result);


	// ��������� ������� "��-���������", "��-��������"...
	void TranslateAdverbAdjectiveRule(int EngNodeNo);

};







#endif
