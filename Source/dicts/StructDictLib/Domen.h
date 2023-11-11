// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma once

#include "common/cortege.h"

#include "morph_dict/common/utilit.h"
#include "morph_dict/common/json.h"
#include "StructDictConsts.h"


struct TDomenItem {
    dom_item_id_t InnerDomItemId;
    std::string DomItemStr;
    bool operator < (const TDomenItem& _X) const { return DomItemStr < _X.DomItemStr; }
    bool operator == (const TDomenItem& _X) const { return DomItemStr == _X.DomItemStr; }
};

class TItemContainer;

class CDomen {

    //serialized
    std::string DomStr;
    char Source;
    bool IsDelim;
    std::vector<std::string> DomainParts;

    // runtime
    BYTE DomNo;
    TItemContainer *m_pParent;
    bool m_bFreed;
    std::vector<BYTE> DomainPartPtrs;
    dom_item_id_t m_MaxDomItemId;
    std::vector< TDomenItem> m_DomItems;
    std::vector< size_t>  m_ItemId2ItimeIndex;
public:
    bool IsFree;

    CDomen();

    ~CDomen();

    dom_item_id_t AddItemByEditor(const std::string& item_str);

    bool IsEmpty() const;

    bool IsFreedDomain() const;

    const std::string& GetDomStr() const;

    char GetDomainSource() const;

    bool DomainIsDelim() const {return IsDelim; };

    void MakeFree();

    nlohmann::json WriteToJson() const;

    void ReadFromJson(TItemContainer* parent, BYTE domNO, nlohmann::json& js);

    void InitDomainParts(const std::unordered_map<std::string, BYTE>& ident2ptr);

    const std::vector<BYTE> GetParts() const { return DomainPartPtrs; };

    void AddFromSerialized(const std::string& line);

    const std::string& GetFirstDomStr() const { return m_DomItems[0].DomItemStr; };

    const std::string& GetDomItemStrById(const dom_item_id_t Item) const;

    dom_item_id_t GetDomItemIdByStr(const std::string& item_str) const;

    void WriteItemsToStream(std::ofstream& outp) const;
};
