// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef bserialze_h
 #define bserialze_h

#include  "utilit.h"

// ============== DWORD =====================
inline size_t get_size_in_bytes (const DWORD& t)
{
	return sizeof(DWORD);
};
inline size_t save_to_bytes(const DWORD& i, BYTE* buf)
{
	*((DWORD*)buf) = i;
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(DWORD& i, const BYTE* buf)
{
	i = *((DWORD*)buf);
	return get_size_in_bytes(i);
};



// ============== int =====================
inline size_t get_size_in_bytes (const int& t)
{
	return sizeof(int);
};
inline size_t save_to_bytes(const int& i, BYTE* buf)
{
	*((int*)buf) = i;
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(int& i, const BYTE* buf)
{
	i = *((int*)buf);
	return get_size_in_bytes(i);
};


// ============== CHAR =====================
inline size_t get_size_in_bytes (const char& t)
{
	return 1;
};

inline size_t save_to_bytes(const char& i, BYTE* buf)
{
	*((char*)buf) = i;
	return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(char& i, const BYTE* buf)
{
	i = *((char*)buf);
	return get_size_in_bytes(i);
};


// ============== BYTE =====================
inline size_t get_size_in_bytes (const BYTE& t)
{
	return 1;
};

inline size_t save_to_bytes(const BYTE& i, BYTE* buf)
{
	*((BYTE*)buf) = i;
	return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(BYTE& i, const BYTE* buf)
{
	i = *((BYTE*)buf);
	return get_size_in_bytes(i);
};



// ============== WORD =====================
inline size_t get_size_in_bytes (const WORD& t)
{
	return 2;
};

inline size_t save_to_bytes(const WORD& i, BYTE* buf)
{
	*((WORD*)buf) = i;
	return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(WORD& i, const BYTE* buf)
{
	i = *((WORD*)buf);
	return get_size_in_bytes(i);
};


// ============== QWORD =====================
inline size_t get_size_in_bytes (const QWORD& t)
{
	return 8;
};

inline size_t save_to_bytes(const QWORD& i, BYTE* buf)
{
	*((QWORD*)buf) = i;
	return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(QWORD& i, const BYTE* buf)
{
	i = *((QWORD*)buf);
	return get_size_in_bytes(i);
};

// ============== DOUBLE =====================
inline size_t get_size_in_bytes (const double& t)
{
	return sizeof(t);
};
inline size_t save_to_bytes(const double& i, BYTE* buf)
{
	*((double*)buf) = i;
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(double& i, const BYTE* buf)
{
	i = *((double*)buf);
	return get_size_in_bytes(i);
};



// ============== PAIR =====================
template <class T>
inline size_t get_size_in_bytes (const pair<T, T>& t)
{
	return get_size_in_bytes(t.first) * 2;
};

template <class T>
inline size_t save_to_bytes(const pair<T, T>& t, BYTE* buf)


{
	buf += save_to_bytes(t.first, buf);
	buf += save_to_bytes(t.second, buf);
	return get_size_in_bytes(t);
};

template <class T>
inline size_t restore_from_bytes(pair<T, T>& t, const BYTE* buf)
{
	buf += restore_from_bytes(t.first, buf);
	buf += restore_from_bytes(t.second, buf);
	return get_size_in_bytes(t);
};

//
// ============== PAIR =====================
template <class T>
inline size_t get_size_in_bytes (const pair<QWORD, T>& t)
{
	return get_size_in_bytes(t.first) + get_size_in_bytes(t.second);
};

template <class T>
inline size_t save_to_bytes(const pair<QWORD, T>& t, BYTE* buf)


{
	buf += save_to_bytes(t.first, buf);
	buf += save_to_bytes(t.second, buf);
	return get_size_in_bytes(t);
};

template <class T>
inline size_t restore_from_bytes(pair<QWORD, T>& t, const BYTE* buf)
{
	buf += restore_from_bytes(t.first, buf);
	buf += restore_from_bytes(t.second, buf);
	return get_size_in_bytes(t);
};


// ============== TRIPLE =====================


template <class T>
inline size_t get_size_in_bytes (const troika<T, T, T>& t)
{
	return get_size_in_bytes(t.first) * 3;
};

template <class T>
inline size_t save_to_bytes(const troika<T, T, T>& t, BYTE* buf)
{
	buf += save_to_bytes(t.first, buf);
	buf += save_to_bytes(t.second, buf);
	buf += save_to_bytes(t.third, buf);
	return get_size_in_bytes(t);
};

template <class T>
inline size_t restore_from_bytes(troika<T, T, T>& t, const BYTE* buf)
{
	buf += restore_from_bytes(t.first, buf);
	buf += restore_from_bytes(t.second, buf);
	buf += restore_from_bytes(t.third, buf);
	return get_size_in_bytes(t);
};


const int VectorMaxStructSize = 200;

template <class T>
bool BinaryReadItem (FILE* fp, T& V)
{
    BYTE buffer[VectorMaxStructSize];
    size_t size_of_t = get_size_in_bytes(V);
    if (fread ((void*)buffer, size_of_t, 1, fp)!=1)
		return false;
    restore_from_bytes (V, buffer);
    return true;
}

template <class T>
void ReadVectorInner (FILE* fp, vector<T>& V, size_t Count)
{
#ifdef  WIN32
	if (_setmode( _fileno( fp ), _O_BINARY ) != _O_BINARY) 
		throw CExpc(Format("ReadVectorInner: File is not opened in binary mode!"));
#endif

	try
	{
		T dummy;
		size_t size_of_t = get_size_in_bytes(dummy);

		BYTE buffer[VectorMaxStructSize];
		assert (size_of_t < VectorMaxStructSize);

		V.reserve(V.size() + Count);
		if (V.capacity() < Count) 
			throw CExpc(Format("cannot allocate %u bytes in ReadVectorInner",size_of_t*Count));

		for (int i = 0; i < Count; i++)
		{
			if (fread ((void*)buffer, size_of_t, 1, fp)!=1)
				throw CExpc(Format("cannot read %i item in ReadVectorInner",i));
			restore_from_bytes (dummy, buffer);
			V.push_back(dummy);
		};
	}
	catch ( length_error  &e ) 
	{
		fprintf (stderr, "ReadVectorInner:length_error exception is caught: %s\n", e.what( ));
		throw;
	}
	catch ( exception &e ) 
	{
		fprintf (stderr, "ReadVectorInner: general std::exception is caught: %s\n", e.what( ));
		throw;
	};

};

template <class T>
size_t GetSizeInBytes()
{
    T dummy;
    return get_size_in_bytes(dummy);
}

template <class T>
inline void ReadVector (const string& FileName, vector<T>& V)
{
	V.clear();
	file_off_t sz = FileSize(FileName.c_str());

	FILE* fp = fopen (FileName.c_str(),"rb");
	if (!fp) return;
 	
	T dummy;
	size_t size_of_t = get_size_in_bytes(dummy);
	size_t Count = (size_t)sz/size_of_t;
	try {
        ReadVectorInner(fp, V, Count);
		fclose (fp);
		fp = 0;
	}
	catch (...)
	{
		if (fp) fclose (fp);
		throw;
	}
	
};


template <class T>
bool BinaryWriteItem (FILE* fp, const T& V)
{
    BYTE buffer[VectorMaxStructSize];
    save_to_bytes(V, buffer);
    size_t size_of_t = get_size_in_bytes(V);
    return fwrite((void*)buffer, size_of_t, 1, fp) == 1;
}

template <class T>
bool WriteVectorInner (FILE* fp, const vector<T>& V)
{
	#ifdef  WIN32
		if (_setmode( _fileno( fp ), _O_BINARY ) != _O_BINARY) 
		{
			ErrorMessage("WriteVectorInner", "File is not opened in binary mode!");
			return false;
		};
	#endif

	T dummy;
	size_t size_of_t = get_size_in_bytes(dummy);

	BYTE buffer[VectorMaxStructSize];
	assert (size_of_t < VectorMaxStructSize);

	size_t  count = V.size();

	for (size_t i =0; i < count; i++)
	{
		save_to_bytes(V[i], buffer);
		if (!fwrite((void*)buffer, size_of_t, 1, fp)) return false;
	};
	return true;
};


template <class T>
inline bool WriteVector (const string& FileName, const vector<T>& V)
{
	FILE* fp = fopen (FileName.c_str(),"wb");
	if (!fp)
	{
		ErrorMessage ("Cannot write vector to "+FileName);
		return false;
	};
	bool b = WriteVectorInner(fp, V);
	fclose (fp);
	return b;
};

#endif


