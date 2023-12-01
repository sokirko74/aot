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
    unit = NULL;
    m_Descriptors = 0;
    ulen = slen = 0;
    m_InputOffset = 0;
    m_PageNumber = UnknownPageNumber;
    m_OborotNo = -1;
    m_Language = morphUnknown;
};

BYTE CGraLine::GetTokenLength() const { 
    return ulen; 
};

const char* CGraLine::GetToken() const { 
    return unit; 
};


BYTE CGraLine::GetScreenLength() const { 
    return slen; 
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


void CGraLine::MakeSpaces(size_t SpacesLength) {
    m_Descriptors = _QM(ODel) | _QM(OSpc);
    m_Status = stSpace;
    ulen = (BYTE) SpacesLength;
    slen = (BYTE) SpacesLength;
    memset(const_cast<char *>(unit), ' ', SpacesLength);
};


int CGraLine::ToInt() const {
    char s[100];
    strncpy(s, unit, ulen);
    s[ulen] = 0;
    return atoi(s);
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
    return (ulen == 1) && (unit[0] == c);
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
    return (s.length() == ulen) && !s.compare(0, ulen, unit);
};

void CGraLine::SetSpace() {
    m_Status |= stSpace;
};

void CGraLine::SetEOLN() {
    m_Status |= stEOLN;
};

void CGraLine::SetNotPrint() {
    m_Status |= stNotPrint;
};

void CGraLine::SetPunct() {
    m_Status |= stPunct;
};

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

void CGraLine::SetIdent() {
    m_Status |= stIdent;
};

void CGraLine::SetPageBreak() {
    m_Status |= stPageBreak;
};

void CGraLine::SetTextAreaEnd() {
    m_Status |= stTextAreaEnd;
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

void CGraLine::AddLength(const CGraLine &L) {
    ulen += L.GetTokenLength();
    slen += L.GetScreenLength();
};


void CGraLine::SetToken(const char *s) {
    unit = s;
};


size_t GetInternetAddressStarter(const char *s) {
    if (!strncmp(s, "http://", strlen("http://"))) return strlen("http://");
    if (!strncmp(s, "HTTP://", strlen("http://"))) return strlen("http://");

    if (!strncmp(s, "ftp://", strlen("ftp://"))) return strlen("ftp://");
    if (!strncmp(s, "FTP://", strlen("ftp://"))) return strlen("ftp://");

    if (!strncmp(s, "ftp.", strlen("ftp."))) return strlen("ftp.");
    if (!strncmp(s, "FTP.", strlen("ftp."))) return strlen("ftp.");

    if (!strncmp(s, "www.", strlen("www."))) return strlen("www.");
    if (!strncmp(s, "WWW.", strlen("www."))) return strlen("www.");

    if (!strncmp(s, "www2.", strlen("www2."))) return strlen("www2.");
    if (!strncmp(s, "WWW2.", strlen("www2."))) return strlen("www2.");

    return 0;
}


size_t CGraLine::LengthUntilDelimiters(const char *s, const CGraphmatFile *G) {
    bool bElectronicAddress = GetInternetAddressStarter(s) > 0;
    int i = 0;
    for (i = 0; i < CriticalTokenLength; i++) {

        if (is_alpha((BYTE) s[i])) {
            if (i == 0) // prohibit apostrophe at the first position
                if (s[i] == Apostrophe)
                    break;
            continue;
        };


        if (isdigit((BYTE) s[i])) continue;

        if (G->m_pDicts->IsRegisteredKeyModifier(s, i)) {
            SetKeyModifier();
            break;
        };

        if (i > 0) {
            if (s[i] == '-') //  let an inner hyphen be part of the word, for example "test-test"
                if ((i + 1 < CriticalTokenLength) && isdigit((BYTE) s[0]) && isdigit((BYTE) s[i + 1]) &&
                    isdigit((BYTE) s[i - 1])) // not ok for "1-2"
                    break;
                else continue;
            if ((s[i] == '.') && (i + 1 < CriticalTokenLength)) {
                if ((isdigit((BYTE) s[i - 1]) == isdigit((BYTE) s[i + 1]))
                    && ((G->m_Language != morphRussian)    // prohibit  "." as a word part  for Russian
                        || !is_russian_alpha((BYTE) s[i - 1])  // for example: г.Самара, В.И.Ленин
                        || !is_russian_alpha((BYTE) s[i + 1])
                    )
                        )
                    continue; //  if "." delimits alphas or digits, let an inner full stops be part of the word, for example "www.lenta.ru" or 1.12.12
                // we exclude cases, if the full stop delimits digits and alphas, since it can lead to tokenization errors, for example such "1.We go to the north;2.We go to the south;"
            };

            if (s[i] == '/') continue; //  let an inner slash be part of the word, for example "TCP/IP"
            if (s[i] == '_') continue; //  let an underscore be part of the word, for example "al_sokirko"
        };

        if (bElectronicAddress) {
            if (s[i] == '.') continue;
            if (s[i] == '_') continue;
            if (s[i] == '/') continue;
            if (s[i] == '\\') continue;
            if (s[i] == ':') continue;
        };
        if (s[i] == '@') {
            // sokirko@medialingua.ru
            if (i + 1 < CriticalTokenLength) {
                if (is_alpha((BYTE) s[i + 1]) || isdigit((BYTE) s[i + 1])) {
                    bElectronicAddress = true;
                    continue;
                };
            };

        };

        break;
    };
    //  exclude the last full stop or  slash from the word,
    //  for example we  do not consider "Israel/" as one word, but as two tokens "Israel" and "/".
    //  but  do not exclude the last hyphen, cf German examples:
    //   "Reichsfinanz- und Reichsinnenministers"

    for (; i > 0 && (s[i - 1] == '.' || s[i - 1] == '/' || s[i - 1] == ':' || s[i - 1] == '\''); i--);

    if (i == 0)
        return 1; // return 1, if this is not a word and not a number
    else {
        if (bElectronicAddress)
            SetElectronicAddress();

        return i;
    };
}


// читает из буфера b в структуру L
size_t CGraLine::ReadWord(size_t Offset, const CGraphmatFile *G, uint32_t &PageNumber) {
    PageNumber = UnknownPageNumber;
    assert(G->m_Language < 8); // see bit field  optimization
    m_Language = G->m_Language;
    const char *s;
    const char *In = (const char *) &(G->GetInputBuffer()[0]);
    char *Out = const_cast<char *>(unit);
    BYTE len;
    ulen = slen = 0;


    m_InputOffset = Offset;


    /*   If Carriage Return occurs ...*/
    if (In[Offset] == '\r') {
        if (In[Offset + 1] != '\n') {
            if (G->m_bFilterUnprintableSymbols) {
                Out[0] = ' ';
                SetNotPrint();
            } else {
                Out[0] = In[Offset];
                SetPunct();
            };
            Offset++;
            slen = 1;
            ulen = 1;
        } else {
            do {
                Out[ulen] = '\r';
                Out[ulen + 1] = '\n';
                if (In[Offset + 1] != '\n') break;
                ulen += 2;
                slen++;
                Offset += 2;
            } while ((In[Offset] == '\r') && (ulen < CriticalTokenLength));
            SetEOLN();
        }
    } else if (In[Offset] == '\n') {
        do {
            Out[ulen] = '\n';
            ulen += 1;
            Offset++;
            slen++;

        } while ((In[Offset] == '\n') && (ulen < CriticalTokenLength));
        SetEOLN();
    } else
        /*  if TCP/IP occurs ..*/
    if (G->m_pDicts->FindInIdents(In + Offset, len) && len <= CriticalTokenLength) {
        slen = len;
        ulen = len;
        memcpy(Out, In + Offset, ulen);
        Offset += ulen;
        SetIdent();
    } else
        /*  if a Bracket occurs ..*/
    if (isbracket((BYTE) In[Offset])) {
        *Out = In[Offset];
        slen = ulen = 1;
        Offset++;
        SetPunct();
    } else
        /* if a Space or Tabulation occurs ... */
    if (isnspace((BYTE) In[Offset])) {
        for (;
                isnspace(In[Offset]) && ulen < CriticalTokenLength;
                ulen++, slen += (In[Offset] == '\t') ? G->m_TabSize : 1, Offset++
                )
            Out[ulen] = In[Offset];

        SetSpace();
    } else
        /*
        последовательность из восклицательных и вопросительных знаков
        (используется как конец предложения)
        */
    if (((BYTE) In[Offset] == '?')
        || ((BYTE) In[Offset] == '!')
            ) {
        for (int ch1 = In[Offset];
             ((In[Offset] == '?')
              || (In[Offset] == '!'))
             && (ulen < CriticalTokenLength);
             Offset++,
                     slen++, ulen++
                )
            Out[ulen] = In[Offset];

        SetPunct();
    } else
        /* if it is a hard delimiter */
    if (std::iswpunct(In[Offset]) || is_pseudo_graph((BYTE) In[Offset])) {
        for (int ch1 = In[Offset];
             (In[Offset] == ch1) && (ulen < CriticalTokenLength);
             Offset++,
                     slen++, ulen++
                )
            Out[ulen] = ch1;

        SetPunct();
    } else
        /*  If it is not printable symbol and if the non-printable symbols should be filtered */
    if (((BYTE) In[Offset] < 32)
        || ((BYTE) In[Offset] == cIonChar)
        || ((BYTE) In[Offset] == cNumberChar)
        || ((BYTE) In[Offset] == cPiChar)
        || ((BYTE) In[Offset] == cCompanyChar)
        || ((BYTE) In[Offset] == cEllipseChar)
            ) {
        if (G->m_bFilterUnprintableSymbols || !In[Offset]) {
            Out[0] = ' ';
            SetNotPrint();
        } else {
            Out[0] = In[Offset];
            SetPunct();
        };

        if ((BYTE) In[Offset] == cParagraph)
            SetParagraphChar();

        Offset++;
        slen = 1;
        ulen = 1;
    } else
        /* If a word, number or something else  occurs ...*/
    {
        int WordLength;

        bool bCanBeSpaceDelimitedWord =
                ((G->GetInputBuffer().size() - Offset) > 2)
                && is_spc_fill(In[Offset + 1]);

        s = bCanBeSpaceDelimitedWord ? G->m_pDicts->SearchSpace(In + Offset, &WordLength) : NULL;

        if (s != NULL) {
            // spaced words
            ulen = (BYTE) strlen(s);
            strncpy(Out, s, ulen);
            Offset += WordLength;
        } else {
            const char *CurrIn = In + Offset;

            WordLength = LengthUntilDelimiters(CurrIn, G);


            // sequence N% is one token)
            if (WordLength == 1)
                if ((BYTE) CurrIn[WordLength - 1] == 'N')
                    if (Offset + WordLength < G->GetInputBuffer().size())
                        if ((BYTE) CurrIn[WordLength] == '%') {
                            WordLength++;
                        };

            if (Offset + WordLength >= G->GetInputBuffer().size())
                WordLength = G->GetInputBuffer().size() - Offset;

            if (WordLength >= CriticalTokenLength)
                WordLength = CriticalTokenLength;

            strncpy(Out, CurrIn, WordLength);

            slen = ulen = WordLength;
            Offset += WordLength;
        }
    };
    assert (ulen <= CriticalTokenLength);

    return Offset;
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

short CGraLine::GetPageNumber() const
{
    return m_PageNumber;
}

void CGraLine::SetPageNumber(short page) {
    m_PageNumber = page;
}

std::string CGraLine::GetGraphematicalLine() const
{
    std::string result;

    if (!IsSoft())
    {
        if (!IsNotPrint()
            && (GetToken() != NULL)
            && (GetToken()[0] != 0)
            )
        {
            result.append(GetToken(), GetTokenLength());
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

    if (IsPageBreak())
    {
        result.append(Format(" PGBR%zu", m_PageNumber));
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

void CGraLine::InitNonContextDescriptors(bool b_force_to_rus)
{
    bool	fl_ra = false,  /* rus alpha */
        fl_la = false,  /* lat alpha */
        fl_lw = false,  /* lower alpha */
        fl_up = false,  /* upper alpha */
        fl_decimal_number = false;

    size_t TokenLength = GetTokenLength();
    const char* Token = GetToken();
    BYTE first_char = (BYTE)Token[0];


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


        if (TokenLength > 1)
            SetDes(OPlu);

        return;
    }
    bool has_hyphen = false;
    for (int i = 0; i < TokenLength; i++)
    {
        if ((BYTE)Token[i] == Apostrophe) continue;
        if ((BYTE)Token[i] == cHyphenChar) has_hyphen = true;
        if (m_Language == morphRussian)
        {
            if (is_russian_alpha((BYTE)Token[i]))
                fl_ra = true;
            else
                if (is_latin_alpha((BYTE)Token[i]))
                    fl_la = true;
        }
        else
            fl_la = fl_la || is_latin_alpha((BYTE)Token[i]);

        fl_decimal_number = fl_decimal_number || isdigit((BYTE)Token[i]);

        if ((m_Language != morphGerman)
            || (((BYTE)Token[i] != szlig) // ignore these symbols, they are equal in uppercase an in lowercase
                && ((BYTE)Token[i] != Nu)
                )
            )
        {
            fl_lw = fl_lw || is_lowercase((BYTE)Token[i]);
            fl_up = fl_up || is_uppercase((BYTE)Token[i]);
        };

    }

    if (!fl_decimal_number
        && (fl_ra || fl_la)
        )
    {
        // встретились русские и латинские буквы, 
        // но нет дефиса, чтобы не трогать "IP-адрес"
        if (fl_ra && fl_la && !has_hyphen)
            if (!b_force_to_rus) // если не надо приводить к русскому алфавиту
                SetDes(OUnk); // установить дескриптор "вопрос"
            else // попробовать привести слово к русскому алфав.
            {
                char s[255];
                if (!force_to_rus(s, Token, TokenLength)) // если не удалось привести
                    SetDes(OUnk);
                else
                {
                    memcpy(const_cast<char*>(Token), s, TokenLength); // приводим
                    SetDes(ORLE);  // говорим, что это русская лексема
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

    if (((BYTE)first_char == '\'')
        && (TokenLength == 1)
        )
    {
        SetDes(ODel);
        SetDes(OPun);
    };


    if ((fl_decimal_number && !fl_la && !fl_ra))
        SetDes(ODigits);

    if (fl_decimal_number && (fl_ra || fl_la)) SetDes(ONumChar);

    if (GetDescriptors() == 0) SetDes(OUnk);
}


