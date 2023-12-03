// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Graphematical Module (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1996-2001)

#include "StdGraph.h"
#include  "../../common/gra_descr.h"
#include  "../../common/single_byte_encoding.h"
#include  "graline.h"
#include  "GraphmatFile.h"
#include "GraphanDicts.h"


CGraLine::CGraLine() {
    m_Status = 0;
    m_Descriptors = 0;
    m_TokenScreenLength = 0;
    m_InputOffset = 0;
    m_OborotNo = -1;
    m_Language = morphUnknown;
};

BYTE CGraLine::GetTokenLength() const { 
    return (BYTE)m_Token.length();
};

const std::string& CGraLine::GetToken() const { 
    return m_Token;
};

const std::string& CGraLine::GetTokenUpper() const {
    return m_TokenUpper;
};

void CGraLine::Initialize(MorphLanguageEnum l, size_t offset) {
    assert(l < 16); // see bit field  optimization
    m_Language = l;
    m_InputOffset = offset;
    m_TokenScreenLength = 0;
}

BYTE CGraLine::GetScreenLength() const { 
    // "\t" => 8 
    // "\r\n" -> 1
    return m_TokenScreenLength; 
};

uint32_t CGraLine::GetInputOffset() const { 
    return m_InputOffset; 
};

uint64_t CGraLine::GetDescriptors() const { 
    return m_Descriptors; 
};

bool CGraLine::HasMacroSyntaxDelimiter() const {
    return (m_Descriptors
            & (_QM(CS_Undef)
               | _QM(CS_Simple)
               | _QM(CS_Parent)
               | _QM(CS_Heading)
               | _QM(CS_Explan)
            )
           ) > 0;
};

bool CGraLine::IsWordOrNumberOrAbbr() const {
    return
            (m_Descriptors
             & (_QM (ORLE)
                | _QM (OLLE)
                | _QM (ODigits)
                | _QM (ONumChar)
                | _QM (OAbbr1)
             )
            ) != 0;
}


bool CGraLine::IsSingleSpaceToDelete() const {
    return (GetTokenLength() == 1)
           && (GetToken()[0] == ' ') // we delete only space, leaving alone tabulations
           && (m_Status == stSpace); // this line means that there is no other meaning for this
    // space, for example, it prevents to consider a converted "</p" to be deleted
};


void CGraLine::MakeSpaces(size_t spaces_count) {
    m_Descriptors = _QM(ODel) | _QM(OSpc);
    m_Status = stSpace;
    m_TokenScreenLength = (BYTE)spaces_count;
    m_Token = std::string(spaces_count, ' ');
    m_TokenUpper = m_Token;
};


int CGraLine::ToInt() const {
    return atoi(m_Token.c_str());
};

bool CGraLine::IsBulletWord() const
{
    return HasDes(ODigits) || IsOneAlpha();
};

bool CGraLine::IsNotPrint() const {
    return (m_Status & stNotPrint) != 0;
};

bool CGraLine::IsEnglishName() const {
    return (m_Status & stEnglishName) != 0;
};

bool  CGraLine::IsOneAlpha() const
{
    return  ((HasDes(ORLE) || HasDes(OLLE)) && (GetTokenLength() == 1))
        || ((GetTokenLength() == 1) && is_latin_alpha((unsigned char)GetToken()[0]));
};

bool  CGraLine::IsComma() const
{
    return GetTokenLength() == 1 && GetToken()[0] == (BYTE)',';
};


bool  CGraLine::FirstUpper() const
{
    return  HasDes(OUp) || HasDes(OUpLw);
};


bool CGraLine::IsIdent() const {
    return (m_Status & stIdent) != 0;
};;

bool CGraLine::IsGrouped() const {
    return (m_Status & stGrouped) != 0;
};

bool CGraLine::IsAbbreviation() const {
    return (m_Status & stAbbreviation) != 0;
};

bool CGraLine::IsParagraphChar() const {
    return (m_Status & stParagraphChar) != 0;
};

bool CGraLine::IsPageBreak() const {
    return (m_Status & stPageBreak) != 0;
};

bool CGraLine::IsTextAreaEnd() const {
    return (m_Status & stTextAreaEnd) != 0;
};

bool CGraLine::IsParagraphTag() const {
    return (m_Status & stParagraphTag) != 0;
};

bool CGraLine::IsKeyModifier() const {
    return (m_Status & stKeyModifier) != 0;
};

bool CGraLine::IsElectronicAddress() const {
    return (m_Status & stElectronicAddress) != 0;
};

bool CGraLine::IsChar(int c) const {
    return m_Token.length() == 1 && m_Token[0] == c;
};

bool CGraLine::is_lowercase(int ch) const
{
    if (m_Language == morphGerman)
        return is_german_lower(ch);
    else
        return is_russian_lower(ch) || is_english_lower(ch);

};

