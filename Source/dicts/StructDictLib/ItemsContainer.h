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

protected:
	dom_item_id_t	InsertDomItem(const char* ItemStr, BYTE DomNo);

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

	void			InitDomensConsts();
	dom_item_id_t	GetItemIdByItemStr (const std::string&  ItemStr, BYTE DomNo) const;
	dom_item_id_t	GetItemIdByItemStr (const std::string&  ItemStr, const char* DomStr) const;
	BYTE			GetDomenNoByDomStr (const char* DomStr, bool throw_excp=false) const;

	void			UpdateConstDomens();
	void			BuildDomens (std::string path);
	void			BuildDomItems (std::string path);
	BYTE			GetDomNoForLePlus (const std::string& s) const;

	BYTE		GetFieldNoByFieldStrInner (const char* FieldStr) const; 
	bool		ClearFields ();
	void		BuildFields(std::string path);
	bool		WriteDomItems(std::string path) const ;



};
