// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include "GraphmatFile.h"
#include "Consent.h"
#include "GraphanDicts.h"
#include "../common/gra_descr_primitives.h"
#include "algorithm"


std::string CGraphmatFile :: GetGraphematicalLine (size_t LineNo) const
{
	const CGraLine& L = GetUnits()[LineNo];
	std::string result;

	{
		int l;

		if (!L.IsSoft())
		{ 
			if (!L.IsNotPrint()
				&& (L.GetToken() != NULL)
				&& (L.GetToken()[0] != 0)
				) 
			{
				result.append(L.GetToken(), L.GetTokenLength());
			}
			else {
				if (LineNo == 0) {
					result.append(1, ' ');
				}
				else {
					result.append(1, GraphematicalSPACE);
				}
			}
		}  
		else
		{ 
			for (size_t k=0; k<L.GetTokenLength(); ++k)
				switch ((unsigned char)L.GetToken()[k]) 
				{
					case ' '  : result.append(1, GraphematicalSPACE); break;
					case '\t' : result.append(1, GraphematicalTAB); break;
					case '\n' : result.append(1, GraphematicalEOLN); break;
					case '\r' :  break;
					default   : assert (false); break;
				};
		};

		result.append(std::max(1, 32 - (int)result.length()), ' ');
	}
	
	
	result.append(Format("%zu %zu", L.GetInputOffset(), L.GetTokenLength()));

	// write descriptors 
	for (int l = 0; l < 63; l++) {
		if ((L.GetDescriptors() & _QM(l)) > 0)
		{
			result.append(" ");
			result.append(GetDescriptorStr(l));
		};
	}

	short OborotNo  = GetOborotNo(LineNo);
	if (OborotNo != -1)
	{
		result.append( Format(" EXPR_NO%zu", m_pDicts->m_Oborottos[OborotNo].m_UnitNo) );

		if (m_pDicts->m_Oborottos[OborotNo].m_bFixedFet)
			result.append(" FIXED ");
	}

	if (L.IsPageBreak())
	{
		result.append(Format(" PGBR%zu", GetPageNumber(LineNo)));
	}

	if (L.IsParagraphTag())
	{
		result.append (" PARTAG");
	}
	return result;
};


void CGraphmatFile :: WriteGraphMat (const char* filename) const
{
	std::ofstream outp(filename, std::ios::binary);

	for (size_t i=0; i<GetUnits().size(); i++)
	{  
		auto line = convert_to_utf8(GetGraphematicalLine(i), m_Language);
		outp << line << "\n";
	};
}