bool CGraLine::is_uppercase(int ch) const
{
    if (m_Language == morphGerman)
        return is_german_upper(ch);
    else
        return is_russian_upper(ch) || is_english_upper(ch);

};

bool CGraLine::IsAsterisk() const {
    return IsChar((unsigned char) '*') || IsChar(149);
};

bool CGraLine::HasSingleSpaceAfter() const { return (m_Status & stSingleSpaceAfter) > 0; };


bool CGraLine::IsString(const std::string& s) const {
    return s == m_Token;
};

bool CGraLine::IsUpperString(const std::string& s) const {
    return s == m_TokenUpper;
}

bool CGraLine::IsOneOfUpperStrings(const std::vector<std::string>& v) const {
    for (const auto& s : v) {
        if (s == m_TokenUpper) {
            return true;
        }
    }
    return false;
}


void CGraLine::SetParagraphChar() {
    m_Status |= stParagraphChar;
};

void CGraLine::SetParagraphTag() {
    m_Status |= stParagraphTag;
};

void CGraLine::SetKeyModifier() {
    m_Status |= stKeyModifier;
};

void CGraLine::SetElectronicAddress() {
    m_Status |= stElectronicAddress;
};


void CGraLine::SetSingleSpaceAfter() {
    m_Status |= stSingleSpaceAfter;
};


void CGraLine::SetEnglishName() {
    m_Status |= stEnglishName;
};

void CGraLine::DelDes(Descriptors d) {
    m_Descriptors &= ~(_QM(d));
};


void CGraLine::SetDes(Descriptors d) {
    m_Descriptors |= _QM(d);
};

bool CGraLine::HasDes(Descriptors d) const {
    return (m_Descriptors & _QM(d)) > 0;
};


void CGraLine::AddStatus(uint16_t add_state) {
    m_Status |= add_state;
};

void CGraLine::AppendToken(const CGraLine &t) {
    m_Token += t.m_Token;
    m_TokenUpper += t.m_TokenUpper;
    m_TokenScreenLength += t.m_TokenScreenLength;
};




bool CGraLine::ReadEolns(const char* in_str) {
    assert (m_TokenScreenLength == 0);
    size_t len = 0;
    while (in_str[len] == '\r' && in_str[len + 1] == '\n' && len + 1 < CriticalTokenLength)
    {
        len += 2;
        m_TokenScreenLength++;
    }
    if (len == 0) {
        while (in_str[len] == '\n' && len < CriticalTokenLength)
        {
            len++;
        }
    }
    if (len == 0) {
        while (in_str[len] == '\r' && len < CriticalTokenLength)
        {
            len++;
        }
    }
    if (len > 0) {
        SetToken(stEOLN, in_str, len);
        return true;
    }
    return false;
}

bool CGraLine::ReadSpaces(const char* in_str, int tab_size) {
    assert(m_TokenScreenLength == 0);
    size_t len = 0;
    while (isnspace((BYTE)in_str[len])) {
        len++;
        m_TokenScreenLength += (in_str[len] == '\t') ? tab_size : 1;
    }
    if (len > 0) {
        SetToken(stSpace, in_str, len);
        return true;
    }
    return false;
}

void CGraLine::SetToken(short status, const char * s, size_t len, BYTE screen_len) {
    assert(len > 0);
    if (len >= CriticalTokenLength) {
        len = CriticalTokenLength;
    }
    m_Token = std::string(s, len);
    m_TokenUpper = m_Token;
    RmlMakeUpper(m_TokenUpper, m_Language);
    m_Status |= status;
    if (screen_len > 0)
        m_TokenScreenLength = screen_len;
    else if (m_TokenScreenLength == 0) {
        m_TokenScreenLength = len;
    }
}


void CGraLine::SetOborot(short oborotNo, bool bFixed) {
    m_OborotNo = oborotNo;
    if (m_OborotNo != -1 && bFixed) {
        SetDes(OFixedOborot);
    }
    else {
        DelDes(OFixedOborot);
    }
}

std::string CGraLine::GetGraphematicalLine() const
{
    std::string result;

    if (!IsSoft())
    {
        if (!IsNotPrint())
        {
            result.append(m_Token);
        }
        else {
            result.append(1, GraphematicalSPACE);
        }
    }
    else
    {
        for (size_t k = 0; k < GetTokenLength(); ++k)
            switch (GetToken()[k])
            {
            case ' ': result.append(1, GraphematicalSPACE); break;
            case '\t': result.append(1, GraphematicalTAB); break;
            case '\n': result.append(1, GraphematicalEOLN); break;
            case '\r':  break;
            default: assert(false); break;
            };
    };

    result.append(Format("\t%zu %zu\t", GetInputOffset(), GetTokenLength()));

    // write descriptors 
    for (int l = 0; l < 63; l++) {
        if ((GetDescriptors() & _QM(l)) > 0)
        {
            result.append(" ");
            result.append(GetDescriptorStr(l));
        };
    }

    if (m_OborotNo != -1)
    {
        result.append(Format(" EXPR_NO%zu", m_OborotNo));
    }

    if (IsParagraphTag())
    {
        result.append(" PARTAG");
    }
    return convert_to_utf8(result, m_Language);
};

