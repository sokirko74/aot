#if !defined(eng_semantic_str)
	#define eng_semantic_str


#include "seman/SemanLib/SemanticStructure.h"
#include "morph_dict/AgramtabLib/eng_consts.h"
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
		ing_tn				 // working (морфологически совпадает с gerund_tn)
};


const uint64_t eAllPersons = _QM(eFirstPerson) | _QM(eSecondPerson) | _QM(eThirdPerson);
const uint64_t	eAllNumbers = _QM(eSingular) | _QM(ePlural);
const uint64_t eAllGenders   = _QM(eMasculinum) | _QM(eFeminum);
const uint64_t eAllVerbForms   = _QM(eInfinitive) | _QM(ePastIndef)  | _QM(ePastParticiple) | _QM(eGerund);
const uint64_t eAllDegrees   = _QM(eComparativ) | _QM(eSupremum);



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
	std::string        m_PosleLog;
	// домен D_POSITION (<,<<,>>,>,^, "dir_obj _")
	std::string	      m_Position;

};


struct CTenseHistory {
	EngVerbTenseEnum	m_Tense;
	std::string				m_Maker;
	CTenseHistory(EngVerbTenseEnum	Tense, 	std::string Maker)
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
	 // время для английской глагольной формы, если узел не является гл. формой, то
	 // m_Tense = zero_tn;
	 EngVerbTenseEnum	m_Tense;

public:
    
     SPosleLog		 m_PosleLog;
     // флаг для синтеза, чтобы он не пытался поставить  слово в форму, указананную в граммемах
     bool            m_bDoNotChangeForm;
 

	 //  использует для порождение всп. глагола be перед основным глаголом и конвертации глагола  в P2
	 // вспомогательный  глагол будет добавлен функцией BuildAuxiliaryVerbs
	 bool			 m_bMorphologicalPassiveForm;
	 bool			 m_bImperative;
	 std::string			 m_OneselfStr;


	 
     std::vector<CTenseHistory>  m_TenseHistory;
	 
	 void SetTense(EngVerbTenseEnum	NewTense, std::string Maker);
	 EngVerbTenseEnum GetTense() const;
	 void Init();
	 CEngSemWord();
	 CEngSemWord (const CSemWord& X);
	// принадлежит ли данная часть речи набору частей речи, который приписан слову?
	 virtual bool   HasPOS (part_of_speech_t POS) const;
};

enum ArticleCauseEnum {	ArticleFromDict, ArticleFromOrdNum, ZeroArticleForProperNames, 
						ZeroArticleBecauseOfPossessive, DefArticleBeforeClausePredicate,
						DefArticleBecauseOfNominalSupplement, DefArticleForAbstractLocal,
						NoIndefArticleForMassNouns, IndefArticleAfterAs, DefArticleBecauseDefiniteClause, 
						DefArticleForSingleRanks, OverwriteArticleForTimeNodes, OverwriteArticleForTerminNodes, ClauseRelRuleParticipleArticle,
                        IndefByModalVerb, UnknownArticleCause};

extern std::string GetArticleCauseHistory (const std::vector<ArticleCauseEnum>& t);
extern part_of_speech_t GetOnePOS(part_of_speech_mask_t poses);
enum ArticleEnum { 
	ZeroArticle, 
	DefArticle, 
	IndefArticle,
	UnknownArticle
};

extern ArticleEnum ArticleTypeByString(const std::string& s);
extern std::string ArticleStringByType(ArticleEnum t);

class CEngSemNode : public CSemNode
{
	ArticleEnum				m_Article;
	std::vector<ArticleCauseEnum>	m_ArticleCauseHistory;

public:
	std::vector<CEngSemWord> m_Words;
 	virtual const CSemWord& GetWord(int WordNo) const { return m_Words[WordNo];};
	virtual		  int		GetWordsSize() const { return m_Words.size();};
 
	int					RusNode;
	std::vector<CSemPattern> m_Patterns;

	std::vector<CSemPattern> m_CopulPatterns;
	

