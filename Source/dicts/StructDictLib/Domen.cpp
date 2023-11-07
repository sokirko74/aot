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

 bool CDomen::ReadFromStr(TItemContainer* parent, BYTE domNO, const std::string& line) {
    StringTokenizer tok(line.c_str(), ";");
    tok();
    DomId = atoi(tok.val());
    tok();
    ItemsCount = atoi(tok.val());
    tok();
    DropDownCount = atoi(tok.val());
    tok();
    DomStr = tok.val();
    assert (!DomStr.empty());

    if (!tok()) return false;
    if (strlen(tok.val()) == 0) return false;
    Source = tok.val()[0];
    tok();
    IsDelim = (atoi(tok.val()) == -1) ? true : false;
    tok();
    IsFree = (atoi(tok.val()) == -1) ? true : false;
    tok();
    Color = atoi(tok.val());

    PartsSize = 0;
    // может быть равен нулю
    Format[0] = 0;
    if (Source == dsExpres) {
        tok();
        strcpy(Format, tok.val());
    }
    m_pParent = parent;
    m_DomNo = domNO;
    return true;
}

void CDomen::MakeFree() {
    free(m_DomainItemsBuffer);
    m_DomainItemsBuffer = 0;
    m_bFreed = true;
}

void CDomen::WriteDomainToStream(FILE* fp) const {
    fprintf(fp, "%i;%i;%i;%s;%c;%i;%i;%i;%s\n",
            DomId,
            ItemsCount,
            DropDownCount,
            DomStr.c_str(),
            Source,
            IsDelim ? -1 : 0,
            IsFree ? -1 : 0,
            Color,
            Format[0] ? Format : "");

}