#pragma once 

#include "Homonym.h"
#include "graphan/GraphanLib/graline.h"
#include "synan/SimpleGrammarLib/GrammarItem.h"
#include "synan/SimpleGrammarLib/InputSymbol.h"

class CLemmatizer;

class CLemWord  
{

	// graphematical descriptors in one std::string (without some binary flags that could be restored by CLemWord::BuildGraphemDescr() )
    uint64_t   m_GraDescrs;
	std::vector<CHomonym> m_MorphHomonyms;
	MorphLanguageEnum m_Language;
	
	void		ProcessGraphematicalDescriptors();

	bool IsEqualToGrammarItem(const CHomonym& h, const CGrammarItem& I) const;

public:

	Descriptors   m_TokenType;
	
	std::string  m_UnparsedGraphemDescriptorsStr;

	// input word form
	std::string m_strWord;

	// input word uppercase form 
	std::string m_strUpperWord;

	size_t m_LettersCount;

	// is graphematical space 
	bool m_bSpace;

	// graphematical descriptor RLE or LLE
	bool m_bWord; 

	// a single comma
	bool m_bComma;

	// a single hyphen
	bool m_bDash;

	// graphematical register 
	RegisterEnum m_Register;

	//  offset in the graphematcil buffer (= in the input file for text files)
	int	 m_GraphematicalUnitOffset;

	// true, if this word has a space before ot it is at the beginning of the sentence.
	bool	m_bHasSpaceBefore;

	//  true if the word was deleted and should be ignored 
	bool	m_bDeleted;

	bool	m_bFirstUpperAlpha;
	bool    m_bQuoteMark;


	std::set<CInputSymbol> m_AutomatSymbolInterpetationUnion;


	CLemWord(MorphLanguageEnum l);
	void CreateFromToken(const CGraLine& token);
	void DeleteOborotMarks();
	bool HasDes(Descriptors g) const;
    void DelDes(Descriptors g);
    void AddDes(Descriptors g);
	
	void	Reset();
	
	
	
	void	SetWordStr (std::string NewValue);
	const std::string& GetWord() const;
	void  InitLevelSpecific(short oborot_no, CHomonym*);
	void CreateDefaultHomonym(short oborot_no = -1);


	bool	FindLemma(std::string strLemma) const;	
	int		GetHomonymByPOS(BYTE POS) const;
    bool    HasPos(BYTE POS) const;
    bool    HasGrammem(BYTE Grammem) const;
	int		GetHomonymByGrammem(BYTE grammem) const;
	int		GetHomonymByPOSandGrammem(BYTE POS, BYTE grammem) const;
    int     GetHomonymByPosesandGrammem(part_of_speech_mask_t Poses, BYTE grammem) const;
	bool	IsWordUpper(const char* s)	const  {return m_strUpperWord == s; };

	void	SetAllOtherHomsDel(int iHom);
	
	


    virtual size_t	GetHomonymsCount() const; 
	virtual const CHomonym* GetHomonym(int i) const;
	virtual CHomonym* GetHomonym(int i);
	virtual void EraseHomonym(int iHom);
    virtual CHomonym* AddNewHomonym();

	void DeleteMarkedHomonymsBeforeClauses();
	void SetHomonymsDel(bool Value);
	void DeleteAllHomonyms();
	void SafeDeleteMarkedHomonyms();
	bool LemmatizeFormUtf8(const std::string& s, const CLemmatizer* pLemmatizer);

	bool IsFirstOfGraPair(EGraPairType type) const;
	bool IsFirstOfGraPair() const;
	bool IsSecondOfGraPair(EGraPairType type) const;
	bool IsSecondOfGraPair() const;


	int		GetOborotNo() const;
	bool	HasOborot1() const;
	bool	HasOborot2() const;
	bool	IsInOborot() const;
	bool	CanBeSynNoun() const;
	void	KillHomonymOfPartOfSpeech(int iPartOfSpeech);
    std::string  GetDebugString(const CHomonym* pHomonym, bool bFirstHomonym)  const;
    std::string BuildGraphemDescr ()  const;
    part_of_speech_mask_t GetPoses() const;
    uint64_t   GetGrammems() const;
    bool    HasAnalyticalBeRus() const;

	void AddDescriptor(const std::string& some_string_label);
	void DeleteHomonymByNotTerminalSymbol(const CInputSymbol& s);
	void BuildTerminalSymbolsByWord(const std::vector<CGrammarItem>& grm_items, size_t end_of_stream_symbol);
	bool IsPredicted() const;
};
