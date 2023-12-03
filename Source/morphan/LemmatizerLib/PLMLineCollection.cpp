#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"
#include "PLMLineCollection.h"
#include <fstream>


CLemmatizedText::CLemmatizedText()
{
	m_pLemmatizer = NULL;
};


void CLemmatizedText::CreateFromTokemized(const CGraphmatFile* Gr)
{
	if (!m_pLemmatizer) {
		throw CExpc("no morphology in CLemmatizedText");
	}

	m_PlmItems.clear();

    std::vector<CFormInfo> lem_results;
	lem_results.reserve(5);

	bool bInFixedExpression = false;
	
	for (size_t lineNo = 0; lineNo < Gr->GetTokensCount(); lineNo++)
	{
		std::string strProcess  = Gr->GetUnits()[lineNo].GetGraphematicalLine();

		//=====   do not lemmatize oborots with EXPR=Fixed!
		if (Gr->HasDescr(lineNo, OFixedOborot))
		{
			bInFixedExpression = true;
		};

		if (bInFixedExpression)
		{
			m_PlmItems.push_back(strProcess);
			if (Gr->HasDescr(lineNo, OEXPR2))	
				bInFixedExpression = false;
			continue;
		};
		if (m_pLemmatizer->GetLanguage() == Gr->GetTokenLanguage(lineNo))
		{
			std::string word = convert_from_utf8(Gr->GetTokenUtf8(lineNo).c_str(), m_pLemmatizer->GetLanguage());
			m_pLemmatizer->CreateParadigmCollection(false, 
					word, 
					!Gr->HasDescr(lineNo, OLw), true, lem_results);

			if (lem_results.empty() ) {
				m_PlmItems.push_back(strProcess + Format("\t-?? %s ?? -1 0", Gr->GetUpperString(lineNo)) );
			}
			else
				for( int i=0; i < lem_results.size(); i++ )
				{
					std::string line;
					if (i > 0) line = 	"  ";
                    line +=  strProcess + "\t" + lem_results[i].FormatAsInPlmLine();
					m_PlmItems.push_back(line);
				}
		}
		else
		{
			m_PlmItems.push_back(strProcess);
		}
	}
}




bool CLemmatizedText::SaveToFile(std::string filename) const
{
	try
	{
		std::ofstream outp(filename.c_str(), std::ios::binary);
		if (!outp.is_open()) return false;
		for (auto i : m_PlmItems) {
			outp <<  i << "\n";
		}
	}
	catch(...)
	{
		return false;
	}
	return true;
}