	bool				m_bNotUseTo;

	void SetGrammemsRich(uint64_t g);
	uint64_t GetGrammemsRich() const;
	void AddOneGrammemRich(int g);
	void AddGrammemsRich(uint64_t  grammems);
	bool HasGrammemRich(int g) const;
	void DeleteGrammemsRich(uint64_t g);
	

	CEngSemNode();
	CEngSemNode (const CSemNode& X);
	void Init ();

	EngVerbTenseEnum GetTense() const;
	part_of_speech_t GetPos() const;
	bool IsLemma(std::string Lemma) const;

	void SetArticle(ArticleEnum s, ArticleCauseEnum cause);
	ArticleEnum GetArticle() const;
	ArticleCauseEnum  GetLastArticleCause() const;
	std::string GetArticleCauseHistory() const;
};


class CEngSemRelation : public CSemRelation 
{
public:
	int  m_RusRel;
	bool m_bInterpreted;
	EClauseRelsType m_Type;	

	CSemPattern  m_Pattern; 
	int	m_PatternNoInArticle;

	std::string m_engPrep;
	bool m_bSourceClauseIsMain;
	EGramCortegeType m_GramType; //по какому типу перевелось

	int  m_iHoleNum;
	bool m_bAuxFlag;

	
	CEngSemRelation (CValency Valency,long SourceNodeNo,long TargetNodeNo, std::string SyntacticRelation);
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
	std::vector<SWeightToCortege> m_WeightGramCorteges;
	EGramCortegeType m_Type;

