// BinaryDictionary.cpp : Implementation of CBinaryDictionary
#include "stdafx.h"
#include "../common/utilit.h"
#include "BinaryDictionary.h"
#include "SetOfWordPairs.h"
#include <stdlib.h>
#include "../common/bserialize.h"

#include "../AprRead/all_labels.h"


inline size_t get_size_in_bytes (const WordPair& t)
{
	return get_size_in_bytes(t.rus) + get_size_in_bytes(t.eng) + data_length;

};

inline size_t save_to_bytes(const WordPair& i, BYTE* buf)
{
	buf += save_to_bytes(i.rus, buf);
	buf += save_to_bytes(i.eng, buf);
	memcpy(buf, i.data, data_length);
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(WordPair& i, const BYTE* buf)
{
	buf += restore_from_bytes(i.rus, buf);
	buf += restore_from_bytes(i.eng, buf);
	memcpy(i.data, buf, data_length);
	return get_size_in_bytes(i);
}

label_info all_labels[] = 
{
{LI_NONE, " "},// -- 95
{LI_PART_OF_SPEECH,  "a"}, // -- 23959
{LI_PART_OF_SPEECH,  "adv"}, // -- 2761
{LI_NONE, "cj"},// -- 95
{LI_NONE, "compar"}, // -- 26
{LI_NONE, "conj"},// -- 12
{LI_NONE, "corr"},// -- 8
{LI_NONE, "emph"},// -- 10
{LI_NONE, "indef"},// -- 20
{LI_NONE, "int"},// -- 340
{LI_NONE, "inter"},// -- 15
{LI_PART_OF_SPEECH, "n"}, // -- 61317
{LI_NONE, "num"},// -- 28
{LI_NONE, "p"},// -- 947
{LI_NONE, "part"},// -- 5
{LI_NONE, "pass"},// -- 146
{LI_NONE, "past"},// -- 429
{LI_NONE, "pers"},// -- 16
{LI_NONE, "phr"},// -- 2015
{LI_PL, "pl"}, // -- 4318
{LI_NONE, "poss"},// -- 13
{LI_PREDIC, "predic"}, // -- 175
{LI_NONE, "pref"},// -- 84
{LI_NONE, "prep"},// -- 157
{LI_NONE, "pres"},// -- 5
{LI_NONE, "pron"},// -- 84
{LI_NONE, "refl"},// -- 104
{LI_NONE, "rel"},// -- 16
{LI_SINGLE, "sing"}, // -- 89
{LI_NONE, "suff"},// -- 177
{LI_NONE, "superl"},// -- 17
{LI_PART_OF_SPEECH, "v"}, // -- 12836
{LI_THEME, "��"},// -- 789
{LI_LANG, "�������"},// -- 287
{LI_THEME, "���"},// -- 314
{LI_LANG, "����"},// -- 6760
{LI_NONE, "����"},// -- 1017
{LI_NONE, "����"},// -- 32
{LI_LANG, "����"},// -- 69
{LI_NONE, "�������"},// -- 7
{LI_NONE, "���"},// -- 1335
{LI_NONE, "������"},// -- 55
{LI_NONE, "�����"},// -- 452
{LI_NONE, "����"},// -- 533
{LI_NONE, "���"},// -- 7
{LI_NONE, "����"},// -- 36
{LI_NONE, "����"},// -- 10
{LI_NONE, "����"},// -- 342
{LI_THEME, "����"},// -- 1357
{LI_THEME, "������"},// -- 219
{LI_NONE, "����"},// -- 90
{LI_NONE, "���"},// -- 3262
{LI_NONE, "����"},// -- 11
{LI_NONE, "���"},// -- 12
{LI_NONE, "����"},// -- 39
// "�"},// -- 510
{LI_NONE, "���"},// -- 153
{LI_NONE, "��"},// -- 35
//{LI_NONE, "��"},// -- 134 !!!
{LI_THEME, "����"},// -- 3093
{LI_NONE, "�����"},// -- 6
{LI_NONE, "������"},// -- 142
{LI_THEME, "���"},// -- 74
{LI_NONE, "�����"},// -- 5
{LI_THEME, "���"},// -- 533
{LI_LANG, "�������"},// -- 5
{LI_NONE, "�����"},// -- 22
{LI_THEME, "�����"},// -- 3011
{LI_NONE, "����"},// -- 45
{LI_THEME, "����"},// -- 1094
{LI_NONE, "�������"},// -- 197
{LI_THEME, "����"},// -- 69
//"��"},// -- 234
//"�������"},// -- 24
{LI_LANG, "����"},// -- 9
{LI_THEME, "����"},// -- 779
//"����"},// -- 800
{LI_LANG, "����"},// -- 289
{LI_NONE, "����"},// -- 127
{LI_NONE, "�����"},// -- 60
{LI_NONE, "����"},// -- 1034
{LI_NONE, "���"},// -- 56
{LI_THEME, "���"},// -- 134
{LI_LANG, "��.-����"},// -- 78
{LI_LANG, "��.-���"},// -- 8
{LI_LANG, "��.-���"},// -- 80
{LI_LANG, "�����"},// -- 8
//"��"},// -- 100
{LI_THEME, "�.-�"},// -- 247
{LI_NONE, "����"},// -- 735
//"�����"},// -- 11
{LI_NONE, "���"},// -- 111
{LI_NONE, "���������"},// -- 28
//"����"},// -- 411
{LI_NONE, "����"},// -- 2123
//"�"},// -- 317
{LI_LANG, "���"},// -- 161
{LI_THEME, "������"},// -- 98
{LI_LANG, "���"},// -- 73
{LI_PRAGMA, "����"},// -- 231
{LI_THEME, "���"},// -- 189
{LI_LANG, "���"},// -- 107
{LI_THEME, "���"},// -- 2840
{LI_LANG, "��"},// -- 131
{LI_NONE, "���"},// -- 6
//"�"},// -- 12
{LI_LANG, "�����"},// -- 62
{LI_PRAGMA, "����"},// -- 27
{LI_THEME, "����"},// -- 211
{LI_NONE, "����"},// -- 383
{LI_LANG, "���"},// -- 21
{LI_PRAGMA, "�����"},// -- 2906
{LI_NONE, "���"},// -- 25
{LI_NONE, "���"},// -- 352
{LI_NONE, "����"},// -- 9
{LI_THEME, "����"},// -- 175
{LI_THEME, "���"},// -- 139
{LI_PRAGMA, "����"},// -- 29
{LI_LANG, "���"},// -- 576
{LI_THEME, "���"},// -- 75
{LI_THEME, "�����"},// -- 552
{LI_NONE, "���"},// -- 208
{LI_NONE, "���"},// -- 161
{LI_LANG, "�������"},// -- 5
{LI_THEME, "���"},// -- 959
{LI_THEME, "���"},// -- 2923
//"����������"},// -- 41
//"����"},// -- 10
{LI_THEME, "�����"},// -- 370
{LI_NONE, "������"},// -- 215
{LI_NONE, "���"},// -- 596
{LI_THEME, "���"},// -- 434
//"��"},// -- 187
{LI_THEME, "���"},// -- 1861
{LI_NONE, "���"},// -- 757
//"�����"},// -- 50
{LI_LANG, "���"},// -- 102
{LI_PRAGMA, "�������"},// -- 181
{LI_NONE, "������"},// -- 24
{LI_NONE, "�������"},// -- 23
{LI_NONE, "������"},// -- 19
{LI_LANG, "�������"},// -- 26
{LI_PRAGMA, "������"},// -- 7
//"�����"},// -- 691
{LI_THEME, "�����"},// -- 13
//"������������"},// -- 6
{LI_NONE, "���"},// -- 158
//"��"},// -- 2390
{LI_PRAGMA, "����"},// -- 41
{LI_THEME, "����"},// -- 150
//"�"},// -- 9
{LI_NONE, "�������"},// -- 71
{LI_NONE, "����"},// -- 73
{LI_THEME, "���"},// -- 10
{LI_LANG, "����"},// -- 25
{LI_NONE, "������"},// -- 723
{LI_THEME, "�����"},// -- 228
{LI_NONE, "�����.-��"},// -- 14
{LI_LANG, "������"},// -- 7
{LI_LANG, "������"},// -- 9
{LI_NONE, "����"},// -- 1150
{LI_NONE, "�����"},// -- 166
//"�����"},// -- 558
{LI_PRAGMA, "�������"},// -- 369
//"����"},// -- 94
{LI_NONE, "�����"},// -- 280
{LI_NONE, "����"},// -- 331
{LI_NONE, "��������"},// -- 23
{LI_THEME, "������"},// -- 204
{LI_NONE, "����"},// -- 16
{LI_NONE, "�����"},// -- 359
{LI_PRAGMA, "����"},// -- 7577
{LI_NONE, "�����"},// -- 40
{LI_PRAGMA, "����"},// -- 3684
{LI_THEME, "���"},// -- 448
{LI_LANG, "���"},// -- 57
{LI_NONE, "�����"},// -- 40
{LI_LANG, "�����"},// -- 139
{LI_THEME, "���"},// -- 49
//"�"},// -- 231
{LI_THEME, "�.-�"},// -- 1234
{LI_NONE, "���"},// -- 64
{LI_LANG, "������"},// -- 22
{LI_THEME, "��"},// -- 34
{LI_LANG, "�����"},// -- 18
{LI_NONE, "��"},// -- 3241
//"��"},// -- 1728
{LI_NONE, "�����"},// -- 426
//"����"},// -- 460
{LI_NONE, "������"},// -- 35
{LI_THEME, "����"},// -- 3052
{LI_THEME, "�����"},// -- 1348
{LI_NONE, "����"},// -- 52
{LI_NONE, "����"},// -- 47
{LI_THEME, "����"},// -- 165
{LI_THEME, "���"},// -- 545
{LI_NONE, "�����"},// -- 27
{LI_NONE, "����"},// -- 41
//"���"},// -- 170
{LI_NONE, "�����"},// -- 344
{LI_NONE, "�����"},// -- 436
{LI_NONE, "���"},// -- 28
{LI_THEME, "���"},// -- 3357
//"��"},// -- 155
//"��"},// -- 76
{LI_THEME, "���"},// -- 208
{LI_NONE, "���"},// -- 50
{LI_LANG, "���"},// -- 24
{LI_THEME, "����"},// -- 114
//"�����"},// -- 230
//"����"},// -- 33
{LI_NONE, "���"},// -- 1730
{LI_NONE, "����"},// -- 182
{LI_THEME, "���"},// -- 1305
{LI_NONE, "������"},// -- 382
{LI_THEME, "�����"},// -- 368
{LI_NONE, "���"},// -- 246
{LI_THEME, "�����"},// -- 54
//"���"},// -- 226
{LI_THEME, "����"},// -- 210
{LI_LANG, "��"},// -- 635
{LI_THEME, "���"},// -- 1467
{LI_LANG, "�����"},// -- 15
{LI_NONE, "����"},// -- 730
//"�"},// -- 234
//"�����"},// -- 294
//"����"},// -- 25
{LI_THEME, "����"},// -- 58
{LI_THEME, "����"},// -- 122
{LI_LANG, "����"},// -- 940
{LI_NONE, "����"},// -- 622
{LI_NONE, "���"},// -- 101
{LI_THEME, "��"},// -- 385
{LI_THEME, "��"},// -- 532
{LI_THEME, "���"},// -- 179
{LI_PRAGMA, "����.-����"},// -- 271
{LI_NONE, "���"},// -- 397
{LI_NONE, "���"},// -- 11
{LI_LANG, "����-���"},// -- 52
{LI_THEME, "��"},// -- 2037
{LI_LANG, "��"},// -- 45
};

const label_info* GetTransLabels()
{
	return all_labels;
}

const size_t GetTransLabelsCount()
{
	return sizeof(all_labels)/sizeof(all_labels[0]);
}

/////////////////////////////////////////////////////////////////////////////
// CBinaryDictionary


int compare1(const void *p1, const void *p2)
{
	WordPair *w1 = (WordPair *)p1;
	WordPair *w2 = (WordPair *)p2;
	if(w1->eng != w2->eng) return w1->eng - w2->eng;
	if(w1->rus != w2->rus) return w1->rus - w2->rus;
	return 0;
}

int compare2(const void *p1, const void *p2)
{
	WordPair **w1 = (WordPair **)p1;
	WordPair **w2 = (WordPair **)p2;
	if((*w1)->eng != (*w2)->eng) return (*w1)->eng - (*w2)->eng;
	if((*w1)->rus != (*w2)->rus) return (*w1)->rus - (*w2)->rus;
	return 0;
}

bool LessRus1 (const WordPair& w1, const WordPair& w2) 
{
	return (w1.rus < w2.rus) || ((w1.rus == w2.rus) && (w1.eng < w2.eng));
}

bool LessRus (const WordPair& w1, const WordPair& w2) 
{
	return (w1.rus < w2.rus);
}

bool LessEng (const WordPair &w1, const WordPair &w2) 
{
	return (w1.eng < w2.eng);
}


bool LessEng1 (const WordPair &w1, const WordPair &w2) 
{
	return (w1.eng < w2.eng) || ((w1.eng==w2.eng) && (w1.rus < w2.rus));
}


CBinaryDictionary::CBinaryDictionary()
{
	eng_vec.reserve(260000);
}

std::string CBinaryDictionary::get_dict_path() const
{
	
	return std::string(GetRegistryString("Software\\Dialing\\ENGLISH_RUSSIAN_DICT\\DictPath"));
}


bool CBinaryDictionary::Load()
{
	fprintf (stderr, "Reading binary dict from %s\n", get_dict_path().c_str());
	ReadVector(get_dict_path(),eng_vec);
	if (eng_vec.empty())
		return false;

	std::sort(eng_vec.begin(), eng_vec.end(), LessEng1);


	rus_vec = eng_vec;;
	std::sort(rus_vec.begin(), rus_vec.end(), LessRus1);
	return true;
}


//---------------------------------------------------------------------------
long CBinaryDictionary::GetIndirectId(long index) const
{
	return rus_vec[index].eng;
}

long CBinaryDictionary::GetDirectId(long index) const
{
	return eng_vec[index].rus;
}

//---------------------------------------------------------------------------

long CBinaryDictionary::GetFlagCount() const
{
	return 8;
}
//---------------------------------------------------------------------------
bool CBinaryDictionary::GetFlag(long pair_index, long flag_index, string& text, bool is_direct, DWORD& flag) const
{
	// TODO: Add your implementation code here
	const WordPair& wp = (is_direct) ? eng_vec[pair_index] : rus_vec[pair_index];
	
	
	switch(flag_index){
	case 0:
		flag = wp.data[0] & (BYTE)7; // first 3 bits
		switch(flag){
		case 0:
			text = "���";
			break;
		case 1:
			text = "����";
			break;
		case 2:
			text = "����";
			break;
		case 3:
			text = "�����";
			break;
		}
		break;
	case 1:
		flag = (wp.data[0] & (BYTE)8) != 0; // bit # 3
		if(flag)
			text = "����";
		break;
	case 2:
		flag = (wp.data[0] & (BYTE)16) != 0; // bit # 4
		if(flag)
			text = "��";
		break;
	case 3:
		flag = (wp.data[0] & (BYTE)32) != 0; // bit # 5
		if(flag)
			text = "������";
		break;
	case 4:
		flag = wp.data[1];
		if(flag)
			text = const_cast<char*>(all_labels[flag].str);
		break;
	case 5:
		flag = wp.data[2];
		if(flag)
			text = const_cast<char*>(all_labels[flag].str);
		break;
	case 6:
		flag = wp.data[3];
		if(flag)
			text = const_cast<char*>(all_labels[flag].str);
		break;
	case 7:
		flag = wp.data[4];
		if(flag)
			text = const_cast<char*>(all_labels[flag].str);
		break;
	default:
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------



CSetOfWordPairs CBinaryDictionary::TranslateDirect(long id) const
{
	// TODO: Add your implementation code here
	typedef std::vector<WordPair>::const_iterator iter;
	WordPair wp;
	wp.eng = id;

	std::pair<iter, iter> range = std::equal_range(eng_vec.begin(), eng_vec.end(), wp, LessEng);
	int begin = range.first - eng_vec.begin();
	int end = range.second - eng_vec.begin();

	CSetOfWordPairs p;
	p.SetDictionary(this, true);
	p.SetRange(begin, end);
	return p;
}

CSetOfWordPairs CBinaryDictionary::TranslateIndirect(long id) const
{
	// TODO: Add your implementation code here
	typedef std::vector<WordPair>::const_iterator iter;
	WordPair wp;
	wp.rus = id;

	std::pair<iter, iter> range = std::equal_range(rus_vec.begin(), rus_vec.end(), wp, LessRus);
	int begin = range.first - rus_vec.begin();
	int end = range.second - rus_vec.begin();

	CSetOfWordPairs p;
	p.SetDictionary(this, false);
	p.SetRange(begin, end);
	return p;
}

bool CBinaryDictionary::HavePair(long id1, long id2) const
{
	typedef std::vector<WordPair>::const_iterator iter;
	WordPair wp;
	wp.eng = id1;

	std::pair<iter, iter> range = std::equal_range(eng_vec.begin(), eng_vec.end(), wp, LessEng);
	for(iter i = range.first; i != range.second; ++i){
		if(i->rus == id2){
			return true;
		}
	}
	return false;
}
