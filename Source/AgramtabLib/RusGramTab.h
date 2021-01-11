	// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#pragma once

#include "agramtab_.h"


class CRusGramTab : public CAgramtab {
	const static size_t  rStartUp = 0xC0E0; //Аа 
	const static size_t  rEndUp = 0x10000;  //яя
	const static size_t rMaxGrmCount = rEndUp - rStartUp; // // 16159  (16 Кб) 
public:
	CAgramtabLine* Lines[rMaxGrmCount];
	CRusGramTab();
	~CRusGramTab();

	part_of_speech_t			GetPartOfSpeechesCount() const;
	const char* GetPartOfSpeechStr(part_of_speech_t i) const;
	size_t			GetGrammemsCount()  const;
	const char* GetGrammemStr(size_t i) const;
	size_t			GetMaxGrmCount() const;
	CAgramtabLine*& GetLine(size_t LineNo);
	const CAgramtabLine* GetLine(size_t LineNo) const;
	size_t		GramcodeToLineIndex(const char* s)  const;
	std::string		LineIndexToGramcode(uint16_t i) const;

	bool		ProcessPOSAndGrammems(const char* tab_str, part_of_speech_t& PartOfSpeech, grammems_mask_t& grammems) const;
	const char* GetRegistryString() const;

	bool GleicheCase(const char* gram_code_noun, const char* gram_code_adj) const;
	bool GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const;
	grammems_mask_t GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const;

	bool GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const;
	bool ConflictGenderNumber(const char* gram_code1, const char* gram_code2) const;//with absent grammems check, less strict than GleicheGenderNumber
	bool ConflictGrammems(grammems_mask_t g1, grammems_mask_t g2, grammems_mask_t breaks) const;//with absent grammems check, less strict than GleicheGenderNumber
	bool GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const;
	long GetClauseTypeByName(const char* TypeName) const;

	const char* GetClauseNameByType(long type) const;
	const size_t GetClauseTypesCount() const;

	bool IsStrongClauseRoot(const part_of_speech_mask_t poses) const;
	bool is_month(const char* lemma) const;
	bool is_small_number(const char* lemma) const;
	bool IsMorphNoun(part_of_speech_mask_t poses)  const;
	bool is_morph_adj(part_of_speech_mask_t poses) const;
	bool is_morph_participle(part_of_speech_mask_t poses) const;
	bool is_morph_pronoun(part_of_speech_mask_t poses) const;
	bool is_morph_pronoun_adjective(part_of_speech_mask_t poses) const;
	bool is_left_noun_modifier(part_of_speech_mask_t poses, grammems_mask_t grammems) const;
	bool is_numeral(part_of_speech_mask_t poses) const;
	bool is_verb_form(part_of_speech_mask_t poses) const;
	bool is_infinitive(part_of_speech_mask_t poses) const;
	bool is_morph_predk(part_of_speech_mask_t poses) const;
	bool is_morph_adv(part_of_speech_mask_t poses) const;
	bool is_morph_article(part_of_speech_mask_t poses) const;
	bool is_morph_personal_pronoun(part_of_speech_mask_t poses, grammems_mask_t grammems) const;
	bool IsSimpleParticle(const char* lemma, part_of_speech_mask_t poses) const;
	bool IsSynNoun(part_of_speech_mask_t poses, const char* Lemma) const;
	bool IsStandardParamAbbr(const char* WordStrUpper) const;
	bool FilterNounNumeral(std::string& gcNoun, const std::string& gcNum, grammems_mask_t& grammems) const override;
	grammems_mask_t ChangeGleicheAncode1(GrammemCompare CompareFunc, const std::string& wordGramCodes, std::string& groupGramCodes, const grammems_mask_t wordGrammems) const override;
};

extern bool GenderNumberCaseRussian(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool FiniteFormCoordRussian(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool CaseNumber(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool CaseGender(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool CaseNumberGender(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool CaseNumberGender0(const CAgramtabLine* l1, const CAgramtabLine* l2); //with absent grammems check
extern bool GenderNumber0(const CAgramtabLine* l1, const CAgramtabLine* l2); //with absent grammems check
extern bool CaseNumber0(const CAgramtabLine* l1, const CAgramtabLine* l2); //with absent grammems check

