// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef __EngGRAMTAB_H_
#define __EngGRAMTAB_H_


#include "agramtab_.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// CEngGramTab
	const unsigned int eStartUp =  0x6161; //aa†
	const unsigned int eEndUp =  0x7A7B;  //zz + 1
	const unsigned int eMaxGrmCount  = eEndUp -eStartUp; // // 5911  (5  б) 

	const BYTE ePartOfSpeechesCount = 17;
	// не должно быть пробелов между словами 
	const char ePartOfSpeeches[ePartOfSpeechesCount][20] = 
	{
		"NOUN",
		"ADJECTIVE", // полное прилагательное
		"VERB",
		"VBE",
		"MOD",
		"NUMERAL",
		"CONJ",
		"INT",
		"PREP",
		"PART",
		"ARTICLE",
		"ADVERB",
		"PN",
		"ORDNUM",
		"PRON",
		"POSS",
		"PN_ADJ"
	};
	const short eGrammemsCount = 34;
    const char eGrammems[eGrammemsCount][8] = {
      "sg", "pl", "m", "f", "anim", "perf", "nom", "obj", "narr","geo", 
      "prop" ,"pers", "poss", "pred", "uncount", "ref", "dem", "mass", "comp", "sup", 
      "1", "2", "3", "prsa", "inf", "pasa", "pp", "ing", "fut", "if", "plsq", "plsgs", "name","org"};





class CEngGramTab : public CAgramtab
{
public:
	CAgramtabLine*  Lines[eMaxGrmCount];
	CEngGramTab();
	~CEngGramTab();
	
	BYTE GetPartOfSpeechesCount () const {return ePartOfSpeechesCount;};
	const char*   GetPartOfSpeechStr(BYTE i) const {return ePartOfSpeeches[i];};
	size_t GetGrammemsCount()  const {return eGrammemsCount;};
	const char*   GetGrammemStr(size_t i) const {return eGrammems[i];};
	size_t GetMaxGrmCount() const {return eMaxGrmCount;};
	CAgramtabLine*& GetLine(size_t LineNo) {return Lines[LineNo];}
	const CAgramtabLine* GetLine(size_t LineNo) const {return Lines[LineNo];};
	size_t s2i(const char * s ) const { return  (unsigned char) s[0]*0x100+(unsigned char) s[1] - eStartUp;};


	string i2s(WORD i)  const
	{ 
		i += eStartUp;
		char res[3];

		res[0] = (i >> 8);
		res[1] = (0xFF & i);
		res[2] = 0;
		return  res;
	};

	const char* GetRegistryString() const 
	{
		return "Software\\Dialing\\Lemmatizer\\English\\Egramtab";
	};

	long GetClauseTypeByName(const char* TypeName) const {assert(false); return 0;};
	const char* GetClauseNameByType(long type) const {assert(false); return 0;};
	bool GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const;
	bool GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const;
	const size_t GetClauseTypesCount() const 
	{
		return 0;
	};
	
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
	bool is_morph_personal_pronoun (size_t poses, QWORD grammems) const;
	bool is_morph_article(size_t poses) const;

	bool IsSimpleParticle(const char* lemma, size_t poses) const;
	bool IsSynNoun(size_t Poses, const char* Lemma) const;
	bool IsStandardParamAbbr (const char* WordStrUpper) const;
	bool GleicheCase(const char* gram_code_noun, const char* gram_code_adj) const;
	bool GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const;
	QWORD GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const;
	


};


#endif
