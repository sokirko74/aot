#include "LemmaList.h"
#include "fstream"

CLemmaList::CLemmaList() {
    m_Poses = 0;
};

void CLemmaList::set_poses(part_of_speech_mask_t poses) {
    m_Poses = poses;
}

void CLemmaList::add_lemma(const std::string& lemma) {
	std::string  lowerLemma = lemma;
	MakeLowerUtf8(lowerLemma);
	m_WordDict.insert(lowerLemma);
}

bool CLemmaList::has_lemma(const std::string& lemma) const {
	std::string  lowerLemma = lemma;
	MakeLowerUtf8(lowerLemma);
	return m_WordDict.find(lowerLemma) != m_WordDict.end();
};

bool CLemmaList::has_lemma_with_poses(part_of_speech_mask_t poses, const std::string& lemma) const{
	if (!(m_Poses & poses))
		return false;

	std::string  lowerLemma = lemma;
	MakeLowerUtf8(lowerLemma);
	return m_WordDict.find(lowerLemma) != m_WordDict.end();
};

void CLemmaList::read_from_file(std::string path) {
    std::ifstream inp(path.c_str());
    if (!inp.is_open()) {
        throw CExpc("Can't open file \'%s\'.", path.c_str());
    }

    std::string s;
    while (std::getline(inp, s)) {
        int i = s.find("//");
        if (i != std::string::npos)
            s.erase(i);
        Trim(s);
        if (!s.empty()) {
            add_lemma(s);
        }
    };

}

bool CLemmaList::is_empty_list() const {
    return m_WordDict.empty();
}