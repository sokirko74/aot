#if !defined(semantic_rus_str)
#define semantic_rus_str

#pragma warning (disable : 4786)

#include "SemanticStructure.h"
#include "SemanticWeight.h"
#include "../SynCommonLib/RelationsIterator.h"




// ==== класс русское слово
class CRusSemWord  : public CSemWord
{
public:	 
  // графематическая помета ЗПР
  bool           m_IsPunct;
  // графематическая помета ИЛЕ
  bool           m_ILE;
  // знаки препинаия, которые стоят в тексте после этого слова 
  string         m_PostPuncts;
  string		 m_GraphDescrs;
  //  лемма прилагательного превосходной степени, которая была заменена на положительную степень
  string		 m_SupAdjLemma;
  bool			 m_bRomanNumber;
  long			 m_SynWordNo;

  void			Init();
  CRusSemWord();
  CRusSemWord   ( long WordNo, string Lemma );

  // словоформа - глагольная форма, которая заканчивается на "ся", "сь" 
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
	//  если в GF-главном стоит после двоеточия ВВОДН
	bool			m_bSaveIsParenthesis;
	// предпочтительный порядок следования валентностей в тексте, который вычисляется по словарной статье
	vector<long>      m_ValsLayout;
	// несовместные валентности (взяты из поля INCONS)
	vector<PairOfLong>      ProhibitedPairOfVals;
	// множество множеств валентностей совместных друг с другом

	vector<VectorInt>     m_NotProhibitedValSets;
	//  если в GF-главном стоит после двоеточия c_опр (например, ПРИЛ:с_опр),
    // то следующий флаг - истина
	bool			m_bFullGleiche;

	// если узел - существительное, тогда здесь лежат глаголы, 
	// для которых это существительные является S0
	StringVector    m_VerbsOfS0;

	bool			  m_bConjWord;

		/* эти два слота ограничивают отрезок, в котором можно
	  искать актанты данного узла, например,
	   "как ушел, так и пришел"
	   здесь актанты "ушел" могут быть только 
	   между "как" и "так"
	   если m_MinActantWordNo	 == -1, тогда
	   актанты можно искать по всей клаузе
	*/
	long			m_MinActantWordNo;
	long			m_MaxActantWordNo;

};

// ==== класс "Русский узел"
class CRusSemNode : public CSemNode, public CRusInterpNodeInfo
{
public:
	vector<CRusSemWord>		m_Words;
	const CSemWord&	GetWord(int WordNo) const { return m_Words[WordNo];};
	int		GetWordsSize() const { return m_Words.size();};

	//  номер группы, из который был образован этот узел (в наборе IRelationsIteratorPtr::FirmGroups)
	int						m_SyntaxGroupNo; 
	// Помечает узел, если этот узел вошел цикл
	bool					m_bInCycle;
	// номер слова, на котором стоял предлог, если этот предлог был
	long			m_PrepWordNo;
	// узел является сравнительнымм прилагательным, для которого не надо искать второго актанта 
	// например, "прыгайте выше, друзья"
	bool			m_bCompWithOneActant;
	// истина, если этот узел является первым по тексту узлом в термине
	bool			m_bFirstInTermin;
	// узел является сравнительнымм прилагательным
	bool			m_bCompAdj;

	//  сохраненные номера исходящих и входящих семантических отношений 
	// (используются на участках программы, где множество отношений не меняется)
    vector<long>	m_InRels;
	vector<long>	m_OutRels;

	long 			m_MemNodeNo;

	BYTE			m_IncomRelsCount; 


	UINT			m_RichPoses;

	bool			m_bQuestionWord;
	string			m_AntecedentStr;


	CRusSemNode();

	bool IsILENode() const;
	// проверяет, стоит ли после последнего слова узла знак препинания
	bool HasPostPunct(BYTE punct_char) const;
    // проверяет, стоит ли сразу же после последнего слова узла запятая
	bool HaveCommaAfter() const;
    // дает  номер статьи для текущей интерпреции для проверки SF
	WORD         GetSemCorrectUnitNo() const;
	// проверяет, что валентности не являются несовместными
	// (здесь используется слот m_RelationId, в котором хранится уникальный номер валентности
	// в графе, который строит функция СRusSemStructure::IndexRelations)
	bool RelsCanBeTogether(const long RelationId1,  const long RelationId2) const;
	// является ли данный узел кавычкой
	bool   IsOneQuoteMark () const;
	// является ли данный узел тире
	bool   IsOneDash () const;
	bool IsMNA_ButNotCHEM () const;
    // сначала лежат IsWordContainer(),потом IsAbstract(), и для каждой клаузы отдельно
    bool operator < (const CRusSemNode& Node) const;
	bool IsAnaphoricPronoun () const;
	QWORD GetInnerGrammems() const;
	bool IsLemma(string Lemma) const;
	bool IsWordForm(string WordForm) const;
	bool IsLemmaList( const char*  FirstLemma, ... ) const;
	bool IsEqualMorph (const CRusSemNode& N) 	const;
	void  SetMainWordNo (long WordNo);
	

};

class CMemNode : public CInterpNodeInfo, public CRusInterpNodeInfo
{
public:
	vector<CRusMorphHomonym> m_Words;
	CDictUnitInterp			 m_Interp;
	long					 m_PrepsCount;
	long					 m_FirstPrepUnitNo;
	QWORD					 m_Grammems;
	//случай, когда этот узел участвовал в построении отношений
	set<long>				 m_Cases;
	CMemNode () {
		m_PrepsCount = 0;
		m_FirstPrepUnitNo = ErrUnitNo;
		m_Grammems = 0;
	};
};

// ==== класс "Русское отношение"
class CRusSemRelation : public CSemRelation {
public:
	// истина, что отношение не может проходить только над одной запятой (оно должно проходить еще над чем-то)
    bool				m_CannotHaveOnlyCommaBetween;
	// истина, если отношение согласовано по SF (для текущего древесного варианта)
	bool				m_bSemFetAgree;
	// номер поля эквивалнтности, которое включает отношения, выходящие из одного узла и противоречащие друг другу 
	long				m_EqualFieldId;
	//длина отношения в "жестких" синтаксических деревьях
	long			m_LeapsCount;
	//технический слот: помечает отношения, которые точно останутся в древесном варианте
	long			m_bUniqueRelation;
	// номер межклаузного правила, по которому была проведена эта межклаузная связь
	long			m_ClauseRuleNo;

