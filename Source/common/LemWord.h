#ifndef lemword_h
 #define lemword_h

#include "Homonym.h"
#include "PlmLine.h"
#include "gra_descr.h"

class CLemWord  
{
	// graphematical descriptors in one string (without some binary flags that could be restored by CLemWord::BuildGraphemDescr() )
	

    QWORD   m_GraDescrs;
    int		ProcessGraphematicalDescriptors(const char* LineStr);
public:

	// ======= Graphematics ======================	
    string  m_UnparsedGraphemDescriptorsStr;

	// input word form
	string m_strWord;

	// input word uppercase form 
	string m_strUpperWord;

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

    int m_TokenLengthInFile;

	// true, if this word has a space before ot it is at the beginning of the sentence.
	bool	m_bHasSpaceBefore;

	//  true if the word was deleted and should be ignored 
	bool	m_bDeleted;

	// is morphologically predicted
	bool m_bPredicted;


	CLemWord(); 
	
	void DeleteOborotMarks();
	bool AddNextHomonym(const char* strPlmLine);
	bool ProcessPlmLineForTheFirstHomonym(const char* strPlmLine, MorphLanguageEnum langua, int& OborotNo);
	bool HasDes(Descriptors g) const;
    void DelDes(Descriptors g);
    void AddDes(Descriptors g);
	
	void	Reset();
	
	
	
	void	SetWordStr (string NewValue, MorphLanguageEnum langua);

	bool	FindLemma(string strLemma) const;	
	int		GetHomonymByPOS(BYTE POS) const;
    bool    HasPos(BYTE POS) const;
    bool    HasGrammem(BYTE Grammem) const;
	int		GetHomonymByGrammem(BYTE grammem) const;
	int		GetHomonymByPOSandGrammem(BYTE POS, BYTE grammem) const;
    int     GetHomonymByPosesandGrammem(size_t Poses, BYTE grammem) const;
	bool	IsWordUpper(const char* s)	const  {return m_strUpperWord == s; };

	void	SetAllOtherHomsDel(int iHom);
	
	


    virtual size_t	GetHomonymsCount() const	= 0; 
	virtual const CHomonym* GetHomonym(int i) const	 = 0;
	virtual CHomonym* GetHomonym(int i) = 0;
	virtual void EraseHomonym(int iHom) = 0;;
    virtual CHomonym* AddNewHomonym() = 0;;

	void DeleteMarkedHomonymsBeforeClauses();
	void SetHomonymsDel(bool Value);

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
    virtual void InitLevelSpecific(CHomonym* pHom) {};
    string  GetPlmStr (const CHomonym* pHomonym, bool bFirstHomonym)  const;
    string  GetDebugString(const CHomonym* pHomonym, bool bFirstHomonym)  const;
    string BuildGraphemDescr ()  const;
    size_t  GetPoses() const;
    QWORD   GetGrammems() const;
    bool    HasAnalyticalBe() const;

    QWORD GetGraDescr() const { return m_GraDescrs; }
};


#endif
