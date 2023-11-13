#pragma once

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

	void InitFieldsAndDomains();
	bool InitDomainsConsts();

public:
   BYTE                 ValFieldNo;
   BYTE                 RefFieldNo;
   BYTE                 GramFetFieldNo;
   BYTE                 POFieldNo;
   BYTE                 GramRestrFieldNo;
   BYTE                 LexFetFieldNo;
   BYTE                 ObligLexFetFieldNo;
   BYTE                 ArticleFieldNo;
   BYTE                 LexFunctFieldNo;
   BYTE                 SemFetFieldNo;
   BYTE                 NESOVMFieldNo;

   //константы доменов
   BYTE ActantsDomNo;
   BYTE LexDomNo;
   BYTE LexPlusDomNo;
   BYTE CollocDomNo;
   BYTE AbbrDomNo;
   BYTE FieldDomNo;
   BYTE SemFetDomNo;
   BYTE SemRelDomNo;
   BYTE LexFunctDomNo;
   BYTE CaseDomNo;
   BYTE CaseNumberDomNo;
   BYTE ActDomNo;
   BYTE MNADomNo;
   BYTE SynRelDomNo;
   BYTE SynGroupDomNo;
   BYTE LemGroupBeginingDomNo;
   BYTE PositionDomNo;
   BYTE VerbFetDomNo;

   // лексические функции 
   dom_item_id_t A0LexFunctNo;
   dom_item_id_t S0LexFunctNo;


   // синтаксические отношения 
   dom_item_id_t IndirObjSynONo; //к_доп
   dom_item_id_t ReverseSynONo; //X! // пассивная валентность на именную группу
   dom_item_id_t UncoordAttrSynONo; //нс_опр
   dom_item_id_t CoordAttrSynONo; //с_опр
   dom_item_id_t DirectObjSynONo; //п_доп
   dom_item_id_t SubjSynONo; //подл
   dom_item_id_t EnglSubjSynONo; //subj
   dom_item_id_t ParatAttrSynONo; //прим_опр
   dom_item_id_t FromPredicSynONo; //отпредик
   dom_item_id_t PostSpecifSynONo; //уточн


   // уточнения групп 
   dom_item_id_t QuoteMarkNo;
   dom_item_id_t ILENo;
   dom_item_id_t InfinitiveNo;
   dom_item_id_t NegativeNo;
   dom_item_id_t PassiveNo;

	  // части речи 
   dom_item_id_t AdjNo;
   dom_item_id_t AdvNo;
   dom_item_id_t PossPronNo;
   dom_item_id_t NumeralNo;
         

	// константы main GF 
   dom_item_id_t AdvAdjMainNo;
   dom_item_id_t NounMainNo;
   dom_item_id_t VerbMainNo;
   dom_item_id_t AdjMainNo;
   dom_item_id_t PronMainNo;


	// синтаксичесикие  группы 
   dom_item_id_t ClauseGrpNo;
   dom_item_id_t NounGrpNo;
   dom_item_id_t AdverbialGrpNo;
   dom_item_id_t VerbGrpNo;
   dom_item_id_t	CopulNo;
   dom_item_id_t	ModalCopulNo;


	// графеты
   dom_item_id_t NumerComplexNo;
   dom_item_id_t NumerSymbComplexNo;
   dom_item_id_t HyphenNo;
	  
  	  
	// падежи
   dom_item_id_t NominativeNo;
   dom_item_id_t InstrumentalisNo;
   dom_item_id_t InstrumentalisAdjNo;
   dom_item_id_t GenitivNo;
   dom_item_id_t DativNo;
   dom_item_id_t VocativNo;
   dom_item_id_t AccusativNo;
   dom_item_id_t NominativePluralisNo;
   dom_item_id_t InstrumentalisPluralisNo;
   dom_item_id_t GenitivPluralisNo;
   dom_item_id_t DativPluralisNo;
   dom_item_id_t VocativPluralisNo;
   dom_item_id_t AccusativPluralisNo;

   dom_item_id_t NominativeSingularNo;
   dom_item_id_t InstrumentalisSingularNo;
   dom_item_id_t GenitivSingularNo;
   dom_item_id_t DativSingularNo;
   dom_item_id_t VocativSingularNo;
   dom_item_id_t AccusativSingularNo;

	bool IsGenitiv (dom_item_id_t ItemNo) const
	{
		return     (ItemNo == GenitivSingularNo)
				|| (ItemNo == GenitivPluralisNo)
				|| (ItemNo == GenitivNo);
	};
	 


	dom_item_id_t   SelfLabelNo;
	dom_item_id_t   MainWordVarNo;
	dom_item_id_t   MNANo;
	dom_item_id_t   S_And_InstrNo;
	dom_item_id_t RightDirectionNo;

   std::string               m_DictName;
   std::string               m_DictPath;
   long                 m_LastUpdateTime;


   
   bool					m_bOwnDictionary;
   CTranslatorHolder*   m_TranslatorHolder;
   
   CDictionary* GetRoss () {return &m_Ross;}
   const CDictionary* GetRoss () const {return &m_Ross;}
   bool			OpenRossHolder (const std::string strPathName, bool bDontLoadExamples);

   dom_item_id_t GetItemNoByItemStr(const std::string& ItemStr, const char* DomStr) const ;
   dom_item_id_t GetItemNoByItemStr1(const std::string& ItemStr, BYTE domNo) const;

   const std::string& GetDomItemStrWrapper(dom_item_id_t item_id) const;
   const std::string& GetDomItemStrWrapper1(size_t cortege_no, BYTE item_no) const;

   TCortege GetCortegeCopy(size_t cortege_no) const;
	
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
	// читает поле AUX, возвращает номер главного слова
	long				GetDopFields(long UnitNo, std::vector<CDopField>& DopFields) const;
	bool				GetVal(long UnitNo, CValency& V) const;
	void				GetLexFuncts (size_t UnitNo,  std::vector<CLexicalFunctionField>& OutVector, DictTypeEnum type = NoneRoss, const CRossHolder* pRossHolderObor = NULL) const;
	// по словарной статье предлога или союза выдает семантическое отношение, которое они выражает
	CValency			GetSemRelOfPrepOrConj(uint16_t UnitNo) const;
	long				GetSemMainWordFromArticle (long UnitNo) const;

	// проверяет, что ItemNo принадлежит домену Д_ГГ_уточн
	bool				IsVerbFet (dom_item_id_t item_id) const;
	// проверяет, что ItemNo принадлежит домену D_SYN_REL
	bool				IsSynRel (dom_item_id_t item_id) const;
	// проверяет, что ItemNo принадлежит домену D_1
	bool				IsLemGroupBegining (dom_item_id_t item_id) const;
	// проверяет, что ItemNo принадлежит домену D_GROUPS
	bool				IsSynGroup (dom_item_id_t item_id) const;
	// проверяет, что ItemNo принадлежит домену D_POSITION
	bool				IsPosition (dom_item_id_t item_id) const;
	// выдает номер первой константы, из CortegeNo-го кортежа массиве m_GramCorteges 
	// (Значение до запятой)
	dom_item_id_t		GetSynRel(const  TCortege& C) const;
	// выдает номер второй константы, из CortegeNo-го кортежа массиве m_GramCorteges 
	// (Значение после запятой)1
	dom_item_id_t		GetSynFet(const  TCortege& C) const;
	bool				IsCompAdjCortege(const  TCortege& C) const;
	bool IsCase(dom_item_id_t item_id) const;
};



