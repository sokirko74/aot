#ifndef INCL_ALL_LABELS_H
#define INCL_ALL_LABELS_H

// the labels are sorted here
enum{
	LI_PART_OF_SPEECH,
	LI_PL,
	LI_PREDIC,
	LI_SINGLE,
	LI_PRAGMA,
	LI_THEME,
	LI_LANG,
	LI_NONE
};
struct label_info{
	int type;
	const char * str;
};


//! return index of label, or -1 if no such
extern int find_label(char *label);
// i is an index in array of labels
extern const char *get_label_string(int i);
extern int get_label_kind(int i);
extern const label_info* GetTransLabels();
const size_t GetTransLabelsCount();


#endif // INCL_ALL_LABELS_H
