// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "morph_dict/lemmatizer_base_lib/rus_numerals.h"
#include "RusFormatCaller.h"


bool check_number_noun_coordination(const uint64_t &number_grammems, const CSynPlmLine &Noun,
                                    const CAgramtab *piRusGramTab) {
    if (!Noun.GetGramcodes()) return false;
    if (!(Noun.GetGrammems() & (1 << rPlural))) return false;

    for (int i = 0; i < strlen(Noun.GetGramcodes()); i += 2) {
        uint64_t gram;
        piRusGramTab->GetGrammems(Noun.GetGramcodes() + i, gram);
        if (gram & (1 << rPlural)) {
            if (number_grammems & ((1 << rNominativ) | (1 << rAccusativ))) //"пять мальчиков", "пять мальчиков"
            {
                if (gram & (1 << rGenitiv))
                    return true;
            }
            if (number_grammems  //"пятью мальчиками", "о пяти мальчиках"...
                & gram
                & ((1 << rGenitiv)
                   | (1 << rDativ)
                   | (1 << rInstrumentalis)
                   | (1 << rLocativ)
                )
                    )
                return true;
        };
    }

    return false;
}

void set_noun_numeral_group_type(CGroup &G, EGroupType type) {
    G.m_GroupType = type;
    G.m_bRussianNounNumeralGroup = true;
}

/*
Примеры:
    двадцать пять  // количественное числительное
    тридцать шестой  // порядковое числительное
	20 тысяч (первое число - цифрами)
	20 тыс. (с сокращением)
*/
bool CRusFormatCaller::format_for_numbers(CGroup &G) {
    if (W1.get_word().empty()) return false;

    int i = get_main_word(G.m_iFirstWord);
    int first_main_word = i;

    if ((!is_numeral(Wi)  // либо числительное, которое не является наречным числительным
         || GetOpt()->m_NumberAdverbsList.has_lemma(Wi.get_lemma()) //"свыше ста пяти человек"
        )
        && !HasInSet(g_BigNumerals, Wi.get_lemma())
        // либо большое числительное, которое является  морф. существительным

        && !Wi.HasFlag(fl_digit)
        // либо просто цифра
            )
        return false;


    int last_main_word = i;
    bool bHasAlreadyWords = false;
    for (i = get_next_main_word(i); i != sent.size(); i = get_next_main_word(i)) {
        if ((!is_numeral(Wi)
             && !Wi.HasPOS(NUMERAL_P)
             && !Wi.HasFlag(fl_digit)
             && !HasInSet(g_BigNumerals, Wi.get_lemma())
        )
                )
            break;
        if (Wi.HasFlag(fl_digit) && bHasAlreadyWords)
            break;


        // Захватить точку в группу, если перед нами сокращение
        if ((i + 1 < sent.size())
            && sent[i + 1].HasFlag(fl_fullstop)
            && Wi.has_grammem(rInitialism)
                )
            i++;

        last_main_word = i;

        bHasAlreadyWords |= Wi.HasFlag(fl_le);
    };


    if (last_main_word == first_main_word) return false;

    G.m_iLastWord = get_maximal_group(last_main_word).m_iLastWord;;


    G.m_GroupType = NUMERALS;
    int LastWordExceptFullStop = G.m_iLastWord;
    if (sent[LastWordExceptFullStop].HasFlag(fl_fullstop))
        LastWordExceptFullStop--;
    const CGroup &MainGroup = get_maximal_group(LastWordExceptFullStop);
    G.m_MainGroup = MainGroup;
    G.SetGrammems(MainGroup.GetGrammems());
    std::string debug = GetGramTab()->GrammemsToStr(G.GetGrammems());
    G.m_GramCodes = std::string(sent[G.m_iFirstWord].GetGramcodes());

    for (i = G.m_iFirstWord; i < G.m_iLastWord; i++)
        create_syn_rel(G, i + 1, i, NUMERALS);


    if (HasInSet(g_BigNumerals, sent[LastWordExceptFullStop].get_lemma())
        && (sent[LastWordExceptFullStop].has_grammem(rGenitiv))
            ) {
        if (sent[G.m_iFirstWord].HasFlag(fl_digit) &&
            (sent[G.m_iFirstWord].GetGrammems() & (_QM(rNominativ) | _QM(rAccusativ))) >
            0) //Цена упала до 10 тысяч рублей
            G.SetGrammems(G.GetGrammems() | _QM(rNominativ) | _QM(rAccusativ));
        else if (LastWordExceptFullStop >= G.m_iFirstWord)
            if ((sent[LastWordExceptFullStop - 1].HasFlag(fl_small_number)
                 && sent[LastWordExceptFullStop - 1].has_grammem(rNominativ)
                 && sent[LastWordExceptFullStop].has_grammem(rSingular)
                )
                ||
                (!sent[LastWordExceptFullStop - 1].HasFlag(fl_small_number)
                 && sent[LastWordExceptFullStop - 1].has_grammem(rNominativ)
                 && sent[G.m_iLastWord].has_grammem(rPlural)
                )
                    )
                G.SetGrammems(_QM(rNominativ) | _QM(rAccusativ) | _QM(rPlural));
    };
    for (i = G.m_iFirstWord; i <= G.m_iLastWord; i++)
        if (HasInSet(g_BigNumerals, sent[i].get_lemma()))
            G.m_bNumeralMoreThanThousand = true;
    return true;
};

