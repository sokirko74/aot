#include "Domen.h"
#include "../../common/util_classes.h"

CDomen::CDomen() {
    m_bFreed = false;
    m_UnusedDomItemId = 0;
}

CDomen::~CDomen() {
};

bool CDomen::IsEmpty() const {
    return m_DomItems.empty();
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
    m_ItemId2ItemIndex.clear();
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


const std::string& CDomen::GetDomItemStrById(const dom_item_id_t item_id) const {
    size_t index = m_ItemId2ItemIndex[get_inner_item_index(item_id)];
    return  m_DomItems[index].DomItemStr;
}


dom_item_id_t CDomen::GetDomItemIdByStr(const std::string& item_str) const {
    TDomenItem item = { EmptyDomItemId, item_str };
    auto it = std::lower_bound(m_DomItems.begin(), m_DomItems.end(), item);
    if (it == m_DomItems.end() || it->DomItemStr != item_str) {
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

void CDomen::UpdateIndex(uint32_t inner_item_id, size_t offset) {
    if (inner_item_id >= m_ItemId2ItemIndex.size()) {
        m_ItemId2ItemIndex.resize(inner_item_id + 100, EmptyDomItemId);
    }
    if (m_ItemId2ItemIndex[inner_item_id] != EmptyDomItemId) {
        int dummy = 0;
    }
    assert(m_ItemId2ItemIndex[inner_item_id] == EmptyDomItemId);
    m_ItemId2ItemIndex[inner_item_id] = offset;

}

void CDomen::AddFromSerialized(const std::string& line) {
    auto delim = line.find('\t');
    uint32_t inner_item_id = atoi(line.substr(0, delim).c_str());
    if (inner_item_id >= m_UnusedDomItemId) {
        m_UnusedDomItemId = inner_item_id + 1;
    }
    std::string new_item_str = line.substr(delim + 1);
    if (!m_DomItems.empty()) {
        if (m_DomItems.back().DomItemStr.compare(new_item_str) >= 0) {
            throw CExpc("bad order in %s: string \"%s\" >= \"%s\" ", DomStr.c_str(), 
                m_DomItems.back().DomItemStr.c_str(), new_item_str.c_str());
        }
    }
    m_DomItems.push_back({ inner_item_id , new_item_str });
    UpdateIndex(inner_item_id, m_DomItems.size() - 1);
}

dom_item_id_t CDomen::AddItemByEditor(const std::string& item_str) {
    TDomenItem new_item = { m_UnusedDomItemId, item_str };
    auto it = std::lower_bound(m_DomItems.begin(), m_DomItems.end(), new_item);
    assert(it == m_DomItems.end() || !(*it == new_item));
    size_t offset = it - m_DomItems.begin();
    for (auto& i: m_ItemId2ItemIndex) {
        if (i != EmptyDomItemId && i >= offset) {
            ++i;
        }
    }
    m_DomItems.insert(it, new_item);
    ++m_UnusedDomItemId;
    UpdateIndex(new_item.InnerDomItemId, offset);
    assert(GetDomItemStrById(new_item.InnerDomItemId) == item_str);
    return build_item_id(DomNo, new_item.InnerDomItemId);

}