	// истина, если название отношение согласовано с частью речи 
	// узла, в которое оно идет
	// например, PROPERT обычно идет в прилашательное// а SUB в существительное	
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


// истина, если между двумя узлами стоит глагол в личной форме
const BYTE HaveFiniteVerbBetween = 1;
	// истина, если между двумя узлами стоит только союз c запятой или без нее
	// ( и ничего больше).
	// например 1. Петя и Вася, 
	//			2.больше, чем в Москве
	// Предполагается, что в этом случае между этими двумя узлами нельзя провести 
	// простую валентную стрелку.
const BYTE HaveOnlyConjBetween = 2;
	// истина, если два узла имеют только одну запятую между собой
	// и больше ничего
const BYTE HaveOnlyCommaBetween = 4;
	// истина, если два узла имеют запятую между собой
	// могут иметь что-то еще
const BYTE HaveCommaBetween = 8;
    // см. комментарий к функции AreOldNodesOfTheSameCase
const BYTE OldNodesOfTheSameCase= 16;


// ==== класс "Русская клаузы"
class CRusSemClause : public CSemClause 
{
	// матрица, в которой сохранены значения некоторых позиционных  оценок узлов 
	vector < vector<BYTE> > m_GrammarMatrix;
public:
	// ======= чисто синтаксическая информация
	// синтаксический номер клаузы
	long						m_SyntaxClauseNo;
	// номер подлежащего  (по синтаксическому анализу - подчеркивается одной чертой в VisualSynan) 
	long						m_SubjectWordNo;
	// номер сказуемого (по синтаксису - подчеркивается двумя чертами в VisualSynan)
	long						m_PredicateWordNo;
	// номер вершины клаузы
	long						m_RootWordNo;
    // тип клаузы 
	long						m_ClauseType;
	// номер морфологического варианта клаузы
	long						m_ClauseVariantNo;

	// =======  словарная интерпретация 
	// текущий номер словарной интерпретации
	long						m_CurrLexVariantNo;
	// номер в массиве результатов анализа всех клауз (два одинаковых морфологических варианта 
	// одной клаузы анализируются только один раз)
	long						m_AlreadyBuiltClauseVariantNo;

	// номер узла, который считается вершиной клаузы (по синтаксическому критерию)
	// чаще всего значение этого слота равно значению m_RootWordNo,
	// но если m_PredicateWordNo = -1, тогда семантика сама ищет вершину
	long						m_ClauseSyntaxTop;
	// номер клаузы, которая подчинила данную клаузу с помощью межклаузного отношения
	long						m_HostClause;
	// номер правила, по которому было поставлено межклаузное отношение
	long						m_ClauseRuleNo;
	// множетство добавленных морфологических вариантов клауз
	vector<CSyntaxClauseVariant> m_AddedClauses;
	// номер первого узла клаузы
	long						m_BeginNodeNo;
	// номер последнего узла клаузы
	long						m_EndNodeNo;
	// стек, для сохранения значений номеров узлов
	stack<long>			m_NodeRef;
	// если в клаузе есть "больше"  или "меньше",  то прямое дополнение может стоять в родительном  
	bool						m_bHasNumeralComp; // больше или меньше

	// есть ли в сравнительный союз "ЧЕМ"
	bool						m_bHasCHEM;
	// кол-во узлов, принадлежащих предметной области предложения
	int						    m_BestPONodesCount;
    // естьли в клаузе  глагол с отрицанием
	bool						m_bSaveClauseHasNegationInVerb;



	// освобождает номер слова WordNo (на это место будет поставлено новое слово)
	void FreeWordNo (long WordNo);
	CRusSemClause();
	// выдает число узлов клаузы
	long GetNodesCount() const;
	
	void InitGramMatrix ();
	void AddFlagToGramMatrix (long NodeNo1, long NodeNo2, BYTE Value);
	// выдает значение позиционных оценок для пары узлов 
	const BYTE GetGramMatrix (long NodeNo1, long NodeNo2) const;
	void UpdateSizeOfGramMatrix();
	

	// сохраняет и восстанавливает ссылки на узлы 
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
// одно клаузное правило, которое проводит одно клаузное отношение
typedef bool (CRusSemStructure::*TClauseRule)(long, long, long);	  
struct CSemanClauseRule {
	// собственно, сама функция
	TClauseRule		m_Rule;
	// направление применения этого правила
	DirectEnum		m_Direction;
	// имя правила
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

// Этот класс используется пока только для нахождения групп времени
struct CNodeHypot {
	// номер узла
	long m_NodeNo;
	// этаж в поле LEXi
	long m_LexFetNo;
	// этаж в поле GFi
	long m_GramCortegeNo;
	// номер предлога в словаре оборотов
	long m_PrepNo;
	// номер предлога в поле PREP
	long m_PrepNoInArticle;
	// верно, если  GF = ЦК_порядк для этого узла
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
 в этом  классе содержится вариант и информация о лучшем
 дереве этого варианта
*/
class CLexVariant : public CTreeOfLexVariantWeight
{
	public:
	// перечень всех СемУ
	vector<CRusSemNode>			m_Nodes;
	// перечень всех СемО
	vector<CRusSemRelation>		m_Relations;

	// перечень всех дополнительных СемО
	vector<CRusSemRelation>		m_DopRelations;

	// клаузы предложений
	vector<CRusSemClause>		m_Clauses; 


	// перечень всех СинО (как их построил синтаксис)
	vector<CSynRelation>		m_SynRelations;

	
	// перечень всех LF, найденных в предложении
	vector<CLexFunctRel>		m_LexFuncts;

	// перечень всех семантичесиких отношений, которые пришли
	// из поля AUX терминов, которые нашел тезаурус
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

	// этот слот обозначает, что структура была создана, 
	// но  не проинициализирована
	bool m_bNew;
	// =====  определяющая информация
	// добавленные клаузы
	vector<CSyntaxClauseVariant>	m_AddedClauses;

	//======  информация о самых лучших лексических вариантах 
	vector<CLexVariant> m_BestLexVariants;

