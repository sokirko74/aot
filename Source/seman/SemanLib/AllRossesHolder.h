#ifndef AllRossHolder_H
#define AllRossHolder_H

#include "struct_dict_holder.h"
#include "HierarchyHolder.h"




// абстрактный  класс.  Функция Initialize() pure methood. 
class CAllRossesHolder : public CTranslatorHolder 	
{ 
protected:
	CStructDictHolder            m_RossDoc;
	CStructDictHolder            m_LocRossDoc;
	CStructDictHolder            m_TimeRossDoc;
	CStructDictHolder            m_RusOborDoc;
	CStructDictHolder            m_RusCollocsDoc;
	CStructDictHolder            m_EngRossDoc;
	CStructDictHolder            m_EngCollocsRossDoc;
	CStructDictHolder            m_EngOborRossDoc;

	CStructDictHolder            m_FinThesRossDoc;
	CStructDictHolder            m_LocThesRossDoc;
	CStructDictHolder            m_OmniThesRossDoc;
	CStructDictHolder            m_CompThesRossDoc;
	bool					m_bDontLoadExamples;

 public:

	 CHierarchyHolder	     m_HierarchySemRelDoc;
	 CHierarchyHolder	     m_HierarchySemFetDoc;

	 CAllRossesHolder();
	 
 	 CStructDictHolder*		GetRossHolder (DictTypeEnum Type);
  	 const CStructDictHolder*	GetRossHolder (DictTypeEnum Type)  const;
  	 DictTypeEnum		GetTypeByRossHolder (const CStructDictHolder* pHolder)  const;
	 DictTypeEnum		GetTypeByStr (std::string DictName);
	 CDictionary*		GetRoss (DictTypeEnum Type);
	 std::string		GetRossPath(DictTypeEnum RossId)  const;
	 DictTypeEnum		GetRegisteredRossId(std::string FileName)  const;

	 bool				InitHierarchies();
	 virtual void		Initialize();
	 
	 

};



#endif
