// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef __RUSGRAMTAB_H_
#define __RUSGRAMTAB_H_


#include "agramtab_.h"       
#include "rus_consts.h"       

/////////////////////////////////////////////////////////////////////////////

const unsigned int  StartUp =  0xC0E0; //Àà 
const unsigned int  EndUp = 0x10000;  //ÿÿ
const unsigned int MaxGrmCount  = EndUp -StartUp; // // 16159  (16 Êá) 
const BYTE rPartOfSpeechCount = 22;
// íå äîëæíî áûòü ïðîáåëîâ ìåæäó ñëîâàìè 
const char rPartOfSpeeches[rPartOfSpeechCount][20] = 
{	"Ñ",  // 0
	"Ï", // 1
	"Ã", // 2
	"ÌÑ", // 3
	"ÌÑ-Ï", // 4
	"ÌÑ-ÏÐÅÄÊ", // 5
	"×ÈÑË", // 6
	"×ÈÑË-Ï", // 7
	"Í", // 8
	"ÏÐÅÄÊ", //9 
	"ÏÐÅÄË", // 10
	"ÏÎÑË", // 11
	"ÑÎÞÇ", // 12
	"ÌÅÆÄ", // 13
	"ÂÂÎÄÍ",// 14
	"ÔÐÀÇ", // 15
	"×ÀÑÒ", // 16
	"ÊÐ_ÏÐÈË",  // 17
	"ÏÐÈ×ÀÑÒÈÅ", //18
	"ÄÅÅÏÐÈ×ÀÑÒÈÅ", //19
	"ÊÐ_ÏÐÈ×ÀÑÒÈÅ", // 20
	"ÈÍÔÈÍÈÒÈÂ"  //21
};

const short GrammemsCount = 52;
const char Grammems[GrammemsCount][10] = { 
					// 0..1
				   	"ìí","åä",
					// 2..8
					"èì","ðä","äò","âí","òâ","ïð","çâ",
					// ðîä 9-12
					"ìð","æð","ñð","ìð-æð",
					// 13..15
					"íñò","áóä","ïðø",
					// 16..18
					"1ë","2ë","3ë",	
					// 19
					"ïâë",
					// 20..21
					"îä","íî",	
					// 22
					"ñðàâí",
					// 23..24
					"ñâ","íñ",	
					// 25..26
					"íï","ïå",
					// 27..28
					"äñò","ñòð",
					// 29-31
					"0", "àááð", "îò÷",
					// 32-33
					"ëîê", "îðã",
					// 34-35
					"êà÷", "äôñò",
					// 36-37 (íàðå÷èÿ)
					"âîïð", "óêàçàò",
					// 38..39
					"èìÿ","ôàì",
					// 40
					"áåçë",
					// 41,42
					"æàðã", "îï÷",
					// 43,44,45
					"ðàçã", "ïðèòÿæ", "àðõ",
					// äëÿ âòîðîãî ðîäèòåëüíîãî è âòîðîãî ïðåäëîæíîãî
					"2",
					"ïîýò", "ïðîô",
					"ïðåâ", "ïîëîæ"
					};




const int rClauseTypesCount = 12;
const char rClauseTypes [rClauseTypesCount][30] = 
{
	"ÃË_ËÈ×Í", 
	"ÄÏÐ",
	"ÊÐ_ÏÐ×",
	"ÊÐ_ÏÐÈË",
	"ÏÐÅÄÊ",
	"ÏÐ×",	 
	"ÈÍÔ",	
	"ÂÂÎÄ",
	"ÒÈÐÅ",
	"ÍÑÎ",
	"ÑÐÀÂÍ",
	"ÊÎÏÓË"
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

	// Ñòàíäàðòíîå ñîãëàñîâàíèå ìåæäó äâóìÿ èìåíàìè  ïî  ÷èñëó è ïàäåæó
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
