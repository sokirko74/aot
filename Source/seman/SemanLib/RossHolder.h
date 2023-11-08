#ifndef RossHolder_H
#define RossHolder_H

#include "morph_dict/common/utilit.h"
#include "common/cortege.h"

#include "TranslatorHolder.h"
#include "DictUnitInterp.h"
#include "Valency.h"


const int MaxWordLen = 32;



// значени поля AUX, например, SUB (С1, С2)
struct CDopField {
	std::string  m_RelationStr;
	bool	m_IsActant1; // A or C
	BYTE	m_Word1;
	bool	m_IsActant2; // A or C
	BYTE	m_Word2;
};



// класс поля лексической функции, который содержит значения поля LF (см. формат поля)
// LF = Oper1 : сдавать 1
struct CLexicalFunctionField {
	// название лексической функции
	std::string			m_LexFunct;
	// значение лексической функции
	std::string			m_Lemma;
	// номер значения для m_Lemma
	char			m_MeanNum;
	// номер словарной статьи в словаре для <m_Lemma,m_MeanNum>
	uint16_t			m_LexFunctWordUnitNo;
	// предлог, с помощью которого слово-параметр управляет словом-ситуацией 
	CRossInterp		m_Prep;


	CLexicalFunctionField (std::string LexFunct, std::string Lemma, char MeanNum = -1)
	{
		m_LexFunct = LexFunct;
		m_Lemma = Lemma;
		m_MeanNum = MeanNum;
		m_LexFunctWordUnitNo = ErrUnitNo;
	};

    bool operator == ( const std::string& X ) const
    {
		return (X == m_Lemma);
	};
};




class CRossHolder {

	CDictionary			m_Ross;

public:
   BYTE                 ValFieldNo;
   BYTE                 RefFieldNo;
   BYTE                 GramFetFieldNo;
   BYTE                 POFieldNo;
   BYTE                 GramRestrFieldNo;
   BYTE                 LexFetFieldNo;
   BYTE                 ObligLexFetFieldNo;
   BYTE                 ArticleFieldNo;
   BYTE                 ActDomNo;
   BYTE                 SemFetFieldNo;
   BYTE                 NESOVMFieldNo;

   // лексические функции 
   BYTE                 A0LexFunctNo;
   BYTE                 S0LexFunctNo;


   // синтаксические отношения 
   int IndirObjSynONo; //к_доп
   int ReverseSynONo; //X! // пассивная валентность на именную группу
   int UncoordAttrSynONo; //нс_опр
   int CoordAttrSynONo; //с_опр
   int DirectObjSynONo; //п_доп
   int SubjSynONo; //подл
   int EnglSubjSynONo; //subj
   int ParatAttrSynONo; //прим_опр
   int FromPredicSynONo; //отпредик
   int PostSpecifSynONo; //уточн


   // уточнения групп 
	  int QuoteMarkNo;
	  int ILENo;
	  int InfinitiveNo;
	  int NegativeNo;
	  int PassiveNo;

	  // части речи 
	  int AdjNo;
	  int AdvNo;
	  int PossPronNo;
	  int NumeralNo;

	  // константы полей
	  BYTE SemFeatureNo; 
	  BYTE LexDerivationNo;  
	  BYTE LemmaVariantsNo;  
	  BYTE LexFuncsNo;  
	  BYTE GramFeatureNo;
	  BYTE LinkNo;

	  //константы доменов
	  BYTE ActantsDomNo;
	  BYTE LexDomNo;
	  BYTE LexPlusDomNo;
	  BYTE CollocDomNo;
	  BYTE AbbrDomNo;
	  BYTE FieldDomNo;
	  BYTE EmptyDomNo;
	  BYTE SemFetDomNo;
	  BYTE SemRelDomNo;
	  BYTE LexFunctDomNo;
      

	  // константы main GF 
	  int AdvAdjMainNo;
	  int NounMainNo;
	  int VerbMainNo;
	  int AdjMainNo;
	  int PronMainNo;


	  // синтаксичесикие  группы 
	  int ClauseGrpNo;
	  int NounGrpNo;
	  int AdverbialGrpNo;
      int VerbGrpNo;
	  int 	CopulNo;
	  int 	ModalCopulNo;


	  // графеты
	  int NumerComplexNo;
	  int NumerSymbComplexNo;
	  int HyphenNo;
	  
  	  
	  // падежи
	  int NominativeNo;
	  int InstrumentalisNo;
	  int InstrumentalisAdjNo;
	  int GenitivNo;
	  int DativNo;
	  int VocativNo;
	  int AccusativNo;
	  int NominativePluralisNo;
	  int InstrumentalisPluralisNo;
	  int GenitivPluralisNo;
	  int DativPluralisNo;
	  int VocativPluralisNo;
	  int AccusativPluralisNo;

	  int NominativeSingularNo;
	  int InstrumentalisSingularNo;
	  int GenitivSingularNo;
	  int DativSingularNo;
	  int VocativSingularNo;
	  int AccusativSingularNo;
	  bool IsGenitiv (int ItemNo) const 
	  {
		  if (ItemNo == -1) return false;
		  return    (ItemNo== GenitivSingularNo) 
			     || (ItemNo== GenitivPluralisNo) 
				 || (ItemNo== GenitivNo);
	  };


  


   BYTE                 SelfLabelNo;
   BYTE                 LexFunctFieldNo;
   int                 MainWordVarNo;
   int                 MNANo;
   BYTE                 MNADomNo;
   BYTE                 SynRelDomNo;
   BYTE                 SynGroupDomNo;
   BYTE                 LemGroupBeginingDomNo;
   BYTE                 PositionDomNo;
   BYTE                 VerbFetDomNo;
   int                 S_And_InstrNo;
   int					RightDirectionNo;

