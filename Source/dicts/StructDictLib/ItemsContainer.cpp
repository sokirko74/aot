// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#pragma warning  (disable : 4530)

#include "StdRoss.h"
#include <algorithm>
#include <stdio.h>
#include "ItemsContainer.h"
#include "morph_dict/common/util_classes.h"


TItemContainer::TItemContainer() {

};


BYTE TItemContainer::GetDomenNoByDomStr(const char *DomStr, bool throw_expc) const {
    for (BYTE i = 0; i < m_Domens.size(); i++)
        if (m_Domens[i].GetDomStr() == DomStr)
            return i;
    if (throw_expc) {
        throw CExpc("cannot find struct dict domeain %s", DomStr);
    }
    return ErrUChar;
};

dom_item_id_t TItemContainer::GetItemIdByItemStr(const std::string& ItemStr, BYTE DomNo) const {
    if (DomNo == ErrUChar) return EmptyDomItemId;

    if (DomNo == LexPlusDomNo) {
        DomNo = GetDomNoForLePlus(ItemStr);
        if (DomNo == ErrUChar) {
            return EmptyDomItemId;
        };
    };

    const CDomen& D = m_Domens[DomNo];
    if (D.GetDomainSource() == dsUnion) {
        for (auto& part_dom_no : D.GetParts()) {
            auto id = m_Domens[part_dom_no].GetDomItemIdByStr(ItemStr);
            if (!is_null(id)) {
                return id;
            }
        }
        return EmptyDomItemId;
    }
    else {
        return m_Domens[DomNo].GetDomItemIdByStr(ItemStr);
    };
}

dom_item_id_t TItemContainer::GetItemIdByItemStr(const std::string& ItemStr, const char *DomStr) const {
    BYTE DomNo = GetDomenNoByDomStr(DomStr);
    if (DomNo == ErrUChar) return EmptyDomItemId;
    return GetItemIdByItemStr(ItemStr, DomNo);
};



void TItemContainer::InitDomensConsts() {

    ActantsDomNo = GetDomenNoByDomStr("D_ACTANTS", true);
    LexDomNo = GetDomenNoByDomStr("D_RLE", true);
    LexPlusDomNo = GetDomenNoByDomStr("D_RLE_PLUS", true);
    IntegerDomNo = GetDomenNoByDomStr("D_INTEGER", true);
    TitleDomNo = GetDomenNoByDomStr("D_TITLE", true);
    CollocDomNo = GetDomenNoByDomStr("D_COLLOC", true);
    AbbrDomNo = GetDomenNoByDomStr("D_ABBR", true);
    FieldDomNo = GetDomenNoByDomStr("D_FIELDS", true);
    WildCardDomNo = GetDomenNoByDomStr("D_", true);
    EnglDomNo = GetDomenNoByDomStr("D_ENGL", true);
};


void TItemContainer::UpdateConstDomens() {
    assert(!m_Domens[WildCardDomNo].IsEmpty());
    WildCardDomItemNo = build_item_id(WildCardDomNo, 0);
};


void TItemContainer::BuildDomens(std::string path) {
    m_Domens.clear();
    std::ifstream inp;
    inp.open(path);
    if (!inp.good()) {
        throw CExpc("cannot open file %s", path.c_str());
    }
    auto domains = nlohmann::json::parse(inp);
    std::unordered_map<std::string, BYTE>  doms_idents;
    for (auto d: domains) {
        BYTE dom_no = (BYTE)m_Domens.size();
        CDomen T;
        T.ReadFromJson(this, dom_no, d);
        m_Domens.emplace_back(T);
        doms_idents[T.GetDomStr()] = dom_no;
    }
    for (auto& d : m_Domens) {
        d.InitDomainParts(doms_idents);
    }
    InitDomensConsts();
}