// Примеры:
//    22,6 

bool CRusFormatCaller::format_for_num_complex(CGroup &G) {
    if ((G.m_iFirstWord + 2 >= sent.size())
        || !W2.HasFlag(fl_punct)
        || W2.is_lemma("(") || W2.is_lemma(")")
        || !W3.HasFlag(fl_digit)
            )
        return false;

    if (get_maximal_group_size(G.m_iFirstWord) != 1) return false;
    if (get_maximal_group_size(G.m_iFirstWord + 2) != 1) return false;

    G.m_iLastWord = G.m_iFirstWord + 2;
    if (G.m_iLastWord + 1 < sent.size()) {
        if (sent[G.m_iLastWord + 1].is_month()
            && (W2.get_word() != "-" || !W1.HasPOS(NUMERAL_P) || !W3.HasPOS(NUMERAL_P))) //события 5-24 декабря
            return false;
    }

    if (!W1.HasFlag(fl_digit) && sent[G.m_iLastWord].has_space_before()
        || W1.GetPoses() != W3.GetPoses()) // кроме "на 1,7 - 1,8%"
        return false;

    G.m_Cause = "numbers and punctuation marks";
    G.m_GroupType = C_NUMERALS;
    G.SetGrammems(sent[G.m_iLastWord].GetGrammems());
    G.m_MainGroup = G.m_iFirstWord;

    for (size_t i = G.m_iFirstWord; i < G.m_iLastWord; i++)
        create_syn_rel(G, i + 1, i, C_NUMERALS);

    return true;
};


// Примеры:
//    статья 26 
//    п. 26     // факультативная точка
//    н.  27.1

bool CRusFormatCaller::format_for_noun_num(CGroup &G) {
    const CGroup &H = get_maximal_group(G.m_iFirstWord);
    int i = H.m_MainWordNo;
    G.m_iLastWord = H.m_iLastWord + 1;
    if (G.m_iLastWord >= sent.size()) return false;
    if (sent[G.m_iLastWord].is_single_punct((unsigned char) '.')) G.m_iLastWord++;
    if (G.m_iLastWord >= sent.size()) return false;
    int k = get_main_word(G.m_iLastWord);

    if (!is_numeral(Wk)
        && !Wk.HasPOS(NUMERAL_P)
        && !Wk.HasFlag(fl_digit)
        && (Wk.get_word() != "№")
            )
        return false;

    // запрещаем фразу "дом несколько", поскльку "несколько" - числительное
    if (GetOpt()->m_NumberAdverbsList.has_lemma(Wk.get_lemma())) return false;
    auto lm = !Wi.has_lemma() ? Wi.get_word() : Wi.get_lemma();
    if (!GetOpt()->m_NounNumList.has_lemma(lm)
        && !(Wi.has_des(OUpLw) &&
             (Wi.has_des(OLLE) || k == sent.size() - 1 || Wi.has_des(OQuoted) && !sent[k + 1].has_des(OQuoted)) &&
             (i == k - 1) && Wk.HasFlag(fl_digit)))
        return false; //Банк вводит вклад "Доход 2013"

    G.m_GroupType = NOUN_NUMERAL;

    G.m_iLastWord += get_maximal_group_size(G.m_iLastWord) - 1;
    const CGroup &MainGroup = get_maximal_group(G.m_iFirstWord);
    change_words_in_group_gramcodes(get_maximal_group(k), GetOpt()->m_MasSingNomNounGramCode, CaseNumberGender0);
    if (Wk.get_word() == "№" && is_numeral(sent[k + 1])) //я вышел из дома № 26
    {
        Wk.SetGramcodes(GetOpt()->m_MasSingNomNounGramCode);
        change_words_in_group_gramcodes(get_maximal_group(k + 1), GetOpt()->m_MasSingNomNounGramCode,
                                        CaseNumberGender0);
    }
    G.m_MainGroup = MainGroup;
    G.SetGrammems(Wi.GetGrammems());
    create_syn_rel(G, get_main_word_in_group(MainGroup), get_main_word_in_group(get_maximal_group(G.m_iLastWord)),
                   NOUN_NUMERAL);
    return true;
};


size_t get_number_with_this_case(const CSynPlmLine &L, RussianGrammemsEnum Case, const CAgramtab *piRusGramTab) {
    unsigned int ret_gram = 0;
    if (!L.GetGramcodes()) return 0;
    for (int i = 0; i < strlen(L.GetGramcodes()); i += 2) {
        uint64_t gram;
        piRusGramTab->GetGrammems(L.GetGramcodes() + i, gram);
        if (gram & _QM(Case)) {
            if (gram & _QM(rPlural))
                ret_gram |= _QM(rPlural);

            if (gram & (1 << rSingular))
                ret_gram |= _QM(rSingular);
        }
    }

    return ret_gram;
}


// --- наречные числительные
// много домов 
// несколько столов и стульев

