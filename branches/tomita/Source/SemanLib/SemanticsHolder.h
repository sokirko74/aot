// RossDoc.h : interface of the CRossDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(Semanticsolder_H)
#define Semanticsolder_H
#include "AllRossesHolder.h"
#include "../common/GramInfo.h"


// частичная словарная интерпретация 
struct  CPartialInterp {
	// номер элемента в поле CONTENT или в термине
	short          m_ItemNo;
	// является ли данный элемент главным словом
	bool           m_bMainWord;

	CPartialInterp () {
		m_ItemNo = -1;
		m_bMainWord = false;
	};
};

// тезаурусная интерпретация 
struct  CThesInterp {
	// уникальный номер тезауруса
    long m_ThesaurusId;
	// уникальный номер термина
	long m_TerminId;
	CThesInterp () {
		m_ThesaurusId = -1;
		m_TerminId = -1;
	};
};



// частичная тезаурусная  интерпретация 
struct CThesPartialInterp : public CThesInterp, public CPartialInterp
{

};

// частичная словарная интерпретация 
struct CRossPartialInterp : public CRossInterp, public CPartialInterp
{
	// номер значения в поле LEX, с которым совпал этот элемент
	char           m_LexFetNo;
	 // номер значения в поле PREP, с которым совпал этот предлог, приписанный этому элементу
	char           m_PrepNoInArticle;
	CRossPartialInterp() : CRossInterp(), CPartialInterp() {
		m_LexFetNo = -1;
		m_PrepNoInArticle = 1;


	};
};

enum OpenCollocTypeEnum {NoneType, ThesType,  RossType};

// интерпретация открытого словосочетания
class COpenCollocInterp  {
	  // тезаурусная интерпретация 
	  CThesPartialInterp m_ThesInterp;
	  // словарная интерпретация 
	  CRossPartialInterp m_RossInterp;
public:
      // тип открытого словосочетания 
	  OpenCollocTypeEnum m_Type;
	  
	  CThesPartialInterp& GetThesInterp() {
		  assert (m_Type == ThesType) ;
		  return m_ThesInterp;
	  };
	  const CThesPartialInterp& GetThesInterp() const {
		  assert (m_Type == ThesType) ;
		  return m_ThesInterp;
	  };

	  CRossPartialInterp& GetRossInterp() {
		  assert (m_Type == RossType) ;
		  return m_RossInterp;
	  };
	  const CRossPartialInterp& GetRossInterp() const {
		  assert (m_Type == RossType) ;
		  return m_RossInterp;
	  };
	 
	  COpenCollocInterp () {
			m_Type = NoneType;
	  };
};









struct SEngEquiv
{
	SEngEquiv() : m_iMeanNum(10), m_RusPoses(-1) {};
	string  m_StrEngWord;
	int		m_iMeanNum;
	long	m_RusPoses;
	string  m_StrLexFunc;
	string  m_StrNumeral;
};

// класс грамматического и семантического выражения i-го актанта (формируется из полей VAL, GFi, LEXi, MANLEXi, SFi словарной статьи)





// класс оборота - соответствует одной словарной статтье из русского словаря оборотов
struct  CObor {
	// поле TITLЕ	
	string	m_UnitStr;
	// номер слованой статьи
	WORD    m_UnitNo;
 	
    // подчинительный союз-оборот
	bool m_bRusSubConj;
	// подчинительный союз-оборот, у которого есть помета RESTR = подл !
	bool m_bRusSubConjCanBeAfterSubject;
	// сочинительный союз-оборот
	bool m_bRusCoordConj;
	// предлог-оборот
	bool m_bRusOborPrep;
    // модальный обстоятельственный оборот  
	bool m_bRusModalOborAdverbial;
    // вводный обстоятельственный оборот 
	bool m_bRusIntrExpr;
	// усилительный ( с частицей НИ) обстоятельственный оборот 
	bool m_bRusNegOborAdverbial;
    // обстоятельственный оборот
	bool m_bRusOborAdverbial;

