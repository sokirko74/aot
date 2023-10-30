#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>

#include "morph_dict/common/util_classes.h"
#include "morph_dict/lemmatizer_base_lib/MorphanHolder.h"

CMorphanHolder MorphHolderRus;
CMorphanHolder MorphHolderEng;


void make_bin(std::string &r, std::string &rc, std::string &e, std::string &ec, uint32_t d[5], std::ostream &out)
{
	DwordVector r_id = MorphHolderRus.GetParadigmIdsByNormAndAncode(r, rc);
	DwordVector e_id = MorphHolderEng.GetParadigmIdsByNormAndAncode(e, ec);
	//out << r_id << '\t';
	//out << e_id << " ";
	//out << d[0] << " " << d[1] << " " << d[2] << " " << d[3] << " " << d[4];
	//out << endl;
	//if (r_id.empty())
	//		printf ("%s %s is not found in the morph. dictionary!\n",r.c_str(), rc.c_str());
	//if (e_id.empty())
	//		printf ("%s %s is not found in the morph. dictionary!\n",e.c_str(), ec.c_str());
	for(int i = 0; i < r_id.size(); i++){
		for(int j = 0; j < e_id.size(); j++){
			out.write((char*)&(r_id[i]), sizeof(uint32_t));
			out.write((char*)&(e_id[j]), sizeof(uint32_t));
			unsigned char D[5];
			for(int k = 0; k < 5; k++) D[k] = d[k];
			out.write((char*)D, 5);
		}
	}
}

void get_english_ancode(std::string e, std::string &ec)
{
    std::vector<CFormInfo> ParadigmCollection;

	if (!MorphHolderEng.m_pLemmatizer->CreateParadigmCollection(true, e, true, false, ParadigmCollection))
		throw CExpc(Format("Cannot lemmatize %s by English lemmatizer" , e.c_str()));

//	assert(count > 0);
	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;
		std::string ancode = Paradigm.GetAncode(0);
		if (ec=="na" && ( ancode=="nc" || ancode=="ne" || ancode=="ni"))	
		{
			ec = ancode;
			return;
		};
		if (ec=="nb" && ( ancode=="nd" || ancode=="ng" || ancode=="nk"))	
		{
			ec = ancode;
			return;
		};
	}
}


int main(int argc, char ** argv)
{
	try 
	{
			assert(argc >= 3);
			bool Bug = ( (argc == 4) && !strcmp(argv[3],"txt") );
			std::ifstream in(argv[1]);
			std::ofstream out;
			if (!Bug)
			  out.open(argv[2], std::ios::binary);
			else
			  out.open(argv[2]);

			MorphHolderRus.LoadLemmatizer(morphRussian);
			MorphHolderEng.LoadLemmatizer(morphEnglish);

			int i = 0;
			FILE * fp = fopen(argv[1],"r");
			if (!fp)
			{
				fprintf (stderr, "Cannot open file %s", argv[1]);
				return 0;
			};
			int LineNo = 0;
			char buffer[10000];
			while(fgets(buffer, 10000, fp))
			{
				LineNo++;
				StringTokenizer tok(convert_from_utf8(buffer,morphRussian).c_str(),  "\r\n\t ");
				std::string r  = tok.next_token();
				std::string rc = tok.next_token();
				std::string e = tok.next_token();
				std::string ec = tok.next_token();
				uint32_t d[5];
				if (sscanf(tok.get_rest(),"%i %i %i %i %i", &d[0], &d[1], &d[2], &d[3], &d[4]) != 5)
				{
					fprintf (stderr," Bad format at line %s (line no = %i)\n", buffer, LineNo);
					return 0;
				};
				
				if ( Bug)
				{
					std::string save_ec = ec;
					get_english_ancode(e,ec);
					out << r <<" "<< rc << " "<< e << " "<<  ec;
					out << " "<< d[0] <<" "<< d[1] <<" "<< d[2] <<" "<< d[3] <<" " <<d[4];
					out << std::endl;
					if (save_ec !=  ec)
						std::cerr<<e<<std::endl;
				}
				else
				  make_bin(r, rc, e, ec, d, out);
				if(i%1000==0)  std::cerr << i << '\r';
				i++;
			}
			fclose (fp);
			out.close();

			return 0;
	}
	catch (CExpc e)
	{
		fprintf (stderr, "exception occurred: %s!\n", e.m_strCause.c_str());
		return 1;
	}
	catch (...)
	{
		fprintf (stderr, "some exception occurred!\n");
		return 1;
	}
}