bool CRusFormatCaller::format_for_number_adverb(CGroup &G) {
    const CGroup &H = get_maximal_group(G.m_iFirstWord);

    int j = H.m_MainWordNo;
    if (!is_numeral(Wj)) return false;
    if (!GetOpt()->m_NumberAdverbsList.has_lemma(Wj.get_lemma()))
        return false;
    if (H.m_iLastWord + 1 >= sent.size())
        return false;

    int i = get_main_word(H.m_iLastWord + 1);
    if (i <= H.m_iLastWord)
        return false;

    /*
    нужно, чтобы собиралась группа "больше меня" как много "меня",
    чтобы	фраза "Я хочу больше тебя" переводилась бы "I want more than you",
    а  не "I want bigger than you"
    поэтому здесь не morph_noun
    .*/
    if (!(Wi.is_syn_noun() ||
          (Wi.HasPOS(NUMERAL) && Wi.has_grammem(rComparative) && ((Wi.GetGrammems() & rAllCases) == 0))))
        return false; // || Wi.HasPOS(NUMERAL) "более 5 человек"

    if (HasInSet(g_BigNumerals, Wi.get_lemma()))
        return false;


    G.m_iLastWord = get_maximal_group(i).m_iLastWord;

    if (G.m_iLastWord >= sent.size())
        return false;
    G.m_Cause = "adverb numeral group +NP(plural, genitive)";
    G.m_GroupType = NUMERAL_ADVERB;
    const CGroup &MainGroup = get_maximal_group(i);
    G.m_MainGroup = MainGroup;

    if (is_numeral(Wj)) {

        if (!(((Wj.has_grammem(rNominativ)
                || ((Wj.GetGrammems() & rAllCases) == 0)  // "больше", "меньше"
               )
               && Wi.has_grammem(rGenitiv)
              )
              || (Wj.has_grammem(rGenitiv)
                  && Wi.has_grammem(rGenitiv)
              )
              || (Wj.has_grammem(rDativ)
                  && Wi.has_grammem(rDativ)
              )
              || (Wj.has_grammem(rAccusativ)
                  && Wi.has_grammem(rGenitiv)
              )
              || (Wj.has_grammem(rInstrumentalis)
                  && Wi.has_grammem(rInstrumentalis)
              )
              || (Wj.has_grammem(rLocativ)
                  && Wi.has_grammem(rLocativ)
              )
        ))
            return false;

        int number = rPlural;
        if (Wj.has_grammem(rNominativ)
            || Wj.has_grammem(rAccusativ)
            // отличаем случай "больше воды"(ед) от "больше денег" (мн)
            || ((Wj.GetGrammems() & rAllCases) == 0) // больше, меньше 
                ) {
            unsigned int numbers = get_number_with_this_case(Wi, rGenitiv, GetGramTab());
            if ((numbers & (1 << rSingular)) && !(numbers & (1 << rPlural)))
                number = rSingular;
        }


        if ((Wj.has_grammem(rNominativ) && Wj.has_grammem(rAccusativ))
            || ((Wj.GetGrammems() & rAllCases) == 0)
                ) {
            change_words_in_group_grammems(get_maximal_group(i), (1 << rGenitiv) | (1 << number),
                                           rAllCases | rAllNumbers);
            if ((Wj.GetGrammems() & rAllCases) == 0) // больше, меньше 
                G.SetGrammems(_QM(rAccusativ) | _QM(rNominativ) | _QM(number));//_QM(rPlural) | _QM(rSingular));
            else
                G.SetGrammems(Wj.GetGrammems());

        } else {
            G.SetGrammems(Wj.GetGrammems() & Wi.GetGrammems());
            change_words_in_group_grammems(get_maximal_group(i), (Wi.GetGrammems() & Wj.GetGrammems()) | (1 << rPlural),
                                           rAllCases | rAllNumbers);
        }

        G.SetGrammems(G.GetGrammems() | _QM(number));

    }

    create_syn_rel(G, get_main_word_in_group(MainGroup), get_main_word(G.m_iFirstWord), NUMERAL_ADVERB);
    return true;
};

bool CRusFormatCaller::is_small_number_group(size_t WordNo) {
    if (WordNo >= sent.size())
        return false;

    int i = get_maximal_group_no(WordNo);
    if (i == -1)
        return sent[WordNo].HasFlag(fl_small_number);
    else {
        if (sent[GetGroups()[i].m_iLastWord].HasFlag(fl_small_number))
            return true;
    }
    return false;

};

bool NumberLikeAdj(const CSynPlmLine &L) {
    return L.is_word_upper("1")
           || L.is_lemma("ОДИН");

};