	CObor ()
	{
	   m_bRusSubConj = false;
	   m_bRusCoordConj = false;
	   m_bRusOborPrep = false;
	   m_bRusModalOborAdverbial = false;
	   m_bRusIntrExpr = false;
	   m_bRusNegOborAdverbial = false;
	   m_bRusOborAdverbial = false;
	   m_bRusSubConjCanBeAfterSubject = false;
	   m_UnitNo = ErrUnitNo;
	};

};

// класс простого бинарного отношения
struct CSimpleBinaryRelation {
	// исходный узел
	long    m_SourceNodeNo;
	// конечный узел
	long    m_TargetNodeNo;

	CSimpleBinaryRelation (long SourceNodeNo,	long    TargetNodeNo)
	{
		m_SourceNodeNo = SourceNodeNo;
	    m_TargetNodeNo = TargetNodeNo;
	};

	CSimpleBinaryRelation()
	{
		m_SourceNodeNo = -1;
	    m_TargetNodeNo = -1;	
	}
};

// класс синтаксического отношения
struct CSynRelation : public CSimpleBinaryRelation
{
	string			   m_SynRelName;
	bool			   m_bPragmaticallyStrong; 			
	CSynRelation (long SourceNodeNo,	long    TargetNodeNo, 	string SynRelName) :
		CSimpleBinaryRelation(SourceNodeNo,	TargetNodeNo)
	{
			m_SynRelName = SynRelName;
			m_bPragmaticallyStrong = false;
	};

	CSynRelation() : CSimpleBinaryRelation()
	{
		m_bPragmaticallyStrong = false;
	};
	bool operator == (const CSynRelation X) const
	{
		return   (X.m_SourceNodeNo ==  m_SourceNodeNo)
			   &&(X.m_TargetNodeNo ==  m_TargetNodeNo)
			   && (X.m_SynRelName == m_SynRelName);
	};

};


// класс синтаксического отношения
struct CSemThesRelation : public CSimpleBinaryRelation
{
	string			   m_SemRelName;
	CSemThesRelation (long SourceNodeNo,	long    TargetNodeNo, 	string SemRelName) :
		CSimpleBinaryRelation(SourceNodeNo,	TargetNodeNo)
	{
			m_SemRelName = SemRelName;
	};

	CSemThesRelation() : CSimpleBinaryRelation()
	{
	};
	bool operator == (const CSemThesRelation X) const 
	{
		return   (X.m_SourceNodeNo ==  m_SourceNodeNo)
			   &&(X.m_TargetNodeNo ==  m_TargetNodeNo)
			   && (X.m_SemRelName == m_SemRelName);
	};

};



// класс отношения LF
struct CLexFunctRel : public CSimpleBinaryRelation
{
	// название отношения
	string			m_LexFunctName;
	CDictUnitInterp m_Prep;
	string			m_LexFunParamLemma;


	CLexFunctRel (long SourceNodeNo,	long    TargetNodeNo, 	string LexFunctName) :
		CSimpleBinaryRelation(SourceNodeNo,	TargetNodeNo)
	{
			m_LexFunctName = LexFunctName;
	};

	CLexFunctRel() : CSimpleBinaryRelation()
	{
	};
	bool operator == (const CLexFunctRel X)
	{
		return   (X.m_SourceNodeNo ==  m_SourceNodeNo)
			   &&(X.m_TargetNodeNo ==  m_TargetNodeNo)
			   && (X.m_LexFunctName == m_LexFunctName);
	};

};


/*
 Ссылка на словарную статью (например в поле LF). Здесь не указывается,
 в каком словаре  содержится эта статья, но подразумевается, что  это РОСС (русские слова) 
 или АОСС (английские слова).
*/

struct CDictReference 
{
	string			m_UnitStr;
	BYTE			m_MeanNum;
	
	CDictReference(string UnitStr, BYTE   MeanNum)
	{
		m_UnitStr = UnitStr;
		m_MeanNum = MeanNum;
	};
	CDictReference()
	{
		m_UnitStr = "";
		m_MeanNum = 10;
	};
};

bool WordMeanComp(const CDictReference& arg1, const CDictReference& arg2);


struct SLexFunIndexes
{
	bool EqByLexFunArg(CDictReference& arg) const
	{
		return !WordMeanComp(m_LexFunArg,arg) && !WordMeanComp(arg, m_LexFunArg);
	};

