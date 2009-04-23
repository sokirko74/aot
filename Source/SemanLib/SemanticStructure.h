#if !defined(semantic_str_h)
#define semantic_str_h

#pragma warning (disable : 4786)

#include "../common/utilit.h"
#include "RossHolder.h"
#include "../StructDictLib/Ross.h"
#include "SemanticsHolder.h"
#include "SemPattern.h"

 
typedef vector<long> VectorLong;


// класс запросов к словарю РОСС (запоминает запросы к словарю, если попадается
// запрос, который уже был, выдает запомненное старое значение)
// этот класс используется  только в функции CSemanticStructure::HasItem

struct CRossQuery {
	// в какой  словарь
	DictTypeEnum	m_TypeEnum;
	// из какой статьи
    WORD			m_UnitNo;
	// про какое поле
	string			m_FieldStr;
	// для какого номер актанта
	BYTE			m_LeafId;
	// для какого номер подактанта
	BYTE			m_BracketLeafId;
	// какую константу
	string			m_ItemStr;
	// из домена
	string			m_DomStr;
	// результат, содержится ли константа m_ItemStr в поле m_FieldStr и т.д.
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

	// граммемы, которые приписаны слову
	QWORD          m_FormGrammems;
	// общие граммемы, которые приписаны слову
	QWORD          m_TypeGrammems;

public:	 
	// прописная или строчная буква
	RegisterEnum   m_CharCase;
	// входное слово прописными буквами
	string        m_Word;
	// лемма
	string        m_Lemma;
	// номер парадигмы в морф. словаре 
	long           m_ParadigmId;
	// добавочный номер парадигмы в морф. словаре (для приложений типа "муж-алкоголик")
	long           m_AdditParadigmId;
	// части речи, которые приписаны слову 
	size_t         m_Poses;
	// номер слова в синтаксическом представлении
	long           m_WordNo;
	// Полные эквиваленты слова, здесь обычно лежит 
	// аспектный вариант глагола (совершенный, несовершенный виды)
	StringVector m_WordEquals; 
	// используется в словах типа "трехоконный", которые переводятся в "3-fenestral"	
	string		m_NumeralPrefix;

	long			m_WordWeight;
	// является ли это слово наречием "по-английски", "по-хорошему"...
	bool			m_bAdverbFromAdjective;	

	// указатель на все словари системы 
	const CSemanticsHolder* m_pData;
	// две обертки для функций из CSemanticsHolder
	const CRossHolder* GetRossHolder (DictTypeEnum Type)   const;  
	const CDictionary* GetRoss (DictTypeEnum Type) const;

	

	CSemWord();
	CSemWord (long WordNo, string Lemma );

	void Init();
	// является ли данное слово кавычкой
	bool IsQuoteMark () const;
	// принадлежит ли граммема слову 
	bool HasOneGrammem(int grammem) const;
	bool operator == ( const long& X ) const;
	// проверка, что часть речи POS принадлежит  данному слову 
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
	// предлоги  
	vector<CRossInterp>		m_Preps;
	// союзы (для отношений)
    CRossInterp				m_Conj;

	// граммемы отношения или узла
	QWORD                   m_Grammems;

	// предлог или союз, который не был найден в оборотах, но он был прописан в статье.
	//  Такой предлог или союз пишется прямо в строку.
	string					m_Other;

	// номер используемого кортежа в векторе CSemPattern::m_GramCorteges 
	long                    m_CortegeNo;

	// сохраненный кортеж  CSemPattern::m_GramCorteges[m_CortegeNo]
	TCortege                m_Cortege;

	// добавлено для статьи "не ранее", в которой используется специальная константа А1(НЕ)
	string                  m_AlgStr; 

	void SetEmpty();
    // проверяет, приписан ли узлу предлог PrepNo
	bool HasThePrep (WORD UnitNo) const;

};

enum NodeTypeEnum  { SimpleNode, MNA, Copul,  ModalCopul, SJA, Situat};
/*
  MNA - оператор однородности
  Copul - копульный узел ("я считал его великим ученым")
  ModalCopul - модально копульный узел ("авам не о чем волноваться")
  SJA - 	узел, который выделяется из пассивного глагола на СЯ и  заполняет его первую валентность
*/