/*
 строит группу для числительного оба + местоимение:
	"Оба они"
	"о них обеих"
*/
bool CRusFormatCaller::format_for_both(CGroup &G) {
    if (G.m_iFirstWord + 1 >= sent.size())
        return false;

    // рассматриваем только атомарные группы
    if (get_maximal_group_size(G.m_iFirstWord) > 1)
        return false;

    if (get_maximal_group_size(G.m_iFirstWord + 1) > 1)
        return false;

    int i_number = -1, i_noun = -1;

    if (sent[G.m_iFirstWord].is_lemma("ОБА")) {
        i_number = G.m_iFirstWord;
        if (!sent[G.m_iFirstWord + 1].HasPOS(PRONOUN)
            || !sent[G.m_iFirstWord + 1].has_grammem(rPlural)
                )
            return false;
        else
            i_noun = G.m_iFirstWord + 1;
    } else if (sent[G.m_iFirstWord].HasPOS(PRONOUN)
               && sent[G.m_iFirstWord].has_grammem(rPlural)) {
        i_noun = G.m_iFirstWord;
        if (sent[G.m_iFirstWord + 1].is_lemma("ОБА"))
            i_number = G.m_iFirstWord + 1;
        else
            return false;
    }

    if ((i_number == -1) || (i_noun == -1))
        return false;

    if (sent[i_number].GetGrammems() & sent[i_noun].GetGrammems() & rAllCases) {
        G.m_iLastWord = (i_number > i_noun) ? i_number : i_noun;
        G.SetGrammems(sent[i_noun].GetGrammems());
        set_noun_numeral_group_type(G, NUMERAL_NOUN);
        const CGroup &MainGroup = get_maximal_group(i_noun);
        G.m_MainGroup = MainGroup;
        create_syn_rel(G, get_main_word_in_group(MainGroup), i_number, NUMERAL_NOUN);
        return true;
    }
    return false;

}


bool CRusFormatCaller::gleiche_for_big_numbers(int i_noun, int i_number, uint64_t &new_group_grammems) {
    bool arabicNumber = sent[i_number].HasFlag(fl_digit);
    int i_first_noun = get_maximal_group(i_noun).m_iLastWord;
    const CRusGramTab *R = (CRusGramTab *) GetGramTab();
    CGroup &Gi = ((CGroup &) get_maximal_group(i_number));
    bool has_point = arabicNumber && FindFloatingPoint(sent[Gi.m_iLastWord].get_word()) != -1 &&
                     Gi.m_GroupType != NUMERALS; //float point кроме КОЛИЧ
    const CGroup &NP = get_maximal_group(i_first_noun);

    if ((NP.GetGrammems() & _QM(rPlural)) == 0) {
        return false; //пять девочка
    }
    auto noun_gram_codes = sent[i_first_noun].GetGramCodes();
    bool noun_can_be_pl_gen = GetOpt()->GetGramTab()->FindGrammems(noun_gram_codes.c_str(),
                                                                   _QM(rGenitiv) | _QM(rPlural));

    if (!arabicNumber) {
        /*
        когда числительное стоит в номинативе или аккузативе,
        тогда ИГ должна быть в родительном, например:
        "Я вижу пять девочек"
        "Пять девочек пришло"
        Во всех остальных случаях, числительное и ИГ должны быть согласованы
        по падежу.
        */
        bool number_can_be_nom_or_acc = (sent[i_number].GetGrammems() | _QM(rNominativ) | _QM(rAccusativ)) > 0;
        bool nom_case = number_can_be_nom_or_acc && noun_can_be_pl_gen;
        uint64_t common_cases = (NP.GetGrammems() & sent[i_number].GetGrammems() & rAllCases);
        bool gleiche_case = (common_cases & ~_QM(rNominativ)) > 0; // убираем "пять девочки"
        if (!nom_case && !gleiche_case) {
            return false;
        }
        Gi.m_GramCodes = "";
        uint64_t new_grm = NP.GetGrammems() & ~rAllCases;
        uint64_t cases = 0;
        if (nom_case) {
            new_grm |= _QM(rGenitiv) | _QM(rNominativ) | _QM(rAccusativ);
        }
        if (gleiche_case) {
            new_grm |= NP.GetGrammems() & sent[i_number].GetGrammems() & rAllCases;;
        }
        Gi.SetGrammems(new_grm);
        sent[i_noun].SetGrammems(new_grm);
        new_group_grammems = new_grm;
    } else {
        uint64_t cases = 0;
        if (NP.GetGrammems() & _QM(rIndeclinable)) {
            cases = rAllCases; // 5 пальто
        } else if (noun_can_be_pl_gen) {
            cases |= _QM(rGenitiv) | _QM(rNominativ) | _QM(rAccusativ); // 5 девочек
        } else {
            // 5 руками
            uint64_t noun_cases = 0;
            if (noun_gram_codes.empty()) {
                noun_cases = NP.GetGrammems() & rAllCases;
            } else {
                for (size_t i = 0; i < noun_gram_codes.length(); i += 2) {
                    auto g = GetOpt()->GetGramTab()->GetAllGrammems(noun_gram_codes.substr(i, 2).c_str());
                    if ((g & (_QM(rNominativ) | _QM(rAccusativ))) == 0) {
                        noun_cases |= g & rAllCases;;
                    }
                }
            }
            cases |= noun_cases & sent[i_number].GetGrammems();
        }
        if (cases == 0) {
            return false;
        }
        uint64_t new_grm = (NP.GetGrammems() & ~rAllCases) | cases;
        sent[i_noun].SetGrammems(new_grm);
        new_group_grammems = new_grm;
    }
    return true;
}

/*
Проверяет согласование между существительным(i_noun) и малым числительным(i_number).
Если change_grammems true, тогда меняет граммемы у группы существительного.
new_group_grammems - новые граммемы  группы(ЧИCЛ-СУЩ).
*/