	bool EqByLexFunValue(CDictReference& arg) const
	{
		return !WordMeanComp(m_LexFunValue,arg) && !WordMeanComp(arg, m_LexFunValue);
	};


	// лексическая  функция  (S0, Oper1...)
	string			  m_LexFun;
	// аргумент (стоит в скобках)
	CDictReference    m_LexFunArg;
	// значение  (стоит  после знака равно)
	CDictReference	  m_LexFunValue;

	// например, Oper1(экзамен) = сдавать.
	// здесь "экзамен" - аргумент, а "сдавать" - значение
	// или S0(эказаминировать) = экзамен, здесь "эказаменировать" аргумент


};



bool WordInList(const char* word_list, int count, string word);
bool LexFunArgComp(const SLexFunIndexes& arg1, const SLexFunIndexes& arg2);
bool LexFunValueComp(const SLexFunIndexes& arg1, const SLexFunIndexes& arg2);

// класс, в котором по номеру статьи из Aoss,  EngCollocRoss или EngObor можно получить один из переводов, 
// стоящих в поле RUS (заголовок и номер значения) статьи m_EngUnitNo
struct CEngUnitNoToRusUnit
{
	// заголовок русской словарной статьи
    string  m_RusUnitStr;
	// номер значения русской словарной статьи
	BYTE    m_RusMeanNum;
	// номер словарной статьи в АОССе
	WORD	m_EngUnitNo;	

	CEngUnitNoToRusUnit()
	{
		m_RusMeanNum = 10;
	}

	// сортровка по русскому входу (как в россе)
	bool operator < (const CEngUnitNoToRusUnit& RusEquiv ) const
	{
		if( RusEquiv.m_RusUnitStr < m_RusUnitStr )		
			return true;

		if( RusEquiv.m_RusUnitStr > m_RusUnitStr )		
			return false;
		
		if( RusEquiv.m_RusUnitStr == m_RusUnitStr )		
		{
			if( RusEquiv.m_RusMeanNum == 10 )
				return false;
			return RusEquiv.m_RusMeanNum < m_RusMeanNum;
		}
		return false;
	}

	bool operator == (const CEngUnitNoToRusUnit& RusEquiv ) const
	{
		return			( ( RusEquiv.m_RusUnitStr == m_RusUnitStr )	
					&&	( RusEquiv.m_RusMeanNum == m_RusMeanNum ))
				||	( ( RusEquiv.m_RusUnitStr == m_RusUnitStr )	
					&& ( RusEquiv.m_RusMeanNum == 10) );

	}
};





class CHierarchyHolder;


struct LocPrep {
	string  m_RusPrepAndCase;
    string  m_RelationStr;
	string  m_EngPrep;
};

class CCollocItem;
struct CCollocItemRef {

	long CollocNo;
	long ItemNo;
	CCollocItemRef (long _CollocNo,	long _ItemNo)
	{
		CollocNo = _CollocNo;
		ItemNo = _ItemNo;
	};
};

struct CCollocItemRefCollect
{
  string                Item;
  vector<CCollocItemRef> Refs;	 
  CCollocItemRefCollect(const string S) 
  {
	  Item = S;
  };

};

struct LessCollocItemRefCollect 
{
	inline bool  operator ()( const CCollocItemRefCollect& _Y, const string& X ) const
	{
		return  _Y.Item < X;
	};
	inline bool  operator () ( const string& X, const CCollocItemRefCollect& _Y  ) const
	{
		return  X < _Y.Item;
	};
	inline bool  operator () ( const CCollocItemRefCollect& _Y1, const CCollocItemRefCollect& _Y2 ) const
	{
		return  _Y1.Item < _Y2.Item;
	};
};



//pedef pair<string, string> stringPair;
struct CAbbrFunct {
	string m_AbbrForm;
	string m_FullForm;
	string m_FunctName;
	CAbbrFunct (string AbbrForm,string FullForm,	string FunctName)
	{
	   m_AbbrForm= AbbrForm;
	   m_FullForm = FullForm ;
	   m_FunctName = FunctName;
	};
};



enum AbstractArticleEnum  {  atEmptyType, atArticlePlug, atAdditionArticle };