enum MNATypeEnum  { NotMNA, SimpleMNA, RepeatMNA, CHEMOborot, KAK_MNA};
/*
  NotMNA - не является оператором однородности 
  Simple - простой оператор однородности (однородный ряд с одним союзом)
  RepeatMNA - повторяющийся или двойной союз
  CHEMOborot - сравнительный оборот (союзом "чем" в сравнительном обороте)
*/

/*
 информация об узле, которая приходит из словарных статей
*/
enum SemCategoryEnum  {scSituation=0, scObject=1, scLabel=2, scRelation=3};
extern const char* GetStrByCategory (SemCategoryEnum t);

struct  CInterpNodeInfo  {
	// сем. категория ( LABL, LABL.OBJ, LABL.SIT, REL)	
	SemCategoryEnum	m_SemCategory;
	//SF узла
	vector<QWORD>	m_NodeSemFets;
	// предметные области узла
	StringVector  m_POs;
	bool			m_ThesAbbrForm;

	// является ли узел названием организации	
	bool			m_bOrgName;
	// является ли узел именем собственным
	bool			m_bProper;


    // лексические функции для текущей словарной интерпретации узла 
	vector<CLexicalFunctionField>	m_LexFunctFields;

	StringVector	m_HigherConcepts;

	// ситуации, которые были упомянуты в лексических функциях, значениями которых
	// является данный узел
	// для слова "сдавать"  здесь будет "экзамен"
	// для слова "брать"  здесь будет "барьер"
	// поскольку Oper1(экзамен) = сдавать, Real(барьер)= брать
	vector<CDictReference> m_ConnectedSits;

	// валентная структура узла в текущей словарной интерпретации
	vector<CValency>  m_Vals;
};


class CSemNode : public CInterpNodeInfo
{
	// внешние граммемы группы 
	QWORD			m_Grammems;

protected:
	// все словарные интерпретации узла  
    CDictUnitInterpVector   m_Interps;
    // текущая интерпретация
    long                    m_CurrInterp;

public:
    // виртуальные функции, которые переопределяются в классах CRusSemNode и CEngSemNode
	virtual const CSemWord&	GetWord(int WordNo) const = 0;
	virtual		  int		GetWordsSize() const = 0;

	// главное слово узла (для примитивных узлов m_MainWordNo=0)
	long			m_MainWordNo;

	// если узел был образован из группы, то здесь хранится тип группы 
	string			m_SynGroupTypeStr;

    // аношкинские коды группы
    string			m_GramCodes;
    // type ancode 
    string			m_TypeAncode;
	// номер клаузы, к которой приписан данный  узел
	long			m_ClauseNo;
    // номер клаузы, к которой приписан данный  узел
	CSynRealization	m_SynReal;


	//==========           технические параметры  =====================
    // Помечает узел, если до него дошли при обходе в глубину
	bool					m_bReached;
	// Помечает узел, если этот узел нужно удалить, но пока его удалять нельзя, поскольку где-то 
	// могут сбиться индексы
	bool					m_bToDelete;	
	//  номер подмножества, в которое входит данный узел, из некоторого множества 
	long					m_Tag;

    // истина, если справа и слева по тексту от  текущего узла находятся кавычки
	bool					m_bQuoteMarks;

	// =============     словарная  интерпретация  ==================
	//интерпретация открытого словосочетания
	COpenCollocInterp		m_Colloc;
	// уникальный номер словосочетания, в которое входит данный узел
	long                    m_CollocId;

	// интерпретация в тезаурусе( которая берется из синтаксиса или из аббревиатур)
	long			m_TerminId;
	long			m_ThesaurusId;





	// тип узла  
	NodeTypeEnum	m_NodeType;

	// если m_NodeType == MNA, то у него могут быть  разновидности, тип которых содержится в следующем слоте 
	MNATypeEnum		m_MNAType;


	// операторы отношения (не, даже, бы)
	StringVector   m_RelOperators;

    // Номер абстрактного узла
	long			m_AbstractNodeId;



	CSemNode();

