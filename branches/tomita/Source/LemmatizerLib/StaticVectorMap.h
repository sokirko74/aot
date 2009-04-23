// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef __index_vector_h_
#define __index_vector_h_

/*
	CStaticVectorMap is a class for storing and retrieving a vector of T by a key.
	The key is of DWORD and there should be no holes between two neighbour keys. i.e.  
	for each i key[i]+1 = key[i+1].  
*/
template	<class T>
class CStaticVectorMap 
{
	// the keys, each item points to the beginning of the vector. For example,  
	// "m_Keys[i] = j" means that the vector No i starts from j position.
	// One "CStaticVectorMap" stores  m_Keys.size() vectors.
	vector<	DWORD > m_Keys;

	// the base of the index 
	vector< T >		m_Base;
	
public:
	typedef typename vector<T>::const_iterator const_iter_t;
	
	
	const_iter_t	GetVectorEnd (DWORD VectorNo) const
	{
		return (	VectorNo+1 == m_Keys.size() )
				?		m_Base.end()	
					:	m_Base.begin() + m_Keys[VectorNo + 1];
	}
	const_iter_t	GetVectorBegin (DWORD VectorNo) const
	{
		assert(VectorNo < m_Keys.size());
		return  m_Base.begin() + m_Keys[VectorNo];
	}
	DWORD	GetVectorLength (DWORD VectorNo) const
	{
		const_iter_t begin = GetVectorBegin(VectorNo);
		const_iter_t end = GetVectorEnd(VectorNo);
		return  end - begin;
	}

	DWORD	size() const 
	{
		return m_Keys.size();
	}

	bool	empty() const 
	{
		return m_Keys.empty();
	}

	void Create(vector< vector<T> >& src)
	{
		m_Keys.resize( src.size() );
		m_Base.clear();
		for (DWORD i = 0; i < src.size(); i++)
		{
			m_Keys[i] = m_Base.size();
			m_Base.insert(m_Base.end(), src[i].begin(), src[i].end());
		};
	}


	bool LoadVectorMap(string filename)
	{
		FILE* fp;
		try
		{
			fp = fopen(filename.c_str(), "rb");
			if (!fp)
				return false;

			DWORD Length;
			fread ((void *)&Length, 1, sizeof(DWORD), fp);
			assert(Length > 0);
			m_Keys.clear();
			ReadVectorInner(fp, m_Keys, Length);
			
			
			fread ((void *)&Length, 1, sizeof(DWORD), fp);
			m_Base.clear();
			ReadVectorInner(fp, m_Base, Length);

			fclose (fp);
		}
		catch(...)
		{
			return false;
		}
		return true;
	}

	bool SaveVectorMap(string filename)
	{
		try
		{
			FILE* fp = fopen(filename.c_str(), "wb");
			if (!fp)
				return(false);

			DWORD nCount = m_Keys.size();
			fwrite((void *)&nCount, sizeof(DWORD), 1, fp);
			WriteVectorInner(fp, m_Keys);

			nCount = m_Base.size();
			fwrite ((void *)&nCount, sizeof(DWORD), 1, fp);
			WriteVectorInner(fp, m_Base);

			fclose(fp);
		}
		catch(...)
		{
			return false;
		}
		return true;
	}

	size_t GetBaseSize() const
	{
		return m_Base.size();
	};

	bool GetKeyByBaseNo(DWORD BaseNo, DWORD& KeyNo, DWORD& DataNo) const
	{
		KeyNo = 0;
		if (BaseNo != 0) 
		{
			vector<DWORD>::const_iterator it = lower_bound(m_Keys.begin(), m_Keys.end(), BaseNo);
			if (it == m_Keys.begin()) return false;
			if (it == m_Keys.end()) return false;
			if (BaseNo < *it) it--;
			KeyNo =  it - m_Keys.begin();
		};
		DataNo = BaseNo - m_Keys[KeyNo];
		return true;
	};
	


};


#endif
