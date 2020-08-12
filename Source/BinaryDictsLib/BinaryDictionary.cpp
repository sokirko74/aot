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
{LI_THEME, _R("ав")},// -- 789
{LI_LANG, _R("австрал")},// -- 287
{LI_THEME, _R("авт")},// -- 314
{LI_LANG, _R("амер")},// -- 6760
{LI_NONE, _R("анат")},// -- 1017
{LI_NONE, _R("антр")},// -- 32
{LI_LANG, _R("араб")},// -- 69
{LI_NONE, _R("артикля")},// -- 7
{LI_NONE, _R("арх")},// -- 1335
{LI_NONE, _R("археол")},// -- 55
{LI_NONE, _R("архит")},// -- 452
{LI_NONE, _R("астр")},// -- 533
{LI_NONE, _R("афр")},// -- 7
{LI_NONE, _R("бакт")},// -- 36
{LI_NONE, _R("банк")},// -- 10
{LI_NONE, _R("библ")},// -- 342
{LI_THEME, _R("биол")},// -- 1357
{LI_THEME, _R("биохим")},// -- 219
{LI_NONE, _R("бирж")},// -- 90
{LI_NONE, _R("бот")},// -- 3262
{LI_NONE, _R("бран")},// -- 11
{LI_NONE, _R("бум")},// -- 12
{LI_NONE, _R("бухг")},// -- 39
// "в")},// -- 510
{LI_NONE, _R("вет")},// -- 153
{LI_NONE, _R("вм")},// -- 35
//{LI_NONE, _R("во")},// -- 134 !!!
{LI_THEME, _R("воен")},// -- 3093
{LI_NONE, _R("возвр")},// -- 6
{LI_NONE, _R("возвыш")},// -- 142
{LI_THEME, _R("вор")},// -- 74
{LI_NONE, _R("вульг")},// -- 5
{LI_THEME, _R("вчт")},// -- 533
{LI_LANG, _R("гавайск")},// -- 5
{LI_NONE, _R("гельм")},// -- 22
{LI_THEME, _R("геогр")},// -- 3011
{LI_NONE, _R("геод")},// -- 45
{LI_THEME, _R("геол")},// -- 1094
{LI_NONE, _R("геральд")},// -- 197
{LI_THEME, _R("гидр")},// -- 69
//"гл")},// -- 234
//"глагола")},// -- 24
{LI_LANG, _R("голл")},// -- 9
{LI_THEME, _R("горн")},// -- 779
//"грам")},// -- 800
{LI_LANG, _R("греч")},// -- 289
{LI_NONE, _R("груб")},// -- 127
{LI_NONE, _R("детск")},// -- 60
{LI_NONE, _R("диал")},// -- 1034
{LI_NONE, _R("дип")},// -- 56
{LI_THEME, _R("дор")},// -- 134
{LI_LANG, _R("др.-греч")},// -- 78
{LI_LANG, _R("др.-евр")},// -- 8
{LI_LANG, _R("др.-рим")},// -- 80
{LI_LANG, _R("египт")},// -- 8
//"ед")},// -- 100
{LI_THEME, _R("ж.-д")},// -- 247
{LI_NONE, _R("жарг")},// -- 735
//"женск")},// -- 11
{LI_NONE, _R("жив")},// -- 111
{LI_NONE, _R("звукоподр")},// -- 28
//"знач")},// -- 411
{LI_NONE, _R("зоол")},// -- 2123
//"и")},// -- 317
{LI_LANG, _R("инд")},// -- 161
{LI_THEME, _R("информ")},// -- 98
{LI_LANG, _R("ирл")},// -- 73
{LI_PRAGMA, _R("ирон")},// -- 231
{LI_THEME, _R("иск")},// -- 189
{LI_LANG, _R("исп")},// -- 107
{LI_THEME, _R("ист")},// -- 2840
{LI_LANG, _R("ит")},// -- 131
{LI_NONE, _R("ихт")},// -- 6
//"к")},// -- 12
{LI_LANG, _R("канад")},// -- 62
{LI_PRAGMA, _R("канц")},// -- 27
{LI_THEME, _R("карт")},// -- 211
{LI_NONE, _R("кино")},// -- 383
{LI_LANG, _R("кит")},// -- 21
{LI_PRAGMA, _R("книжн")},// -- 2906
{LI_NONE, _R("кож")},// -- 25
{LI_NONE, _R("ком")},// -- 352
{LI_NONE, _R("косв")},// -- 9
{LI_THEME, _R("косм")},// -- 175
{LI_THEME, _R("кул")},// -- 139
{LI_PRAGMA, _R("ласк")},// -- 29
{LI_LANG, _R("лат")},// -- 576
{LI_THEME, _R("лес")},// -- 75
{LI_THEME, _R("лингв")},// -- 552
{LI_NONE, _R("лит")},// -- 208
{LI_NONE, _R("лог")},// -- 161
{LI_LANG, _R("малайск")},// -- 5
{LI_THEME, _R("мат")},// -- 959
{LI_THEME, _R("мед")},// -- 2923
//"междометия")},// -- 41
//"мест")},// -- 10
{LI_THEME, _R("метал")},// -- 370
{LI_NONE, _R("метеор")},// -- 215
{LI_NONE, _R("мин")},// -- 596
{LI_THEME, _R("миф")},// -- 434
//"мн")},// -- 187
{LI_THEME, _R("мор")},// -- 1861
{LI_NONE, _R("муз")},// -- 757
//"нареч")},// -- 50
{LI_LANG, _R("нем")},// -- 102
{LI_PRAGMA, _R("неодобр")},// -- 181
{LI_NONE, _R("неправ")},// -- 24
{LI_NONE, _R("неприст")},// -- 23
{LI_NONE, _R("несовр")},// -- 19
{LI_LANG, _R("новозел")},// -- 26
{LI_PRAGMA, _R("образн")},// -- 7
//"обыкн")},// -- 691
{LI_THEME, _R("океан")},// -- 13
//"определённого")},// -- 6
{LI_NONE, _R("опт")},// -- 158
//"от")},// -- 2390
{LI_PRAGMA, _R("офиц")},// -- 41
{LI_THEME, _R("охот")},// -- 150
//"п")},// -- 9
{LI_NONE, _R("палеонт")},// -- 71
{LI_NONE, _R("парл")},// -- 73
{LI_THEME, _R("пед")},// -- 10
{LI_LANG, _R("перс")},// -- 25
{LI_NONE, _R("полигр")},// -- 723
{LI_THEME, _R("полит")},// -- 228
{LI_NONE, _R("полит.-эк")},// -- 14
{LI_LANG, _R("польск")},// -- 7
{LI_LANG, _R("португ")},// -- 9
{LI_NONE, _R("поэт")},// -- 1150
{LI_NONE, _R("презр")},// -- 166
//"преим")},// -- 558
{LI_PRAGMA, _R("пренебр")},// -- 369
//"прил")},// -- 94
{LI_NONE, _R("прост")},// -- 280
{LI_NONE, _R("проф")},// -- 331
{LI_NONE, _R("психиатр")},// -- 23
{LI_THEME, _R("психол")},// -- 204
{LI_NONE, _R("пчел")},// -- 16
{LI_NONE, _R("радио")},// -- 359
{LI_PRAGMA, _R("разг")},// -- 7577
{LI_NONE, _R("реакт")},// -- 40
{LI_PRAGMA, _R("редк")},// -- 3684
{LI_THEME, _R("рел")},// -- 448
{LI_LANG, _R("рим")},// -- 57
{LI_NONE, _R("ритор")},// -- 40
{LI_LANG, _R("русск")},// -- 139
{LI_THEME, _R("рыб")},// -- 49
//"с")},// -- 231
{LI_THEME, _R("с.-х")},// -- 1234
{LI_NONE, _R("сад")},// -- 64
{LI_LANG, _R("санскр")},// -- 22
{LI_THEME, _R("св")},// -- 34
{LI_LANG, _R("сканд")},// -- 18
{LI_NONE, _R("сл")},// -- 3241
//"см")},// -- 1728
{LI_NONE, _R("собир")},// -- 426
//"сокр")},// -- 460
{LI_NONE, _R("социол")},// -- 35
{LI_THEME, _R("спец")},// -- 3052
{LI_THEME, _R("спорт")},// -- 1348
{LI_NONE, _R("стат")},// -- 52
{LI_NONE, _R("стил")},// -- 47
{LI_THEME, _R("стих")},// -- 165
{LI_THEME, _R("стр")},// -- 545
{LI_NONE, _R("страх")},// -- 27
{LI_NONE, _R("студ")},// -- 41
//"сущ")},// -- 170
{LI_NONE, _R("театр")},// -- 344
{LI_NONE, _R("текст")},// -- 436
{LI_NONE, _R("тел")},// -- 28
{LI_THEME, _R("тех")},// -- 3357
//"тж")},// -- 155
//"тк")},// -- 76
{LI_THEME, _R("тлв")},// -- 208
{LI_NONE, _R("топ")},// -- 50
{LI_LANG, _R("тур")},// -- 24
{LI_THEME, _R("унив")},// -- 114
//"употр")},// -- 230
//"усил")},// -- 33
{LI_NONE, _R("уст")},// -- 1730
{LI_NONE, _R("фарм")},// -- 182
{LI_THEME, _R("физ")},// -- 1305
{LI_NONE, _R("физиол")},// -- 382
{LI_THEME, _R("филос")},// -- 368
{LI_NONE, _R("фин")},// -- 246
{LI_THEME, _R("фольк")},// -- 54
//"фон")},// -- 226
{LI_THEME, _R("фото")},// -- 210
{LI_LANG, _R("фр")},// -- 635
{LI_THEME, _R("хим")},// -- 1467
{LI_LANG, _R("хинди")},// -- 15
{LI_NONE, _R("церк")},// -- 730
//"ч")},// -- 234
//"часто")},// -- 294
//"чаще")},// -- 25
{LI_THEME, _R("шахм")},// -- 58
{LI_THEME, _R("школ")},// -- 122
{LI_LANG, _R("шотл")},// -- 940
{LI_NONE, _R("шутл")},// -- 622
{LI_NONE, _R("эвф")},// -- 101
{LI_THEME, _R("эк")},// -- 385
{LI_THEME, _R("эл")},// -- 532
{LI_THEME, _R("элк")},// -- 179
{LI_PRAGMA, _R("эмоц.-усил")},// -- 271
{LI_NONE, _R("энт")},// -- 397
{LI_NONE, _R("этн")},// -- 11
{LI_LANG, _R("южно-афр")},// -- 52
{LI_THEME, _R("юр")},// -- 2037
{LI_LANG, _R("яп")}// -- 45
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
bool CBinaryDictionary::GetFlag(long pair_index, long flag_index, std::string& text, bool is_direct, DWORD& flag) const
{
	// TODO: Add your implementation code here
	const WordPair& wp = (is_direct) ? eng_vec[pair_index] : rus_vec[pair_index];
	
	
	switch(flag_index){
	case 0:
		flag = wp.data[0] & (BYTE)7; // first 3 bits
		switch(flag){
		case 0:
			text = _R("сущ");
			break;
		case 1:
			text = _R("глаг");
			break;
		case 2:
			text = _R("прил");
			break;
		case 3:
			text = _R("нареч");
			break;
		}
		break;
	case 1:
		flag = (wp.data[0] & (BYTE)8) != 0; // bit # 3
		if(flag)
			text = _R("множ");
		break;
	case 2:
		flag = (wp.data[0] & (BYTE)16) != 0; // bit # 4
		if(flag)
			text = _R("ед");
		break;
	case 3:
		flag = (wp.data[0] & (BYTE)32) != 0; // bit # 5
		if(flag)
			text = _R("предик");
		break;
	case 4:
		flag = wp.data[1];
		if(flag)
			text = all_labels[flag].str;
		break;
	case 5:
		flag = wp.data[2];
		if(flag)
			text = all_labels[flag].str;
		break;
	case 6:
		flag = wp.data[3];
		if(flag)
			text = all_labels[flag].str;
		break;
	case 7:
		flag = wp.data[4];
		if(flag)
			text = all_labels[flag].str;
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