   std::string               m_DictName;
   std::string               m_DictPath;
   long                 m_LastUpdateTime;


   
   bool					m_bOwnDictionary;
   CTranslatorHolder*   m_TranslatorHolder;
   
   CDictionary* GetRoss () {return &m_Ross;}
   const CDictionary* GetRoss () const {return &m_Ross;}
   bool			OpenRossHolder (const std::string strPathName, bool bDontLoadExamples);
   long         GetItemNoByItemStr(const std::string& ItemStr, const char* DomStr) const ;
   bool         InitDomainsConsts();
   

	
   bool InitConsts();
   void GetSimpleFieldItemsFromArticle (long UnitNo, std::string FieldStr, BYTE LeafId, BYTE BracketLeafId, StringVector& Items) const;
   void GetFullFieldItemsFromArticle (long UnitNo, std::string FieldStr, BYTE LeafId, BYTE BracketLeafId, StringVector& Items) const;
   CRossHolder(CTranslatorHolder* TranslatorHolder);
   ~CRossHolder();
   bool HasBeenModified(const long T);
   // проверят только первый элемент кортежа
   bool					HasFieldValue(std::string strFieldName, std::string strValue, long UnitNo, BYTE LeafId = 0, BYTE BracketLeafId = 0  ) const;
   // проверят все значения поля
   bool					HasFullFieldValue(std::string strFieldName, std::string strValue, long UnitNo, BYTE LeafId = 0, BYTE BracketLeafId = 0) const;
   void					GetFieldValues(std::string strFieldName, long UnitNo, std::vector<TCortege>& vectorCorteges, BYTE  LeafId = 0, BYTE BracketLeafId = 0 ) const;
   bool					HasItem (uint16_t UnitNo, const std::string FieldStr, const std::string ItemStr, const std::string DomStr, BYTE LeafId, BYTE BracketLeafId)  const;
   


	bool				HasCX (uint16_t UnitNo, const std::string CX, const std::string DomStr) const;
	uint16_t				LocateUnit (const char* UnitStr, BYTE MeanNum) const;
	const char*			GetDomItemStrInner (long ItemNo) const;
	// читает поле AUX, возвращает номер главного слова
	long				GetDopFields(long UnitNo, std::vector<CDopField>& DopFields) const;
	bool				GetVal(long UnitNo, CValency& V) const;
	void				GetLexFuncts (size_t UnitNo,  std::vector<CLexicalFunctionField>& OutVector, DictTypeEnum type = NoneRoss, const CRossHolder* pRossHolderObor = NULL) const;
	// по словарной статье предлога или союза выдает семантическое отношение, которое они выражает
	CValency			GetSemRelOfPrepOrConj(uint16_t UnitNo) const;
	long				GetSemMainWordFromArticle (long UnitNo) const;

	// проверяет, что ItemNo принадлежит домену Д_ГГ_уточн
	bool				IsVerbFet (uint32_t ItemNo) const;
	// проверяет, что ItemNo принадлежит домену D_SYN_REL
	bool				IsSynRel (uint32_t ItemNo) const;
	// проверяет, что ItemNo принадлежит домену D_1
	bool				IsLemGroupBegining (uint32_t ItemNo) const;
	// проверяет, что ItemNo принадлежит домену D_GROUPS
	bool				IsSynGroup (uint32_t ItemNo) const;
	// проверяет, что ItemNo принадлежит домену D_POSITION
	bool				IsPosition (uint32_t ItemNo) const;
	// выдает номер первой константы, из CortegeNo-го кортежа массиве m_GramCorteges 
	// (Значение до запятой)
	long				GetSynRel(const  TCortege& C) const;
	// выдает номер второй константы, из CortegeNo-го кортежа массиве m_GramCorteges 
	// (Значение после запятой)1
	long				GetSynFet(const  TCortege& C) const;
	long				IsCompAdjCortege(const  TCortege& C) const;

};




inline TCortege GetCortegeCopy (const CDictionary* Ross, size_t CortegeNo)
{
	TCortege G = *Ross->_GetCortege(CortegeNo);

	long i=0;
	for (; i < Ross->m_MaxNumDom; i++)
	{
      long ItemNo = Ross->GetCortegeItem(CortegeNo, i);
	  G.m_DomItemNos[i] =  ItemNo;
	  if (ItemNo == -1) break;
	};

	if (i < 10)
		G.m_DomItemNos[i] =  -1;

    return G;
};


extern std::string WriteToString  (const CDictionary* Ross, const TCortege10& C);


template <int size>
class short_string
{
	char m_buffer[size];
public:
	short_string() 
	{
	};
	short_string(const char* buffer) 
	{
		int len = strlen (buffer);
		assert (len < size);
		strcpy (m_buffer, buffer);
	};
	operator char* () 
	{
		return m_buffer;
	};
	operator const char* () 
	{
		return m_buffer;
	};

	operator std::string () 
	{
		return std::string(m_buffer);
	};
	operator const std::string () 
	{
		return std::string(m_buffer);
	};
	bool operator == (const short_string& X)  const
	{
		return strcmp (m_buffer, X.m_buffer) == 0;
	};
	bool operator < (const short_string& X)  const
	{
		return strcmp (m_buffer, X.m_buffer) < 0;
	};

	short_string operator = (const short_string& X) 
	{
		strcpy (m_buffer, X.m_buffer);
		return *this;
	};
	short_string operator = (const std::string& X) 
	{
		assert (X.length() < size);
		strcpy (m_buffer, X.c_str());
		return *this;
	};

};





extern void rml_LOG( const char* format, ... );


template<class _II, class _Ty> inline
	bool _find(_II It, const _Ty& _V)
{
	  return !(find(It.begin(), It.end(), _V) ==  It.end());
}


#endif