bool CGraLine::is_latin_alpha(int ch) const
{
    if (m_Language == morphGerman)
        return is_german_alpha(ch);
    else
        return is_english_alpha(ch);
};

// в первой строке находятся латинские буквы, который могут быть заменены на
// русские
const static std::string latin_chars = "AaEe3KkMHOoPpCcyXxuT";

// во второй строке находятся русские
const static std::string russian_chars = _R("АаЕеЗКкМНОоРрСсуХхиТ");


BYTE force_rus_char(BYTE ch)
{
    size_t i = latin_chars.find(ch);

    if (i == std::string::npos) return 0;

    return russian_chars[i];
}



std::string force_to_rus(const std::string& s)
{
    std::string res;
    for (BYTE ch : s) {
        if (ch < 128 && is_english_alpha(ch)) {
            ch = force_rus_char(ch);
            if (ch == 0)
                return "";
        }
        res += ch;
    }
    return res;
}

void CGraLine::InitNonContextDescriptors(bool b_force_to_rus)
{
    bool	fl_ra = false,  /* rus alpha */
        fl_la = false,  /* lat alpha */
        fl_lw = false,  /* lower alpha */
        fl_up = false,  /* upper alpha */
        fl_decimal_number = false;

    BYTE first_char = (BYTE)m_Token[0];


    if (IsSpace())
    {
        SetDes(ODel);
        SetDes(OSpc);
        return;
    }

    if (IsEOLN())
    {
        SetDes(ODel);
        SetDes(OEOLN);
        return;
    }

    if (IsIdent())
    {
        SetDes(ONumChar);
        return;
    }

    if (IsElectronicAddress())
    {
        SetDes(OElectAddr);
        SetDes(ONumChar);
        return;
    }


    if (IsNotPrint())
    {
        SetDes(ODel);
        if (IsParagraphChar())
            SetDes(OParagraph);
        else
            SetDes(ONil);

        return;
    }

    if (IsPunct())
    {
        SetDes(OPun);

        int BracketClassNo = isbracket(first_char);

        if (BracketClassNo)
            SetDes((BracketClassNo == 1) ? OOpn : OCls);
        else
            if (first_char == cHyphenChar)
                SetDes(OHyp);

        if (m_Token.length() > 1)
            SetDes(OPlu);

        return;
    }
    bool has_hyphen = false;
    for (auto& c: m_Token)
    {
        if (c == Apostrophe) continue;
        if (c == cHyphenChar) has_hyphen = true;
        if (m_Language == morphRussian)
        {
            if (is_russian_alpha((BYTE)c))
                fl_ra = true;
            else
                if (is_latin_alpha(c))
                    fl_la = true;
        }
        else
            fl_la = fl_la || is_latin_alpha(c);

        fl_decimal_number = fl_decimal_number || isdigit((BYTE)c);

        if ((m_Language != morphGerman)
            || ((c!= szlig) // ignore these symbols, they are equal in uppercase an in lowercase
                && (c!= Nu)
                )
            )
        {
            fl_lw = fl_lw || is_lowercase((BYTE)c);
            fl_up = fl_up || is_uppercase((BYTE)c);
        };

    }

    if (!fl_decimal_number && (fl_ra || fl_la))
    {
        // встретились русские и латинские буквы, 
        // но нет дефиса, чтобы не трогать "IP-адрес"
        if (fl_ra && fl_la && !has_hyphen)
            if (!b_force_to_rus) // если не надо приводить к русскому алфавиту
                SetDes(OUnk); // установить дескриптор "вопрос"
            else // попробовать привести слово к русскому алфав.
            {
                auto s = force_to_rus(m_Token);
                if (s.empty()) {
                    SetDes(OUnk);
                }
                else {
                    SetDes(ORLE);
                    SetToken(0, s.c_str(), s.length());
                }
            }
        else
            if (fl_ra)
                SetDes(ORLE);
            else
                SetDes(OLLE);

        if (fl_lw && !fl_up)
            SetDes(OLw);

        if (!fl_lw && fl_up)
            SetDes(OUp);

        if (fl_lw && is_uppercase(first_char))
            SetDes(OUpLw);

        return;
    }

    if (((BYTE)first_char == '\'') && m_Token.length()== 1)
    {
        SetDes(ODel);
        SetDes(OPun);
    };


    if ((fl_decimal_number && !fl_la && !fl_ra))
        SetDes(ODigits);

    if (fl_decimal_number && (fl_ra || fl_la)) SetDes(ONumChar);

    if (GetDescriptors() == 0) SetDes(OUnk);
}


