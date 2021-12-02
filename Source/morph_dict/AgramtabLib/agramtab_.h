// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#pragma once
#include "../common/utilit.h"


struct CAgramtabLine
{
    CAgramtabLine(size_t SourceLineNo);
    part_of_speech_t			m_PartOfSpeech;
    grammems_mask_t			m_Grammems;
    size_t			m_SourceLineNo;
};



typedef bool(*GrammemCompare)(const CAgramtabLine* l1, const CAgramtabLine* l2);

class CAgramtab {

public:
    const std::string GramtabFileName = "gramtab.tab";
    bool	m_bInited;
    MorphLanguageEnum  m_Language;
    bool m_bUseNationalConstants;

    CAgramtab();
    virtual ~CAgramtab();


    virtual CAgramtabLine*& GetLine(size_t LineNo) = 0;
    virtual const CAgramtabLine* GetLine(size_t LineNo)  const = 0;
    virtual size_t GetMaxGrmCount()  const = 0;
    virtual part_of_speech_t GetPartOfSpeechesCount()  const = 0;
    virtual const char* GetPartOfSpeechStr(part_of_speech_t i) const = 0;
    virtual size_t GetGrammemsCount()  const = 0;
    virtual const char* GetGrammemStr(size_t i) const = 0;
    virtual size_t GramcodeToLineIndex(const char* s) const = 0;
    virtual std::string LineIndexToGramcode(uint16_t i) const = 0;
    virtual const char* GetRegistryString() const = 0;
    virtual long GetClauseTypeByName(const char* TypeName) const = 0;
    virtual const char* GetClauseNameByType(long type) const = 0;
    virtual bool GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const = 0;
    virtual bool GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const = 0;
    virtual const size_t GetClauseTypesCount() const = 0;
    virtual bool  ProcessPOSAndGrammems(const char* tab_str, part_of_speech_t& PartOfSpeech, grammems_mask_t& grammems) const;

    /*
        IsStrongClauseRoot is a function which returns true iff part of speech this can occur only
        once in a clause. Usually the word which has this part of sppech is a root of a clause. Not all
        roots are "strong". For example,  infinitives can occur many times in a clause, each infinitive will
        a variant of the clause root, but these infinitive roots are not mutually exclusive, hence they are not
        strong.
    */
    virtual bool IsStrongClauseRoot(const part_of_speech_mask_t poses) const = 0;
    virtual bool is_month(const char* lemma) const = 0;
    virtual bool is_small_number(const char* lemma) const = 0;
    virtual bool IsMorphNoun(part_of_speech_mask_t poses)  const = 0;
    virtual bool is_morph_adj(part_of_speech_mask_t poses) const = 0;
    virtual bool is_morph_participle(part_of_speech_mask_t poses) const = 0;
    virtual bool is_morph_pronoun(part_of_speech_mask_t poses) const = 0;
    virtual bool is_morph_pronoun_adjective(part_of_speech_mask_t poses) const = 0;
    virtual bool is_left_noun_modifier(part_of_speech_mask_t poses, grammems_mask_t grammems) const = 0;
    virtual bool is_numeral(part_of_speech_mask_t poses) const = 0;
    virtual bool is_verb_form(part_of_speech_mask_t poses) const = 0;
    virtual bool is_infinitive(part_of_speech_mask_t poses) const = 0;
    virtual bool is_morph_predk(part_of_speech_mask_t poses) const = 0;
    virtual bool is_morph_adv(part_of_speech_mask_t poses) const = 0;
    virtual bool is_morph_personal_pronoun(part_of_speech_mask_t poses, grammems_mask_t grammems) const = 0;
    virtual bool is_morph_article(part_of_speech_mask_t poses)  const = 0;

    virtual bool IsSimpleParticle(const char* lemma, part_of_speech_mask_t poses) const = 0;
    virtual bool IsSynNoun(part_of_speech_mask_t poses, const char* Lemma) const = 0;
    virtual bool IsStandardParamAbbr(const char* WordStrUpper) const = 0;

    virtual bool GleicheCase(const char* gram_code_noun, const char* gram_code_adj) const = 0;
    virtual bool GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const = 0;
    virtual grammems_mask_t GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const = 0;

    void	Read(const char* FileName);
    void	LoadFromRegistry();
    int		AreEqualPartOfSpeech(const char* grm1, const char* grm2);
    bool	ProcessPOSAndGrammemsIfCan(const char* tab_str, part_of_speech_t* PartOfSpeech, grammems_mask_t* grammems) const;
    char* grammems_to_str(grammems_mask_t grammems, char* out_buf) const;
    bool	FindGrammems(const char* gram_codes, grammems_mask_t grammems) const;
    bool	GetGramCodeByGrammemsAndPartofSpeechIfCan(part_of_speech_t Pos, grammems_mask_t grammems, std::string& gramcodes)  const;
    bool	GetPartOfSpeechAndGrammems(const BYTE* AnCodes, part_of_speech_mask_t& Poses, grammems_mask_t& Grammems) const;
    std::string  GrammemsToStr(grammems_mask_t grammems) const;
    std::string  GetTabStringByGramCode(const char* gram_code) const;
    part_of_speech_t	GetPartOfSpeech(const char* gram_code) const;

    grammems_mask_t	GetAllGrammems(const char* gram_code) const;
    size_t	GetSourceLineNo(const char* gram_code) const;
    grammems_mask_t	GetAllGrammemsThatContains(const char* gram_code) const;
    bool	GetGrammems(const char* gram_code, grammems_mask_t& grammems)  const;
    part_of_speech_t	GetFirstPartOfSpeech(const part_of_speech_mask_t poses) const;

    std::string	GetAllPossibleAncodes(part_of_speech_t pos, grammems_mask_t grammems) const;
    std::string	GetGramCodes(part_of_speech_t pos, grammems_mask_t grammems, GrammemCompare CompareFunc)const;
    grammems_mask_t Gleiche(GrammemCompare CompareFunc, const char* gram_codes1, const char* gram_codes2) const;
    std::string GleicheAncode1(GrammemCompare CompareFunc, const char* gram_codes1, const char* gram_codes2) const;
    std::string GleicheAncode1(GrammemCompare CompareFunc, std::string gram_codes1, std::string gram_codes2) const;
    std::string GleicheAncode1(GrammemCompare CompareFunc, std::string GramCodes1, std::string GramCodes2, std::string& GramCodes1pair) const;
    std::string UniqueGramCodes(std::string gram_codes) const;
    std::string FilterGramCodes(const std::string& gram_codes, grammems_mask_t grammems1, grammems_mask_t grammems2) const;
    std::string FilterGramCodes(grammems_mask_t breaks, std::string gram_codes, grammems_mask_t g1) const;
    bool CheckGramCode(const char* gram_code) const;
    virtual bool FilterNounNumeral(std::string& gcNoun, const std::string& gcNum, grammems_mask_t& grammems) const { assert(false); return false; };
    virtual grammems_mask_t ChangeGleicheAncode1(GrammemCompare, const std::string&, std::string&, const grammems_mask_t) const { assert(false); return 0; };
};

