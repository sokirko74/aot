// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef __RUSGRAMTAB_H_
#define __RUSGRAMTAB_H_


#include "agramtab_.h"       
#include "rus_consts.h"       

/////////////////////////////////////////////////////////////////////////////

const unsigned int  StartUp =  0xC0E0; //Аа 
const unsigned int  EndUp = 0x10000;  //яя
const unsigned int MaxGrmCount  = EndUp -StartUp; // // 16159  (16 Кб) 
const BYTE rPartOfSpeechCount = 22;
// не должно быть пробелов между словами 
const char rPartOfSpeeches[rPartOfSpeechCount][20] = 
{	"С",  // 0
	"П", // 1
	"Г", // 2
	"МС", // 3
	"МС-П", // 4
	"МС-ПРЕДК", // 5
	"ЧИСЛ", // 6
	"ЧИСЛ-П", // 7
	"Н", // 8
	"ПРЕДК", //9 
	"ПРЕДЛ", // 10
	"ПОСЛ", // 11
	"СОЮЗ", // 12
	"МЕЖД", // 13
	"ВВОДН",// 14
	"ФРАЗ", // 15
	"ЧАСТ", // 16
	"КР_ПРИЛ",  // 17
	"ПРИЧАСТИЕ", //18
	"ДЕЕПРИЧАСТИЕ", //19
	"КР_ПРИЧАСТИЕ", // 20
	"ИНФИНИТИВ"  //21
};

const short GrammemsCount = 52;
const char Grammems[GrammemsCount][10] = { 
					// 0..1
				   	"мн","ед",
					// 2..8
					"им","рд","дт","вн","тв","пр","зв",
					// род 9-12
					"мр","жр","ср","мр-жр",
					// 13..15
					"нст","буд","прш",
					// 16..18
					"1л","2л","3л",	
					// 19
					"пвл",
					// 20..21
					"од","но",	
					// 22
					"сравн",
					// 23..24
					"св","нс",	
					// 25..26
					"нп","пе",
					// 27..28
					"дст","стр",
					// 29-31
					"0", "аббр", "отч",
					// 32-33
					"лок", "орг",
					// 34-35
					"кач", "дфст",
					// 36-37 (наречия)
					"вопр", "указат",
					// 38..39
					"имя","фам",
					// 40
					"безл",
					// 41,42
					"жарг", "опч",
					// 43,44,45
					"разг", "притяж", "арх",
					// для второго родительного и второго предложного
					"2",
					"поэт", "проф",
					"прев", "полож"
					};




const int rClauseTypesCount = 12;
const char rClauseTypes [rClauseTypesCount][30] = 
{
	"ГЛ_ЛИЧН", 
	"ДПР",
	"КР_ПРЧ",
	"КР_ПРИЛ",
	"ПРЕДК",
	"ПРЧ",	 
	"ИНФ",	
	"ВВОД",
	"ТИРЕ",
	"НСО",
	"СРАВН",
	"КОПУЛ"
};




class CRusGramTab : public CAgramtab{
	public:
	CAgramtabLine*  Lines[MaxGrmCount];
	CRusGramTab();
	~CRusGramTab();

    BYTE			GetPartOfSpeechesCount () const;
	const char*		GetPartOfSpeechStr(BYTE i) const;
	size_t			GetGrammemsCount()  const;
	const char*		GetGrammemStr(size_t i) const;
	size_t			GetMaxGrmCount() const;
	CAgramtabLine*&			GetLine(size_t LineNo);
	const CAgramtabLine*	GetLine(size_t LineNo) const;
	size_t		s2i(const char * s )  const;
	string		i2s(WORD i) const;

    bool		ProcessPOSAndGrammems (const char* tab_str, BYTE& PartOfSpeech,  QWORD& grammems) const;
	const char* GetRegistryString() const;

	bool GleicheCase(const char* gram_code_noun, const char* gram_code_adj) const;
	bool GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const;
	QWORD GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const;

	bool GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const;
	bool ConflictGenderNumber(const char* gram_code1, const char* gram_code2) const;//with absent grammems check, less strict than GleicheGenderNumber
	bool ConflictGrammems(QWORD g1, QWORD g2, QWORD breaks) const;//with absent grammems check, less strict than GleicheGenderNumber
	bool GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const;
	long GetClauseTypeByName(const char* TypeName) const;

