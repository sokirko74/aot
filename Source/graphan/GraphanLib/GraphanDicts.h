// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#ifndef __GRAPHMAT_DICTS_H
#define __GRAPHMAT_DICTS_H

#include "graline.h"       
#include "abbrev.h"
#include "morph_dict/common/util_classes.h"


const int MaxSpaceWordLen = 100;

const size_t MaxNameSize = 100;

struct CIdent {
	std::vector<CGraLine> m_tokens;
};

class CGraphanDicts {
	void _add_oborot(CGraphemOborot o);
public:

	MorphLanguageEnum m_Language;

	CGraphanDicts(MorphLanguageEnum Language);
	~CGraphanDicts();
	void FreeData();

	

	std::vector<std::string> m_SpacedWords;
	void   ReadSpacedWords (std::string path);
	const std::string& SearchSpacedWords(const char *In, BYTE& len) const;



	// Идентификатор - это что-то, что содержит в себе  знак препинания, но является одним словом
	// типа C++, TCP/IP.
	std::unordered_multimap<std::string, CIdent> m_Idents;


	// John, Bill
	std::unordered_set<std::string>	m_EnglishNames;
	void ReadENames (std::string path);
	

	mutable _share_pointer_t<CDictionary*>	m_pOborDictionary;
	const CDictionary* GetOborDic() const
	{
		return m_pOborDictionary.m_Pointer;
	};

	void  SetOborDic(CDictionary* Dic) 
	{
		m_pOborDictionary.SetPointer(Dic, false );
	};

	
	std::map<uint16_t, std::vector<uint16_t> >		m_OborottosFirstWordIndex;
	std::vector<CGraphemOborot>			m_Oborottos;
	StringVector					m_OborotTokens;
	

	
	
	void		BuildOborot (const std::string& s, int OborotNo,bool bFixedFet);
	void		BuildOborottos ();
    
	std::unordered_multimap<std::string, CAbbrev> m_Abbrevs;
	void		ReadAbbrevations();

    
    
	StringVector m_KeyModifiers;
	bool		IsRegisteredKeyModifier(const char* Str, size_t Len) const;

	StringVector m_Keys;
	bool		ReadKeyboard(std::string FileName);
	

	std::unordered_set<std::string> m_Extensions;
	bool IsFileExtension(const std::string& s) const;
	void ReadExtensions(std::string path);
	


};



#endif
