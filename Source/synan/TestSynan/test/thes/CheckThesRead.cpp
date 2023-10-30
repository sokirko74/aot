#include "synan/SynCommonLib/ThesaurusForSyntax.h"
#include "morph_dict/agramtab/RusGramTab.h"
#include "morph_dict/agramtab/EngGramTab.h"
#include <fstream>

#undef NDEBUG  // enable asserts

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cerr << "specify thes folder to test";
		return 1;
	}

	CThesaurus* T = new CThesaurus;
	if (!T) return 0;
	T->m_MainLanguage = morphRussian;

	CRusGramTab RusGramTab;
	RusGramTab.LoadFromRegistry();
	T->m_pMainGramTab = &RusGramTab;

	CEngGramTab EngGramTab;
	EngGramTab.LoadFromRegistry();
	T->m_pEngGramTab = &EngGramTab;

	CDictionary OborDic;
	auto path = GetRegistryString("Software\\Dialing\\Obor\\DictPath");
	OborDic.Load(path.c_str());
	T->m_pOborDictionary = &OborDic;
	std::string input_directory = argv[1];
	T->SetDirectory(input_directory);
	if (!T->ReadThesaurusFromDisk()) return 1;
	auto canon_path = std::filesystem::path(input_directory) / "atomic_group.canon.txt";
	std::ifstream inp;
	inp.open(canon_path);
	size_t error_count = 0;
	for (auto m : T->m_Models) {
		for (auto a : m.m_AtomGroups) {
			char canon_line[100];
			inp.getline(canon_line, 99);
			std::stringstream ss;
			ss << m.m_ModelId << " " << a.m_LanguageId << " " << (int)a.m_PartOfSpeech << " " << a.m_Grammems;
			if (ss.str() != canon_line) {
				std::cerr << "error in model_id = " << m.m_ModelId << " canon = " << canon_line << " result line " << ss.str() << "\n";
				++error_count;
			}
		}
	}

	inp.close();


	//std::ofstream outp;
	//outp.open(canon_path);
	//for (auto m : T->m_Models) {
	//	for (auto a : m.m_AtomGroups) {
	//		outp << m.m_ModelId << " " << a.m_LanguageId << " " << (int)a.m_PartOfSpeech << " " << a.m_Grammems << "\n";
	//	}
	//}
	//outp.close();

	delete T;
	return error_count > 0  ? 1 : 0;
	
}

