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

	BYTE			GetPartOfSpeechesCount() const;
	const char* GetPartOfSpeechStr(BYTE i) const;
	size_t			GetGrammemsCount()  const;
	const char* GetGrammemStr(size_t i) const;
	size_t			GetMaxGrmCount() const;
	CAgramtabLine*& GetLine(size_t LineNo);
	const CAgramtabLine* GetLine(size_t LineNo) const;
	size_t		s2i(const char* s)  const;
	std::string		i2s(WORD i) const;

	bool		ProcessPOSAndGrammems(const char* tab_str, BYTE& PartOfSpeech, QWORD& grammems) const;
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
	bool is_month(const char* lemma) const;
	bool is_small_number(const char* lemma) const;
	bool IsMorphNoun(poses_mask_t poses)  const;
	bool is_morph_adj(poses_mask_t poses) const;
	bool is_morph_participle(poses_mask_t poses) const;
	bool is_morph_pronoun(poses_mask_t poses) const;
	bool is_morph_pronoun_adjective(poses_mask_t poses) const;
	bool is_left_noun_modifier(poses_mask_t poses, QWORD grammems) const;
	bool is_numeral(poses_mask_t poses) const;
	bool is_verb_form(poses_mask_t poses) const;
	bool is_infinitive(poses_mask_t poses) const;
	bool is_morph_predk(poses_mask_t poses) const;
	bool is_morph_adv(poses_mask_t poses) const;
	bool is_morph_article(poses_mask_t poses) const;
	bool is_morph_personal_pronoun(poses_mask_t poses, QWORD grammems) const;
	bool IsSimpleParticle(const char* lemma, poses_mask_t poses) const;
	bool IsSynNoun(poses_mask_t poses, const char* Lemma) const;
	bool IsStandardParamAbbr(const char* WordStrUpper) const;
	bool FilterNounNumeral(std::string& gcNoun, const std::string& gcNum, QWORD& grammems) const override;
	QWORD ChangeGleicheAncode1(GrammemCompare CompareFunc, const std::string& wordGramCodes, std::string& groupGramCodes, const QWORD wordGrammems) const override;
};

extern bool GenderNumberCaseRussian(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool FiniteFormCoordRussian(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool CaseNumber(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool CaseGender(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool CaseNumberGender(const CAgramtabLine* l1, const CAgramtabLine* l2);
extern bool CaseNumberGender0(const CAgramtabLine* l1, const CAgramtabLine* l2); //with absent grammems check
extern bool GenderNumber0(const CAgramtabLine* l1, const CAgramtabLine* l2); //with absent grammems check
extern bool CaseNumber0(const CAgramtabLine* l1, const CAgramtabLine* l2); //with absent grammems check

