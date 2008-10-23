// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef __GERGRAMTAB_H_
#define __GERGRAMTAB_H_


#include "agramtab_.h"       
#include "ger_consts.h"       


/////////////////////////////////////////////////////////////////////////////
// CEngGramTab
const unsigned int gStartUp =  0x4141; //AA 
const unsigned int gEndUp =  0x7A7B;  //zz + 1
const unsigned int gMaxGrmCount  = gEndUp -gStartUp; // // 5911  (5 Êá) 

const BYTE gPartOfSpeechesCount = 17;

const char gPartOfSpeeches[gPartOfSpeechesCount][20] = {
	"ART",
	"ADJ", 
	"ADV",
	"EIG",
	"SUB",
	"VER",
	"PA1",
	"PA2",
	"PRO",
	"PRP",
	"KON",
	"NEG",
	"INJ",
	"ZAL",
	"ZUS",
	"PROBEG",
	"INF"
};
const short gGrammemsCount = 62;
const char gGrammems[gGrammemsCount][10] = {

//common unknown 0..3
"noa", // ohne artikel
"prd", // predikativ
"pro",
"tmp",


// eigennamen 4..12
"nac","mou","cou","geo","wat","geb","std","lok","vor",  

//  reflexive Verben 13..14
"sich-akk","sich-dat",

// verb clasess 15..18
"sft","non","mod","aux",

// verb forms 19..26
"kj1","kj2","pa1","pa2","eiz","imp","prt","prae",

//adjective 27..29
"gru","kom","sup",

// konjunk 30..34
"pri","inf","vgl","neb","unt",


//pronouns 35..41
"per","dem","inr","pos","ref","rin","alg",

//adjective's articles 42.44
"sol","ind","def",

//persons 45..47
"1",  "2",  "3",  

//genus 48..50
"fem","mas","neu",


// number 51..52
"plu","sin",

//cases 53..56
"nom","gen","dat","akk",

//abbreviation 57
"abbr",

//Einwohnerbezeichnung 58
"ew",

//Transitiv 59,60,61
"trans", "intra", "imper"


};


const int gClauseTypesCount = 3;
const char gClauseTypes [gClauseTypesCount][30] = 
{
	"VERBSATZ", "PARTIZIPIALSATZ", "INFINITIVSATZ"
};





class CGerGramTab : public CAgramtab
{
public:
	CAgramtabLine*  Lines[gMaxGrmCount];
	CGerGramTab();
	~CGerGramTab();

	BYTE GetPartOfSpeechesCount () const {return gPartOfSpeechesCount;};
	const char*   GetPartOfSpeechStr(BYTE i) const {return gPartOfSpeeches[i];};
	size_t GetGrammemsCount()  const{return gGrammemsCount;};
	const char*   GetGrammemStr(size_t i) const{return gGrammems[i];};
	size_t GetMaxGrmCount() const{return gMaxGrmCount;};
	CAgramtabLine*& GetLine(size_t LineNo) {return Lines[LineNo];}
	const CAgramtabLine* GetLine(size_t LineNo) const {return Lines[LineNo];}
	size_t s2i(const char * s ) const { return  (unsigned char) s[0]*0x100+(unsigned char) s[1] - gStartUp;};

	string i2s(WORD i) const
	{ 
		i += gStartUp;
		char res[3];

		res[0] = (i >> 8);
		res[1] = (0xFF & i);
		res[2] = 0;
		return  res;
	};
	int GetGroupTypebyName(const char* TypeName) const;


    
	const char* GetRegistryString() const 
	{
		return "Software\\Dialing\\Lemmatizer\\German\\Ggramtab";
	};

	long GetClauseTypeByName(const char* TypeName) const;
	const char* GetClauseNameByType(long type) const;
	bool GleicheGenderNumber(const char* gram_code1, const char* gram_code2) const;
	bool GleicheSubjectPredicate(const char* gram_code1, const char* gram_code2) const;
	const size_t GetClauseTypesCount() const 
	{
		return gClauseTypesCount;
	};
	bool IsStrongClauseRoot(const DWORD Poses) const;
	bool is_month (const char* lemma) const;
	bool is_small_number (const char* lemma) const;
	bool IsMorphNoun (size_t Poses)  const;
	bool is_morph_adj (size_t poses) const;
	bool is_morph_participle (size_t poses) const;
	bool is_morph_pronoun (size_t poses) const;
	bool is_morph_pronoun_adjective(size_t poses) const;
	bool is_left_noun_modifier  (size_t poses, QWORD grammems) const;
	bool is_numeral (size_t poses) const;
	bool is_verb_form (size_t poses) const;
	bool is_infinitive(size_t poses) const;
	bool is_morph_predk(size_t poses) const;
	bool is_morph_adv(size_t poses) const;
	bool is_morph_personal_pronoun (size_t poses, QWORD grammems) const;
	bool is_morph_article(size_t poses) const;

	bool IsSimpleParticle(const char* lemma, size_t poses) const;
	bool IsSynNoun(size_t Poses, const char* Lemma) const;
	bool IsStandardParamAbbr (const char* WordStrUpper) const;
	bool GleicheCase(const char* gram_code_noun, const char* gram_code_adj) const;
	bool GleicheCaseNumber(const char* gram_code1, const char* gram_code2) const;
	QWORD GleicheGenderNumberCase(const char* common_gram_code_noun, const char* gram_code_noun, const char* gram_code_adj) const;
	
};



const QWORD gBestimtheit = ( _QM(gAdjektiveOhneArtikel) | _QM(gAdjektiveMitUnbestimmte) | _QM(gAdjektiveMitBestimmte));
const QWORD gAllCases = (_QM(gNominativ) | _QM(gGenitiv) | _QM(gDativ) | _QM(gAkkusativ));
const QWORD gAllNumbers = ( _QM(gPlural) | _QM(gSingular) );
const QWORD gAllGenders = ( _QM(gFeminin) | _QM(gMaskulin) | _QM(gNeutrum));
const QWORD gAllPersons = ( _QM(gErstePerson) | _QM(gZweitePerson) | _QM(gDrittePerson));
const QWORD gAllVerbForms = ( _QM(gKonj1) | _QM(gKonj2) | _QM(gImperativ)| _QM(gPrasens)| _QM(gPraeteritum));
const QWORD gAllVerbClasses = ( _QM(gModal) | _QM(gAuxiliar) | _QM(gSchwach)| _QM(gNichtSchwach));






#endif //__GERGRAMTAB_H_