bool CRusFormatCaller::gleiche_for_plural_numbers(int i_noun, int i_number, uint64_t &new_group_grammems,
                                                  bool small_number) {
    int i_last_noun, i_number_group;
    uint64_t noun_grammems, number_grammems;

    int i_first_noun = get_maximal_group(i_noun).m_iLastWord;

    const CGroup &NP = get_maximal_group(i_first_noun);

    if (!is_noun_group(NP))
        return false;

    if (NP.size() == 1) {
        i_last_noun = i_first_noun;
        noun_grammems = sent[i_first_noun].GetGrammems();
    } else {
        i_last_noun = NP.m_iLastWord;
        noun_grammems = NP.GetGrammems();
        if ((noun_grammems & rAllGenders) == 0) { // Забираем род у существительного, его нет в грам. группы
            noun_grammems |= sent[i_last_noun].GetGrammems() & rAllGenders;
        }
    }

    i_number_group = get_maximal_group_no(i_number);

    if (i_number_group == -1)
        number_grammems = sent[i_number].GetGrammems();
    else
        number_grammems = GetGroups()[i_number_group].GetGrammems();


    if (!small_number) {
        return gleiche_for_big_numbers(i_noun, i_number, new_group_grammems);
    } else {
        size_t i_last_number = get_maximal_group(i_number).m_iLastWord;
        bool bFirstCase = false;
        auto word = sent[i_last_number].get_upper_word();
        if (word.empty()) return false;

        if (word.back() == '2' || word.back() == '3' || word.back() == '4') {
            // "2 мальчика"
            uint64_t cases = 0;
            if (noun_grammems & _QM(rGenitiv)) {
                cases |= _QM(rAccusativ) | _QM(rNominativ);
            } else {
                cases |= rAllCases;
            }
            change_words_in_group_grammems(
                    get_maximal_group(i_last_noun),
                    cases,
                    rAllCases);
            new_group_grammems = _QM(rPlural) | cases;
            return true;
        } else if ("ДВА" == word ||
                   "ОБА" == word ||
                   "ПОЛТОРА" == word
                ) {
            // мужской и средний рода:
            // существительное должно быть в родительном падеже
            if (((noun_grammems & _QM(rMasculinum))
                 || (noun_grammems & _QM(rNeutrum))
                )
                && (noun_grammems & _QM(rGenitiv))
                    ) {
                if (!sent[i_noun].HasFlag(fl_noun_has_adj_declination)) {
                    if (noun_grammems & _QM(rSingular)) {
                        // "два  мальчика"
                        // "два  облака"
                        change_words_in_group_grammems(
                                get_maximal_group(i_last_noun),
                                _QM(rMasculinum) | _QM(rNeutrum) | _QM(rGenitiv) | _QM(rSingular),
                                rAllCases | rAllNumbers);
                        bFirstCase = true;
                    }
                } else    // адъективное склонение
                if (noun_grammems & _QM(rPlural)) {
                    // "два  нищих"
                    bFirstCase = true;
                    change_words_in_group_grammems(
                            get_maximal_group(i_last_noun),
                            _QM(rMasculinum) | _QM(rNeutrum) | _QM(rGenitiv) | _QM(rPlural),
                            rAllCases | rAllNumbers);

                };

            }
        } else if (("ДВЕ" == word) ||
                   ("ОБЕ" == word) ||
                   ("ПОЛТОРЫ" == word)
                ) {
            //
            if (noun_grammems & _QM(rFeminum))

                if (!sent[i_noun].HasFlag(fl_noun_has_adj_declination)) {
                    if ((noun_grammems & _QM(rGenitiv))
                        && (noun_grammems & _QM(rSingular))
                            ) {
                        change_words_in_group_grammems(
                                get_maximal_group(i_last_noun),
                                _QM(rGenitiv) | _QM(rSingular),
                                rAllCases | rAllNumbers);

                        bFirstCase = true;
                    }
                } else    //адъективное склонение
                if ((noun_grammems & _QM(rNominativ))
                    && (noun_grammems & _QM(rPlural))
                        ) {
                    // "две запятые"
                    change_words_in_group_grammems(
                            get_maximal_group(i_last_noun),
                            _QM(rNominativ) | _QM(rPlural),
                            rAllCases | rAllNumbers);
                    bFirstCase = true;
                }

        } else if ("ТРИ" == word ||
                   "ЧЕТЫРЕ" == word) {
            if ((noun_grammems & _QM(rGenitiv)) &&
                (noun_grammems & _QM(rSingular))) {
                change_words_in_group_grammems(
                        get_maximal_group(i_last_noun),
                        _QM(rGenitiv) | _QM(rSingular),
                        rAllCases | rAllNumbers);
                bFirstCase = true;
            }

            if (sent[i_noun].HasFlag(fl_noun_has_adj_declination)
                && (noun_grammems & _QM(rFeminum))
                    ) {
                change_words_in_group_grammems(
                        get_maximal_group(i_last_noun),
                        _QM(rNominativ) | _QM(rPlural),
                        rAllCases | rAllNumbers);
                bFirstCase = true;

            };
        }

        if (bFirstCase) {
            if (noun_grammems & _QM(rAnimative))
                new_group_grammems = _QM(rPlural) | _QM(rNominativ);
            else
                new_group_grammems = _QM(rPlural) | _QM(rNominativ) | _QM(rAccusativ);

            return true;
        }


        if (noun_grammems & _QM(rPlural)) // "двумя слонами", "двух слонах", "двум слонам", "двух слонов"
            if (number_grammems & rAllCases & noun_grammems) {

                if (number_grammems & _QM(rNominativ)) // "два", "три", "четыре"
                    if ((number_grammems & _QM(rAccusativ) & noun_grammems)  // запретить "два слонов"
                        || sent[i_noun].HasFlag(fl_noun_has_adj_declination) // запретить "два лешие"
                            ) {
                        return false;
                    };


                new_group_grammems = (number_grammems & rAllCases & noun_grammems) | _QM(rPlural);
                change_words_in_group_grammems(get_maximal_group(i_last_noun), new_group_grammems,
                                               rAllCases | rAllNumbers);
                return true;
            }

    }
    return false;
}


