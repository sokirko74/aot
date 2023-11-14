// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma once 

#include "morph_dict/common/utilit.h"
#include "Domen.h"
#include "Field.h"
#include "../../common/cortege.h"
#include "morph_dict/common/bserialize.h"


class TItemContainer  
{
public :
	MorphLanguageEnum	m_Language;
    std::string RossPath;
	std::string DomItemsFile;
	std::string DomItemsTextFile;
	std::vector <CDomen> m_Domens;
	std::string DomensFile;
	std::vector<CField> Fields;
	std::string FieldsFile;
	bool	m_bDontLoadExamples;
	

	//константы доменов
	BYTE ActantsDomNo;
	BYTE LexDomNo;
	BYTE LexPlusDomNo;
	BYTE TitleDomNo;
	BYTE CollocDomNo;
	BYTE AbbrDomNo;
	BYTE FieldDomNo;
	BYTE WildCardDomNo;
	BYTE IllDomNo;
	BYTE IntegerDomNo;
	int WildCardDomItemNo;


	TItemContainer();

	void 			ErrorMessage (std::string s ) const;
	bool			InitDomensConsts();
	dom_item_id_t	GetItemIdByItemStr (const std::string&  ItemStr, BYTE DomNo) const;
	dom_item_id_t	GetItemIdByItemStr (const std::string&  ItemStr, const char* DomStr) const;
	BYTE			GetDomenNoByDomStr (const char* DomStr) const;

	void			UpdateConstDomens();
	bool			BuildDomens (char* LastReadLine);
	bool			BuildDomItems ();
	BYTE			GetDomNoForLePlus ( const char*  s) const;
	bool			IsStandardRusLexeme (const char* s) const; 

	bool		CanBeRusAbbr (const char*  s) const;
	bool		CanBeRusColloc  (const char*  s) const;
	BYTE		GetFieldNoByFieldStrInner (const char* FieldStr) const; 
	bool		ClearFields ();
	void		BuildFields();
	bool		WriteFields() const;
	bool		WriteDomens () const;
	bool		WriteDomItems() const ;



};
