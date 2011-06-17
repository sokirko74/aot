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
	bool GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const;
	long GetClauseTypeByName(const char* TypeName) const;

	const char* GetClauseNameByType(long type) const;
	const size_t GetClauseTypesCount() const;

	bool IsStrongClauseRoot(const DWORD Poses) const;
	bool is_month (const char* lemma) const;
	bool is_small_number (const char* lemma) const;
	bool IsMorphNoun (size_t Poses)  const;
	bool is_morph_adj (size_t poses) const;
	bool is_morph_participle (size_t poses) const;
	bool is_morph_pronoun (size_t poses) const;
	bool is_morph_pronoun_adjective(size_t poses) const;
	bool is_left_noun_modifier  (size_t poses, QWORD grammems) const;
	bool is_numeral (size_t poses) const;
	bool is_verb_form (size_t poses) const;
	bool is_infinitive(size_t poses) const;
	bool is_morph_predk(size_t poses) const;
	bool is_morph_adv(size_t poses) const;
	bool is_morph_article(size_t poses) const;
	bool is_morph_personal_pronoun (size_t poses, QWORD grammems) const;
	bool IsSimpleParticle(const char* lemma, size_t poses) const;
	bool IsSynNoun(size_t Poses, const char* Lemma) const;
	bool IsStandardParamAbbr (const char* WordStrUpper) const;
	
};

extern bool GenderNumberCaseRussian (const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool FiniteFormCoordRussian (const CAgramtabLine* l1, const CAgramtabLine* l2);

#endif //__RUSGRAMTAB_H_