	// примитивный узел - это такой узел, который состоит только из одного слова
	bool	IsPrimitive() const;
	// является ли данный узел абстрактным (не содержащий слов)
    bool	IsAbstract () const; 	
    // IsWordContainer = "содержит ли данный узел слова"
	// версия функции IsWordContainer с GetWordsSize() 
	// очень наглядна, но очень медленна,
	// поэтому лучше использовать тип узла
	bool	IsWordContainer () const;
	// выдает номер самого левого слова данного узла
	long	GetMaxWordNo () const;
	// выдает номер самого правого слова данного узла
	long	GetMinWordNo () const;
	// проверяет наличие оператора среди  RelOperators
	bool	HasRelOperator (string oper) const;
	// удаляет из оператор из  m_RelOperators
	void	DelRelOperator(const string& oper);
    // проверяет, приписан ли узлу хотя бы один предлог
	bool	HasSomePrep () const;
    // проверяет, приписан ли узлу предлог PrepNo
	bool	HasThePrep (WORD UnitNo) const;

	bool	IsTimeRossNode() const;
	bool	IsMainTimeRossNode() const;
	bool	IsThesNode() const;
	bool	IsTrueLocNode() const;
	bool	HasPOS (size_t POS) const;
	bool	IsComma() const;
	bool	IsLemma(string Lemma) const;
	// проверяет, что данный узел является пассивным глаголом
	bool	IsPassiveVerb () const;

    // дает тип словаря для текущей интерпретации
	DictTypeEnum		GetType()   const;
	// дает  текущую интерпрецию
	CDictUnitInterp*	GetInterp();
	// дает  константную текущую интерпрецию
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

	// дает  номер статьи для текущей интерпреции
    WORD	GetUnitNo() const;
	QWORD	GetGrammems() const;
	void	SetGrammems(QWORD g);
	void	AddOneGrammem(int g);
	bool	HasOneGrammem(int g) const;
	bool	HasGrammems(QWORD g) const;
	void	AddGrammems(QWORD  grammems);
	void	DeleteGrammems(QWORD grammems);

	// выдает части речи, которые приписаны главному слову узла
	long	GetNodePoses() const;

};

enum PositionTypeEnum {
	UnknownPosType,
	FromArticlePosType,
	FromAlgorithmPosType
};

struct CSemRelation : public CSimpleBinaryRelation{
	// валентность, которая приписана этому слову
	CValency		m_Valency;
	// синтаксическое отношение, которое лежит в основе этого семантического отношения
	// если данное СемО пришло из жесктих синтаксических отношений, то здесь лежит СинО, 
	// взятое непосредственно из синтаксиса, иначе здесь лежит отношение, которое записано в поле GFi
	string			m_SyntacticRelation;
    //технический слот: помечает те отношения, которые нужно будет удалить
	bool			m_bToDelete;
	//здесь записывается информация о том, как реализуется лексически и грамматически это отношение
	CSynRealization	m_SynReal;
	//технический слот: используется для выделения одного подмножества отношений среди некоторого множества
	long			m_bRelUse;
	// помета того, что GFi был помечен реверсинвым отношением ("X!")
	bool				m_bReverseRel;

	// перечень LEX, которым должно удовлетворять данное отношение
	StringVector		m_LexFets;

	//SF отношения
	vector<QWORD>	m_SemFets;

	bool			m_bDopRelation;

	// позиция, которая указывается для синтеза 
	string				m_Position;
	PositionTypeEnum    m_PosType; 

// отношение, которое на этапе идеализации должно подвеситься к узлу SIT , а пока подвешивается	// к вершине клаузы
	bool				m_bSituatRelation;

	void      Init();
    CSemRelation ();
    CSemRelation (const CValency& Valency, 
	             long SourceNodeNo, 
				 long TargetNodeNo, 
				 string SyntacticRelation);
	
    // истина, если отношению приписаны предлоги
	bool  HasSomePrep() const;
};



// Наклонение предложения (изъявительное, восклицательное, вопорсительное)
enum SentenceMoodEnum {Indicative, Exclamative, Interrogative};



// ==== класс 
struct CSemClause 
{
	// есть ли в клаузе частица "БЫ"
	bool						m_HasParticleBY;
	// предметная область клаузы
	string						m_BestPO;
		// является ли клауза вопросительной 
	bool						m_bQuestion;
	// содержит ли клаузы союз, используемый как частица
	// например, "c такими воспоминаниями он и засыпал"
	bool						m_bHasParticleConjAnd;

	//означает, что вся клаузы закавычена (прямая речи)
	bool						m_bQuoted;
	//означает, что вся заключена в скобки
	bool						m_bBracketed;
	CSemClause();
	void  Copy(const CSemClause& C);
};



