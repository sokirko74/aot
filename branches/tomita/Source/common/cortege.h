// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)


#ifndef cortege_h
 #define cortege_h

#include <assert.h>
#include "../common/utilit.h"

const BYTE ErrUChar	= 254;

struct TCortege10;

const char _FieldFormat[] = "%-8s= %s";
const char _FieldFormatEqual[] = "%-8s== %s";


template <int MaxNumDom>
struct TBasicCortege {
public:
	BYTE m_FieldNo;      // индекс в массиве Ross :: Fields
	BYTE m_SignatNo;  // индекс в массиве Ross :: Fields[FunNo].m_Signats
	BYTE m_LevelId;
	BYTE m_LeafId;
	BYTE m_BracketLeafId;
	int  m_DomItemNos[MaxNumDom];  

	int GetItem  (size_t index)  const
	{
		assert (index<MaxNumDom);
		return m_DomItemNos[index];
	};
	int SetItem  (size_t index, long Value) 
	{
		assert (index<MaxNumDom);
		return m_DomItemNos[index] =  Value;
	};

	TBasicCortege()
	{
	  for (size_t i=0; i< MaxNumDom;i++)
		  SetItem(i,-1);
	  m_SignatNo = 0;
	  m_FieldNo = ErrUChar;
	  m_LeafId = 0;
	  m_BracketLeafId = 0;
	};
	BYTE GetSignatNo()  const
	{
	  return (m_SignatNo  & (~128)) ;
	};

	void SetSignatNo(BYTE SignatNo) 
	{
	  m_SignatNo = SignatNo | (128 & m_SignatNo);
	};

	bool IsEqual() const
	{
	   return (m_SignatNo  & 128) > 0 ;
	};

	void SetEqual()
	{
	   m_SignatNo  |=  128;
	};
	const  char* GetFieldFormat() const
	{
	  return IsEqual() ? _FieldFormatEqual : _FieldFormat;

	};

	bool HasEqualItems(const TBasicCortege&  X, BYTE _MaxNumDom)  const 
	{  
		for (BYTE i=0; i< _MaxNumDom;i++)
			if (GetItem(i) != X.GetItem(i))
				return false;
		return true;
	};

	bool EqualCortege(const TBasicCortege& X, BYTE _MaxNumDom)  const 
	{  
	  return    (m_FieldNo  == X.m_FieldNo)  
	             && (m_SignatNo == X.m_SignatNo)
				 && (m_LevelId  == X.m_LevelId)
				 && (m_LeafId   == X.m_LeafId)
				 && (m_BracketLeafId   == X.m_BracketLeafId)
				 && HasEqualItems (X, _MaxNumDom);
	};

	bool IsEqualWithWildCard(const TBasicCortege& X, WORD EmptyDomItem, BYTE _MaxNumDom) const
	{  
		if    (!(   (m_FieldNo  == X.m_FieldNo)  
			 && (      (m_LevelId == ErrUChar)
					|| (X.m_LevelId == ErrUChar)
					|| (m_LevelId == X.m_LevelId)
				)
			 && (      (m_LeafId == ErrUChar)
					|| (X.m_LeafId == ErrUChar)
					|| (m_LeafId == X.m_LeafId)
				)
			 && (      (m_BracketLeafId == ErrUChar)
					|| (X.m_BracketLeafId == ErrUChar)
					|| (m_BracketLeafId == X.m_BracketLeafId)
				)

			 )) return false;

		for (size_t i=0; i< _MaxNumDom;i++)
		   if (     (GetItem(i)  != X.GetItem(i))
				&&  (GetItem(i)  != EmptyDomItem)
				&&  (X.GetItem(i) != EmptyDomItem)
			  )
		   return false; 

		return true;
	};

	TBasicCortege<MaxNumDom>& operator = (const TBasicCortege<10>& X)
	{
		m_FieldNo = X.m_FieldNo;
		m_LeafId = X.m_LeafId;
		m_BracketLeafId = X.m_BracketLeafId;
		m_LevelId = X.m_LevelId;
		m_SignatNo = X.m_SignatNo;
		for (int i =0; i < MaxNumDom; i++)
			SetItem(i, X.GetItem(i));

		return *this;
	};

};


template <int MaxNumDom>
size_t get_size_in_bytes (const TBasicCortege<MaxNumDom>& t)
{

	return	get_size_in_bytes(t.m_FieldNo) 
			+ get_size_in_bytes(t.m_SignatNo) 
			+ get_size_in_bytes(t.m_LevelId) 
			+ get_size_in_bytes(t.m_LeafId) 
			+ get_size_in_bytes(t.m_BracketLeafId) 
			+ get_size_in_bytes(t.m_DomItemNos[0])*MaxNumDom; 
};

template <int MaxNumDom>
size_t save_to_bytes(const TBasicCortege<MaxNumDom>& i, BYTE* buf)
{
	buf += save_to_bytes(i.m_FieldNo, buf);
	buf += save_to_bytes(i.m_SignatNo, buf);
	buf += save_to_bytes(i.m_LevelId, buf);
	buf += save_to_bytes(i.m_LeafId, buf);
	buf += save_to_bytes(i.m_BracketLeafId, buf);
	for (int j = 0;  j < MaxNumDom; j++)
		buf += save_to_bytes(i.m_DomItemNos[j], buf);

	return get_size_in_bytes(i);
};

template <int MaxNumDom>
size_t restore_from_bytes(TBasicCortege<MaxNumDom>& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.m_FieldNo, buf);
	buf += restore_from_bytes(i.m_SignatNo, buf);
	buf += restore_from_bytes(i.m_LevelId, buf);
	buf += restore_from_bytes(i.m_LeafId, buf);
	buf += restore_from_bytes(i.m_BracketLeafId, buf);
	for (int j = 0;  j < MaxNumDom; j++)
		buf += restore_from_bytes(i.m_DomItemNos[j], buf);
	return get_size_in_bytes(i);
};




struct TCortege10 : public TBasicCortege<10> 
{
	TCortege10()
	{
		for (size_t i=0; i< 10;i++)
			SetItem(i,-1);
		m_SignatNo = 0;
		m_FieldNo = ErrUChar;
		m_LeafId = 0;
		m_BracketLeafId = 0;
	};

	TCortege10 (const TBasicCortege<3>& X)
	{
		m_FieldNo = X.m_FieldNo;
		m_LeafId = X.m_LeafId;
		m_BracketLeafId =  X.m_BracketLeafId;
		m_LevelId = X.m_LevelId;
		m_SignatNo = X.m_SignatNo;
		for (int i =0; i < 3; i++)
			SetItem(i, X.GetItem(i));
	};
	TCortege10 (const TBasicCortege<10>& X)
	{
		m_FieldNo = X.m_FieldNo;
		m_LeafId = X.m_LeafId;
		m_BracketLeafId = X.m_BracketLeafId;
		m_LevelId = X.m_LevelId;
		m_SignatNo = X.m_SignatNo;
		for (int i =0; i < 10; i++)
			SetItem(i, X.GetItem(i));
	};


};







#endif