// пять мальчиков
bool CRusFormatCaller::format_for_number_noun_private(CGroup &G) {
    bool small_number = is_small_number_group(G.m_iFirstWord);
    grammems_mask_t grammems = G.GetGrammems();
    const CGroup &G1 = get_maximal_group(G.m_iFirstWord);

    int i = get_main_word(G.m_iFirstWord);

    const CGroup &G2 = get_maximal_group(G1.m_iLastWord + 1);

    if (small_number && !is_noun_group(G2))
        return false;

    if (!gleiche_for_plural_numbers(G2.m_MainWordNo, G1.m_MainWordNo, grammems, small_number))
        return false;
    set_noun_numeral_group_type(G, NUMERAL_NOUN);
    G.m_MainGroup = G2;
    G.m_OtherGroup = G1;
    G.SetGrammems(grammems, G1.m_GramCodes.c_str());
    create_syn_rel(G, G2.m_MainWordNo, G1.m_MainWordNo, G.m_GroupType);
    G.m_iLastWord = G2.m_iLastWord;
    return true;
}

// мальчиков пять
bool CRusFormatCaller::format_for_noun_number_private(CGroup &G) {
    const CGroup &G1 = get_maximal_group(G.m_iFirstWord);
    uint64_t grammems = G.GetGrammems();
    if (G1.m_iLastWord + 1 >= sent.size()) return false;
    const CGroup &G2 = get_maximal_group(G1.m_iLastWord + 1);
    bool small_number = is_small_number_group(G2.m_iFirstWord);
    if (!is_numeral(sent[G2.m_MainWordNo])
        && !sent[G2.m_MainWordNo].HasFlag(fl_digit)
        || (!sent[G.m_iFirstWord].has_grammem(rPlural) && !small_number)
            )
        return false;

    const CGroup &NumberG = get_maximal_group(G.m_iFirstWord + 1);
    const CSynPlmLine &Num = sent[NumberG.m_MainWordNo];

    if (Num.m_UnitType == EClause) return false;

    if (NumberLikeAdj(Num))
        return false;

    if (is_numeral(Num)) {
        //  с числительными "МНОГО" или "МАЛО" не надо образовывать АППРОКС_ИГ
        if (RussianNumerals.GetCardinalNumeral(Num.get_lemma()) == -1 && !Num.HasFlag(fl_digit))
            return false;
    } else if (!Num.HasFlag(fl_digit))
        return false;
    /*
    (1)
    можно сказать "человек сто/двести"
    нельзя сказать "человек тысяча/миллион"
    т.е. больше тысячи нельзя
    */
    if (Num.HasFlag(fl_digit)) {
        if (
               Num.get_word().length() > 3 // = "5000"
            || Num.HasPOS(NUMERAL_P)
                )
            return false;
    };
    uint64_t number_grammems;
    if (NumberG.size() == 1) {
        number_grammems = Num.GetGrammems();
        // см. выше (1)
        if (HasInSet(g_BigNumerals, Num.get_lemma()))
            return false;
    } else {
        if (NumberG.m_GroupType != NUMERALS && NumberG.m_GroupType != C_NUMERALS)
            return false;
        // см. выше (1)
        if (NumberG.m_bNumeralMoreThanThousand)
            return false;
        if (!sent[NumberG.m_iLastWord].HasPOS(NUMERAL))
            return false;
        number_grammems = NumberG.GetGrammems();
    }

    if (!gleiche_for_plural_numbers(G1.m_MainWordNo, G2.m_MainWordNo, grammems, small_number))
        return false;
    set_noun_numeral_group_type(G, NOUN_NUMERAL_APPROX);
    G.m_MainGroup = G1;
    G.m_OtherGroup = G2;
    G.SetGrammems(grammems, G2.m_GramCodes.c_str());
    create_syn_rel(G, G1.m_MainWordNo, G2.m_MainWordNo, G.m_GroupType);
    G.m_iLastWord = G2.m_iLastWord;
    return true;
}

