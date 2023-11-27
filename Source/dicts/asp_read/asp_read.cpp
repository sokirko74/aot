#include "morph_dict/common/utilit.h"
#include "morph_dict/common/bserialize.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"
#include <utility>


void get_id(CMorphanHolder& holder, std::string str, grammems_mask_t  grammems, DwordVector &res)
{
	
    std::vector<CFormInfo> ParadigmCollection;

	std::string word_s8 = convert_from_utf8(str.c_str(), holder.m_CurrentLanguage);
	if (!holder.m_pLemmatizer->CreateParadigmCollection(false, word_s8, false, false,  ParadigmCollection))
		throw CExpc("Cannot lemmatize %s by Russian lemmatizer" , str.c_str());

	for(auto& p: ParadigmCollection)
	{
		if(!p.m_bFound) continue;
		part_of_speech_t part = holder.m_pGramTab->GetPartOfSpeech(p.GetSrcAncode().c_str());
		if(part != INFINITIVE)
			continue;
		if ((grammems & holder.m_pGramTab->GetAllGrammems(p.GetCommonAncode().c_str())) != grammems) {
			continue;
		};
		std::string norm = p.GetWordForm(0);
		EngRusMakeLower(norm);
		if (norm != word_s8) {
			res.push_back(p.GetParadigmId());
		}
		else {
			std::cerr << "ignore perf-nonperf pair " << convert_to_utf8(word_s8, morphRussian) << "\n";
		}
	}
}


int main(int argc, char ** argv)
{
	CMorphanHolder morphHolderRus;

	if (argc != 3)
	{
		std::cerr << "bad number of arguments, need 3, " << argc << " are provided\n";
		return 1;
	}

	try
	{
		morphHolderRus.LoadMorphology(morphRussian);

		std::vector<std::pair<uint32_t, uint32_t>> pairs;

		{	// reading

			std::ifstream in(argv[1]);
			assert(in.is_open());
			std::string line;
			while(std::getline(in, line))
			{
				Trim(line);
				if (line.empty()) {
					continue;
				}
				StringTokenizer tok(line.c_str(), "\t");
				std::string l = tok();
				std::string r = tok();

				DwordVector l_vec, r_vec;

				get_id(morphHolderRus, l, _QM(rNonPerfective), l_vec);
				get_id(morphHolderRus, r, _QM(rPerfective), r_vec);

				for(auto l_id : l_vec)
				{
					for(auto r_id : r_vec){
						pairs.push_back({ l_id, r_id });
					}
				}
			}
			in.close();
		}
		WriteVector(argv[2], pairs);
		std::cerr << "written " << pairs.size() << " verb aspect pairs\n";
	}
	catch (std::exception& e)
	{
		std::cerr << "exception occurred:" << e.what() << "\n";
		return 1;
	}
	catch (...)
	{
		std::cerr << "some exception occurred!\n";
		return 1;
	}

	return 0;
}
