// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)
#pragma once

#include  "morph_dict/common/utilit.h"
#include  "common/gra_descr.h"


const uint16_t stSpace = 1;
const uint16_t stEOLN = 2;
const uint16_t stGrouped = 4;
const uint16_t stAbbreviation = 8;
const uint16_t stNotPrint = 16;
const uint16_t stParagraphChar = 64;
const uint16_t stEnglishName = 128;

const uint16_t stKeyModifier = 256;
const uint16_t stPunct = 512;
const uint16_t stElectronicAddress = 1024;

const uint16_t stTextAreaEnd = 2048;
const uint16_t stSingleSpaceAfter = 4096;
const uint16_t stIdent = 8192;
const uint16_t stParagraphTag = 8192 * 2;
const uint16_t stPageBreak = 8192 * 4;


class CGraphmatFile;


struct CGraphemOborot {
    std::string m_UnitStr;
    uint16_t m_UnitNo;
    // статья имеет RESTR = fixed !
    bool m_bFixedFet;
    std::vector<uint16_t> m_TokenIds;

    bool operator==(const std::string &s) {
        return s == m_UnitStr;
    };
};


class CGraLine {

    std::string m_Token;
    std::string m_TokenUpper;
    // длина  строки с учетом табуляции (screen length) и разрядки
    // Длина каждого конца строк - один символ.
    BYTE m_TokenScreenLength;

    uint64_t m_Descriptors : 60;
    MorphLanguageEnum m_Language : 4;
    uint16_t m_Status;
    uint32_t m_InputOffset;

    // memory optimization
    short m_OborotNo;

    bool is_latin_alpha(int ch) const;
    bool IsNotPrint() const;
public:

    CGraLine();

    BYTE GetTokenLength() const;
    BYTE GetLettersCount() const;

    const std::string& GetToken() const;
    const std::string& GetTokenUpper() const;
    std::string GetTokenUtf8() const;
    std::string GetTokenUpperUtf8() const;

    BYTE GetScreenLength() const;

    uint32_t GetInputOffset() const;

    uint64_t GetDescriptors() const;

    
    // we leave these function in the header, since VC doesn't want to make them inline
    bool IsSpace() const { return (m_Status & stSpace) != 0; };

    bool IsEOLN() const { return (m_Status & stEOLN) != 0; };

    bool IsSoft() const { return IsSpace() || IsEOLN(); };

    bool IsPunct() const { return (m_Status & stPunct) != 0; };

    bool IsIdent() const;;

    bool IsEnglishName() const;

    bool IsGrouped() const;

    bool IsAbbreviation() const;

    bool IsParagraphChar() const;

    bool IsPageBreak() const;

    bool IsTextAreaEnd() const;

    bool IsElectronicAddress() const;

    bool IsParagraphTag() const;

    bool IsKeyModifier() const;

    bool IsChar(int c) const;

    bool IsAsterisk() const;

    bool HasSingleSpaceAfter() const;

    bool IsString(const std::string &) const;

    bool IsUpperString(const std::string&) const;

    bool IsOneOfUpperStrings(const std::vector<std::string>& v) const;

    int ToInt() const;

    bool HasMacroSyntaxDelimiter() const;

    bool IsWordOrNumberOrAbbr() const;

    void SetParagraphChar();

    void SetParagraphTag();

    void SetKeyModifier();

    void SetSingleSpaceAfter();

    void Initialize(MorphLanguageEnum l, size_t offset);

    void SetElectronicAddress();

    void SetEnglishName();

    void DelDes(Descriptors d);

    void SetDes(Descriptors d);

    bool HasDes(Descriptors d) const;

    void MakeSpaces(size_t SpacesLength);

    void AddStatus(uint16_t add_state);

    void AppendToken(const CGraLine &L);

    void ReadWord(const char* in_str);

    bool IsSingleSpaceToDelete() const;

    void SetOborot(short oborotNo, bool fixed);

    std::string GetGraphematicalLine() const;

    void InitNonContextDescriptors(bool _force_to_rus);

    bool IsBulletWord() const;
    bool IsOneAlpha() const;
    bool IsComma() const;
    bool FirstUpper() const;
    bool is_lowercase(int ch) const;
    bool is_uppercase(int ch) const;
    bool ReadEolns(const char* in_str);
    bool ReadSpaces(const char* in_str, int tab_size);
    void SetToken(short status, const char* s, size_t len, BYTE screen_len=0);
    void SetStatus(short status);
    MorphLanguageEnum GetTokenLanguage() const;
    short  GetOborotNo() const;
};
