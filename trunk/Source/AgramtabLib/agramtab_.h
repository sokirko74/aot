// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef agramtab_h
 #define agramtab_h


#include "../common/utilit.h"



struct CAgramtabLine 
{
	 CAgramtabLine (size_t SourceLineNo);
	 BYTE			m_PartOfSpeech;
	 QWORD			m_Grammems;
	 size_t			m_SourceLineNo;
};



typedef bool(*GrammemCompare)(const CAgramtabLine* l1, const CAgramtabLine* l2);

class CAgramtab {
	bool Read (const char *FileName);
	bool ReadAndCheck (const char * FileName);

   public :
	bool	m_bInited;

	CAgramtab ();
    virtual ~CAgramtab ();


	virtual CAgramtabLine*& GetLine(size_t LineNo) = 0;
	virtual const CAgramtabLine* GetLine(size_t LineNo)  const = 0;
	virtual size_t GetMaxGrmCount()  const = 0;
	virtual BYTE GetPartOfSpeechesCount()  const = 0;
	virtual const char*   GetPartOfSpeechStr(BYTE i) const = 0;
	virtual size_t GetGrammemsCount()  const = 0;
	virtual const char*   GetGrammemStr(size_t i) const = 0;
	virtual size_t s2i(const char * s ) const = 0;
	virtual string i2s(WORD i) const = 0;
	virtual const char* GetRegistryString() const = 0;
	virtual long GetClauseTypeByName(const char* TypeName) const = 0;
	virtual const char* GetClauseNameByType(long type) const  = 0;
	virtual bool GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const = 0;
	virtual bool GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const = 0;
	virtual const size_t GetClauseTypesCount() const = 0;
	virtual bool  ProcessPOSAndGrammems (const char* tab_str, BYTE& PartOfSpeech,  QWORD& grammems) const;

	/*
		IsStrongClauseRoot is a function which returns true iff part of speech this can occur only
		once in a clause. Usually the word which has this part of sppech is a root of a clause. Not all
		roots are "strong". For example,  infinitives can occur many times in a clause, each infinitive will
		a variant of the clause root, but these infinitive roots are not mutually exclusive, hence they are not
		strong.
	*/
	virtual bool IsStrongClauseRoot(const poses_mask_t poses) const = 0;
	virtual bool is_month (const char* lemma) const = 0;
	virtual bool is_small_number (const char* lemma) const = 0;
	virtual bool IsMorphNoun (poses_mask_t poses)  const = 0;
	virtual bool is_morph_adj (poses_mask_t poses) const = 0;
	virtual bool is_morph_participle (poses_mask_t poses) const = 0;
	virtual bool is_morph_pronoun (poses_mask_t poses) const = 0;
	virtual bool is_morph_pronoun_adjective(poses_mask_t poses) const = 0;
	virtual bool is_left_noun_modifier  (poses_mask_t poses, QWORD grammems) const = 0;
	virtual bool is_numeral (poses_mask_t poses) const = 0;
	virtual bool is_verb_form (poses_mask_t poses) const  = 0;
	virtual bool is_infinitive(poses_mask_t poses) const  = 0;
	virtual bool is_morph_predk(poses_mask_t poses) const  = 0;
	virtual bool is_morph_adv(poses_mask_t poses) const  = 0;
	virtual bool is_morph_personal_pronoun (poses_mask_t poses, QWORD grammems) const = 0;
	virtual bool is_morph_article(poses_mask_t poses)  const = 0;

	virtual bool IsSimpleParticle(const char* lemma, poses_mask_t poses) const  = 0;
	virtual bool IsSynNoun(poses_mask_t poses, const char* Lemma) const  = 0;
	virtual bool IsStandardParamAbbr (const char* WordStrUpper) const = 0;

	virtual bool GleicheCase(const char* gram_code_noun, const char* gram_code_adj) const = 0;
	virtual bool GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const  = 0;
	virtual QWORD GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const  = 0;
	
	bool	LoadFromRegistry ();
	bool	LoadFromRegistryAndCheck ();
	int		AreEqualPartOfSpeech (char *grm1, char* grm2) ;
	bool	ProcessPOSAndGrammemsIfCan (const char* tab_str, BYTE* PartOfSpeech,  QWORD* grammems) const;
	char*	grammems_to_str (QWORD grammems, char* out_buf) const;
	bool	FindGrammems (const char* gram_codes, QWORD grammems) const;
	bool	GetGramCodeByGrammemsAndPartofSpeechIfCan(BYTE Pos, QWORD grammems, string& gramcodes)  const;
	bool	GetPartOfSpeechAndGrammems(const BYTE* AnCodes, DWORD& Poses, QWORD& Grammems) const;
	string  GrammemsToStr(QWORD grammems) const;
	string  GetTabStringByGramCode(const char* gram_code) const;
	BYTE	GetPartOfSpeech(const char* gram_code) const;
	
	QWORD	GetAllGrammems(const char *gram_code) const;
	size_t	GetSourceLineNo(const char* gram_code) const;
	QWORD	GetAllGrammemsThatContains(const char *gram_code) const;
	bool	GetGrammems(const char* gram_code, QWORD& grammems)  const;
	BYTE	GetFirstPartOfSpeech(const poses_mask_t poses) const;
	
	string	GetAllPossibleAncodes(BYTE pos, QWORD grammems) const;
	string	GetGramCodes(BYTE pos, QWORD grammems, GrammemCompare CompareFunc)const;
    QWORD Gleiche (GrammemCompare CompareFunc, const char* gram_codes1, const char* gram_codes2) const;
	string GleicheAncode1 (GrammemCompare CompareFunc, const char* gram_codes1, const char* gram_codes2) const;
	string GleicheAncode1 (GrammemCompare CompareFunc, string gram_codes1, string gram_codes2) const;
	string GleicheAncode1 (GrammemCompare CompareFunc, string GramCodes1, string GramCodes2, string& GramCodes1pair) const;
	string UniqueGramCodes(string gram_codes) const;
	string FilterGramCodes(string gram_codes, QWORD grammems1, QWORD grammems2) const;
    string FilterGramCodes(QWORD breaks, string gram_codes, QWORD g1) const;
    bool CheckGramCode(const char* gram_code) const;

};



#endif