class CSemanticStructure   
{ 
public:
	// указатель на все словари системы 
	CSemanticsHolder* m_pData;
    // две обертки для функций из CSemanticsHolder
	CRossHolder* GetRossHolder (DictTypeEnum Type)   const;
	const CDictionary* GetRoss (DictTypeEnum Type) const;

	virtual const vector<CSynRelation>&		GetSynRels() const = 0;
	virtual		  vector<CSynRelation>&		GetSynRels() = 0;
	virtual const vector<CLexFunctRel>&		GetLexFuncts() const = 0;
	virtual		  vector<CLexFunctRel>&		GetLexFuncts() = 0;
	virtual const CSemClause&				GetSemClause(long ClauseNo) const = 0;
	virtual const long						GetSemClausesCount() const = 0;

	// предметная область, которая задана извне
	string						m_PO;


	// наклонение предложения
    SentenceMoodEnum		m_SentenceMood;


	// всякие сообщения, о статистические семантического процесса
	// элементарная информация о клаузах 
	string                 m_ClausePropertiesProtocol;
	// значения всех коэффициентов(весов) для всех клауз 
	string                 m_WeightProtocol;
	// сколько времени занял та или иная подпрограмма
	string                 m_TimeStatictics;
	//какие вараинты клауз были выбраны 
	string                 m_ClauseVariantsStatistics;
	// начинается ли все предложение со слова "но"
	bool					m_bHasConjBut;


    // все запросы, которые делаллись через функцию HasItem	
	vector<CRossQuery> Queries;


	vector<string>   m_IndexedSemFets;


	//===========    работа с дополнительными семантическими отношениями 
	virtual const CSemRelation*	GetDopRelation(int RelNo) const = 0;
	virtual		  CSemRelation*	GetDopRelation(int RelNo)		= 0;
	virtual int					GetDopRelationsSize() const	 = 0;
	virtual void				EraseDopRelation(int RelNo) 	= 0;
	void						GetIncomingDopRelations (long NodeNo, vector<long>& Relations) const;
	// выдает выходящие в узел NodeNo AUX-отношения.
	void						GetOutcomingDopRelations (long NodeNo, vector<long>& Relations) const;
	// выдает узел, на который ссылается анафорическое местоимение
	long						GetAnaphoraTarget(long NodeNo) const;
	// выдает узел, в который идет дополнительное отношение THESAME	
	long						GetEquNode(long NodeNo) const;
	

	

	//===========    работа с семантическими отношениями 
	// виртуальные функции для достпу к переченю всех СемО
    virtual const CSemRelation*	GetRelation(int RelNo) const  = 0;
	virtual		  CSemRelation*	GetRelation(int RelNo)		  = 0;
	virtual int					GetRelationsSize()		const = 0;
	void				ReverseRelation(int iRel)   { swap(GetRelation(iRel)->m_TargetNodeNo, GetRelation(iRel)->m_SourceNodeNo); }
	virtual void				EraseRelation(int RelNo)		=0; 
	virtual void				GetColorAndWidthOfRelation(int RelNo, float& Width, string& Color) = 0;

	
	// выдает все узлы из клаузы ClauseNo, в которые не входит ни одного СемО из другого узла этой же клаузы
	//(межклаузные отношения могут входить!)
	virtual	void				GetClauseRoots (size_t ClauseNo, vector<long>& Roots) const;

