// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma once

#include "morph_dict/common/utilit.h"

#include "StructDictConsts.h"


class TItemContainer;

const int DomStrSize = 100;

class CDomen {
    int DomId;
    std::string DomStr;
    char Format[255];
    char Source;
    bool IsDelim;
    uint16_t ItemsCount;
    int DropDownCount;
    uint32_t Color;
    TItemContainer *m_pParent;
    BYTE m_DomNo;
    bool m_bFreed;

public:
    bool IsFree;
    BYTE Parts[MaxDomensUnionSize];
    BYTE PartsSize;
    int m_StartDomItem;
    int m_EndDomItem;
    char *m_DomainItemsBuffer;
    int m_DomainItemsBufferLength;

    CDomen();

    ~CDomen();

    int AddItem(const char *s, int Length);

    void DelItem(int Offset, int Length);

    bool IsEmpty() const;

    bool IsFreedDomain() const;

    const char* GetDomainItemsBuffer() const;

    const std::string& GetDomStr() const;

    char GetDomainSource() const;

    bool DomainIsDelim() const {return IsDelim; };

    bool ReadFromStr(TItemContainer* parent, BYTE domNO, const std::string& line);

    void MakeFree();

    void WriteDomainToStream(FILE* fp) const;

    int GetDropDownCount() const {return DropDownCount;};

    int GetGuiColor() const {return Color;};
};