	std::vector<TCortege> GetCorteges()
	{
		std::vector<TCortege> corteges;
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
typedef std::map<int,std::vector<CEngInterp> >  CEnglishEquivMap;	



class CEngSemStructure : public CSemanticStructure
{
public:

	const CSemanticStructure&	RusStr;
	std::vector<CEngSemClause>		m_Clauses;

	/*
	  текущее множество переведeнных русских узлов 
	  (те узлы, для которых построены англ. узлы)
	*/
	std::vector<long>			m_InterpretedRusNodes;

	std::vector<CEngSemNode>  m_Nodes;
	
	
	std::vector<CLexFunctRel>		m_LexFuncts;
	virtual const std::vector<CLexFunctRel>&		GetLexFuncts()	const {return m_LexFuncts;};
	virtual		  std::vector<CLexFunctRel>&		GetLexFuncts()	{return m_LexFuncts;};
	std::vector<CSynRelation>		m_SynRelations;
	virtual const std::vector<CSynRelation>&		GetSynRels()  	const {return m_SynRelations;};
	virtual		  std::vector<CSynRelation>&		GetSynRels()  	{return m_SynRelations;};

    virtual const CSemNode&	GetNode(int NodeNo) const {return m_Nodes[NodeNo];}
	virtual		  CSemNode*	GetNodePtr(int NodeNo)  {return &(m_Nodes[NodeNo]);}
	virtual int				GetNodesSize() const {return m_Nodes.size();};
	virtual std::string GetInterfaceWordStr(const CSemNode* pNode, int WordNo) const;
	    // перечень всех СемО
	std::vector<CEngSemRelation>	m_Relations;
    virtual const CSemRelation*	GetRelation(int RelNo) const {return &(m_Relations[RelNo]);}
	virtual		  CSemRelation*	GetRelation(int RelNo)		 {return &(m_Relations[RelNo]);}
	virtual int					GetRelationsSize() const {return m_Relations.size();};
	virtual void				EraseRelation(int RelNo, const char* cause) {m_Relations.erase(m_Relations.begin() +RelNo);};

	virtual void GetColorAndWidthOfRelation(int RelNo,float& Width,std::string& Color);

	//===========    работа с дополнительными семантическими отношениями 
	std::vector<CEngSemRelation>		m_DopRelations;
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
  	 void CreateSimpleEnglNode(std::string lemma, CEngSemNode& engNode, int POS, bool bDontChangeForm, DictTypeEnum DictType= Aoss, int iMean = 1 );
	 void CreateSimpleEnglNodeByOldNode(std::string lemma, CEngSemNode& engNode, int POS, bool bDontChangeForm , CEngSemNode& oldNode);

	int FindEngVal(std::string strRusVal, CEngSemNode& engNode,  std::vector<long>& iBadVals, bool bUseHierarchy = true);
	bool ValencyEq(std::string strRusVal,std::string strEngVal);

	CEngInterp FindBestEnglishNodes( int iRusNode , std::vector<CEngInterp>& vectorEngEquivs, std::vector<long>& bestRels, int& iSubjRel);
	bool HasThisLexFunc(std::string LexFunctName,  CRossInterp  UnitInterp, CLexicalFunctionField& LexicalFunct);
	int GetEnglishNodeBadWeight( int iRusNode, CEngInterp& UnitInterp, std::vector<long>& goodRels, int& iSubjRels);
	int GetReverseRelValNo(const CSemRelation& rusRel,CEngSemNode& reverseNode);
	void CreateEnglNode(CEngInterp UnitInterp, CEngSemNode& engNode, const CSemNode& rusNode);
	void CreateEnglNodeForEngColloc(long UnitNo, CEngSemNode& engNode) const;
	std::string GetItemStr(long lItemNo, DictTypeEnum type /*= Aoss*/) const ;
	void TranslateOneActant( int iRel,  int iEngNode);
	void TranslateActants( int iEngNode, int iClause);
	std::string GetPrep(const TCortege& cortege, DictTypeEnum type);
	std::string HasParticularPrepInField( CRossHolder* pLocRossDoc,int  iRusActant,CEngSemNode&  engActantm);
	

	bool Rule_TranslatePoss( int iRusActant, long RelationNo, const std::vector<TCortege>& GramCorteges, int iEngNode);
	bool Rule_TranslateObj( int iRusActant, long RelationNo, const std::vector<TCortege>& GramCorteges, int iEngNode);
	bool Rule_TranslateSubj( int iRusActant, long RelationNo, const std::vector<TCortege>& GramCorteges, int iEngNode);
	bool Rule_TranslatePrepNounGroup( int iRusActant, long RelationNo, const std::vector<TCortege>& GramCorteges, int iEngNode);
	bool Rule_TranslateRelWithPrepField(int iRusActant,long RelationNo, int iEngNode);

	int  NumPrepPhr(const std::vector<TCortege>& GramCorteges, DictTypeEnum type);
	void ReadMorphFromMainGF(uint16_t UnitNo, DictTypeEnum type, CSemWord& Word) const;
	void InitEngWordAndLemma(CEngSemWord& semWord, std::string strEngLemma) const;

	typedef std::map< std::string,std::vector<SGramCortegesAndType> > SemRelToGramCortegeMap_t;
	EGramCortegeType GetGramCortegeType(TCortege& cortege, DictTypeEnum type);
	
	SemRelToGramCortegeMap_t m_SemRelToGramCortegeMap;
	void FillSemRelToGramCortegeMap();
	typedef std::pair<std::string, std::string> StringPair;
	void FillVectorOfGramCortegeAndType(std::vector<SGramCortegesAndType>& GramCortegesAndTypeV,std::vector<TCortege>& GramCortegesm, DictTypeEnum type );
	std::string BuildSentence();

	std::vector< pair<std::string, std::string> > m_SemRelEquivs;
	void FillSemRelEquivs();

	bool Rule_TranslateInfinitive( int iRusActant, long RelationNo, const std::vector<TCortege>& GramCorteges, int iEngNode);
	bool MakeDeverbative( int iRusActant, int iEngActant, CEngSemNode& engNode);
	bool HasGerund(const std::vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type );
	bool HasToPlusInf(const std::vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type );
	bool HasInf(const std::vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type );
	bool HasItem(const std::vector<TCortege>& GramCorteges, long& CortegeNo, int& ItemNo, DictTypeEnum type, std::string strItem);
	bool HasItem(const std::vector<TCortege>& GramCorteges, long& CortegeNo, DictTypeEnum type, std::string strItem);
	std::string GetGerundPrep(const TCortege& cortege, DictTypeEnum type );


	bool IsSubj (const CEngSemRelation& Rel) const ;
	// с помощью IsSubj ищется субъекта среди выходящих стрелок, выдает номер узла субъекта
	long GetSubj(long NodeNo) const;

	void InitEngVals(CEngSemNode& Node);

	bool AddGXiFromLexFunOperWord(long LexFunctWordUnitNo, CSemPattern& semPattern1, int iValNum, int iOperNum);
	part_of_speech_mask_t EngPOSesByRusPOS(part_of_speech_t rus_pos, std::string lemma);
	int InterpretRusNodeAndItsChildren( CEnglishEquivMap& mapRNodeToENode, int iRusNode);
	int InterpretOneNode( CEnglishEquivMap& mapRNodeToENode, int iRusNode, std::vector<long>& RusRelsToEngRels, int& iSubjRel);

    void FindEnglishEquivMain(CEnglishEquivMap& mapRNodeToENode );	 	 
	void FindEnglishEquiv(std::vector<CEngInterp>& engEquivs, const std::string& rus_lemma, BYTE MeanNo, DictTypeEnum   EngDictType);
	bool FindEnglishEquivHelper(std::vector<CEngInterp>& engEquivs, const CEngUnitNoToRusUnit RusEquivToEngArticleNo, DictTypeEnum type );
    bool InterpretWithPlugArticles (long RusNodeNo, CEnglishEquivMap& mapRNodeToENode);

//моё
	void CreateLexFunNodeAnRel(CLexicalFunctionField& LexicalFunct, int iRusLexFunNode,CSemRelation& rusRel, int iEngNode, int iRusRel);
	bool AddGXiFromLexFunFuncWord(long LexFunctWordUnitNo, CSemPattern& semPattern1, int iValNum, int iFuncNum );
	void CreateEngLexFunDummyRel(CLexicalFunctionField& LexicalFunct, int iLexFunNodeDummy, int iEngNode);
	enum ELexFunType { Oper, Func, Magn, NoneFun};
	bool HasCopul(std::vector<CValency>& Vals, std::string strRusVal);
	int InterpretCopulNodeAndChildren( CEnglishEquivMap& mapRNodeToENode, int iRusCopulNode, int iEngCopulParentNode, int iValNum);	
	void BuildCopulNodeSentence(int iEngNode,bool& bSubjFound, std::string& sent1, std::string& sent2 );
	void IndexSemFets();


	translate_helper helper;
	bool init_helper();
	void CreateEngClauses();
	TCortege CreateCortege(std::string strGX, std::string strDomen);	
	void AddSemRelToGramCortege(std::string semRel, TCortege cortege);
	EGramCortegeType GetGramCortegeType(std::string& gram_str);
	void GetGramCortegesAndTypeFromRel(std::vector<SGramCortegesAndType>& GramCortegesAndTypeV, CEngSemRelation& semRel);
	void GetIncomingRelationsInThisClause(int iNode, std::vector<long>& IncomeRels) const;
	
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

	void GetEngEquivsFromRusArticle(std::vector< SEngEquiv >& vectorEngEquivs, int RusUnitNo, DictTypeEnum   DictType  = Ross, int iRusNode = -1) const; 
	void GetEngEquivsFromRusArticle_test(std::vector< SEngEquiv >& vectorEngEquivs, int RusUnitNo, DictTypeEnum   DictType) const; 
	void IntersectEngEquivs(std::vector< SEngEquiv >& vectorEngEquivsFromRusArticle, std::vector<CEngInterp>& vectorEngEquivsFromAoss );
	bool HasThisGX(const std::vector<TCortege>& gramCorteges, std::string value, DictTypeEnum type) const;
	void ChangeClauseNo(int oldClauseNo, int newClauseNo);

	bool Init();
	bool TranslateToEnglish( );

	bool HasNeg(long UnitNo, DictTypeEnum type );
	bool HasQuest(long UnitNo, DictTypeEnum type );

	void GetChildNodes(int iNode,std::vector<long> &nodes) const;
	void GetAlgNodes(std::vector<long> &nodes) const;
	void ChangeNegWord(int iNode);
	void MakeEngInfinitive(int iNode);
	void ChangeGrammemsForDirObj(int iNode);

	void MoveMeaningRelsToNewNode(int iOldNode,int iNewNode);
	int  MakeBeNodeForEngNode(int iEngNode,std::string sTenseHistory,bool bCheckOtherVerb = true);
	
	void RefineEngCollocPreps();
	void RefineComparativeMNAwithPreps();
	void RefineUnknownRelsSynRel();
	void CorrectNodeNumByRelNum();
	void AddFixedGrammemsToNode();
	
	bool CompareCortegeItems(const CRossHolder* RossHolder,
		const TCortege &X,const TCortege &Y) const;

	bool HasALG(DictTypeEnum type, long UnitNo, std::string strAlg);
	
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

	bool GetSINO(DictTypeEnum type,long unit,std::vector<TCortege>& vecSINO) const;
	void GetSPrp(DictTypeEnum type,long unit,std::vector<TCortege>& vecSPrp) const;
	void GetSArt(DictTypeEnum type,long unit,std::vector<TCortege>& vecSArt) const;
	void GetSGxi(DictTypeEnum type,long unit,std::vector<TCortege>& vecSGxi) const;
	void ConvertClosedCollocToOpen();

	bool IsQuestionClause(long ClauseNo) const { assert(false); return false; }
	bool CheckQuestionClause(int iEngRoot,int &iQueRel,int &iSubRel);
	void HandleQuestionClause(int iEngRoot);
	bool HandleQuestionNode();

	int GetEngRelByRusRel(int iRusRel);
	void FindEngEquivForRusArticle(CEngInterp rusUnit, std::vector<CEngInterp>& vectorEngEquivsFrom, DictTypeEnum   EngDictType);
	void GetEngEquivsFromVector(CEnglishEquivMap& mapRNodeToENode, int iRusNode, std::vector< SEngEquiv >& vectorEngEquivs, std::vector<DictTypeEnum> EngDictTypes);
	SEngEquiv GetV0Value(const CSemNode& rusActant);
	void SetPositionOfActantInColloc(CEngSemRelation& engRel, CSemPattern& semPattern, int iEngNode);
	void CreateBeSemNode(CEngSemNode& semNode, int& iSubjPattern , int& iObjPattern );
	bool CheckDomensForCortege(StringVector& domens, TCortege& cortege, DictTypeEnum   DictType ) const; 
	BYTE GetPosFromPosBit(size_t Pos) const;
	bool IsSubjPattern(const CEngSemRelation& Rel ) const ;
	void CreateEnglNodeForEngObor(long UnitNo, CEngSemNode& engNode) const;
	bool GetDictUnitInterpForObor(std::string oborStr, CRossInterp& interp);	
	bool IsSubConj(CRossInterp interp);
	bool SetSimpleEngPrep(std::string prep, int iNode, int iRel);

	void GetAFieldVector(std::string FieldStr, DictTypeEnum type, std::vector<TCortege>& vectorAGX, long UnitNo) const;
	
	
	
	std::string GetCortegeStr(DictTypeEnum type, const TCortege& cortege) const;
	bool IsGramFet(const CEngSemRelation& Rel , const std::string strPattern) const;
	bool HasGramFet(const CEngSemRelation& Rel, const std::string strPattern) const;
	bool IsObjPattern(const CEngSemRelation& Rel ) const;
	std::string GetConj(const std::vector<TCortege>& GramCorteges, DictTypeEnum type);
	void HideCopul();
	void FindEngWords(std::vector<CEngInterp>& resEngUnits, std::vector< SEngEquiv >& vectorEngEquivs, std::vector<DictTypeEnum> EngDictTypes);
	void CreateHaveSemNode(CEngSemNode& newNode, int& iSubjPattern , int& iObjPattern, int iMeanNum=1 );
	
	void ReverseClauseRel();
	void FindEquivForLexFunct(CEnglishEquivMap& mapRNodeToENode);
	void FilLexFunctRel();
	bool TryToAddNewSemPatternFromLexFunWord(int iRel);

	virtual void            EraseNode(int NodeNo) { m_Nodes.erase(m_Nodes.begin() +NodeNo);};

	// Выдает  номер английского узла по номеру русского 
    long					GetEngNodeByRusNode(long RusNodeNo) const;
	// проверяет, что данный русский узел уже был перевден
	bool					WasInterpreted (long RusNodeNo) const;
	// помечает, что  данный русский узел уже был перевден
	void					SetInterpreted (long RusNodeNo);

	// является ли статья UnitNo заглушечной
	bool					IsPlugArticle(const CRossHolder* RossHolder, uint16_t UnitNo) const;
	bool					IsValFromRossArticle(const CSemRelation& semRel) const;

    // переводит тайм-группу (на вход подается главный узел группы)
    bool					translate_time_node ( int MainTimeNodeNo);
	// переводит элемент тайм-группы с помощью функции SR
	std::string					time_tr_by_ross(long RusNodeNo, std::string &brack);
    // перевод терминов. На вход подается главное слово термина
	bool					translate_termin_node(int MainNodeNo);
	// перевод бинарным словарем 
	bool					translate_binary(long NodeNo);
	// выдает номер первого слова клаузы
	virtual long	GetClauseFirstWordNo(long ClauseNo) const;  
	// выдает номер последенего  слова клаузы
	virtual long	GetClauseLastWordNo(long ClauseNo) const;
    // переводит русские предлоги в английские с помощью оборотов  
	CSynRealization TranslateRelization (const CSynRealization& RusSynReal,  const CSemNode& RusNode)  const;
	// создает узел oneself
	void			CreateOneselfNodes();	

	// освобождает номер слова для дальнеьшего использования
	void			FreeWordNo(long WordNo);	

     // выдает морфологическое представление узла (лемма и граммема)
	virtual std::string	GetMorphologyOfNode(long NodeNo) const;

	// проходит по сыновьям узла указанной структуры
	// и смотрит, есть ли у сыновей поле RUSETENSE, в котором 
	// прописана схема получения английского времени по русскому
	EngVerbTenseEnum handle_AVREM_field(long RelNodeNo,bool bEngStr = false, long TimNodeNo = -1) const;

	//  правило согласования времен
	void			ApplySequenceOfTenseRule();
	// получение по типу EngVerbTenseEnum строкового представления
	std::string			GetTenseString(EngVerbTenseEnum Tense) const;
	// выделение вспомогательного глагола
	void			BuildAuxiliaryVerbs();
	// является ли данный узел вспомогательным  глаголом
	bool			IsAuxVerb(long NodeNo) const;
	// имеет  ли данный узел вспомогательный  глагол
	bool			HasBeVerb(long NodeNo) const;
	// изменяет время глагола, если в поле RESTR статьи этого глагола стоят соотв. пометы
	// Например, помета "not_cont".
	bool			CorrectTenseByDictVerbFeatures(CEngSemNode& VerbEngNode);
	// изменяет время глагола, если в поле RESTR статьи союза стоят соотв. пометы,
	// например, помета "not_fut". Предполагается, что союз подчиняет глагол.
	bool			CorrectTenseByDictConjFeatures(int iNode,CDictUnitInterp ConjInterp);
	// переносит дополнительные отношения в дополнительный список
	void			MoveDopRelations();
	// проверка валидности всех индексов
	void			AssertValidGraph();
	// выдает узел прямого дополнения, если таковой есть
	long 			FindDirObj(long VerbNodeNo) const;
	// устанавливает позицию наречий 
	void			HandleAdverbPositions(long VerbNodeNo);
	// проверяет, что прямое дополнение является "длинным" дополнением
	bool			dir_obj_is_long(int NodeNo) const;
	// получение  левой вершиной клаузы	
	long			GetLeftClauseTop(long ClauseNo) const;
	// узел является самой левой вершиной клаузы	
	bool			IsLeftClauseTop(long NodeNo) const;
	// узел является самой левой вершиной клаузы	
	long			GetMainClauseRoot(long ClauseNo) const;
	// выдает все узлы, которые зависят от данного 	
	void			dfs_out(size_t NodeNo, std::vector<long>& Nodes) const;
	// проверяет, что узел являетс самым левым узлом в клаузе
	
	bool			IsTheVeryLeftNodeOfClause(long NodeNo) const;

	// инициализирует слот CSemRelation::m_SyntacticRelation для валентнтых связей;
	// название связи берется из словарной статьи
	void			InitSyntaxRelations ();
	// инициализирует поле CEngSemNode::m_Article по словарной статье
	void			InitArticleField();
	// провереят, что у  узла есть выходящая валентность RelationStr 
	bool			HasOutRelationByName(long NodeNo, std::string RelationStr) const;
	// выдает выходящие из узла NodeNo внутриклаущные отношения. 
	void			GetOutcomingInClauseRelations (long NodeNo, std::vector<long>& Relations) const;
	// проверяет, что  все выходящие отношения имеют название RelationStr
	bool			CheckAllOutcomingRelationsIfAnyExists (long NodeNo, std::string RelationStr) const;
	// провереят, что у  узла есть выходящая валентность в узел с валентностями Grammems
	bool			HasOutRelationByGrammems(long NodeNo, uint64_t Grammems) const;
	// проверяет, что у  узла есть выходящее отношение, идущее в примитивный узел, который содержит слово  Word
	int				GetOutRelationByWord(long NodeNo, std::string Word) const;
	// проверяет, что у  узла есть выходящее отношение, идущее в узел, которому приписана  часть речи POS
	bool			HasOutRelationByPoses(long NodeNo, part_of_speech_mask_t Pos) const;

	// переводит аббревиатурные формы теримнов
	void			translate_abbr_termin_node(int MainNodeNo);	
    // запись термина в узел 
	bool			set_multiword_termin(int NodeNo, const CInnerTermin* eng_termin, int ThesId);
	// обходим граф от текущей вершины (по стрелкам и против стрелок) и помечает достигнутые вершины 
	// (используется обычный перечень отношений)
	// если Tag != -1, то обход не идеь по уникальным отношениям
	virtual void			dfs(size_t NodeNo, bool ConsiderUseOfNodes,long Tag = -1);

	bool				TranslateCasesIfNeed (long NodeNo);
	bool				has_plural_rel(long NodeNo) const;
	/*
	две функции, которые обрабатывают позиции сравнит. прилагательных в конструкции
	"the sooner, the better". Здесь используются поля POSi, которые 
	устанавливают позиции актантов i-го актанта.
	*/
	void				SetPositionOfChildrenByGrammems (long NodeNo, uint64_t Grammems, std::string Position);
	void				SetPositionsFromConj ();
	void				SetSelectiveRelations();
	long				FindNodeByLeafId (long NodeNo, int LeafId) const;
	/*
		проверяет, что в русской морфологии Lemma может быть глаголом
	*/
	bool				CanBeRusVerb(const std::string& Lemma)  const;

	/*
	устанавливает позицию для дополнений, которые осложнены определительными придаточными
	*/
	void				SetLongRelations();

	/*
		устанавливает в GF отношения константу subj
	*/
	bool SetSubjPattern(CEngSemRelation& semRel);

	/*
		проверяет, что в словарной  статье  узла есть запись GF1 = 1 subj	*/
	bool HasSubjAsFirstValency(const CEngSemNode& N);

	/*
	 находит валентность, которой приписано ГХi = 1 subj, если такой  валентности нет, 
	 возвращает -1.
	*/
	bool FindSubjPattern(const CEngSemNode& N, CSemPattern& Result);


	// Переводит наречия "по-английски", "по-хорошему"...
	void TranslateAdverbAdjectiveRule(int EngNodeNo);

};







#endif