bool CRusFormatCaller::format_for_odin_group(CGroup &G) {
    const CGroup &OdinG = get_maximal_group(G.m_iFirstWord);
    int i_number = OdinG.m_MainWordNo;
    if (!sent[i_number].HasFlag(fl_digit)
        && !sent[i_number].HasPOS(NUMERAL)
            )
        return false;

    if (OdinG.m_iLastWord + 1 >= sent.size()) return false;
    const CGroup &NP = get_maximal_group(OdinG.m_iLastWord + 1);
    int i_noun = NP.m_MainWordNo;
    if (!is_noun_group(NP))
        return false;

    auto R = GetRusGramTab();
    std::string noun_grc = sent[i_noun].GetGramcodes();
    std::string new_grc = R->UniqueGramCodes(
            R->GleicheAncode1(0, noun_grc,
                 (R->GetAllGramCodes(NOUN, rAllCases | _QM(rSingular),
                 CaseNumber))));

    if (new_grc.empty() || sent[i_number].HasFlag(fl_digit) && sent[i_noun].is_month())
        return false;

    if (sent[i_number].HasFlag(fl_digit)) // до 81 унции
    {
        auto num_grc = R->GleicheAncode1(CaseNumberGender0, R->GramCodes().m_GenderNumeral, new_grc);
        num_grc = R->UniqueGramCodes(
                        R->GleicheAncode1(
                             CaseNumberGender0,
                             num_grc,
                             R->UniqueGramCodes(sent[i_number].GetGramcodes())
                             )
                        );
        if (num_grc.empty())
            return false;

        ((CGroup &) get_maximal_group(i_noun)).m_GramCodes = new_grc;
        ((CGroup &) get_maximal_group(i_number)).SetGrammems(
                R->GetAllGrammems(num_grc.c_str()) & ~rAllNumbers | _QM(rSingular), num_grc.c_str());
        change_words_in_group_gramcodes(get_maximal_group(i_number), num_grc.c_str(), CaseNumberGender0);
        change_words_in_group_gramcodes(get_maximal_group(i_noun), new_grc.c_str(), CaseNumberGender0);
        sent[i_noun].SetGramcodes(new_grc);
        G.SetGrammems(R->GetAllGrammems(num_grc.c_str()) & ~rAllNumbers | _QM(rSingular), num_grc.c_str());
        G.m_OtherGroup = get_maximal_group(i_number);
    } else if (!GetGramTab()->GleicheGenderNumberCase(sent[i_noun].m_type_gram_code, sent[i_noun].GetGramcodes(),
                                                      sent[i_number].GetGramcodes())) {
        G.SetGrammems(sent[i_noun].GetGrammems() & sent[i_number].GetGrammems());
        G.m_OtherGroup = get_maximal_group(i_number);
        change_words_in_group_grammems(G, G.GetGrammems(), (rAllNumbers | rAllCases));

    } else
        return false;

    G.m_iLastWord = NP.m_iLastWord;
    set_noun_numeral_group_type(G, NUMERAL_NOUN);
    G.m_MainGroup = NP;
    create_syn_rel(G, get_main_word_in_group(NP), i_number, NUMERAL_NOUN);

    return true;
}


/*
 функция  проверяет, что со слова StartWordNo может начинаться группа ЧИСЛ_СУЩ.
 Проверяет, что на месте StartWordNo стоит одиночное  существительное или группа 
 числительного. Возвращает номер главного слова группы, которой стоит после
 проверенной группы числительного. При ошибке выдает -1.
*/

int CRusFormatCaller::can_start_number_noun_group(int StartWordNo) const {
    int gr_num = get_maximal_group_no(StartWordNo);
    if (gr_num != -1)
        if (GetGroups()[gr_num].m_GroupType == NOUN_NUMERAL)
            return -1;

    int i = (gr_num == -1) ? StartWordNo : GetGroups()[gr_num].m_MainWordNo;
    if (!is_numeral(sent[i])
        && !sent[i].HasFlag(fl_digit)
        && !(gr_num != -1 && GetGroups()[gr_num].m_GroupType == NUMERALS)
            )
        return -1;

    if (NumberLikeAdj(sent[StartWordNo]) && (get_minimal_group(StartWordNo) == -1))
        return -1;

    int k = (gr_num == -1) ? StartWordNo + 1 : GetGroups()[gr_num].m_iLastWord + 1;

    if (k >= sent.size()) return -1;

    k = get_main_word(k);

    return k;
};


bool CRusFormatCaller::format_for_number_noun(CGroup &G) {
    if (GetOpt()->m_NumberAdverbsList.has_lemma(W1.get_lemma()))
        return false;

    if (sent[G.m_iFirstWord].HasFlag(fl_digit) && sent[G.m_iFirstWord].HasPOS(NUMERAL_P)) // "больше 111 рублей"
        return false;

    if (get_maximal_group(G.m_iFirstWord).m_GroupType == SELECTIVE_GR)
        return false;

    if (format_for_both(G))
        return true;

    else if (sent[get_maximal_group(G.m_iFirstWord).m_iLastWord].HasFlag(fl_russian_odin)) {
        return format_for_odin_group(G);
    } else {
        const CGroup &G1 = get_maximal_group(G.m_iFirstWord);
        if (can_start_number_noun_group(G.m_iFirstWord) != -1) {
            return format_for_number_noun_private(G);
        } else if (is_noun_group(G1)) {
            return format_for_noun_number_private(G);
        }
        return false;
    }
};


