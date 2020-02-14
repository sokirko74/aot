// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)


#ifndef __DOMEN_H_
#define __DOMEN_H_

#include "../common/utilit.h"

#include "StructDictConsts.h"



class TItemContainer;
const int DomStrSize = 100;

struct CDomen  {
  int  DomId;
  char DomStr[DomStrSize];
  char Format[255];
  char Source;
  bool IsDelim;
  bool IsFree;
  WORD ItemsCount;
  BYTE Parts[MaxDomensUnionSize];
  BYTE PartsSize;
  int   DropDownCount;
  DWORD Color;
  char* m_Items;
  int    m_ItemsLength;
  TItemContainer*  m_pParent;
  int 	  m_StartDomItem;
  int 	  m_EndDomItem;
  BYTE    m_DomNo;
  bool    m_bFreed;


  CDomen ();
  ~CDomen ();
  int AddItem (const char* s, int Length);
  void DelItem (int Offset, int Length);
  bool IsEmpty () const;
};

#endif //__DOMEN_H_
