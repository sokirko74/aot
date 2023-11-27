#pragma once

#include "morph_dict/common/utilit.h"

class CSemanticsHolder;
class CStructDictHolder;
class CDictionary;


class CSemWord {

    // граммемы, которые приписаны слову
    grammems_mask_t m_FormGrammems;
    // общие граммемы, которые приписаны слову
    grammems_mask_t m_TypeGrammems;

public:
    // прописная или строчная буква
    RegisterEnum m_CharCase;
    // лемма
    std::string m_Lemma;
    std::string m_GramCodes;
    // номер парадигмы в морф. словаре
    long m_ParadigmId;
    // добавочный номер парадигмы в морф. словаре (для приложений типа "муж-алкоголик")
    long m_AdditParadigmId;
    // части речи, которые приписаны слову
    part_of_speech_mask_t m_Poses;
    // номер слова в синтаксическом представлении
    long m_WordNo;
    // Полные эквиваленты слова, здесь обычно лежит
    // аспектный вариант глагола (совершенный, несовершенный виды)
    StringVector m_WordEquals;
    // используется в словах типа "трехоконный", которые переводятся в "3-fenestral"
    std::string m_NumeralPrefix;

    long m_WordWeight;
    // является ли это слово наречием "по-английски", "по-хорошему"...
    bool m_bAdverbFromAdjective;

    // графематическая помета ЦК (цифровой комплекс)
    bool m_ArabicNumber;

    // указатель на все словари системы
    const CSemanticsHolder* m_pData;

    // две обертки для функций из CSemanticsHolder
    const CStructDictHolder* GetRossHolder(DictTypeEnum Type) const;

    const CDictionary* GetRoss(DictTypeEnum Type) const;


    CSemWord();

    CSemWord(long WordNo, std::string Lemma);

    void Init();

    // принадлежит ли граммема слову
    bool HasOneGrammem(grammem_t grammem) const;

    bool operator==(const long& X) const;

    // проверка, что часть речи POS принадлежит  данному слову
    virtual bool HasPOS(part_of_speech_t POS) const = 0;

    bool IsRusSubstPronounP() const;

    grammems_mask_t GetAllGrammems() const;

    grammems_mask_t GetFormGrammems() const;

    void SetFormGrammems(grammems_mask_t);

    grammems_mask_t GetTypeGrammems() const;

    void SetTypeGrammems(grammems_mask_t);

    void AddFormGrammem(grammem_t g);

    virtual const std::string& GetWord() const = 0;
};
