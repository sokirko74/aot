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
    m_Language = morphRussian;
};


BYTE TItemContainer::GetDomenNoByDomStr(const char *DomStr) const {
    for (BYTE i = 0; i < m_Domens.size(); i++)
        if (m_Domens[i].GetDomStr() == DomStr)
            return i;

    return ErrUChar;
};

dom_item_id_t TItemContainer::GetItemIdByItemStr(const std::string& ItemStr, BYTE DomNo) const {
    if (DomNo == ErrUChar) return -1;

    if (DomNo == LexPlusDomNo) {
        DomNo = GetDomNoForLePlus(ItemStr.c_str());
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



bool TItemContainer::InitDomensConsts() {

    ActantsDomNo = GetDomenNoByDomStr("D_ACTANTS");
    if (ActantsDomNo == ErrUChar) {
        throw CExpc("cannot find domain D_ACTANTS");
    }

    LexDomNo = GetDomenNoByDomStr("D_RLE");
    if (LexDomNo == ErrUChar) return false;

    LexPlusDomNo = GetDomenNoByDomStr("D_RLE_PLUS");
    if (LexPlusDomNo == ErrUChar) return false;

    IntegerDomNo = GetDomenNoByDomStr("D_INTEGER");

    TitleDomNo = GetDomenNoByDomStr("D_TITLE");


    CollocDomNo = GetDomenNoByDomStr("D_COLLOC");
    if (CollocDomNo == ErrUChar) return false;

    AbbrDomNo = GetDomenNoByDomStr("D_ABBR");
    if (AbbrDomNo == ErrUChar) return false;

    FieldDomNo = GetDomenNoByDomStr("D_FIELDS");
    if (FieldDomNo == ErrUChar) return false;

    WildCardDomNo = GetDomenNoByDomStr("D_");
    if (WildCardDomNo == ErrUChar) return false;

    return true;
};


void TItemContainer::UpdateConstDomens() {
    assert(!m_Domens[WildCardDomNo].IsEmpty());
    WildCardDomItemNo = build_item_id(WildCardDomNo, 0);
};


bool TItemContainer::BuildDomens(char *LastReadLine) {
    std::ifstream inp;
    inp.open(DomensFile);
    if (!inp.good()) {
        throw CExpc("cannot open file %s", DomensFile.c_str());
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
   
    
    return InitDomensConsts();
}


bool TItemContainer::BuildDomItems() {

    //  reading domen items
    {
        std::ifstream  inp;
        inp.open(DomItemsTextFile);
        std::string line; 
        BYTE  dom_no = -1;
        while (std::getline(inp, line)) {
            if (startswith(line, "-1\t")) {
                const char* dom_str = line.c_str() + 3;
                dom_no = GetDomenNoByDomStr(dom_str);
                if (dom_no == ErrUChar) {
                    throw CExpc("bad domain name %s", dom_str);
                }
            }
            else {
                m_Domens[dom_no].AddFromSerialized(line);
            }
        }
    }

    UpdateConstDomens();

    return true;
}

bool TItemContainer::WriteDomItems() const {
    std::ofstream outp;
    outp.open(DomItemsTextFile);
    for (auto d : m_Domens) {
        outp << "-1\t" << d.GetDomStr() << "\n";
        d.WriteItemsToStream(outp);
    }
    outp.close();
    return true;
};


BYTE TItemContainer::GetDomNoForLePlus(const char *s) const {
    int Number = atoi(s);
    if (Number
        || (!s && (strlen(s) == 1) && (s[0] == '0'))
            )
        return IntegerDomNo;
    else if (IsStandardRusLexeme(s))
        return LexDomNo;
    else if (CanBeRusAbbr(s))
        return AbbrDomNo;
    else if (CanBeRusColloc(s))
        return CollocDomNo;
    else if (!strncmp(s, "D_", 2))
        return LexPlusDomNo;
    else
        return ErrUChar;
};


// строка:
// 1. Cодержащая  только кириллицу;
// 2. Может включать в себя только один дефис;
// 3. Может начинаться с большой буквы, но все остальные буквы должны быть
//    маленькими. 
bool TItemContainer::IsStandardRusLexeme(const char *s) const {
    bool FlagHasHyphen = false;

    if ((strlen(s) == 0)
        || !is_russian_alpha((unsigned char) s[0]))
        return false;

    for (int i = 1; i < strlen(s); i++)
        if (((unsigned char) s[i]) == '-')
            if (FlagHasHyphen)
                return false;
            else
                FlagHasHyphen = true;
        else
/* вообще-то в словаре леммы и словоформы  принято писять маленькими буквами,
но отдельные плохие люди пишут большими, поэтому приходится проверять и те, и другие */
        if (!is_russian_alpha((unsigned char) s[i]))
            return false;

    return ((unsigned char) s[strlen(s) - 1]) != '-';
};


inline bool TItemContainer::CanBeRusAbbr(const char *s) const {

    if (strlen(s) == 1)
        return is_lower_alpha((unsigned char) s[0], m_Language);


    if (strlen(s) == 0)
        return false;

    bool HasUpperAlphaInside = false;

    for (int i = 0; i < strlen(s) - 1; i++) {
        if (!is_russian_alpha((unsigned char) s[i]) && !islower((unsigned char) s[i]) &&
            !strchr("-.//", (unsigned char) s[i]))
            return false;

        HasUpperAlphaInside |= (is_russian_upper((unsigned char) s[i])
                                || (((unsigned char) s[i]) == '/'))
                               && (i > 0);

    };

    return HasUpperAlphaInside || (((unsigned char) s[strlen(s) - 1]) == '.');
}


inline bool TItemContainer::CanBeRusColloc(const char *s) const {
    if ((strlen(s) < 4)
        || (!strchr(s, ' ')
            && !strchr(s, ':')
        )
            )
        return false;

    for (int i = 0; i < strlen(s); i++)
        if (!is_russian_alpha((unsigned char) s[i])
            && !isdigit((unsigned char) s[i])
            && !strchr("-/,\\$:;.|()\"~ ", (unsigned char) s[i])
                )
            return false;

    return true;
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



void TItemContainer::BuildFields() {
    ClearFields();
    
    std::ifstream inp;
    inp.open(FieldsFile);
    if (!inp.good()) {
        throw CExpc("cannot open file %s", FieldsFile.c_str());
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


bool TItemContainer::WriteFields() const {
    auto fields = nlohmann::json::array();
    for (auto f : Fields) {
        fields.push_back(f.GetFieldJson());
    };

    std::ofstream outf(MakeFName(FieldsFile, "json"));
    outf << fields.dump(4);
    outf.close();
    return true;
    
};

bool TItemContainer::WriteDomens() const {
    std::ofstream outf(DomensFile);
    if (!outf.is_open())
    {
        throw CExpc("Cannot write to %s", DomensFile.c_str());
    };
    auto domains = nlohmann::json::array();
    for (auto& d : m_Domens) {
        domains.push_back(d.WriteToJson());
    }
    outf << domains.dump(4);
    outf.close();
    return true;
}


void TItemContainer::ErrorMessage(std::string s) const {
    ::ErrorMessage(RossPath, s);
};


