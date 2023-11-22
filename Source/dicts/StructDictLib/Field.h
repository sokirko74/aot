#pragma once

#include "Signat.h"
#include "morph_dict/common/rapidjson.h"


struct  CField  
{
        
       std::string		FieldStr;
	   std::vector<CSignat>	m_Signats;
       char				TypeRes;
       int				OrderId;
	   bool				IsApplicToActant;

       //runtime
       std::vector<BYTE> SignatId2SignatNo;


	   CField& operator = (const CField& _F);
       void ReadFromJson(const rapidjson::Value& js);
};

