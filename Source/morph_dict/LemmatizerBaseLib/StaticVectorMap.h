// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef __index_vector_h_
#define __index_vector_h_

/*
	CStaticVectorMap is a class for storing and retrieving a std::vector of T by a key.
	The key is of uint32_t and there should be no holes between two neighbour keys. i.e.  
	for each i key[i]+1 = key[i+1].  
*/
template	<class T>
class CStaticVectorMap 
{
	// the keys, each item points to the beginning of the std::vector. For example,  
	// "m_Keys[i] = j" means that the std::vector No i starts from j position.
	// One "CStaticVectorMap" stores  m_Keys.size() vectors.
	std::vector<	uint32_t > m_Keys;

	// the base of the index 
	std::vector< T >		m_Base;
	
public:
	typedef typename std::vector<T>::const_iterator const_iter_t;
	
	
	const_iter_t	GetVectorEnd (uint32_t VectorNo) const
	{
		return (	VectorNo+1 == m_Keys.size() )
				?		m_Base.end()	
					:	m_Base.begin() + m_Keys[VectorNo + 1];
	}
	const_iter_t	GetVectorBegin (uint32_t VectorNo) const
	{
		assert(VectorNo < m_Keys.size());
		return  m_Base.begin() + m_Keys[VectorNo];
	}
	uint32_t	GetVectorLength (uint32_t VectorNo) const
	{
		const_iter_t begin = GetVectorBegin(VectorNo);
		const_iter_t end = GetVectorEnd(VectorNo);
		return  end - begin;
	}

	uint32_t	size() const 
	{
		return m_Keys.size();
	}

	bool	empty() const 
	{
		return m_Keys.empty();
	}

	void Create(std::vector< std::vector<T> >& src)
	{
		m_Keys.resize( src.size() );
		m_Base.clear();
		for (uint32_t i = 0; i < src.size(); i++)
		{
			m_Keys[i] = m_Base.size();
			m_Base.insert(m_Base.end(), src[i].begin(), src[i].end());
		};
	}


	bool LoadVectorMap(std::string filename)
	{
		FILE* fp;
		try
		{
			fp = fopen(filename.c_str(), "rb");
			if (!fp)
				return false;

			uint32_t Length;
			fread ((void *)&Length, 1, sizeof(uint32_t), fp);
			assert(Length > 0);
			m_Keys.clear();
			ReadVectorInner(fp, m_Keys, Length);
			
			
			fread ((void *)&Length, 1, sizeof(uint32_t), fp);
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

	bool SaveVectorMap(std::string filename)
	{
		try
		{
			FILE* fp = fopen(filename.c_str(), "wb");
			if (!fp)
				return(false);

			uint32_t nCount = m_Keys.size();
			fwrite((void *)&nCount, sizeof(uint32_t), 1, fp);
			WriteVectorInner(fp, m_Keys);

			nCount = m_Base.size();
			fwrite ((void *)&nCount, sizeof(uint32_t), 1, fp);
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

	bool GetKeyByBaseNo(uint32_t BaseNo, uint32_t& KeyNo, uint32_t& DataNo) const
	{
		KeyNo = 0;
		if (BaseNo != 0) 
		{
			std::vector<uint32_t>::const_iterator it = lower_bound(m_Keys.begin(), m_Keys.end(), BaseNo);
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
