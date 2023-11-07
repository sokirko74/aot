#include "Domen.h"
#include "../../common/util_classes.h"

CDomen::CDomen() {
    m_DomainItemsBuffer = 0;
    m_DomainItemsBufferLength = 0;
    m_StartDomItem = -1;
    m_EndDomItem = -1;
    m_bFreed = false;
}

int CDomen::AddItem(const char *s, int Length) {
    m_DomainItemsBufferLength += Length + 1;
    m_DomainItemsBuffer = (char *) realloc(m_DomainItemsBuffer, m_DomainItemsBufferLength);
    memcpy(m_DomainItemsBuffer + m_DomainItemsBufferLength - Length - 1, s, Length);
    m_DomainItemsBuffer[m_DomainItemsBufferLength - 1] = 0;
    return m_DomainItemsBufferLength - Length - 1;
}

void CDomen::DelItem(int Offset, int Length) {
    memmove(m_DomainItemsBuffer + Offset, m_DomainItemsBuffer + Offset + Length + 1, m_DomainItemsBufferLength - (Offset + Length + 1));
    m_DomainItemsBufferLength -= Length + 1;
    m_DomainItemsBuffer = (char *) realloc(m_DomainItemsBuffer, m_DomainItemsBufferLength);
    m_EndDomItem--;
    if (m_DomainItemsBufferLength == 0) {
        m_StartDomItem = -1;
        m_EndDomItem = -1;
    };
};

CDomen::~CDomen() {
    if (m_DomainItemsBuffer != 0)
        free(m_DomainItemsBuffer);
};

bool CDomen::IsEmpty() const {
    return m_StartDomItem == -1;
};

bool CDomen::IsFreedDomain() const {
    return m_bFreed;
}

const char* CDomen::GetDomainItemsBuffer() const {
    return m_DomainItemsBuffer;
}

const std::string& CDomen::GetDomStr() const {
    return DomStr;
}

char CDomen::GetDomainSource() const {
    return Source;
}


void CDomen::MakeFree() {
    free(m_DomainItemsBuffer);
    m_DomainItemsBuffer = 0;
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