	// выдает входящие из узла NodeNo отношения. Если   UseUse = true, то выдает только те отношения,
	// для которых CSemRelation::m_RelUse==true
	void			GetIncomingRelations  (long NodeNo, vector<long>& Relations, bool UseUse = false) const;
	// выдает входящие из узла NodeNo внутриклаущные отношения. 
	void			GetIncomingInClauseRelations (long NodeNo, vector<long>& Relations) const;
	// выдает входящие из узла NodeNo межклаузные отношения. 
	void			GetIncomingClauseRelations (long ClauseNo, vector<long>& Relations) const;
	// выдает инцидентные узлу NodeNo отношения. 
	void			GetRelations  (long NodeNo, vector<long>& Relations, bool UseUse= false) const;
	// выдает число входящих в узел NodeNo отношений.
	long			GetIncomingRelationsCount (long NodeNo, bool UseUse = false) const;
	// выдает выходящие в узел NodeNo отношения.
	void			GetOutcomingRelations (long NodeNo, vector<long>& Relations, bool UseUse= false) const;
	// выдает число выходящих в узел NodeNo отношений.
	long			GetOutcomingRelationsCount (long NodeNo,  bool UseUse = false) const;
	// выдает узлы, в которые входят выходящие в узел NodeNo отношения.
	void			GetOutcomingNodes (long NodeNo, vector<long>& Nodes, bool UseUse = false) const;
	// выдает узлы, в которые входят отношения из узлов, из которых идут отношения в узел NodeNo
	void			GetBrothers (long NodeNo, vector<long>& Nodes, bool UseUse = false) const;
	// избавляется от дублей отношений, где дубль - это отношение, которое идет из того же и туда же, что
	// некоторое другое отношение
	void			DeleteDubleRelations();
	// печатает отношения в Debug
	void			PrintRelations() const;
	// проверяет, что сущетсвует хотя бы одно отношение, которое идет между ClauseNo1 и ClauseNo2
	bool			AreConnectedClauses (long ClauseNo1, long ClauseNo2) const;
	// выдает все отношения между NodeNo1 и NodeNo2
	void			FindRelations (long NodeNo1, long NodeNo2, vector<long>& Rels) const;
	// выдает первое отношение (случайное) между NodeNo1 и NodeNo2
	long			FindFirstRelation (long NodeNo1, long NodeNo2) const;
	// выдает первое дополнительное отношение (случайное) между NodeNo1 и NodeNo2
	long			FindDopFirstRelation (long NodeNo1, long NodeNo2) const;
	// удаляет отношения Rels
	void			DeleteRelations(vector<long>& Rels);
	// переносит все отношения, инцидентные узлу FromNode,  к узлу ToNode
	void			MoveRelations(long FromNode, long ToNode);		
	// переносит все входящие отношения в узел FromNode  к узлу ToNode
	void			MoveIncomingRelations(long FromNode, long ToNode);
	// переносит все входящие дополнительные отношения в узел FromNode  к узлу ToNode
	void			MoveIncomingDopRelations(long FromNode, long ToNode);
	// переносит все отношения во всех графах, инцидентные узлу FromNode,  к узлу ToNode
	void			MoveAllRelations(long FromNode, long ToNode);
	// выдает все узлы, в которые не входит ни одного СемО
	void			GetRoots(vector<long>& Roots) const;
	//проверяет, что данное отношение - отношение между  подлежащим и сказуемым   
	bool			IsRusSubj(int iRel) const;
	// выдает первое синтаксическое отношение, начало и конец которого совпадает с началом и концом iSemRelNo
	long			GetSynRelBySemRel(long iSemRelNo) const;

	

	
	
	//======================    работа с семантическими  узлами (СемУ)
    // виртуальные функции для доступу к переченю всех СемУ
    virtual const CSemNode&		GetNode(int NodeNo)							const = 0;
	virtual		  CSemNode*		GetNodePtr(int NodeNo)							  = 0;
	virtual int					GetNodesSize()								const = 0;
	virtual string				GetInterfaceWordStr(const CSemNode* pNode, int WordNo) const = 0; 
    virtual void				EraseNode(int NodeNo) = 0;

	// проверяет, что узлу NodeNo  приписан Tag 
	bool			HasTag (long NodeNo, long Tag) const {return GetNode(NodeNo).m_Tag == Tag;};
	// выдает строковое представление СемУ
	string			GetNodeStr(size_t NodeNo, size_t MaxLength = 65000) const;
	string			GetNodeStr(const CSemNode& N, size_t MaxLength = 65000) const;
	// выдает строковое представление СемУ, уникально нумеруя абстрактные узлы
	string			GetNodeStr1(size_t NodeNo, size_t MaxLength = 65000) const;
	// выдает части речи, которые приписаны узлу
	string			GetNodePosesStr(long NodeNo) const;
	// выдает морфологическое представление узла (лемма и граммема)
	virtual string	GetMorphologyOfNode(long NodeNo) const = 0;
	// выдает лемматическое представление узла
	string			GetNodeLemStr(size_t NodeNo) const;
	// печатает узлы в Debug
	void			PrintNodes() const;
	// печатает лемматическое  представление узлов  в Debug
	void			PrintLemNodes() const;
	// удаляет узел 
	virtual void	DelNode(long NodeNo);
    // провереят, что узел является существительнм или  оператором однородности, который управляет существительными
	bool			IsNounOrMNANoun(int iNode)  const;
	// провереят, что узел является глаголом или  оператором однородности, который управляет глаголами
	bool			IsVerbOrMNAVerb(int iNode) const;
	// провереят, что узел является инфинитив или  оператором однородности, который управляет инфинитивами
	bool			IsInfinitiveOrMNAInfinitive(int iNode) const;

