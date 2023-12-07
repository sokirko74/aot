#pragma once

#include "common/gra_descr.h"
#include "morph_dict/common/utilit.h"
#include "morph_dict/lemmatizer_base_lib/AncodePattern.h"
#include "list"


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
	bool		  m_bMeta;
	Descriptors   m_TokenType;
	std::string	  m_ItemStrId;
	std::string				m_Token;
	CAncodePattern		m_MorphPattern;
	
	std::map<std::string, std::string> m_Attributes;
	std::string				m_Source; 

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
	

	std::string	GetDumpString() const;
	bool	ReadFromSourceString(std::string Source, std::string& ErrorStr,  MorphLanguageEnum Language);


	void	AddAttribute(std::string Name, std::string Value, MorphLanguageEnum Language, std::string& ErrorStr, const std::string& SourceFileName);
	void	CopyNonEmptyWorkAttributesFrom(const CGrammarItem& Item);
	std::string	toString() const;
	bool	fromString(std::string& Result);
	bool	HasAnyOfWorkingAttributes() const;
};