	//======  вспомогательныя информация для статистики
	long m_NodesCount;
	long m_PanicRelationsCount;
	/*
	 номер калузы, для которой был построен CSemClauseVariantResult так,
	 как он упоминается во в CSemNode::m_ClauseNo из слота
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
	 // в Release просто не будем добавлять отношения, если их слишком много
	 if (m_RelsCount >= MaxRelSet-1)
		 return;
	 m_Rels[m_RelsCount++] = RelNo;
 };
 void Insert (WORD No, WORD RelNo)
 {
	 //assert(m_RelsCount < MaxRelSet-1);
	 // в Release просто не будем добавлять отношения, если их слишком много
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
	// входное СинП одного предложения
	const CSentence*			m_piSent;
   // номер варианта комбинации клаузы 
	long						m_ClauseVariantsCombinationNo;
	// номер варианта комбинации клаузы, который заказал пользователь
	long						m_UserClauseVariantsCombinationNo;
	// номер варианта дерева, который заказал пользователь
	long						m_UserTreeVariantNo;

	// множество наденных множеств гипотез о словосочетаниях для каждой клаузы 
	vector<CSemCollocHypVectorVector>	m_ClauseSetCollocHyps;


	//все узлы и все отношения, которые были построены во всех вариантах
	vector<CMemNode>			m_MemNodes;
	vector<CRusSemRelation>     m_MemRelations;
	long						m_MemoryCaseNo;
	long						m_MemSaveRelationCount;
	size_t						m_SentenceCount;
	map<pair<int,CRusSemNode>, int>		m_PriorNounNodes2Distance; 
	// максимально разрешенное кол-во вариантов деревьев
	long						m_PanicTreeVariantCount;
	// множество всех слабых синтаксических отношений (которые можно изменять на семантике)
	StringVector				m_WeakSynRels;
	// запрещенные  лексические варианты (устанавливаются пользователем)
	vector<CRossInterpAndLemma>			m_UserProhibitedLexVars;
	// число клаузных(морфологических) вариантов			
	long						m_ClauseCombinationVariantsCount; 
	// определяет, в каком режиме нужно запускать межклаузные правила, если 
	// m_ClauseConnectionType != AllPossibleClauseRules, тогда некоторые правила не работают
	ClauseRulesTypesEnum        m_ClauseConnectionType;
	/* на самом последнем этапе с помощью  флага m_bLastTry отключаются некоторые проверки
       по словарным статьям
	*/
	bool						m_bLastTry;
	// сингнал о том, что пользователь хочет остановить весь процесс
	bool						m_bShouldBeStopped;
	// информация для ProgressBar
	long						m_InterfaceClauseNo;
	long						m_AllClausesVariants;

	//  информация для синтеза о том, какие леммы надо заменить на другие
	map<string,string>			m_SynthLemmaToReplace;

	// перечень всех СемУ
    const CSemNode&		GetNode(int NodeNo) const;
	CSemNode*			GetNodePtr(int NodeNo);
	int					GetNodesSize() const;
	string				GetInterfaceWordStr(const CSemNode* pNode, int WordNo) const;
	void				EraseNode(int NodeNo);
	

    // перечень всех СемО
    const CSemRelation*	GetRelation(int RelNo) const;
	CSemRelation*		GetRelation(int RelNo);
	int					GetRelationsSize() const;
	void				EraseRelation(int RelNo);
	void				AddRelation(const CRusSemRelation& R);
	void				GetColorAndWidthOfRelation(int RelNo, float& Width, string& Color);
	// перечень всех дополнительных отношений, найденных в предложении
    const CSemRelation*	GetDopRelation(int RelNo) const;
	CSemRelation*		GetDopRelation(int RelNo);
	int					GetDopRelationsSize() const;
	void				EraseDopRelation(int RelNo);

	size_t				GetSynWordsCount() const;


	// проверяет, что узлы, которые связывает отношение не являются абстрактными
	bool						IsWordContainerRel(int iRel)  const;
	const vector<CSynRelation>&	GetSynRels() const;
	vector<CSynRelation>&		GetSynRels();
	const vector<CLexFunctRel>&	GetLexFuncts() const;
	vector<CLexFunctRel>&		GetLexFuncts() ;


	const CSemClause&			GetSemClause(long ClauseNo) const;
	const long					GetSemClausesCount() const;
	void						FindPossibleAntecedents();

	// межклаузные синтаксические отношения
	vector<CSynRelation>		m_SynClauseRels;
	
    // коэффициенты, с помощью которых взвешивают заключительную структуру
	TreeVariantValueCoefs	m_SemCoefs; 


	// построенные морфологические варианты клауз
	vector<CSemClauseVariantResult>  m_AlreadyBuiltClauseVariants;    

	//вектор всех правил для межклаузных связей
	vector <CSemanClauseRule>  m_ClauseRules;



    CRusSemStructure();
	//=====================   Основные функции
	// основная функция построения предложения 
	long						FindSituations(size_t SentNo);
	// построение узлов по синтаксическому анализу и чтение СинО (интерпретация синтаксиса)
	void						BuildSemNodesBySyntax();
	// основная функция для построения СемП для одного морфологического варианта одной клаузы
	long						FindSituationsForClauseVariantCombination();
	// построение анафорических отношений 
    void						BuildAnaphoricRels();
    // построение одного лексического варианта одной клаузы
	CLexVariant					BuildTheVariant(long ClauseNo);
	// построение графа гипотетических связей  для одного лексического варианта
	bool						BuildHypotRelationsGraph(size_t ClauseNo);
	// запускает процедуру построения межклаузных связей
	void						ConnectClausesForLexVariantCombination();
	// построение LF-параметров для всей клаузы, значение которых обычно - глагол ("сдавать экзамен")
	void						BuildVerbLexFunctParameter(long ClauseNo);
	// построение LF-параметров, значение которых обычно - глагол, для конкретных узлов 
	void						BuildVerbLexFunctParameterForTheNodes (long  SitNodeNo, long ParamNodeNo);
	// построение LF-параметров, значение которых обычно - прилагательное или наречие ("проливной дождь")
	void						BuildAdjOrAdvLexFunctParamNodes(long ClauseNo);
	// выдает число LF в одной клаузе
	size_t						CountLexFunctsInOneClause(long ClauseNo);
	// выдает оценку структуры
	long						GetStructureWeight ();


	//======================    работа с синтаксическими отношениями  (СинО)

	// выдает все входящие в узел NodeNo СинО
	void			GetIncomingSynRelations  (long NodeNo, vector<long>& Relations) const;
	// выдает число инцидентных узлу NodeNo СинО
    long			GetSynRelationsCount (long NodeNo) const;
	// выдает номер узла, из которого выходит отношение, идущее в NodeNo, если число отношений, входящих в NodeNo,
	// равно 1, в противном случае выдает -1
	long			GetSynHost (long NodeNo) const ;
	// выдает число входящих в узел NodeNo СинО
	long			GetIncomingSynRelationsCount (long NodeNo) const;
	// выдает число выходящих из узла NodeNo СинО
	long			GetOutcomingSynRelationsCount (long NodeNo) const;
	// выдает все выходящие из узла NodeNo СинО
	void			GetOutcomingSynRelations (long NodeNo, vector<long>& Relations) const;
	// удаляет синтаксические отношения
	void			DeleteSynRelations(vector<long>& Rels);
	// удаляет входящие и выходящие отношения узла, имющие название rName
	void			DeleteSynRelationsByName(long NodeNo, string Name);
	// переносит входящие синтаксические отношения от FromNode к ToNode
	void			MoveIncomingSynRelations(long FromNode, long ToNode);
	// переносит все синтаксические отношения от FromNode к ToNode
	void			MoveSynRelations(long FromNode, long ToNode);
	// выдает истину, если в узел входит хотя бы одно "сильное" синтаксическое отношение
	bool			HasIncomingNotWeakSynRelation (long NodeNo) const;
	// выдает самый правый узел Х такой, что из X в NodeNo есть путь из "сильных" синтаксических
	// отношений.
	long			GetDefiniteRightHostByNotWeakSynRelation (long NodeNo) const;
	// проверяет, что  у узла NodeNo есть входящее или исходящее отношение RelationStr
	bool			HasSynRelation(long NodeNo, string RelationStr) const; 
	// проверяет, что  у узла NodeNo есть исходящее отношение RelationStr
	bool			HasOutcomingSynRelation(long NodeNo, string RelationStr) const; 
	// свободная локативная валентность
	void			HeuristicsLocative(long ClauseNo); 
	void			DeleteSynRelationsOfClause(long ClauseNo);
	
	