	// проверяет часть речи узла
	bool	HasPOS (const CSemNode& N, size_t POS) const;
    bool	HasPOS (size_t NodeNo, size_t POS) const;
	bool	IsVerbForm(const CSemNode& Node) const;
	// ставит всем узлам m_bToDelete = false
	void			SetNodeToDeleteFalse ();
	// удаляет все узлы, помеченные m_bToDelete==true
	void			DelNodesToDelete();

	// то же с отношениями
	void			SetRelsToDeleteFalse ();
	void			DelRelsToDelete();

	// выдает самый ближайший узел слева (который отстоит не более, чем на десять слов)
	long			FindLeftClosestNode(size_t NodeNo) const;


    //======================    работа со словарными интерпретациями
    // выдает набор частей речи по GF-главному статьи UnitNo
	long			GetPosesFromRusArticle(CRossHolder& Ross,WORD UnitNo) const;
	// проверяет согласование словарной статьи UnitNo со словом W по частям речи 
	bool			GramFetAgreeWithPoses (CRossHolder& Ross,WORD UnitNo, const CSemWord& W ) const;
	// проверяет, что в одном из значений поля CAT стоит константа Type		(семантическая категория)
	bool			HasSemType (const CSemNode& Node, string Type) const;
	// проверяет, что в одном из значений поля SF-главное стоит константа SemFet (семантическая х-ка)
	bool			HasSemFet (const CSemNode& Node, const string& SemFet) const;
	// проверяет, принадлежит ли узлу данная SF или какая-нибудь SF ниже по иерархии	
	bool			HasSemFetOrLower (const CSemNode& Node, const string& SemFet) const;
	// проходит по всем дизъюнктам, если в дизъюнкте отстутствует CAUS и NEG,    
	// и присутствует SemFet, тогда выдает истину
	bool			HasSemFetPro (const vector<QWORD>& SemFets, const string& SemFet) const;
    bool			HasSemFetPro (const CSemNode& Node, const string& SemFet) const;
	// проверяет, что в одном из значений поля FieldStr стоит константа ItemStr из домена DomStr
	bool HasItem (DictTypeEnum DictTy,  WORD UnitNo, const string& FieldStr, const string& ItemStr, const string& DomStr, BYTE LeafId, BYTE BracketLeafId)  const;
	// перевод  словарную интерпретацию в строковое представление
	string			InterpToStr(vector<CDictUnitInterp>::const_iterator I)  const;
	// перевод  интерпретацию открытого словосочетания в строковое представление
	string			OpenCollocInterpToStr(const COpenCollocInterp& I)  const;
	//выдает граммемы, полученные из поля RESTR для текущей словарной интерпретации
	vector<QWORD>	GetGramRestr(const CSemNode& W);
	// проверяет согласование по SF
	bool			GleicheSemFet(const vector<QWORD>& SemFets1, const vector<QWORD>& SemFets2, bool bInclusion) const;
	// индексирует один этаж (один дизъюнкт)
	QWORD			GetOneIndexedSemFet (const vector<string>& SemFet, bool bInlcudeLowerHierarcy,  bool	bInlcudeHigherHierarcy = false);
	// индексирует SF
	vector<QWORD> GetIndexedSemFets (const CSemPattern& P, bool bInlcudeLowerHierarcy,  bool	bInlcudeHigherHierarcy);
	// выдает представление SF как вектор строк
	vector<string>	GetSemFetStr (QWORD SemFet) const;
	// выдает строковое представление SF
	string			GetSemFetsInOneStr (const vector<QWORD>& SemFets) const;
	// проверяет, есть ли в статье AL1, если есть, то выдает его 
	SEngEquiv		GetAL1Value(int NodeNo) const;
   // по словарной статье предлога или союза выдает семантическое отношение, которое они выражает
    CValency		GetSemRelOfPrepOrConj(const CDictUnitInterp& Unit) const;

		



	//======================    работа с узлами по их словарному составу и принадлежности клаузе
	// проверяет, что узел NodeNo принадлежит клаузе ClauseNo
	bool			IsInClause (size_t NodeNo, size_t ClauseNo) const;
	virtual long	GetClauseFirstWordNo(long ClauseNo) const = 0;
	virtual long	GetClauseLastWordNo(long ClauseNo) const = 0;
	virtual string			GetClausePO(long ClauseNo) const  {return "";};
	
	