struct CAbstractArticle {
   long               m_UnitNo;
   string            m_UnitStr;
   vector<TCortege10> m_Article;
   vector<CGramInfo>  m_GramInfos;
   vector<long>       m_ClauseTypes;
   AbstractArticleEnum m_Type;
   StringVector     m_SemTypes;
   StringVector     m_SemFets;
   StringVector     m_LexFets;
   vector<CValency>    m_Vals;
};


class CCollocItem {
public:
  string Item;
  string RelOperator1;
  string RelOperator2;
  
  bool InitCollocItem (string S);
  bool IsHole() const;  
  
};

class CColloc {
public:
	long UnitNo;
	bool IsBlackBox;
	bool IsConditional;
	// m_SemMainWord содержит числовое значение поля СГС  (счет начинается с 1)
	long m_SemMainWord;
	vector<CCollocItem> Items;
	vector<CSynRelation>		m_Rels;
};



const int MaxLeafId  = 10;
typedef pair<string, long>  stringLong;
typedef pair<long, long>     LongLong;

// класс, в котором содержится лексический материал, с помощью которого заполняется
// дырка в группе времение (берется из полей LEX(i) и  PREPi(j)
struct CTimeLexicalFilling { 
	BYTE m_LeafId;
	BYTE m_BracketLeafId;
	vector<stringLong> m_LexFets;
	vector<LongLong>    m_Preps;

	CTimeLexicalFilling  (BYTE LeafId, BYTE BracketLeafId)
	{
		   m_LeafId =  LeafId;
		   m_BracketLeafId = BracketLeafId;
	};
	bool operator == (const CTimeLexicalFilling X) const 
	{
		  return    (m_LeafId == X.m_LeafId) 
				 && (m_BracketLeafId == X.m_BracketLeafId);
	};
};

// класс одной словарной статьи словаря групп времени
struct CTimeUnit {
	StringVector				m_Places;
	long						m_UnitNo;
	vector<CSynRelation>		m_Rels;
	vector<CTimeLexicalFilling>	m_LexicalFillings;
	bool						m_bCanFillNotTimeValency;
	CTimeUnit ()
	{
		m_bCanFillNotTimeValency = false;
		m_UnitNo = -1;
	};
	
	int	GetLexicalFillingNo (const CTimeLexicalFilling& C)  
	{
		vector<CTimeLexicalFilling>::iterator It = find(m_LexicalFillings.begin(), m_LexicalFillings.end(), C);

		if (It == m_LexicalFillings.end())
		{
			m_LexicalFillings.push_back (C);
			return m_LexicalFillings.size() - 1;
		}
		else
			return It - m_LexicalFillings.begin();
	};
};

struct CUnitContent 
{
	string m_UnitStr;
	WORD   m_UnitNo;
	bool operator == (const CUnitContent& X ) const
	{
		return (X.m_UnitNo == m_UnitNo);
	};
	bool operator < (const CUnitContent& X ) const
	{
		return m_UnitNo < X.m_UnitNo;
	};
	
};

//Например, "больше->много", "позже->поздно"
struct CComparAdverb {
	string m_Adverb;
	string m_ComparAdverb;
	CComparAdverb  (string Adverb,string ComparAdverb)
	{
		m_Adverb = Adverb;
		m_ComparAdverb = ComparAdverb;
	};
};



struct CDoubleConj
{
	// ссылка на словарную статью в словаре оборотов
	int				 m_UnitNo;
	// первая часть союза
	StringVector m_FirstPart;
	// вторая часть союза (между словами союза стоит 0)
	StringVector m_SecondPart;
	// например, для союза  "если бы не ... то"
	// m_FirstPart = "ЕСЛИ"; "БЫ"; "НЕ"
	// m_SecondPart = "ТО"

	// означает, что первая часть союза совпадает со второй
	// например, "или ... или"
	bool			m_bRepeating;
};


class CSemanticsHolder  : public CAllRossesHolder 	
{ 
 public:
	long m_LastIndicesUpdateTime;
	

	CSemanticsHolder();

	// список всех локативных предлогов
	vector<LocPrep>      m_LocPreps;

