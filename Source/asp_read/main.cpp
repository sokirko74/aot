#include "../common/utilit.h"
#include "../common/bserialize.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../LemmatizerLib/MorphologyHolder.h"
#include <utility>


void get_id(CMorphologyHolder& morphHolder, std::string str, DwordVector &res, bool is_left)
{
	
    vector<CFormInfo> ParadigmCollection;

	std::string s = str;
	if (!morphHolder.m_pLemmatizer->CreateParadigmCollection(false, s, false, false,  ParadigmCollection))
		throw CExpc(Format("Cannot lemmatize %s by Russian lemmatizer" , str.c_str()));

	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;
		UINT part = morphHolder.m_pGramTab->GetPartOfSpeech(Paradigm.GetSrcAncode().c_str());
		if(part != INFINITIVE)
			continue;
		QWORD gr = morphHolder.m_pGramTab->GetAllGrammems(Paradigm.GetCommonAncode().c_str());
		bool is_perfective = (gr & _QM(rPerfective)) != 0;
		bool is_nonperfective = (gr & _QM(rNonPerfective)) != 0;
		std::string norm = Paradigm.GetWordForm(0);
		for(int i = 0; i < norm.size(); i++)
			norm[i] = norm[i] - _R("А")[0] + _R("а")[0];
		if(norm != str) continue;

		if(is_perfective && is_nonperfective) 
		{
			///std::cerr << "двувидовой:" << str.c_str() << std::endl;
		}
		if(!is_perfective && !is_nonperfective) 
		{
			//std::cerr << "нет граммемы вида:" << str.c_str() << std::endl;
		};
		
		if(is_left && is_nonperfective || !is_left && is_perfective)
			res.push_back(Paradigm.GetParadigmId());
	}
}


int main(int argc, char ** argv)
{
	CMorphologyHolder morphHolderRus;

	if (argc != 3)
	{
		std::cerr << "bad number of arguments, need 3, " << argc << " are provided\n";
		return 1;
	}

	if (!morphHolderRus.LoadGraphanAndLemmatizer(morphRussian))
	{
		std::cerr << "cannot load Russian morphology\n";
		return 1;
	}

	try
	{
		std::vector<std::pair<DWORD, DWORD>> pairs;

		{	// reading

			std::ifstream in(argv[1]);
			assert(in.is_open());
			std::string line;
			while(std::getline(in, line))
			{
				line = convert_from_utf8(line.c_str(), morphRussian);
				Trim(line);
				if (line.empty()) {
					continue;
				}
				StringTokenizer tok(line.c_str(), "\t");
				std::string l = tok();
				std::string r = tok();
				Trim(l);
				Trim(r);
				if (l.back() == '*') {
					l.erase(l.end() - 1);
				}

				if(r.back() == '*') {
					r.erase(r.end() - 1);
				}

				DwordVector l_vec, r_vec;

				get_id(morphHolderRus, l, l_vec, true);
				get_id(morphHolderRus, r, r_vec, false);

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
	catch (CExpc e)
	{
		std::cerr << "exception occurred:" << e.m_strCause << "\n";
		return 1;
	}
	catch (...)
	{
		std::cerr << "some exception occurred!\n";
		return 1;
	}

	return 0;
}
