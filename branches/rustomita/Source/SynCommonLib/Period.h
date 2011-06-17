// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#ifndef period_h
 #define period_h


struct CPeriod 
{
	int m_iFirstWord;
	int m_iLastWord;

	CPeriod ()
	{
	  m_iFirstWord = 0;
	  m_iLastWord = 0;
	};

	void SetPeriod(const CPeriod& X)
	{
	   m_iFirstWord = X.m_iFirstWord;
	   m_iLastWord  = X.m_iLastWord;
	};

	CPeriod (const CPeriod& X) 
	{  
		SetPeriod (X);  
	};

   
	CPeriod (int WordNo) 
	{  
		m_iFirstWord = WordNo;
        m_iLastWord  = WordNo;
	};

	CPeriod (int _first, int _last) 
	{  
		m_iFirstWord = _first;
        m_iLastWord  = _last;  
	};


	bool operator==(const CPeriod& X) const
	{  
	   return (m_iFirstWord == X.m_iFirstWord) && (m_iLastWord == X.m_iLastWord); 
	};

	size_t size()  const 
	{ 
		return m_iLastWord - m_iFirstWord + 1;
	};

	bool is_part_of (const CPeriod& X) const 
	{ 
		return	    (m_iFirstWord >= X.m_iFirstWord) 
				&&	(m_iLastWord <= X.m_iLastWord); 
	};

	bool is_inside_of (const CPeriod& X) const 
	{ 
		return	    is_part_of(X)
				&&	!(*this == X); 
	};


	bool has_intersection_right (const CPeriod& X) const 
	{ 
		return	    (m_iFirstWord < X.m_iFirstWord) 
				&&	(X.m_iFirstWord <= m_iLastWord)
				&&	(m_iLastWord < X.m_iLastWord)
				; 
	};
	bool has_intersection (const CPeriod& X) const 
	{ 
		return	    has_intersection_right(X) 
			    || X.has_intersection_right(*this);
	};

	bool is_on_right (const CPeriod& X) const 
	{ 
	   return  m_iLastWord < X.m_iFirstWord;
	};

};


inline bool GladkijPeriodLess (const CPeriod& _X1, const CPeriod& _X2) 
{  
	   return _X1.is_part_of(_X2) || _X1.is_on_right(_X2); 
};


inline bool StandardPeriodLess (const CPeriod& _X1, const CPeriod& _X2) 
{  
	if (_X1.m_iFirstWord != _X2.m_iFirstWord)
		return _X1.m_iFirstWord < _X2.m_iFirstWord;

	return _X1.m_iLastWord < _X2.m_iLastWord; 
};


#endif
