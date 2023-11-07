#pragma once

#include "Signat.h"
#include "morph_dict/common/json.h"



struct  CField  
{
       //int				FieldId;
       std::string		FieldStr;
	   std::vector<CSignat>	m_Signats;
       char				TypeRes;
       int				OrderId;
	   bool				IsApplicToActant;
	   CField& operator = (const CField& _F);
       nlohmann::json GetFieldJson() const;
       void ReadFromJson(nlohmann::json& js);
};