	const char* GetClauseNameByType(long type) const;
	const size_t GetClauseTypesCount() const;

	bool IsStrongClauseRoot(const poses_mask_t poses) const;
	bool is_month (const char* lemma) const;
	bool is_small_number (const char* lemma) const;
	bool IsMorphNoun (poses_mask_t poses)  const;
	bool is_morph_adj (poses_mask_t poses) const;
	bool is_morph_participle (poses_mask_t poses) const;
	bool is_morph_pronoun (poses_mask_t poses) const;
	bool is_morph_pronoun_adjective(poses_mask_t poses) const;
	bool is_left_noun_modifier  (poses_mask_t poses, QWORD grammems) const;
	bool is_numeral (poses_mask_t poses) const;
	bool is_verb_form (poses_mask_t poses) const;
	bool is_infinitive(poses_mask_t poses) const;
	bool is_morph_predk(poses_mask_t poses) const;
	bool is_morph_adv(poses_mask_t poses) const;
	bool is_morph_article(poses_mask_t poses) const;
	bool is_morph_personal_pronoun (poses_mask_t poses, QWORD grammems) const;
	bool IsSimpleParticle(const char* lemma, poses_mask_t poses) const;
	bool IsSynNoun(poses_mask_t poses, const char* Lemma) const;
	bool IsStandardParamAbbr (const char* WordStrUpper) const;
    bool FilterNounNumeral(string& gcNoun, const string& gcNum, QWORD& grammems) const override;
    QWORD ChangeGleicheAncode1(GrammemCompare CompareFunc, const string& wordGramCodes,  string& groupGramCodes, const QWORD wordGrammems) const override;
};

extern bool GenderNumberCaseRussian (const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool FiniteFormCoordRussian (const CAgramtabLine* l1, const CAgramtabLine* l2);

	// Стандартное согласование между двумя именами  по  числу и падежу
inline bool CaseNumber (const CAgramtabLine* l1, const CAgramtabLine* l2) 
  {
      return ((rAllCases  & l1->m_Grammems & l2->m_Grammems) > 0) &&
		     ((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0) ;
  };
inline bool CaseGender (const CAgramtabLine* l1, const CAgramtabLine* l2) 
  {
      return ((rAllCases  & l1->m_Grammems & l2->m_Grammems) > 0) &&
		     ((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0) ;
  };
inline bool CaseNumberGender (const CAgramtabLine* l1, const CAgramtabLine* l2) 
  {
      return ((rAllCases  & l1->m_Grammems & l2->m_Grammems) > 0) &&
			 ((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0) &&
		     ((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0) ;
  };
inline bool CaseNumberGender0 (const CAgramtabLine* l1, const CAgramtabLine* l2) //with absent grammems check
  {
      return ((rAllCases   & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllCases   & l1->m_Grammems) || !(rAllCases   & l2->m_Grammems)) &&
			 ((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllNumbers & l1->m_Grammems) || !(rAllNumbers & l2->m_Grammems)) &&
		     ((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllGenders & l1->m_Grammems) || !(rAllGenders & l2->m_Grammems));			 ;
  };
inline bool GenderNumber0 (const CAgramtabLine* l1, const CAgramtabLine* l2) //with absent grammems check
  {
      return ((rAllGenders & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllGenders & l1->m_Grammems) || !(rAllGenders & l2->m_Grammems)) &&
			 ((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllNumbers & l1->m_Grammems) || !(rAllNumbers & l2->m_Grammems));			 
  };	
inline bool CaseNumber0 (const CAgramtabLine* l1, const CAgramtabLine* l2) //with absent grammems check
  {
      return ((rAllCases   & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllCases   & l1->m_Grammems) || !(rAllCases   & l2->m_Grammems)) &&
			 ((rAllNumbers & l1->m_Grammems & l2->m_Grammems) > 0 || !(rAllNumbers & l1->m_Grammems) || !(rAllNumbers & l2->m_Grammems));			 ;
  };
#endif //__RUSGRAMTAB_H_