	//======================    работа с лексическими функциями (LF)
    // выдает первую LF, начало и конец которой совпадает с началом и концом iSemRelNo
	long			FindLexFunctBySemRel(long iSemRelNo) const;
    // выдает первую LF, начало и конец которой совпадает с iW1 и iW2 соответственно
	long			FindLexFunct(int iW1, int iW2) const;
	// выдает первую LF, которая входит в NodeNo (слово-ситуация)
	long			FindLexFunctBySituationNodeNo(long NodeNo) const;
	// выдает первую LF, которая выходит из NodeNo 
	long			FindLexFunctByParameterNodeNo(long NodeNo) const;

	// ====================   общие функции инициализации 
	// перевод синтаксической реализации в строку
	string			SynRealToStr(const CSynRealization&  SynReal, string Delimiter) const;
	// перевод графа в текстовое представление
	string			GetTxtGraph();
	// перевод графа в TCL-представление
	string			GetTclGraph(bool ShowUnusedValencies, bool UseIsTop);
	// выдает отношения, которые не нужно упорядочивать в дерево (они не являются основными)
	string	GetOtherRelations();
	// выдает TCL-представление для синтаксических отношений
	string			GetTclSyntaxGraph();
	// выдает ошибку 
	void			ErrorMessage (string Mess) const { m_pData->ErrorMessage(Mess); };
	// проверка валидности всех индексов
	void			AssertValidGraph();
	
	
        
	// ====================   работа с досемантической информацией
	// проверка того, что узлу припиcан предлог
	bool			CheckGroupBeginAndCase(string ItemStr, size_t NodeNo, long& PrepNo) const;
	// чтение SF из тезауруса по корневым концептам
    void			InitThesSemFet (CSemNode& OutNode, const CSemNode& InNode);
	// добавляет SF к узлу
	void			AddSemFet (CSemNode& Node, const string& SemFet);


		
		
		
			
		
		
	// ====================   работа с валентностями
	//инициализация валентностей из текущей словарной интерпретации
	void		InitVals(CSemNode& Node);
	long        FindAbstractPlugArticle (DictTypeEnum type, QWORD Grammems, long Poses, long ClauseType) const;
	void        FindAbstractAdditionArticle (DictTypeEnum type, const CSemNode& Node, vector<long>& Articles,  bool IsClauseSyntaxRoot, long ClauseType);
	void		AddAbstractAdditionVals (DictTypeEnum type, CSemNode& Node, const vector<long>& Articles);


	//============ локативы
	long			GetLocPrepFromBegining (size_t NodeNo) const; 
	bool			HasLocPrepInBegining (size_t NodeNo) const;

	virtual bool IsQuestionClause(long ClauseNo) const = 0;
	// обходим граф от текущей вершины (по стрелкам и против стрелок) и помечает достигнутые вершины 
	// (используется обычный перечень отношений)
	// если Tag != -1, то обход не идеь по уникальным отношениям
	virtual void			dfs(size_t NodeNo, bool ConsiderUseOfNodes,long Tag = -1) = 0;
	// проверяет, что все предложения связано
	bool			IsConnected();

	//  ставит последний знак препинания предложения  при синтезе
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




// функция переносит отношения, помеченные m_bDopRelation, в вектор m_DopRelations
// (дополнительные отношения находились в векторе основных, потому что они ипользовались
// для сопоставления валентных структур)
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

const  string SIMILAR_NUMERALS = "ОДНОР_ЧИСЛ";
const  string NUMERALS = "КОЛИЧ";
const  string C_NUMERALS = "СЛОЖ_ЧИСЛ";
const  string KEYB = "КЛВ";
const  string WEB_ADDR = "ЭЛ_АДРЕС";
const  string NAMES = "ФИО";
const  string NUMERAL_NOUN = "ЧИСЛ_СУЩ";
const  string NOUN_ADJ = "ПРИЛ_СУЩ";
const  string NOUN_NUMERAL = "СУЩ_ЧИСЛ";
const  string NUMERAL_ADVERB = "НАР_ЧИСЛ_СУЩ";
const  string SELECTIVE_GR = "ЭЛЕКТ_ИГ";
const size_t MaxValsCount = 15;




#endif
