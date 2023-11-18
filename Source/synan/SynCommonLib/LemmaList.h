#pragma once

#include "morph_dict/common/utilit.h"

class CLemmaList {
    part_of_speech_mask_t m_Poses; // ignored in  CLemmaList::has_lemma
    StringHashSet m_WordDict; // lowercased lemmas

public:
    CLemmaList();

    void set_poses(part_of_speech_mask_t poses);

    void add_lemma(const std::string& lemma);
  
    bool has_lemma(const std::string& item) const;

    bool has_lemma_with_poses(part_of_speech_mask_t poses, const std::string& item) const;

    void read_from_file(std::string path);

    bool is_empty_list() const;
};
