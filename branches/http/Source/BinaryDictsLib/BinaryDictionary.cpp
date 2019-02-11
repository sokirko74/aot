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
{LI_THEME, "ав"},// -- 789
{LI_LANG, "австрал"},// -- 287
{LI_THEME, "авт"},// -- 314
{LI_LANG, "амер"},// -- 6760
{LI_NONE, "анат"},// -- 1017
{LI_NONE, "антр"},// -- 32
{LI_LANG, "араб"},// -- 69
{LI_NONE, "артикля"},// -- 7
{LI_NONE, "арх"},// -- 1335
{LI_NONE, "археол"},// -- 55
{LI_NONE, "архит"},// -- 452
{LI_NONE, "астр"},// -- 533
{LI_NONE, "афр"},// -- 7
{LI_NONE, "бакт"},// -- 36
{LI_NONE, "банк"},// -- 10
{LI_NONE, "библ"},// -- 342
{LI_THEME, "биол"},// -- 1357
{LI_THEME, "биохим"},// -- 219
{LI_NONE, "бирж"},// -- 90
{LI_NONE, "бот"},// -- 3262
{LI_NONE, "бран"},// -- 11
{LI_NONE, "бум"},// -- 12
{LI_NONE, "бухг"},// -- 39
// "в"},// -- 510
{LI_NONE, "вет"},// -- 153
{LI_NONE, "вм"},// -- 35
//{LI_NONE, "во"},// -- 134 !!!
{LI_THEME, "воен"},// -- 3093
{LI_NONE, "возвр"},// -- 6
{LI_NONE, "возвыш"},// -- 142
{LI_THEME, "вор"},// -- 74
{LI_NONE, "вульг"},// -- 5
{LI_THEME, "вчт"},// -- 533
{LI_LANG, "гавайск"},// -- 5
{LI_NONE, "гельм"},// -- 22
{LI_THEME, "геогр"},// -- 3011
{LI_NONE, "геод"},// -- 45
{LI_THEME, "геол"},// -- 1094
{LI_NONE, "геральд"},// -- 197
{LI_THEME, "гидр"},// -- 69
//"гл"},// -- 234
//"глагола"},// -- 24
{LI_LANG, "голл"},// -- 9
{LI_THEME, "горн"},// -- 779
//"грам"},// -- 800
{LI_LANG, "греч"},// -- 289
{LI_NONE, "груб"},// -- 127
{LI_NONE, "детск"},// -- 60
{LI_NONE, "диал"},// -- 1034
{LI_NONE, "дип"},// -- 56
{LI_THEME, "дор"},// -- 134
{LI_LANG, "др.-греч"},// -- 78
{LI_LANG, "др.-евр"},// -- 8
{LI_LANG, "др.-рим"},// -- 80
{LI_LANG, "египт"},// -- 8
//"ед"},// -- 100
{LI_THEME, "ж.-д"},// -- 247
{LI_NONE, "жарг"},// -- 735
//"женск"},// -- 11
{LI_NONE, "жив"},// -- 111
{LI_NONE, "звукоподр"},// -- 28
//"знач"},// -- 411
{LI_NONE, "зоол"},// -- 2123
//"и"},// -- 317
{LI_LANG, "инд"},// -- 161
{LI_THEME, "информ"},// -- 98
{LI_LANG, "ирл"},// -- 73
{LI_PRAGMA, "ирон"},// -- 231
{LI_THEME, "иск"},// -- 189
{LI_LANG, "исп"},// -- 107
{LI_THEME, "ист"},// -- 2840
{LI_LANG, "ит"},// -- 131
{LI_NONE, "ихт"},// -- 6
//"к"},// -- 12
{LI_LANG, "канад"},// -- 62
{LI_PRAGMA, "канц"},// -- 27
{LI_THEME, "карт"},// -- 211
{LI_NONE, "кино"},// -- 383
{LI_LANG, "кит"},// -- 21
{LI_PRAGMA, "книжн"},// -- 2906
{LI_NONE, "кож"},// -- 25
{LI_NONE, "ком"},// -- 352
{LI_NONE, "косв"},// -- 9
{LI_THEME, "косм"},// -- 175
{LI_THEME, "кул"},// -- 139
{LI_PRAGMA, "ласк"},// -- 29
{LI_LANG, "лат"},// -- 576
{LI_THEME, "лес"},// -- 75
{LI_THEME, "лингв"},// -- 552
{LI_NONE, "лит"},// -- 208
{LI_NONE, "лог"},// -- 161
{LI_LANG, "малайск"},// -- 5
{LI_THEME, "мат"},// -- 959
{LI_THEME, "мед"},// -- 2923
//"междометия"},// -- 41
//"мест"},// -- 10
{LI_THEME, "метал"},// -- 370
{LI_NONE, "метеор"},// -- 215
{LI_NONE, "мин"},// -- 596
{LI_THEME, "миф"},// -- 434
//"мн"},// -- 187
{LI_THEME, "мор"},// -- 1861
{LI_NONE, "муз"},// -- 757
//"нареч"},// -- 50
{LI_LANG, "нем"},// -- 102
{LI_PRAGMA, "неодобр"},// -- 181
{LI_NONE, "неправ"},// -- 24
{LI_NONE, "неприст"},// -- 23
{LI_NONE, "несовр"},// -- 19
{LI_LANG, "новозел"},// -- 26
{LI_PRAGMA, "образн"},// -- 7
//"обыкн"},// -- 691
{LI_THEME, "океан"},// -- 13
//"определённого"},// -- 6
{LI_NONE, "опт"},// -- 158
//"от"},// -- 2390
{LI_PRAGMA, "офиц"},// -- 41
{LI_THEME, "охот"},// -- 150
//"п"},// -- 9
{LI_NONE, "палеонт"},// -- 71
{LI_NONE, "парл"},// -- 73
{LI_THEME, "пед"},// -- 10
{LI_LANG, "перс"},// -- 25
{LI_NONE, "полигр"},// -- 723
{LI_THEME, "полит"},// -- 228
{LI_NONE, "полит.-эк"},// -- 14
{LI_LANG, "польск"},// -- 7
{LI_LANG, "португ"},// -- 9
{LI_NONE, "поэт"},// -- 1150
{LI_NONE, "презр"},// -- 166
//"преим"},// -- 558
{LI_PRAGMA, "пренебр"},// -- 369
//"прил"},// -- 94
{LI_NONE, "прост"},// -- 280
{LI_NONE, "проф"},// -- 331
{LI_NONE, "психиатр"},// -- 23
{LI_THEME, "психол"},// -- 204
{LI_NONE, "пчел"},// -- 16
{LI_NONE, "радио"},// -- 359
{LI_PRAGMA, "разг"},// -- 7577
{LI_NONE, "реакт"},// -- 40
{LI_PRAGMA, "редк"},// -- 3684
{LI_THEME, "рел"},// -- 448
{LI_LANG, "рим"},// -- 57
{LI_NONE, "ритор"},// -- 40
{LI_LANG, "русск"},// -- 139
{LI_THEME, "рыб"},// -- 49
//"с"},// -- 231
{LI_THEME, "с.-х"},// -- 1234
{LI_NONE, "сад"},// -- 64
{LI_LANG, "санскр"},// -- 22
{LI_THEME, "св"},// -- 34
{LI_LANG, "сканд"},// -- 18
{LI_NONE, "сл"},// -- 3241
//"см"},// -- 1728
{LI_NONE, "собир"},// -- 426
//"сокр"},// -- 460
{LI_NONE, "социол"},// -- 35
{LI_THEME, "спец"},// -- 3052
{LI_THEME, "спорт"},// -- 1348
{LI_NONE, "стат"},// -- 52
{LI_NONE, "стил"},// -- 47
{LI_THEME, "стих"},// -- 165
{LI_THEME, "стр"},// -- 545
{LI_NONE, "страх"},// -- 27
{LI_NONE, "студ"},// -- 41
//"сущ"},// -- 170
{LI_NONE, "театр"},// -- 344
{LI_NONE, "текст"},// -- 436
{LI_NONE, "тел"},// -- 28
{LI_THEME, "тех"},// -- 3357
//"тж"},// -- 155
//"тк"},// -- 76
{LI_THEME, "тлв"},// -- 208
{LI_NONE, "топ"},// -- 50
{LI_LANG, "тур"},// -- 24
{LI_THEME, "унив"},// -- 114
//"употр"},// -- 230
//"усил"},// -- 33
{LI_NONE, "уст"},// -- 1730
{LI_NONE, "фарм"},// -- 182
{LI_THEME, "физ"},// -- 1305
{LI_NONE, "физиол"},// -- 382
{LI_THEME, "филос"},// -- 368
{LI_NONE, "фин"},// -- 246
{LI_THEME, "фольк"},// -- 54
//"фон"},// -- 226
{LI_THEME, "фото"},// -- 210
{LI_LANG, "фр"},// -- 635
{LI_THEME, "хим"},// -- 1467
{LI_LANG, "хинди"},// -- 15
{LI_NONE, "церк"},// -- 730
//"ч"},// -- 234
//"часто"},// -- 294
//"чаще"},// -- 25
{LI_THEME, "шахм"},// -- 58
{LI_THEME, "школ"},// -- 122
{LI_LANG, "шотл"},// -- 940
{LI_NONE, "шутл"},// -- 622
{LI_NONE, "эвф"},// -- 101
{LI_THEME, "эк"},// -- 385
{LI_THEME, "эл"},// -- 532
{LI_THEME, "элк"},// -- 179
{LI_PRAGMA, "эмоц.-усил"},// -- 271
{LI_NONE, "энт"},// -- 397
{LI_NONE, "этн"},// -- 11
{LI_LANG, "южно-афр"},// -- 52
{LI_THEME, "юр"},// -- 2037
{LI_LANG, "яп"},// -- 45
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
			text = "сущ";
			break;
		case 1:
			text = "глаг";
			break;
		case 2:
			text = "прил";
			break;
		case 3:
			text = "нареч";
			break;
		}
		break;
	case 1:
		flag = (wp.data[0] & (BYTE)8) != 0; // bit # 3
		if(flag)
			text = "множ";
		break;
	case 2:
		flag = (wp.data[0] & (BYTE)16) != 0; // bit # 4
		if(flag)
			text = "ед";
		break;
	case 3:
		flag = (wp.data[0] & (BYTE)32) != 0; // bit # 5
		if(flag)
			text = "предик";
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
