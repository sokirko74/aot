#include "Field.h"
#include "../../common/cortege.h"

CField& CField::operator = (const CField& _F)
{
	FieldStr = _F.FieldStr;
	TypeRes = _F.TypeRes;
	OrderId = _F.OrderId;
	IsApplicToActant = _F.IsApplicToActant;
	m_Signats = _F.m_Signats;
	return *this;
};

nlohmann::json CField::GetFieldJson() const {
    nlohmann::json signats = nlohmann::json::array();
    for (auto s : m_Signats) {
        nlohmann::json signat = {
            {"signat_id", s.SignatId},
            {"format", s.FormatStr},
        };
        signats.push_back(signat);
    }

    nlohmann::json field = {
        {"name", FieldStr},
        {"type", TypeRes},
        {"actant", IsApplicToActant},
        {"order", OrderId},
        {"signats", signats},
    };
    return field;
};



void CField::ReadFromJson(nlohmann::json& js) {
    SignatId2SignatNo.resize(UnknownSignatId, UnknownSignatId);

    js.at("name").get_to(FieldStr);
    js.at("type").get_to(TypeRes);
    js.at("actant").get_to(IsApplicToActant);
    js.at("order").get_to(OrderId);
    m_Signats.clear();
    for (auto s : js.at("signats")) {
        CSignat S;
        s.at("signat_id").get_to(S.SignatId);
        s.at("format").get_to(S.FormatStr);
        m_Signats.push_back(S);
        if (SignatId2SignatNo[S.SignatId] != UnknownSignatId) {
            throw CExpc("Signats must have unique identifiers; Field = " + FieldStr);
        }
        SignatId2SignatNo[S.SignatId] = m_Signats.size() - 1;
    };

    if (m_Signats.size() >= UnknownSignatId) {
        throw  CExpc("more than %i signats in field %s", UnknownSignatId, FieldStr.c_str());
    }
}