	//=====================   Основные примитивы
	
	//выдает по узлу  его подлежащее
	long						GetRusSubj(int NodeNo);
	// проверяет, что данный узел - сочинительный союз
	bool						IsCoordConj(long NodeNo)  const;
	// проверяет, что данный узел - простой подчинительный союз
	bool						IsSimpleSubordConj(long NodeNo)  const;
	// проверяет, что данный узел - союзное слово
	bool						IsConjWord (long NodeNo) const;
    // проверяет, что данный узел  является вводным словом или оборотом
	bool	IsParenthesis (long NodeNo) const;
	// собирает все узлы из клаузы ClauseNo, которые удовлетворяют  условию Fun2
	// и не помечены m_bToDelete
	void          FindIf(long ClauseNo,bool (CRusSemStructure::*Fun)(long) const,vector <long>& Nodes) const;
	// выдает омоним слова  WordNo для текущей морфологической интерпретации клаузы ClauseNo
	bool  GetHomonym (long ClauseNo, long WordNo, CSynHomonym& Homonym) const;
	// проверяет часть речи узла
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

	//=====================   Межклаузные связи  и работа с клаузами
	// вспомогательные функции для работы межклаузных правил
	// не нарушает ли возможное отношение между NodeNo1 и NodeNo2 законы проективности
	bool			CanHaveRightClauseRelation (long NodeNo1, long NodeNo2) const;
	// ищет внутри клаузы примитивный узел с леммой "КОТОРЫЙ" или "КАКОЙ"
	long			GetKOTORYJorKAKOJ (long ClauseNo) const;
	// создает отношение между клаузами 
	bool            CreateClauseRelation(long ClauseRuleNo, long ClauseNo1, long ClauseNo2, CValency V, long TargetNodeNo, CDictUnitInterp Conj, long SourceNodeNo = -1);
	// проверяет, в начале клаузы стоит подчинительный узел и выдыает его
	const CDictUnitInterp*    GetSubordConjFromTheBegining (long ClauseNo) const;
	// проверяет, что в начале клаузы стоит простой сочинительный узел и выдыает его
	long            GetCoordSimpleConj (long ClauseNo) const;
	// проверяет, что у узла есть незаполненная валентность с указанными характеристиками 
	bool            GetFreeActantPattern (long NodeNo, CSemPattern& P, CSynRealization& SynReal, string SynRel, bool CheckSynRel,  string SynFet, bool CheckSynFet);

