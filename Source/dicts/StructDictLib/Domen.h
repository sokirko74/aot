// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma once

#include "morph_dict/common/utilit.h"
#include "morph_dict/common/json.h"
#include "StructDictConsts.h"


class TItemContainer;

class CDomen {

    //serialized
    std::string DomStr;
    char Source;
    bool IsDelim;
    std::vector<std::string> DomainParts;

    // runtime
    TItemContainer *m_pParent;
    bool m_bFreed;
    std::vector<BYTE> DomainPartPtrs;


public:
    bool IsFree;
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

    void MakeFree();

    nlohmann::json WriteToJson() const;

    void ReadFromJson(TItemContainer* parent, BYTE domNO, nlohmann::json& js);

    void InitDomainParts(const std::unordered_map<std::string, BYTE>& ident2ptr);

    const std::vector<BYTE> GetParts() const { return DomainPartPtrs; };
};
