#ifndef GrammarItem_h
#define GrammarItem_h


#include "../common/utilit.h"
#include "list"


enum MorphSearchStatus {FoundInDictionary=1, NotFoundInDictionary=2, AnyStatus=3};

struct CMorphPattern
{
	MorphSearchStatus	m_SearchStatus;				
	QWORD				m_Grammems;
	size_t				m_Poses;
	string				m_GrmAttribute;

	bool				Init(const CAgramtab* GramTab, string& ErrorMsg);
	void				CopyFrom(const CMorphPattern& P);
	void				SetNull();
	string				ToString() const;
	bool				FromString(const string& line);
	bool				operator < (const CMorphPattern& _X1) const;
	bool				operator ==(const CMorphPattern& _X1) const;

};

struct CTokenListFile 
{
	size_t	m_SavedModifTime;
	StringSet			m_PossibleLemmas;
};


const char ConcatenationMark = ',';
const char OptionalMark = '?';
const char DisjunctionMark = '|';

struct CGrammarItem 
{
	bool				m_bMeta;
	MainTokenTypeEnum   m_TokenType;
	string				m_ItemStrId;
	string				m_Token;
	CMorphPattern		m_MorphPattern;
	
	map<string, string> m_Attributes;
	string				m_Source; 

	//  an index in CWorkGrammar m_TokenListFiles
	//  by deafult is -1
	CTokenListFile*		m_pListFile;


	//  m_bGrammarRoot  is valid only in Work grammars
	bool				m_bGrammarRoot;

	//	m_bSynMain is true if this item should be the main in syntax structure
	bool				m_bSynMain;

	// attribute hom="yes"/"no" should have a special processing,  and it should not 
	//  be a distinguishing feature of a grammar symbol,  that's why it should be excluded from 
	// attributes and mirrored to m_bCanHaveManyHomonyms
	bool				m_bCanHaveManyHomonyms;

	//  register of the first alpha of the input word
	RegisterEnum		m_Register;


	CGrammarItem();
	void InitGrammarItem();
	bool operator <(const CGrammarItem& _X1) const;
	bool operator ==(const CGrammarItem& _X1) const;
	bool RuleItemPartialEqual (const CGrammarItem& _X1)  const;
	

	string	GetDumpString() const;
	bool	ReadFromSourceString(string Source, string& ErrorStr,  MorphLanguageEnum Language);


	bool	AddAttribute(string Name, string Value, MorphLanguageEnum Language, string& ErrorStr);
	void	CopyNonEmptyWorkAttributesFrom(const CGrammarItem& Item);
	string	toString() const;
	bool	fromString(string& Result);
	bool	HasAnyOfWorkingAttributes() const;
	string	GetFullFileName(const string& GrammarFileName) const;
	
};


#endif