void TItemContainer::BuildDomItems(std::string path) {
    {
        std::ifstream  inp;
        inp.open(path);
        std::string line; 
        size_t line_no = 1;
        BYTE  dom_no = ErrUChar;
        while (std::getline(inp, line)) {
            if (startswith(line, "-1\t")) {
                const char* dom_str = line.c_str() + 3;
                dom_no = GetDomenNoByDomStr(dom_str);
                if (dom_no == ErrUChar) {
                    throw CExpc("bad domain name %s in %s", dom_str, path.c_str());
                }
            }
            else {
                m_Domens[dom_no].AddFromSerialized(line);
            }
            ++line_no;
        }
    }

    UpdateConstDomens();

}

bool TItemContainer::WriteDomItems(std::string path) const {
    std::ofstream outp;
    outp.open(path);
    for (auto d : m_Domens) {
        outp << "-1\t" << d.GetDomStr() << "\n";
        d.WriteItemsToStream(outp);
    }
    outp.close();
    return true;
};


static bool Check_D_RLE(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    if (s[0] == '#') {
        return CheckRussianUtf8(s.substr(1));
    }
    else {
        return CheckRussianUtf8(s);
    }
}

// вопр.
// км/сек
static bool CanBeRusAbbr(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    if (s.back() == '.') {
        return CheckRussianLowerUtf8(s.substr(0, s.length() - 1));
    }
    else {
        return CheckRussianLowerUtf8(s);
    }

}

static bool CanBeRusColloc(const std::string& s) {
    if ((s.length() < 4) || (s.find(' ') == s.npos && s.find(':') == s.npos))
        return false;

    return ContainsRussianUtf8(s);
};



BYTE TItemContainer::GetDomNoForLePlus(const std::string& s) const {
    if (atoi(s.c_str()) || (s.length() == 1) && (s[0] == '0'))
        return IntegerDomNo;
    else if (Check_D_RLE(s))
        return LexDomNo;
    else if (CanBeRusAbbr(s))
        return AbbrDomNo;
    else if (CanBeRusColloc(s))
        return CollocDomNo;
    else if (startswith(s, "D_"))
        return LexPlusDomNo;
    else
        return ErrUChar;
};




BYTE TItemContainer::GetFieldNoByFieldStrInner(const char *FieldStr) const {
    BYTE k = 0;


    for (; k < Fields.size(); k++)
        if (Fields[k].FieldStr == FieldStr)
            break;

    if (k == Fields.size())
        return ErrUChar;
    else
        return k;

}

bool TItemContainer::ClearFields() {
    for (size_t i = 0; i < Fields.size(); i++)
        Fields[i].m_Signats.clear();

    Fields.clear();
    return true;
}



void TItemContainer::BuildFields(std::string path) {
    ClearFields();
    
    std::ifstream inp;
    inp.open(path);
    if (!inp.good()) {
        throw CExpc("cannot open file %s", path.c_str());
    }
    auto fields = nlohmann::json::parse(inp);
    for (auto f_js : fields) {
        CField F;
        F.ReadFromJson(f_js);
        for (auto& s : F.m_Signats) {
            s.BuildSignatFormat(this, MaxNumDom, F.FieldStr);
        }
        Fields.emplace_back(F);
    }
    
}




inline bool IsTitle(const char* s)
{
    if (!s) return false;
    for (int i = 0; i < strlen(s); i++)
        if (isdigit((unsigned char)s[i])
            )
            return false;
    return true;

};


dom_item_id_t   TItemContainer::InsertDomItem(const char* ItemStr, BYTE DomNo)
{
    if (DomNo == TitleDomNo)
        if (!IsTitle(ItemStr))
        {
            throw CExpc("Cannot add \"%s\" to title domen!", ItemStr);
        };


    if (DomNo == LexDomNo)
        if (!Check_D_RLE(ItemStr))
        {
            throw CExpc("Cannot add \"%s\" to lexeme domen!", ItemStr);
        };

    if (DomNo == LexPlusDomNo)
    {
        DomNo = GetDomNoForLePlus(ItemStr);
        if (DomNo == ErrUChar)
        {
            throw CExpc("Cannot add \"%s\" to the extended lexeme domen!", ItemStr);
        };
    };

    return  m_Domens[DomNo].AddItemByEditor(ItemStr);
};
