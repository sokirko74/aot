#include "Domen.h"
#include "../../common/util_classes.h"

CDomen::CDomen() {
    m_bFreed = false;
    m_MaxDomItemId = 0;
}

dom_item_id_t CDomen::AddItemByEditor(const std::string& item_str) {
    TDomenItem new_item = { m_MaxDomItemId + 1, item_str };
    auto it = std::lower_bound(m_DomItems.begin(), m_DomItems.end(), new_item);
    assert (it == m_DomItems.end() || !(*it == new_item));
    size_t offset = it - m_DomItems.begin();
    m_DomItems.insert(it, new_item);
    ++m_MaxDomItemId;
    m_ItemId2ItimeIndex.insert(m_ItemId2ItimeIndex.begin() + new_item.InnerDomItemId, offset);
    assert(m_ItemId2ItimeIndex.size() == m_DomItems.size());
    assert(GetDomItemStrById(new_item.InnerDomItemId) == item_str);
    return build_item_id(DomNo, new_item.InnerDomItemId);

}


CDomen::~CDomen() {
};

bool CDomen::IsEmpty() const {
    return !m_DomItems.empty();
};

bool CDomen::IsFreedDomain() const {
    return m_bFreed;
}

const std::string& CDomen::GetDomStr() const {
    return DomStr;
}

char CDomen::GetDomainSource() const {
    return Source;
}


void CDomen::MakeFree() {
    m_DomItems.clear();
    m_ItemId2ItimeIndex.clear();
    m_bFreed = true;
}


nlohmann::json CDomen::WriteToJson()  const {
    nlohmann::json js;
    js["dom_ident"] = DomStr;
    js["category"] = Source;
    js["is_delim"] = IsDelim;
    js["is_free"] = IsFree;
    if (!DomainParts.empty())
        js["parts"] = DomainParts;
    return js;
    
}

void CDomen::ReadFromJson(TItemContainer* parent, BYTE domNO, nlohmann::json& js) {
    DomNo = domNO;
    js.at("dom_ident").get_to(DomStr);
    js.at("category").get_to(Source);
    js.at("is_delim").get_to(IsDelim);
    js.at("is_free").get_to(IsFree);
    if (js.find("parts") != js.end()) {
        js.at("parts").get_to(DomainParts);
    }
 
}

void CDomen::InitDomainParts(const std::unordered_map<std::string, BYTE>& ident2ptr) {
    DomainPartPtrs.clear();

    for (const auto& ident : DomainParts) {
        DomainPartPtrs.push_back(ident2ptr.at(ident));
    }
}

void CDomen::AddFromSerialized(const std::string& line) {
    auto delim = line.find('\t');
    uint32_t inner_item_id = atoi(line.substr(0, delim).c_str());
    if (inner_item_id > m_MaxDomItemId) {
        m_MaxDomItemId = inner_item_id;
    }
    m_DomItems.push_back({ inner_item_id , line.substr(delim + 1) });
    if (inner_item_id < m_ItemId2ItimeIndex.size()) {
        m_ItemId2ItimeIndex.resize(m_ItemId2ItimeIndex.size() + 100);
        m_ItemId2ItimeIndex[inner_item_id] = m_DomItems.size() - 1;
    }
}

const std::string& CDomen::GetDomItemStrById(const dom_item_id_t item_id) const {
    size_t index = m_ItemId2ItimeIndex[get_inner_item_index(item_id)];
    return  m_DomItems[index].DomItemStr;
}


dom_item_id_t CDomen::GetDomItemIdByStr(const std::string& item_str) const {
    TDomenItem item = { EmptyDomItemId, item_str };
    auto it = std::lower_bound(m_DomItems.begin(), m_DomItems.end(), item);
    if (it == m_DomItems.end()) {
        return EmptyDomItemId;
    };
    return build_item_id(DomNo, it->InnerDomItemId);
}

void CDomen::WriteItemsToStream(std::ofstream& outp) const {
    for (auto i : m_DomItems) {
        outp << i.InnerDomItemId << "\t" << i.DomItemStr << "\n";
    }
}

std::vector<std::string>  CDomen::GetAllStrings() const {
    std::vector<std::string> r;
    for (auto i : m_DomItems) {
        r.push_back(i.DomItemStr);
    }
    return r;
}