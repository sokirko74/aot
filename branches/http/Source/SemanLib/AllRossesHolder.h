#ifndef AllRossHolder_H
#define AllRossHolder_H

#include "RossHolder.h"
#include "HierarchyHolder.h"




// абстрактный  класс.  Функция Initialize() pure methood. 
class CAllRossesHolder : public CTranslatorHolder 	
{ 
protected:
	CRossHolder            m_RossDoc;
	CRossHolder            m_LocRossDoc;
	CRossHolder            m_TimeRossDoc;
	CRossHolder            m_RusOborDoc;
	CRossHolder            m_RusCollocsDoc;
	CRossHolder            m_EngRossDoc;
	CRossHolder            m_EngCollocsRossDoc;
	CRossHolder            m_EngOborRossDoc;

	CRossHolder            m_FinThesRossDoc;
	CRossHolder            m_LocThesRossDoc;
	CRossHolder            m_OmniThesRossDoc;
	CRossHolder            m_CompThesRossDoc;
	bool					m_bDontLoadExamples;

 public:

	 CHierarchyHolder	     m_HierarchySemRelDoc;
	 CHierarchyHolder	     m_HierarchySemFetDoc;

	 CAllRossesHolder();
	 
 	 CRossHolder*		GetRossHolder (DictTypeEnum Type);
  	 const CRossHolder*	GetRossHolder (DictTypeEnum Type)  const;
  	 DictTypeEnum		GetTypeByRossHolder (const CRossHolder* pHolder)  const;
	 DictTypeEnum		GetTypeByStr (string DictName);
	 CDictionary*		GetRoss (DictTypeEnum Type);
	 string				GetRossPath(DictTypeEnum RossId)  const;
	 DictTypeEnum		GetRegisteredRossId(string FileName)  const;

	 bool				InitHierarchies();
	 virtual void		Initialize();
	 
	 

};



#endif
