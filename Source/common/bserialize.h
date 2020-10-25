// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#pragma once

#include  "utilit.h"
#include  <fstream>

// ============== uint32_t =====================
inline size_t get_size_in_bytes(const uint32_t &t) {
    return sizeof(uint32_t);
};

inline size_t save_to_bytes(const uint32_t &i, BYTE *buf) {
    *((uint32_t *) buf) = i;
    return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(uint32_t &i, const BYTE *buf) {
    i = *((uint32_t *) buf);
    return get_size_in_bytes(i);
};


// ============== int =====================
inline size_t get_size_in_bytes(const int &t) {
    return sizeof(int);
};

inline size_t save_to_bytes(const int &i, BYTE *buf) {
    *((int *) buf) = i;
    return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(int &i, const BYTE *buf) {
    i = *((int *) buf);
    return get_size_in_bytes(i);
};


// ============== CHAR =====================
inline size_t get_size_in_bytes(const char &t) {
    return 1;
};

inline size_t save_to_bytes(const char &i, BYTE *buf) {
    *((char *) buf) = i;
    return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(char &i, const BYTE *buf) {
    i = *((char *) buf);
    return get_size_in_bytes(i);
};


// ============== BYTE =====================
inline size_t get_size_in_bytes(const BYTE &t) {
    return 1;
};

inline size_t save_to_bytes(const BYTE &i, BYTE *buf) {
    *((BYTE *) buf) = i;
    return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(BYTE &i, const BYTE *buf) {
    i = *((BYTE *) buf);
    return get_size_in_bytes(i);
};


// ============== uint16_t =====================
inline size_t get_size_in_bytes(const uint16_t &t) {
    return 2;
};

inline size_t save_to_bytes(const uint16_t &i, BYTE *buf) {
    *((uint16_t *) buf) = i;
    return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(uint16_t &i, const BYTE *buf) {
    i = *((uint16_t *) buf);
    return get_size_in_bytes(i);
};


// ============== uint64_t =====================
inline size_t get_size_in_bytes(const uint64_t &t) {
    return 8;
};

inline size_t save_to_bytes(const uint64_t &i, BYTE *buf) {
    *((uint64_t *) buf) = i;
    return get_size_in_bytes(i);
};

inline size_t restore_from_bytes(uint64_t &i, const BYTE *buf) {
    i = *((uint64_t *) buf);
    return get_size_in_bytes(i);
};

// ============== DOUBLE =====================
inline size_t get_size_in_bytes(const double &t) {
    return sizeof(t);
};

inline size_t save_to_bytes(const double &i, BYTE *buf) {
    *((double *) buf) = i;
    return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(double &i, const BYTE *buf) {
    i = *((double *) buf);
    return get_size_in_bytes(i);
};


// ============== PAIR =====================
template<class T>
inline size_t get_size_in_bytes(const std::pair<T, T> &t) {
    return get_size_in_bytes(t.first) * 2;
};

template<class T>
inline size_t save_to_bytes(const std::pair<T, T> &t, BYTE *buf) {
    buf += save_to_bytes(t.first, buf);
    buf += save_to_bytes(t.second, buf);
    return get_size_in_bytes(t);
};

template<class T>
inline size_t restore_from_bytes(std::pair<T, T> &t, const BYTE *buf) {
    buf += restore_from_bytes(t.first, buf);
    buf += restore_from_bytes(t.second, buf);
    return get_size_in_bytes(t);
};

//
// ============== PAIR =====================
template<class T>
inline size_t get_size_in_bytes(const std::pair<uint64_t, T> &t) {
    return get_size_in_bytes(t.first) + get_size_in_bytes(t.second);
};

template<class T>
inline size_t save_to_bytes(const std::pair<uint64_t, T> &t, BYTE *buf) {
    buf += save_to_bytes(t.first, buf);
    buf += save_to_bytes(t.second, buf);
    return get_size_in_bytes(t);
};

template<class T>
inline size_t restore_from_bytes(std::pair<uint64_t, T> &t, const BYTE *buf) {
    buf += restore_from_bytes(t.first, buf);
    buf += restore_from_bytes(t.second, buf);
    return get_size_in_bytes(t);
};


// ============== TRIPLE =====================


template<class T>
inline size_t get_size_in_bytes(const troika<T, T, T> &t) {
    return get_size_in_bytes(t.first) * 3;
};

template<class T>
inline size_t save_to_bytes(const troika<T, T, T> &t, BYTE *buf) {
    buf += save_to_bytes(t.first, buf);
    buf += save_to_bytes(t.second, buf);
    buf += save_to_bytes(t.third, buf);
    return get_size_in_bytes(t);
};

template<class T>
inline size_t restore_from_bytes(troika<T, T, T> &t, const BYTE *buf) {
    buf += restore_from_bytes(t.first, buf);
    buf += restore_from_bytes(t.second, buf);
    buf += restore_from_bytes(t.third, buf);
    return get_size_in_bytes(t);
};


const int VectorMaxStructSize = 200;

template<class T>
bool BinaryReadItem(FILE *fp, T &V) {
    BYTE buffer[VectorMaxStructSize];
    size_t size_of_t = get_size_in_bytes(V);
    if (fread((void *) buffer, size_of_t, 1, fp) != 1)
        return false;
    restore_from_bytes(V, buffer);
    return true;
}

template<class T>
void ReadVectorInner(FILE *fp, std::vector<T> &V, size_t Count) {
#ifdef  WIN32
    if (_setmode( _fileno( fp ), _O_BINARY ) != _O_BINARY)
        throw CExpc(Format("ReadVectorInner: File is not opened in binary mode!"));
#endif

    try {
        T dummy;
        size_t size_of_t = get_size_in_bytes(dummy);

        BYTE buffer[VectorMaxStructSize];
        assert (size_of_t < VectorMaxStructSize);
        V.reserve(V.size() + Count);
        if (V.capacity() < Count)
            throw CExpc(Format("cannot allocate %zu bytes in ReadVectorInner", size_of_t * Count));

        for (int i = 0; i < Count; i++) {
            if (fread((void *) buffer, size_of_t, 1, fp) != 1)
                throw CExpc(Format("cannot read %i item in ReadVectorInner", i));
            restore_from_bytes(dummy, buffer);
            V.push_back(dummy);
        };
    }
    catch (std::length_error &e) {
        fprintf(stderr, "ReadVectorInner:length_error exception is caught: %s\n", e.what());
        throw;
    }
    catch (std::exception &e) {
        fprintf(stderr, "ReadVectorInner: general std::exception is caught: %s\n", e.what());
        throw;
    };

};

template<class T>
void ReadVectorInner(std::ifstream &inputStream, std::vector<T> &V, size_t Count) {
    try {
        T dummy;
        size_t size_of_t = get_size_in_bytes(dummy);

        BYTE buffer[VectorMaxStructSize];
        assert(size_of_t < VectorMaxStructSize);
        V.reserve(V.size() + Count);
        if (V.capacity() < Count)
            throw CExpc(Format("cannot allocate %zu bytes in ReadVectorInner", size_of_t * Count));

        for (int i = 0; i < Count; i++) {
            if (!inputStream.read((char *) buffer, size_of_t))
                throw CExpc(Format("cannot read %i item in ReadVectorInner", i));
            restore_from_bytes(dummy, buffer);
            V.push_back(dummy);
        };
    }
    catch (std::length_error &e) {
        fprintf(stderr, "ReadVectorInner:length_error exception is caught: %s\n", e.what());
        throw;
    }
    catch (std::exception &e) {
        fprintf(stderr, "ReadVectorInner: general std::exception is caught: %s\n", e.what());
        throw;
    };

};


template<class T>
size_t GetSizeInBytes() {
    T dummy;
    return get_size_in_bytes(dummy);
}

template<class T>
inline void ReadVector(const std::string &FileName, std::vector<T> &V) {
    V.clear();
    file_off_t sz = FileSize(FileName.c_str());

    FILE *fp = fopen(FileName.c_str(), "rb");
    if (!fp) return;

    T dummy;
    size_t size_of_t = get_size_in_bytes(dummy);
    size_t Count = (size_t) sz / size_of_t;
    try {
        ReadVectorInner(fp, V, Count);
        fclose(fp);
        fp = 0;
    }
    catch (...) {
        if (fp) fclose(fp);
        throw;
    }

};


template<class T>
bool BinaryWriteItem(FILE *fp, const T &V) {
    BYTE buffer[VectorMaxStructSize];
    save_to_bytes(V, buffer);
    size_t size_of_t = get_size_in_bytes(V);
    return fwrite((void *) buffer, size_of_t, 1, fp) == 1;
}

template<class T>
bool WriteVectorInner(FILE *fp, const std::vector<T> &V) {
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

    size_t count = V.size();

    for (size_t i = 0; i < count; i++) {
        save_to_bytes(V[i], buffer);
        if (!fwrite((void *) buffer, size_of_t, 1, fp)) return false;
    };
    return true;
};

template<class T>
bool WriteVectorStream(std::ofstream& outp, const std::vector<T>& vec) {
    T dummy;
    size_t size_of_t = get_size_in_bytes(dummy);

    BYTE buffer[VectorMaxStructSize];
    assert(size_of_t < VectorMaxStructSize);

    for (auto item : vec) {
        save_to_bytes(item, buffer);
        outp.write((char*)buffer, size_of_t);
    };
    return true;
};


template<class T>
inline bool WriteVector(const std::string &FileName, const std::vector<T> &V) {
    FILE *fp = fopen(FileName.c_str(), "wb");
    if (!fp) {
        ErrorMessage("Cannot write std::vector to " + FileName);
        return false;
    };
    bool b = WriteVectorInner(fp, V);
    fclose(fp);
    return b;
};

