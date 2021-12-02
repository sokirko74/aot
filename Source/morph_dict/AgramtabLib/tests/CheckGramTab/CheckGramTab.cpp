#include "morph_dict/AgramtabLib/RusGramTab.h"
#include "morph_dict/AgramtabLib/EngGramTab.h"
#include "morph_dict/AgramtabLib/GerGramTab.h"


void PrintUsage()
{
	std::cout << "Usage: CheckGramTab  (Russian|English|German)\n" <<
		 "Example: CheckGramTab  Russian\n";
	exit(-1);
};

bool Check(CAgramtab* gramtab) 
{
	try {
		gramtab->LoadFromRegistry();

		for (uint16_t i = 0; i < gramtab->GetMaxGrmCount(); i++) {
			for (uint16_t k = 0; k < gramtab->GetMaxGrmCount(); k++) {
				if ((gramtab->GetLine(i) != nullptr) && (gramtab->GetLine(k) != nullptr) && (i != k))
				{
					if ((gramtab->GetLine(i)->m_Grammems == gramtab->GetLine(k)->m_Grammems) && 
						(gramtab->GetLine(i)->m_PartOfSpeech == gramtab->GetLine(k)->m_PartOfSpeech))
					{
						std::cerr << "a dublicate found " << gramtab->LineIndexToGramcode(i) << " ? " << gramtab->LineIndexToGramcode(k) << "\n";
						return false;
					};
				}
			}

		}
		return true;
	}
	catch (CExpc e) {
		std::cerr << e.m_strCause << "\n";
		return false;
	}

}

int main(int argc, char* argv[])
{
	if (argc != 2)
		PrintUsage();
		
	std::string Lng = argv[1];

	if (Lng == "Russian")
	{
		CRusGramTab P;
		if (!Check(&P)) return 1;
	}
	else
		if (Lng == "English")
		{
			CEngGramTab P;
			if (!Check(&P)) return 1;
		}
	else
		if (Lng == "German")
		{
			CGerGramTab P;
			if (!Check(&P)) return 1;
		}
		else
			PrintUsage();
	return 0;
}

