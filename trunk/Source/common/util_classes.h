// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef utilit_classes_h
 #define utilit_classes_h

#include "utilit.h"



template <class	Type, int Size>
struct CSmallVector	{
	Type	 m_Items[Size];
	int	 m_ItemsCount;
	CSmallVector () 
	{
		m_ItemsCount =	0;
	}
	void Add (Type	Item)
	{
		//assert(m_ItemsCount < Size-1);
		//	в Release просто не	будем добавлять	отношения, если	их слишком много
		if	(m_ItemsCount >= Size-1)
			return;
		m_Items[m_ItemsCount++] = Item;
	};
	void Insert (int No, Type Item)
	{
		//assert(m_RelsCount <	MaxRelSet-1);
		//	в Release просто не	будем добавлять	отношения, если	их слишком много
		if	(m_ItemsCount >= Size-1)
			return;

		for (long i = m_ItemsCount; i > No	; i--)
			m_Items[i]	= m_Items[i-1];
		m_Items[No] = Item;
		m_ItemsCount++;
	};
	void  Erase (int No)
	{
		//assert (No <	m_RelsCount);
		for (long i = No; i < m_ItemsCount-1 ;	i++)
			m_Items[i]	= m_Items[i+1];
		m_ItemsCount--;
	};
	void  Clear ()
	{
		m_ItemsCount =	0;
	};
	BYTE size () const	{
		return	m_ItemsCount; 
	};
	bool empty	() const {
		return	m_ItemsCount ==	0; 
	};

	Type& back	() {
		return	m_Items[m_ItemsCount - 1];
	};
	Type& operator[](int No) {	return m_Items[No];	};

	const Type& operator[](int	No)	const {	return m_Items[No];	};

	size_t find_item (const Type& X) const { return find(m_Items, m_Items+m_ItemsCount, X) - m_Items; };

	bool	has (const Type& X) const { return find_item(X) != m_ItemsCount; };


};


template <class T>
class _share_pointer_t
{
public:
	T		m_Pointer;	
	bool	m_bOwnPointer;
	
	_share_pointer_t()
	{
		m_bOwnPointer = true;
		m_Pointer = 0;
	};
	~_share_pointer_t()
	{
		FreePointer();
	};

	void SetPointer(T p, bool bOwnPointer)
	{
		if (m_bOwnPointer)
			if (m_Pointer)
				delete m_Pointer;

		m_Pointer = p;
		m_bOwnPointer = bOwnPointer;
	}
	void FreePointer()
	{
		SetPointer(0, true);
	}
};



class CShortString
{
	vector<char>::const_iterator m_pStringPointer;
public:
	CShortString(vector<char>::const_iterator pData);

	BYTE GetLength() const;
	vector<char>::const_iterator GetData() const;
	const char*	GetString() const;
	//bool  operator <(const CShortString& s) const;
};

class IsLessShortString : public std::binary_function<const CShortString&, const char*, bool>
{
public:
	bool operator()(const CShortString& Item1,	const char* Item2) const;
	bool operator()(const char* Item1,			const CShortString& Item2) const;
	bool operator()(const CShortString& Item1,	const CShortString& Item2) const;
};


class CShortStringHolder : public vector<CShortString> 
{
	vector<char> m_Buffer;
	template<class T>
		bool CreateFromSequence(T begin, T end);
public:	
	void	ReadShortStringHolder(string filename);
	bool	WriteShortStringHolder(const string& FileName) const;
	bool	CreateFromVector(const StringVector& in);
	bool	CreateFromSet(const StringSet& in);
};



struct CMyTimeSpan 
{
	clock_t     m_StartTime;
	double      m_TimeSpan;
	long		m_InterfaceNestId;
	long		m_SequenceId;
	long        m_InvokeCount;
	CMyTimeSpan (long SequenceId = 0, long InterfaceNestId = 0);
	void GetStrRepresentation(const char* Name, char* buffer, double AllClocksCount) const;
};


class CMyTimeSpanHolder 
{
	map<string, CMyTimeSpan>	m_TimeSpans;
	long						m_SequenceId;
	
public:
	bool						m_bTimeSpanHolderEnabled;

	typedef map<string, CMyTimeSpan>::const_iterator	ConstIterator;
	typedef map<string, CMyTimeSpan>::iterator			Iterator;

	CMyTimeSpanHolder();
	void StartTimer(const string& Name, long NestId);
	double EndTimer(const string& Name);
	string GetStrRepresentation (double AllClocksCount = 0) const;
	void ClearTimers();
};


/**
	The Reenterable replacement of strtok().


	Usage:		

	char *text = "word, word2;word3"
	StringTokenizer token(text, " ,;");
	const char *word;
	while((word = token())){
		// handle token here
	}


*/
class StringTokenizer{
	char *text_ptr;
	char *text;
	int i;
	string delims;
	char *_val;

	bool is_delim(char ch) const;
	void initialize(const char *_text, const char *_delims);
public:
	//-------------------------------------------------------
	StringTokenizer(const char *_text, const char *_delims);
	~StringTokenizer();
	const char * operator ()();
	string next_token ();

	const char * get_rest () const;
	const char *val() const;
	int  count() const;
	bool has_next() const;
	void reinitialize(const char *_text, const char *_delims);

};



#endif
