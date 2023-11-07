#include "Field.h"

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
            {"order", s.OrderNo},
            {"format", s.FormatStr},
            {"name", s.FormatName},
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
    js.at("name").get_to(FieldStr);
    js.at("type").get_to(TypeRes);
    js.at("actant").get_to(IsApplicToActant);
    js.at("order").get_to(OrderId);
    m_Signats.clear();
    for (auto s : js.at("signats")) {
        CSignat S;
        s.at("order").get_to(S.OrderNo);
        s.at("format").get_to(S.FormatStr);
        s.at("name").get_to(S.FormatName);
        m_Signats.push_back(S);
    };
}