// "он отдал распоряжение часов в пять"
//  "часов в пять" - АППРОКС_ПГ
bool CRusFormatCaller::format_for_approx_noun_prep(CGroup &G) {
    int i_prep, i_number, i_noun = G.m_iFirstWord;
    const CGroup &H = get_maximal_group(G.m_iFirstWord);
    if (H.size() != 1)
        return false;
    if (!is_noun_group(H))
        return false;
    if (sent[i_noun].HasPOS(PRONOUN))
        return false;
    if (i_noun == sent.size() - 1)
        return false;
    i_prep = i_noun + 1;
    if (get_maximal_group_no(i_prep) != -1)
        return false;

    if (i_prep == sent.size() - 1)
        return false;

    size_t depend_cases;
    if (
            !sent[i_prep].get_simple_preps()
            || sent[i_prep].get_simple_preps()->empty()
            )
        return false;
    else {
        depend_cases = GetOpt()->GetOborDic()->m_Entries[(*sent[i_prep].get_simple_preps())[0]].m_AllPossibleDependCases;
    }

    i_number = i_prep + 1;

    if ((!is_small_number_group(i_number) &&
         !is_numeral(sent[i_number]) &&
         (get_maximal_group(i_number).m_GroupType != NUMERALS)
    )
            )
        return false;

    if (!(depend_cases & sent[i_number].GetGrammems()))
        return false;


    if (!gleiche_noun_numeral_for_approx(i_noun, i_number))
        return false;

    G.m_iLastWord = get_maximal_group(i_number).m_iLastWord;
    G.m_GroupType = APPROX_PREP_NOUN;
    G.m_MainGroup = get_maximal_group(i_prep);
    create_syn_rel(G, get_main_word(i_prep), get_main_word(i_noun), PREP_NOUN);
    create_syn_rel(G, get_main_word(i_noun), get_main_word(i_number), APPROX_PREP_NOUN);
    return true;
}

bool CRusFormatCaller::gleiche_noun_numeral_for_approx(int i_noun, int i_number) {
    if ((i_number >= sent.size()) || (i_noun >= sent.size()))
        return false;

    int i_number_group, number_grammems, i_last_number;

    int noun_gr = get_maximal_group_no(i_noun);
    if (noun_gr != -1)
        return false;

    uint64_t noun_grammems = sent[i_noun].GetGrammems();

    i_number = get_main_word(i_number);
    i_last_number = get_maximal_group(i_number).m_iLastWord;

    if (NumberLikeAdj(sent[i_number]))
        return false;

    if (!is_numeral(sent[i_number]) && !sent[i_number].HasFlag(fl_digit))
        return false;

    i_number_group = get_maximal_group_no(i_number);

    if (i_number_group == -1)
        number_grammems = sent[i_number].GetGrammems();
    else
        number_grammems = GetGroups()[i_number_group].GetGrammems();

    if (is_small_number_group(i_number)) {
        if (sent[i_number].HasFlag(fl_digit)) {
            if (!(((noun_grammems & (1 << rGenitiv)) &&
                   (noun_grammems & (1 << rSingular)))
                  ||
                  ((noun_grammems & (1 << rPlural)) &&
                   (!(noun_grammems & (1 << rNominativ))))))

                return false;

            return true;
        } else {
            size_t i_last_number = get_maximal_group(i_number).m_iLastWord;
            bool already_build = false;
            auto word = sent[i_last_number].get_upper_word();
            if (word.empty()) return false;

            if ("ДВА" == word ||
                "ОБА" == word ||
                "ПОЛТОРА" == word
                    ) {
                if (((noun_grammems & (1 << rMasculinum)) ||
                     (noun_grammems & (1 << rNeutrum))) &&
                    (noun_grammems & (1 << rGenitiv)) &&
                    (noun_grammems & (1 << rSingular))) {
                    already_build = true;
                }
            } else if ("ДВЕ" == word ||
                       "ОБЕ" == word ||
                       "ПОЛТОРЫ" == word) {
                if ((noun_grammems & (1 << rFeminum)) &&
                    (noun_grammems & (1 << rGenitiv)) &&
                    (noun_grammems & (1 << rSingular))) {
                    already_build = true;
                }
            } else if ("ТРИ" == word || "ЧЕТЫРЕ" == word) {
                if ((noun_grammems & (1 << rGenitiv)) &&
                    (noun_grammems & (1 << rSingular))) {
                    already_build = true;
                }
            }

            if (already_build) {
                return true;
            }

            if (noun_grammems & (1 << rPlural))
                if (number_grammems & rAllCases & noun_grammems) {
                    return true;
                }
            return false;
        }
    } else {

        if (!(((number_grammems & _QM(rNominativ)) && sent[i_noun].has_grammem(rGenitiv))
              || ((number_grammems & _QM(rGenitiv)) && sent[i_noun].has_grammem(rGenitiv))
              || ((number_grammems & _QM(rDativ)) && sent[i_noun].has_grammem(rDativ))
              || ((number_grammems & _QM(rAccusativ)) && sent[i_noun].has_grammem(rGenitiv))
              || ((number_grammems & _QM(rInstrumentalis)) && sent[i_noun].has_grammem(rInstrumentalis))
              || ((number_grammems & _QM(rLocativ)) && sent[i_noun].has_grammem(rLocativ))
        ))
            return false;

        return true;
    }

    return false;

}