	// собственно, межклаузные правила (см. примеры в конструкторе)
	bool			TryClauseCHTOBY_GG (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseConditionalImperative (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseCHTO_WITH_ANIMAT (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseKOTORYJ (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseZeroSubordWithoutAntecedent(long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseSubordConj(long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseCHTO_TOT_Dop(long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool			TryClauseSimpleComma (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	long			GetClauseCoordSimpleConj (long ClauseNo1) const;
	// выдает граммемы числа и лица по сказуемому и подлежащему (если последнее есть)
	QWORD			GetPersonAndNumberGrammemsOfPredicate (long NodeNo);
	bool            TryClauseCoordSimpleConj (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	// ищет союзное слово в начале клаузы, если находит, тогда выдает номер узла
	long			GetConjWordFromClause (long ClauseNo) const;
	bool            TryClauseConjWord(long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseSubordDoubleConj (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseAnaphoricSubordWithoutAntecedent (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClausePrichastie (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseDeeprichastie (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseParenthesis (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryBracketClause (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	bool            TryClauseNSO (long ClauseRuleNo, long ClauseNo1, long ClauseNo2);
	
	// применяет все межклаузные правила
	void            ApplyClauseRulesOnly(vector<PairOfLong>& ClausePairs);
	// устанавливает все межклаузные границы и вызывает ApplyClauseRulesOnly
	void            ApplyClauseRules(long GapSize);

	void            FindMotherLandForPustycha();
    // если самая первая клауза начинается с союза 'и', то нужно удалить его 
	void            ProcessParticipleAndInTheFirstClause();
	// если в клаузе есть  союз 'и', который ни к чему не подсоединился
	void            ProcessIsolatedParticipleAnd();
	// истина, если клауза содердит хотя бы одно прилагательное в сравнительной степени
	bool            ClauseHasCompAdj (long ClauseNo) const;
	// истина, если клауза содержит сравнительный собз "чем"
	bool            	ClauseHasChem (long ClauseNo) const;
	// истина, если клауза содержит хотя бы одно отрицание
	bool            ClauseHasNegation (long ClauseNo) const;
	// истина, если клауза содержит хотя бы одно отрицание при глаголе
	bool            ClauseHasNegationInVerb(long ClauseNo) const;
	// проверяет  клаузу на пустоту
	bool			IsEmptyClause(long ClauseNo);
	// удаляет пустые клаузы
	void			DeleteEmptyClauses();
	// удаляет клаузу
	void			DelClause(long ClauseNo);
	// выдает номер первого слова клаузы
	long	GetClauseFirstWordNo(long ClauseNo) const;
	// выдает номер последенего  слова клаузы
	long	GetClauseLastWordNo(long ClauseNo) const;
	


	//=====================   Словосочетания
	// находим все гипотезы словосочетаний
	void			FindCollocsHyps(long ClauseNo);
	// проверяет лексическую возможность существования сл-ния начиная от узла StartNodeNo
	void			AddCollocHyp (long StartNodeNo,vector<long>& RefCollocItems,long ItemNo, CSemCollocHyp& Hyp,vector<CSemCollocHyp>& Hyps);
	// проверяет грамматические ограничения элемента словосочетания  CollocNo для узла NodeNo
	bool			CheckCollocItemGramFet(long CollocNo, long ItemNo, long NodeNo);
	// создает узлы закрытых словосочетаний 
	void			BuildBlackBoxCollocs(long ClauseNo, size_t SetNo);
	

	
    


	// ===========     Локативные узлы

	long			GetLocInterp(string UnitStr, bool& NegativeForm) const;
	void			BuildParataxNodes(long ClauseNo);
	void			PrintAllLocNodes(); 
	bool			ReadLocProperNames();
	bool			GetLocSemRel(long NodeNo, string& RelationStr, long& PrepNo) const;
	long			FindLocHost (long NodeNo, long ClauseNo);
	void			PutLocRelations(long ClauseNo);
	
	// ===========  Операторы однородности MNA
	// проверяет, что NodeNo - явл. повт. союзом
	long			GetRepeatConj (long NodeNo);
	// конвертирует соч. союзы, которые не являются пока MUA, в MUA и находит ближаший справа актант	
	void			ConvertFreeCoordConjToMNA (long ClauseNo);
	// проверяет, что у каждого МНА-оператора по крайней мере по два  потомка
	long			GetMNAViolationsCount(long Tag);
//  выдает число операторов однородностей (MUA), у которых все члены имеют хотя бы одну	// общую  SF
	long			GetSemFetAgreeMNACount(long Tag);
	// выдает грамматическую модель члена однородного ряда по самому правому потомку
	void			GetMNAPattern(size_t NodeNo, long& Poses, QWORD& Grammems) const;
	// выдает грамматическую модель члена однородного ряда, построенную по самому правому потомку
	long			GetFirstMNAMemberIfHas(long NodeNo) const;
	// набор всех отношений, идущих из оператора однородности NodeNo
	void			GetMNARelations(long NodeNo, vector<long>& Rels, bool UseUse) const;
	// набор всех узлов, к которым идет отношение из оператора однородности NodeNo
	void			GetMNAMembers(long NodeNo, vector<long>& Nodes, bool UseUse) const;
	// интерпретирует оператор однородности, построенныы синтаксиом
	void			InterpretSimilarNounGroups (long ClauseNo);
	// строит оператор однородности для предлога типа Иван с Марией
	bool			DealWithNodePrepS(long NodeNo);
	//"я не знаю человека добрее, чем Вася"
	void			BuildOutcomingOf_CHEM_Obor(long ClauseNo);
	// "я не знаю человека добрее Васи"
	void			BuildOutcomingOf_COMP_Obor(long ClauseNo);
	// "Протокол обрабатывает датаграмму как независимую единицу"
	void			BuildMNA_KAK(long ClauseNo);
	// Удаляет все операторы однородности с одним потомком
	void          DeleteMNAWithOneChild(long ClauseNo);
	// строит выходящие отношения для оператора однородности
	void          BuildMNAOutcoming(long ClauseNo);
	// строит входящие отношения для оператора однородности
	void          BuildMNAIncoming(long ClauseNo);
		
	// ======= Группы времени
	// строит все группы времени для указанной клаузы
	void            BuildTimeNodes(long ClauseNo);
	// проверяет, что текущая гипотеза о группе времени согласована с полем SYNREP
	bool            TimeHypotIsSyntaxAgree (CNodeHypotVector& V, CTimeUnit& U);
	// проверяет, что узлу приписана SF = TIME	
	bool            IsIntervalNode(const CSemNode& N) const;
	// проверяет лексическое соответствие узла элементу поля CONTENT
	bool            CheckOneTimeWord(CTimeUnit& TimeUnit,  CNodeHypotVector& Hypots, BYTE PlaceNo, long& AbbrFunctNo);
	// проверяет грамматическое соответствие узла элементу поля CONTENT
	bool            CheckTimeGramFet(CNodeHypot&  Hypot, CTimeUnit& TimeUnit, BYTE PlaceNo, bool IsHole, long Numbers);
    // проверяет семантическое  соответствие узла элементу поля CONTENT
	bool            CheckTimeSemFet(long NodeNo, long UnitNo, BYTE PlaceNo);
	// находит, узел, который может подчинять подч. союз "когда"
	long            FindTimeHost (long NodeNo, long ClauseNo);
	// проверяет SF по ТАймроссу
	bool            HasTimeSemFet(const CSemNode& W, string SemFet) const;
	// проверяет лекс. совп. DictLemma со словом W (с учетом Abbr)
	bool            IsEqualTimeWord(string DictLemma, CRusSemNode& N, long& AbbrFunctNo);
	// строит словарные интерпретации для слова Lemma
    void			GetTimeInterps(string Lemma, string WordStr,  string PostPuncts, CRusSemNode& N) const;
	// проверяет, что узел NodeNo может заполнять GFi = ЦК_x
	bool			CheckTimeNumeral(long NodeNo, string GramFet) const;

	// ===== Интерпретация синтаксиса и некоторый досемантические триггеры 
	// инициализация параметров одного слова
    void						InitWordFeatures(long WordNo, CRusSemWord& SemWord) ;
	// создание примитивного узла по слову
	CRusSemNode					CreatePrimitiveNode(size_t WordNo);
	// создание примитивного узла по фиксированной группе
	CRusSemNode					CreateNode(const CRelationsIterator* RelIt, long GroupNo);
	// интерпретация предлогов
	void			InterpretPrepNouns(long ClauseNo) ;
	// интерпретация ЭЛЕКТ_ИГ
	void			InterpretSelectiveRelations(long ClauseNo) ;
	// интерпретация двойных союзов
	void			InterpretRepeatConj(long ClauseNo) ;
	// интерпретация ПРИЛ_СУЩ (в одном семантическом дереве)
	void			InterpretAdjNounGroupsAfter ();
	// строит отношения между узлом "еще" и чем-то еще
	void			BuildIZCHORelation(long ClauseNo);
	// обрабатывает пустыху типа "это мой дом"
	void			BuildPustychaWithETO(long ClauseNo);
	// Получение синтаксической вершины клаущы
	long			GetClauseSyntaxTop(long ClauseNo) const;
	// строит операторы отношения 
	void			InitRelationOperators (long ClauseNo);



	// ======= Граф гипотетических связей и построение отношений по синтаксису
	// получение всех словарных интерпретаций 
	void			InitInterps (CRusSemWord& W, bool PassiveForm, long ClauseNo, CRusSemNode& N, bool UsePlugArticle = true) const;
	// получение словарных интерпретаций из тезауруса
    void			GetThesInterps(string UnitStr, const CRusSemWord& W, const CThesaurus* Thes, DictTypeEnum	 DictType, CRusSemNode& N) const;
	// выдает номер статьи в тезауруссном РОССе по  номеру термина
	WORD			GetArticleByModel (long TerminId, int ThesaurusId) const;
	// выдает словарные интерпретации в РОССе без приставок
	WORD			GetInterpWithoutPrefix (CRusSemWord& W) const;
	// Инициализация валентностей в текущей слов. интерпретации
	bool			InitValsRussian(long NodeNo);
	// Инициализация DOMAIN
	void			InitPOs(long ClauseNo);
    // построение SF для тезаурусных организаций
	void			InterpretOrganisations (long ClauseNo);
	// уничтожение  всех аношкинских кодов у узла, в которые не входят граммемы  Grammems,
	// и соответ. модификация слота  m_Grammems у узла.
	void			DeleteAllGramCodesWithoutTheGrammems (CRusSemNode& N, QWORD Grammems);
	// усечение морфологической информации в соотв. с полеми RESTR текущей словарной интерпретации
	bool			ApplyGramRestr (size_t ClauseNo);
	//  инициализация матрицы позиционных условий
	void            InitGrammarMatrix1 (long ClauseNo);
	//  инициализация дополнительных условий в той же матрице 
	void            InitGrammarMatrix2 (long ClauseNo);
	// построение всех гипот. отношений от узла NodeNo
	void			FindActants(size_t NodeNo);
	// чтение из словаря модели валентности ValencyNo
	void			GetActantPattern(size_t NodeNo, BYTE ValencyNo, CSemPattern& P);
	// построение всех гипот. отношений от узла NodeNo по модели P
	void			TryPatternOnNodes(size_t NodeNo, size_t ClauseNo, CSemPattern& P);
	// проверка того, что предлог интепретируется отношением RelationStr
	bool			CheckSemanOfPrep(string RelationStr, bool IsAdditionValency, long NodeNo, long& PrepNo);
	// проверка того, что узел NodeNo соответствует одному значению поля GF
	bool			CheckPatternGramFetLine (CSemPattern& P,  CSynRealization& SynRealization, size_t NodeNo);
	// проверка того, что узел NodeNo соответствует одному значению поля GF, причем ГХi = X!:*
	bool			CheckPatternReverseGramFetLine (CSemPattern& P,  CSynRealization& SynRealization, size_t NodeNo);
	// проверка того, что между двумя узлами есть хотя бы одно существительное
	bool			HaveNounNodeBetweenInOneClause (size_t NodeNo1, size_t NodeNo2) const;
	// проходит по всем значениям GFi и запускает CheckPatternGramFetLine
	bool			IsPattern (CSemPattern& P, size_t NodeNo, string& SyntacticRelation, CSynRealization& SynRealization);
	// загружаем пассивную валентность слова (с учетом только помет ("стр"))
	bool          LoadFromDictForPassive(WORD UnitNo, BYTE LeafId, BYTE BracketLeafId, CSemPattern& P);
	// удаление длинных связей 
	void			DeleteLongRelations (long ClauseNo, long MaxRelationLeapsCount, bool bOnlyNouns);
	
	

	// ========  Оценка деревьев
	// основная функция - строит лучшие деревья на графе гипотетических связей  (изначальный граф может быть не связан)
 	long			GetTreeByConnectedComponents (size_t ClauseNo, TreeVariantValue& ResultValue);
	// помечает все компоненты связности разными тагами (CSemNode::m_TagId)
	long			MarkConnectedComponentsWithoutUniqueRelations(long ClauseNo);
	// считает число компонент  связностей 
	long			GetConnectedComponentsCount(long Tag);
	// проверяет, что клауза ClauseNo связана
   	bool			IsConnectedClause(size_t ClauseNo);
	// обходим граф от текущей вершины (по стрелкам и против стрелок) и помечает достигнутые вершины  
	// (используются блокированные отношения)
	void			dfs_quick_for_connected_component(size_t NodeNo);
	// обходим граф от текущей вершины (по стрелкам и против стрелок) и помечает достигнутые вершины 
	// (используется обычный перечень отношений)
	// если Tag != -1, то обход не идеь по уникальным отношениям
	void			dfs(size_t NodeNo, bool ConsiderUseOfNodes,long Tag = -1);
	// проверка наличия цикла в выделенных тагом узлах (узлы, вошедшие в цикл помечается m_bInCycle)
	bool			FindCycle (long Tag);
	// обход графа в поиске циклов 
   	bool			dfs_for_cycle_search (size_t NodeNo, size_t GoalNodeNo);
     // строит лучше деревья для одной компоненты связностей 
	void			GetTree (long Tag, TreeAndValueVector& VarAndVals);
	// оценивает одно дерево по семантическим критериям
	void			ValueTreeVariant (TreeVariantValue& Value, long ClauseNo);
	// оценивает одно дерево по семантическим критериям, которые релевантны всей клаузе
	// т.е. считает те веса, которые равны для всех компонентов,
	// рассматриваемых функцией GetTree
	void			ValueTreeVariantAdditional (TreeVariantValue& Value, long ClauseNo);
    // проверка всех отношений по SF
	void			FindSemFetDisagree(long Tag);
	// выдает количество отношений, не прошедших  проверка всех отношений по SF
	long			GetSemFetDisagreeCount (long Tag);
	// проверяет, что текущая вершина проходит  по синтаксическим критериям 
	bool			TopAgreeWithSyntaxCriteria (long Tag);
	// помечает отношения, которые не согласованы с частью речи узла, в который оно входит
	void			MarkSemRelPOSViolations();
	// выдает число отношений, которые не согласованы с частью речи узла, в который оно входит 
	long			GetSemRelPOSViolationsCount(long Tag);
	// выдает сумм WordWeight всех слов
	long			GetWordWeightSumma (long Tag) const;
	// выдает число отношений, которые не были получены по словарным статьям
	long			GetOptionalValenciesCount(long Tag);
	// выдает число узлов, у которых не заполнены первая валентность
	long			GetObligatoryValencyViolation (long Tag);
	// выдает число анафорических отношений  (чем больше, тем лучше)
	long			GetAnaphoricRelationsCount(long Tag);
	//считает, число валенотностей, которые стоят не на нужном месте, указанном в CSemNode::m_ValsLayout
	long            GetValencyDisorderCount(long Tag);
	// выдает число реверсивных (пассивных) CSemRelation::m_ReverseRel отношений
	long			GetPassiveValencyCount(long Tag) const;
	// выдает число причастий у которых нет актантов 
	long			GetPrichastieWithoutActants(long Tag) const;
	// проверяет некоторое семантическое согласование
	long			GetMiscSemAgreeCount(long Tag) const;
	// удаляет запрещенные пользователем лексический интерпретации
	void			DelProhibitedInterps (string UnitStr, CRusSemNode& N) const;

	
	

	// =====    обработка узла с леммой "САМ"
	void			DeleteSAMNode(long ClauseNo, CRusSemNode& SamNode);
	bool			CheckSAMNode(long ClauseNo) const;
	bool			InsertSAMNode(long ClauseNo, CRusSemNode& SamNode);


	// ======  обработка одного дерева клаузы
	// Идеализация 
	long			Idealize ();
	// усечение морф. интерпретации в соотв. с GFi актантов
	void			ApplySubordinationCases ();
	// усечение SF-главных  в соотв. с СХi 
	void			ApplySubordinationSemfets ();
	// построение субъекта инфинитива
	void			ApplyDopInfinitive ();
	// построение дополнительных отношений для LF
	void			ApplyDopLexFuncts ();
	
	// =======  вспомогательные функции
	// удаляет неиспользованные (m_bUsed) отношения  в клаузе  ClauseNo
	void			DelUnusedRelations (long ClauseNo);
	// ставит новый таг клаузе ClauseNo
	long			SetTagToClause(long ClauseNo);
	// ставит новый таг клаузе всему предложению
	long			SetTagToSentence();
	// ишет узел, который образовался из группы  под номером SyntaxGroupNo
	long			FindNodeBySyntaxGroupNo (long SyntaxGroupNo) const;

   	// выдает два ближайших слова, принадлежащих узлу NodeNo1 и NodeNo2
	void			FindClosestWords(size_t NodeNo1, size_t NodeNo2, size_t& StartWordNo, size_t& EndWordNo) const;
	// выдает расстояние между узлами в словах 
	long			GetDistance(size_t NodeNo1, size_t NodeNo2) const;
	// выдает самый ближайший узел справа (который отстоит не более, чем на десять слов)
	long			FindRightClosestNode(size_t NodeNo) const;
    // выдает узел, которому принадлежит слово WordNo
	long			FindNodeByWordNo(long WordNo) const;
    // выдает узел, которому принадлежит слово WordNo
	long			FindNodeByWordNo(long WordNo, long ClauseNo) const;
	// выдает узлы, упорядоченные по тому, как они идут в руском тексте
	void			GetAllTextOrderedNodes(vector <long>& Nodes) const;

		// выдает длину (в словах) тех отношений, которые выходят из узлов, помеченных Tag.
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
		// проверка валидности всех индексов
		void			AssertValidGraph();
	  	// выдает первый по тексту узел из клаузы
		long			FindFirstNode(long ClauseNo) const;
		// проверяет, что между узлами стоит хотя бы одна запятая
		bool			HaveCommaBetweenInOneClause	(size_t NodeNo1, size_t NodeNo2) const;
		// проверяет, что между узлами стоит один только знак препинания "ch", и ничего больше
		bool			HaveOnlyPunctBetween (size_t NodeNo1, size_t NodeNo2, int punct_char) const;
		// иницилизирует CSemRelation::m_LeapsCount для всех отношений клаузы
		void			BuildRelationLeapsCount(long ClauseNo);
		// считает CSemRelation::m_LeapsCount для одного отношения
		long			GetRelationLeapsCount (long RelNo);
		void			SaveClausesNodeReferences();
		void			RestoreClausesNodeReferences();
		void			PopClausesNodeReferences();
	
			// Устанавливает всем отношениям CSemRelation::m_RelUse = false
		void			SetUseFalse ();
		// Устанавливает всем отношениям CSemRelation::m_RelUse = true
		void			SetUseTrue ();
		// выдает число отношений, для которых CSemRelation::m_RelUse == true
		long			GetUseRelationsCount() const;
		// выдает число отношений, для которых CSemRelation::m_RelUse == true
		long			GetSemanticVolume(long Tag) const;
		// выдает число межклаузных отношений
		long			GetClauseRelationCount ();
		// проверяет, что узлы NodeNo1 и NodeNo2 находятся в указанном расположении (<,<<,>>,>,^)
		bool			CheckDirection (long NodeNo1, long NodeNo2, string Direction) const;
	
		
		bool			IsProjectedNew(long Tag) const;	
		// конвертирует все руские числительные в арабскую запись
		void			ConvertRusNumeralsToArabic();
		// обработка клауз типа "давай пойдем в лес"
		void			DealInvitatoryMoodBeforeTree (long ClauseNo);
		void			DealInvitatoryMoodAfterTree ();
		// удаляет союз в начале клаузы, если он не вошел  в состав повторяющегося союза
		void			DeleteConjInTheBegining(long ClauseNo, CRusSemNode& FirstConjNode);
		// помечает все отношения, которые соединяет две компоненты связностей, при этом 
		// никакое другое отношение не соединяет эти две компоненты
		void			MarkUniqueRelations();
		// проверяет, что между узлами входящими в это отношение нет другой связи, кроме RelNo
		bool			dfs_to_test_unique_relation (size_t NodeNo, size_t RelNo);
		string			GetClauseTreeForTcl();
		// получает SF1 узла NodeNo		
		vector<QWORD> GetSemFetsOfFirstValency(long NodeNo);
		// проверяет, что чразу перед узлм стоит запятая
	    bool			HasCommaBefore (size_t NodeNo) const;
        // строит аналитическую превосходную степень
		bool			BuildAnalyticalSupelative();
		// строит реверсивные (X!) отношения от предложных групп, если 
		// в предложную группу не входит ни одного отношения
		void			FindReverseActantForPreps(size_t ClauseNo);
	   // выдает морфологическое представление узла (лемма и граммема)
	    string	GetMorphologyOfNode(long NodeNo) const ;
		// удаление Copul, которые совсем не имеют детей
		void			DelChildlessCopul();
		// копирует все  дополнительные отношения, кроме анафор, в нормальные отношения
		void			CopyDopRelationsExceptAnaphor();
		// приписывает тез. LEX (список дается в ConceptStrs) узлам, если те являются 
		// разновидностью этой LEX
		void			AscribeFindConceptFets(long ClauseNo, const StringVector& ConceptStrs);
		//  для  каждой  тез. LEX из  статей проверяет 
		//  входит ли данный узел в концепт, обозначенный LEX.
		void			FindConceptFetsFromArticles(long ClauseNo);
		//  проверяет, что глагольная лемма входит в словарь без частицы суффикса СЯ 
		bool			VerbCanBeWithoutReflexiveSuffix(string Lemma) const;
		// уничтожает все "пробелы"  в порядке нумерации слов,
		void			DeleteWordNoGaps();
		// узел является  ПГ или наречием
		bool			IsAdverbial (long NodeNo) const;
		// вычисляет предметную область предложения
		void			CalculateBestCorporaLabel(size_t ClauseNo);
		// находит хозяина для наречий тима "вшестером"
		void			NumeralAdverbRule();
		bool            IsTimeAdverbial(long NodeNo) const
		{
			return    m_Nodes[NodeNo].IsMainTimeRossNode()
				 ||	 (   HasRichPOS (NodeNo, ADV) 
				&& HasSemFet(m_Nodes[NodeNo], "TIME")					 );
		};
		// читает вспомогательные статьи _коеф и _weak_syn_rel	
		bool			ReadAuxiliaryArticles();
		bool		    IsWeakSynRel (const string& S) const
		{
			return binary_search (m_WeakSynRels.begin(),m_WeakSynRels.end(),S);
		};
		// проверяет, можно ли доверять синтаксическому отношению
		bool			IsPragmaticallyStrongSynRel (long RelNo) const;
		// находит синтаксические  отношения, которым можно доверять
		void			FindPragmaticallyStrongSynRel ();
		// находит все вопросительные клаузы  в вопросительном предложении
		void		    FindQuestionClauses ();
		// возвращает  свойства клаузы 
		string			GetClauseProperiesStr(long ClauseNo) const;
		// находит      термины, элементы которых стоят неконтактно
		void			FindDividedTermins();
		// вызывает функцию GetDopFields, строит по результатпм m_ThesSemRelations
		long			AddThesSemRelations(const CRossHolder* Dict, long UnitNo, long StartNodeNo);
		// выдает номер словарной статьи по ID термина
		WORD			GetUnitNoByTerminId(DictTypeEnum   DictType,	long TerminId) const;
		// переводит тире в клаузе типа "тире" в глагол "есть"
		void			ProcessDashClauses();
		bool	IsQuestionClause(long ClauseNo) const 
		{
			return m_Clauses[ClauseNo].m_bQuestion;
		};

		// выдает номер валентности без учета несовместных 
		long GetValencyNoWithouProhibited (long NodeNo, long ValencyNo) const;

		// находит связанные с данным словом ситуации по LF
		void			FindConnectedSituations(long NodeNo);
		// для всех существительных находит глаголы, для которых эти существительные являются S0
		void			FindVerbsOfS0(long NodeNo);
		// проверяет, что клаузы имеют хотя бы одну общую ситуацию (клаузу предсталяет вершина клаузы)
		bool			ClausesHaveCommonSituation (long ClauseNo1, long ClauseNo2) const;
		string	GetClausePO(long ClauseNo) const  {return m_Clauses[ClauseNo].m_BestPO;};
		// привязывает "нечего" к ModalCopul, если в "нечего" не входит ни одной связи
		void			DealUnboundModalCopul(long ClauseNo);
		// создает подлежащее для клаузы быз подлежащего по пред. клаузе
		long			CreateDefaultSubjectFromPreviousClause();
		// ищет аббревиатуры терминов
		void			FindAbbrTermins();
		// Создаем интерпретацию для английских многословных названий типа Adobe Photoshop
		void			FindEnglishTermins();
		// ищет IDENT типа  "дом N 8"		
		void			ProcessEveryIdents(long ClauseNo);
	// проверяет, что группа может обозначать человека (заполнитель валентности NAME)		
		bool			IsHumanName (const CRusSemNode& N)  const;
		// проверяет, что слово может быть субъектом императива
		bool			CanBeSubjectForImperative (long NodeNo, long SourceNodeNo)  const;

		/*
		разбирает случай, когда в клаузе есть одновременно подчинительный и сочинительный 
		например, "Иван пришел, и хоть он очень устал, он пошел на кухню."
		*/
		void			CorrectClauseRelations1();
		/*
		 находит по разным контекстам и 
		 конвертирует прилагательные сравнительной  степени в наречия  
		*/
		bool			FindComparWithNoActantAndCheck(long ClauseNo);
		// собственно конвретация прилагательных сравнительной  степени в наречия  
		void			ConvertComparAdjToComparAdv(long NodeNo);
		// стирает интерпретацию таймгруппы, если таймгруппа заполнила 
		// не временную валентность
		void			ClearInterpsForFreeTimeGroups();
		// см. описание рядом с телом процедуры 
		void			MovePrepNodeToRelationForMainTimeGroups();
		// если в клаузе есть узел САМ и узел  СЕБЯ, тогда удаляем  узел САМ
		void			ProcessSAM_SEBJA();
		void			ConvertVSE2_toOperator();

		void			ProcessAllowableLexVars (string LexVarsStr);
		// ищет такой же клаузный вариант,что уже был
		bool			FindEqualClauseVariant();
		void			ZaplataCompar1(long ClauseNo);
		void			ZaplataCompar2(long ClauseNo);
		/*копирует новые узлы в "память"*/
		void			CopyToMemory(long ClauseNo);
		void			FindOldNodes(long ClauseNo);
		void            RestoreMemoryRelations(long ClauseNo); 
		/*
		 возвращает истину, если оба узла когда-то попались 
		 в обработку процедурой FindActants вместе
		*/
		bool			AreOldNodesOfTheSameCase (long NodeNo1, long NodeNo2) const; 
		

		/*
		 возвращает истину, если узлы согласованы по роду, числу, падежу и одушевленности
		*/
		bool			FullGleiche(long NounNodeNo, long AdjNodeNo) const;
		
		void			CreateVerbAnalyticalForm(long AuxVerbNodeNo );

		void			SolveImperativeHomonymy();
		// функция, которая выдает входящие отношения без использования new
		CRelSet			GetIncomingRelations (long NodeNo, bool UseUse) const;
		// удаляет отношения Rels
		void			DeleteRelSet(CRelSet& R);
		// проверяет, что в одном из значений GF-главного после двоеточия стоит константа ItemStr (син. роль)
		bool			HasGramFetAfterColon (long NodeNo, string ItemStr)  const;
		//выдает узлы, которые заполнили валентности узла  NodeNo в порядке, заданном в валентной структуре
		CRelSet         GetValencyPlaces(long NodeNo, CRelSet& OutRelations)  const;
		//является ли этот узел локатинвым
		bool			IsLocNode(size_t NodeNo) const;
		// выдает все узлы клаузы, в которые не входит ни одного отношения, за исключением тех узлов, что помечены CSemNode::m_bToDelete
		void			GetClauseRootsWithoutDeleted (size_t ClauseNo, vector<long>& Nodes) const;
		void			DelNode(long NodeNo);
		void			PrintMemRelations() const;
		// выдает нормальную форму причастия (мр, ед)
		string			GetNormOfParticiple (const CRusSemWord& W) const;

		// превращает причастие и все его зависимые в отдельную клаузу, если они еще являются отдельной клаузой 
		void			ConvertParticipleTreeToClause();

		// выдает все узлы,  зависимые от данного узла
		void			dfs_in_clause(size_t NodeNo, vector<long>& Nodes) const;
		// строит отношения между лок(темпор) узлами, которые идут друг за другом
		void			BuildCircumsChains(long ClauseNo);
		// строит       учительница -> учитель
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

// максимально расстояние от начала клаузы, на котором может находиться  союзное слово,
// например, "Я знаю дом, в подвале которого нет  мышей"
const int ConjWordDistance = 5;



#endif
