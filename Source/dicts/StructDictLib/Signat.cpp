
#include "Signat.h"
#include "ItemsContainer.h"
#include "../../morph_dict/common/utilit.h"
#include "../../common/cortege.h"


const std::string& CSignat::GetFrmt() const {
    return sFrmt;
}

const std::vector<BYTE>& CSignat::GetDomsWoDelims() const {
    return DomsWoDelims;
}

const std::vector<TSignatItem>& CSignat::GetDomsWithDelims() const {
    return DomsWithDelims;
}

void CSignat::BuildSignatFormat(const TItemContainer* parent, BYTE MaxNumDom, std::string field_name) {
    sFrmt = "";
    sFrmtWithotSpaces = "";
    DomsWithDelims.clear();
    DomsWoDelims.clear();
    if (SignatId >= UnknownSignatId) {
        throw CExpc("signat id (%i) cannot be more than %i", SignatId, UnknownSignatId);
    }

    if (FormatStr.empty()) {
        throw CExpc("Empty signat in %s", field_name.c_str());
    };

    // FunRec.Doms. В конце переменная DomMeanNum должна содержать число
    // значимых доменов.
    for (auto dom_str: split_string(FormatStr, ' ') ) {
        bool is_mult = endswith(dom_str, "+");

        if (is_mult)
            dom_str.pop_back();

        BYTE domenNo = parent->GetDomenNoByDomStr(dom_str.c_str());
        if (domenNo == ErrUChar) {
            throw CExpc("Домен %s  не объявлен в таблице доменов (формат функции %s)!", dom_str.c_str(), field_name.c_str());
        };
        const CDomen& D = parent->m_Domens[domenNo];

        if (D.GetDomainSource() == dsSystem) {
            throw CExpc(
                "Домен %s  объявлен как системный. Такие домены нельзя использовать в сигнатурах(формат функции %s)!",
                dom_str.c_str(), field_name.c_str());
        };

        DomsWithDelims.push_back(TSignatItem(domenNo, is_mult));
        if (D.DomainIsDelim()) {
            sFrmt += " ";
            assert(!D.IsEmpty());
            sFrmt += D.GetFirstDomStr();
            sFrmtWithotSpaces += D.GetFirstDomStr();
        }
        else {
            sFrmt += " %s";
            sFrmtWithotSpaces += "%s";


            if (DomsWoDelims.size() == MaxNumDom) {
                throw CExpc("Функция %s   Число значимых доменов  не может превышать %i!", field_name.c_str(), MaxNumDom);
            }
            if (is_mult) {
                domenNo = parent->GetDomenNoByDomStr("D_MULT");
                if (domenNo == ErrUChar) {
                    throw CExpc(
                        "Домен D_MULT  не объявлен в таблице доменов, а функции %s используется оператор повтора(+)!",
                        field_name.c_str());
                };
            };

            DomsWoDelims.push_back(domenNo);

        };

    };

}