	// список всех словосочетаний 
	vector<CColloc>         m_RusCollocs;
	vector<CCollocItemRefCollect>  m_RusCollocItemRefs;

	// список всех сокращений из TimeRoss
	vector<CAbbrFunct>    m_TimeAbbrPairs;
	vector<CTimeUnit>     m_TimeUnits;
	StringVector   m_RusMonths;
	StringVector   m_RusWeekDays;
	bool                  InitTimeUnits();
	bool                  InitTimeThesLists();

	vector<CObor> Oborottos;
	vector<CDoubleConj>  m_DisruptConj;
	//варианты  английских оборотов	в поле CONTENT
	vector<CUnitContent> m_vectorEngOborStr;
	//варианты  английских словосочетаний в поле CONTENT
	vector<CUnitContent> m_vectorEngCollocStr;



	bool BuildEngOborStr();
	bool BuildEngCollocsStr();
	string GetContentsOborStr(long UnitNo, vector<CUnitContent> & Contents);
	string GetEngOborStr(long UnitNo)
	{
		return GetContentsOborStr(UnitNo, m_vectorEngOborStr);
	}
	string GetEngCollocStr(long UnitNo)
	{
		return GetContentsOborStr(UnitNo, m_vectorEngCollocStr);
	}



	// список всех абстрактных статей из словарей
	typedef map<DictTypeEnum, vector<CAbstractArticle> > AbsractArticleMap;
	AbsractArticleMap   m_AbstractArticles;
	const vector<CAbstractArticle>* GetAbstractArticles(DictTypeEnum type ) const;

	//индексы по полю RUS
	vector<CEngUnitNoToRusUnit>  m_RusEquivs;
	vector<CEngUnitNoToRusUnit>  m_RusEquivsEngObor;
	vector<CEngUnitNoToRusUnit>  m_RusEquivsEngColsocs;
	vector<CEngUnitNoToRusUnit>& GetRusEquivIndexes(DictTypeEnum type);

	//индексы по лексическим функциям
	vector<SLexFunIndexes> m_LexFunIndexesRusByValue;
	vector<SLexFunIndexes> m_LexFunIndexesRusByArg;
	vector<SLexFunIndexes> m_LexFunIndexesEng;	  

	bool CreateAROSSIndex();
	bool CreateEngOborIndex();
	bool CreateEngCollocsROSSIndex();	  
	bool CreateEngDictIndex(DictTypeEnum type, vector<CEngUnitNoToRusUnit>& RusEquivs);
	bool CreateLexFunIndexes(const CDictionary* pRoss, vector<SLexFunIndexes>& LexFunIndexes);
	bool PrintRusEquivs(string strFileName, DictTypeEnum type);



	bool	        ReadAbstractArticles(DictTypeEnum type);
	bool            BuildOborottos ();
	void            GetCustomGrammems (string GramFet, QWORD& Grammems, DWORD& Pose);

	bool			InitializeIndices();


	void			GetPrepsFromArticle (const CDictionary* pRoss, long UnitNo, BYTE LeafId, BYTE BracketLeafId, vector<CRossInterp>& Preps);
	UINT			GetAdverbWith_O_ByAdjective (UINT AdjParadigmId, string AdjWordForm);

	// =========  словосочетания
// добавляет отдельный элемент поля CONTENT в глобальный перечень всех отдельных элементов поля CONTENT	
	CCollocItemRefCollect* InsertRusCollocItemRef(string S);
	// читает поле CONTENT словосочетания CollocUnitNo и создает по нему множество словосочетаний
	bool			BuildColloc (string ContentFieldStr, int CollocUnitNo);
	// запускаем BuildColloc  для всех словосочетаний
	bool			BuildCollocs();
	bool			TokenizeDoubleConj();
	

	DictTypeEnum	IsRegisteredRoss(string FileName) const;
	string			GetRossPath(DictTypeEnum RossId)  const;

};

extern  size_t GetCaseGrammem (const CRossHolder* RossDoc, long ItemNo); 
extern  size_t GetCaseItemNo (CRossHolder* RossDoc, long GrammemNo);
extern  bool IsCase (const CRossHolder* RossDoc, long ItemNo);




#endif
