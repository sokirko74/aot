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


void CField::ReadFromJson(const rapidjson::Value& js) {
    SignatId2SignatNo.resize(UnknownSignatId, UnknownSignatId);
    
    FieldStr = js["name"].GetString();
    TypeRes = js["type"].GetInt();
    IsApplicToActant = js["actant"].GetBool();
    OrderId = js["order"].GetInt();
    m_Signats.clear();
    for (auto& s : js["signats"].GetArray()) {
        CSignat S;
        S.SignatId = s["signat_id"].GetInt();
        S.FormatStr = s["format"].GetString();
        m_Signats.push_back(S);
        if (SignatId2SignatNo[S.SignatId] != UnknownSignatId) {
            throw CExpc("Signats must have unique identifiers; Field = " + FieldStr);
        }
        SignatId2SignatNo[S.SignatId] = (BYTE)(m_Signats.size() - 1);
    };

    if (m_Signats.size() >= UnknownSignatId) {
        throw  CExpc("more than %i signats in field %s", UnknownSignatId, FieldStr.c_str());
    }
}
