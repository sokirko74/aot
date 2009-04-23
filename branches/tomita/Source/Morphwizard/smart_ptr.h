  // smart_ptr.h: interface for the smart_ptr class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SMART_PTR_H__
#define __SMART_PTR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef NULL
#define NULL 0
#endif

//----------------------------------------------------------------------------
// init_ptr class
//----------------------------------------------------------------------------
template<class T>
class init_ptr
{
public:
	/*explicit*/ init_ptr( T* pObj=NULL ) : m_pObj(pObj) {}

	bool		operator!() const		{  return m_pObj==NULL; }
// non-const access ----------------
	T&			operator*()				{ assert(m_pObj); return *m_pObj; }
	T* const	operator->()			{ assert(m_pObj); return m_pObj; }
	T* const	GetPtr()				{ assert(m_pObj); return m_pObj; }
// const access --------------------
	const T&	operator*()	const		{ assert(m_pObj); return *m_pObj; }
	const T* const	operator->() const	{ assert(m_pObj); return m_pObj; }
	const T* const	GetPtr() const		{ return m_pObj; }
	operator T*()						{ return m_pObj; }

	void		Delete()	{ if(m_pObj) { delete m_pObj; m_pObj = NULL; } }

//	template<class U>
//		init_ptr( const init_ptr<U>& ptr ) : m_pObj(const_cast<U*>(ptr.GetPtr())) {}
	//------------------------------
	init_ptr( const init_ptr& ptr )		: m_pObj(ptr.m_pObj) {}
	//------------------------------
/*
 	template<class U>
		init_ptr& operator=( const init_ptr<U>& ptr )
	{
		if( &ptr != this ) m_pObj = const_cast<U*>(ptr.GetPtr());
		return *this;
	}
*/
	//------------------------------
	init_ptr& operator=( const init_ptr& ptr )
	{
		if( &ptr != this ) m_pObj = ptr.m_pObj;
		return *this;
	}
	//------------------------------
	bool operator==( const init_ptr& p ) const { return m_pObj==p.m_pObj; }
	bool operator!=( const init_ptr& p ) const { return m_pObj!=p.m_pObj; }
protected:
	void Reinit( T* pObj=NULL )		{ m_pObj = pObj; }
private:
	T* m_pObj;
};

//----------------------------------------------------------------------------
// const Ptr
//----------------------------------------------------------------------------
template<class T>
class const_ptr
{
public:
	/*explicit*/ const_ptr( const T* pObj=NULL ) : m_pObj(pObj) {}

	bool			operator!() const	{  return m_pObj==NULL; }
// const access --------------------
	const T&		operator*()	const	{ assert(m_pObj); return *m_pObj; }
	const T* const	operator->() const	{ assert(m_pObj); return m_pObj; }
	const T* const	GetPtr() const		{ return m_pObj; }
	operator const	T*()	const		{ return m_pObj; }

//	template<class U>
//		const_ptr( const init_ptr<U>& ptr ) : m_pObj(ptr.GetPtr()) {}
	//------------------------------
	const_ptr( const const_ptr& ptr )		: m_pObj(ptr.m_pObj) {}
	//------------------------------
/*
 	template<class U>
		init_ptr& operator=( const init_ptr<U>& ptr )
	{
		if( &ptr != this ) m_pObj = const_cast<U*>(ptr.GetPtr());
		return *this;
	}
*/
	//------------------------------
	const_ptr& operator=( const const_ptr& ptr )
	{
		if( &ptr != this ) m_pObj = ptr.m_pObj;
		return *this;
	}
	//------------------------------
	bool operator==( const const_ptr& p ) const { return m_pObj==p.m_pObj; }
	bool operator!=( const const_ptr& p ) const { return m_pObj!=p.m_pObj; }
private:
	const T* m_pObj;
};

//----------------------------------------------------------------------------
// comp_ptr class (comparable pointer; for CSortedArray)
//----------------------------------------------------------------------------
template<class T>
class comp_ptr: public init_ptr<T>
{
public:
	/*explicit*/ comp_ptr( T* pObj=NULL ) : init_ptr<T>(pObj) {}

	bool operator<( const comp_ptr& p ) const { 
		return *GetPtr()<*p; }
	bool operator>( const comp_ptr& p ) const { 
		return *p<*GetPtr(); }
	//------------------------------
	bool operator==( const comp_ptr& p ) const
		{ return GetPtr()==p.GetPtr() || (!operator!() && !!p && *GetPtr()==*p); }
	//------------------------------
	bool operator!=( const comp_ptr& p ) const { return !operator==(p); }
//	bool operator==( const comp_ptr& p ) const
//	{ return operator!() && !p ||
//	(!operator!() && !!p && (GetPtr()==p.GetPtr() || *GetPtr()==*p)); }
};


//----------------------------------------------------------------------------
// master_ptr class (object equivalent)
//----------------------------------------------------------------------------
template<class T>
class master_ptr: public init_ptr<T>
{
public:
	/*explicit*/ master_ptr( T* pObj=NULL ) : init_ptr<T>(pObj) {}
	//------------------------------
	master_ptr( const master_ptr& ptr )
		: init_ptr<T>((!ptr)? NULL: new T(*ptr)) 				{}
	//------------------------------
	master_ptr& operator=( const master_ptr& ptr )
	{
		if( &ptr != this )
		{
			Delete();
			if( !!ptr ) Reinit( new T(*ptr) );
		}
		return *this;
	}
	//------------------------------
	master_ptr& operator=( T* pObj )
	{
		if( operator!() || pObj != GetPtr() )
		{
			Delete();
			Reinit(pObj);
		}
		return *this;
	}
	//------------------------------
	~master_ptr()			{ Delete(); }
	//	operator T*()			{ GetPtr(); }
};


#endif // !defined(__SMART_PTR_H__)
