#include "DumpParadigm.h"
#include "wizard.h"
#include "../common/util_classes.h"


bool CMorphSession::operator==(const CMorphSession& X) const {
    return (m_UserName == X.m_UserName)
        && (m_SessionStart == X.m_SessionStart)
        && (m_LastSessionSave == X.m_LastSessionSave);
};

void CMorphSession::SetEmpty() {
    m_UserName = "";
    m_SessionStart = "";
    m_LastSessionSave = "";
};

bool CMorphSession::IsEmpty() const {
    return m_UserName.empty();
};

bool CMorphSession::ReadFromString(const std::string& s) {
    StringTokenizer Tok(s.c_str(), ";\r\n");
    if (!Tok()) {
        SetEmpty();
        return false;
    };
    m_UserName = Tok.val();
    if (!Tok()) {
        SetEmpty();
        return false;
    };
    m_SessionStart = Tok.val();
    if (!Tok()) {
        SetEmpty();
        return false;
    };
    m_LastSessionSave = Tok.val();
    return true;
}

std::string CMorphSession::ToString() const {
    return Format("%s;%s;%s", m_UserName.c_str(), m_SessionStart.c_str(), m_LastSessionSave.c_str());
};


//=====================================================================

void CDumpParadigm::SetEmpty() {
    m_PrefixesStr = "";
    m_TypeGrammemsStr = "";
    m_AuthorStr = "";
    m_SlfStr = "";
    m_FirstSlfLineNo = -1;
    m_Session.SetEmpty();
};

const char TypeGrmField[] = "$type_grm";
const char PrefixesField[] = "$prefixes";
const char SessionField[] = "$session";

bool CDumpParadigm::ReadFromFile(FILE* fp, int& line_no, bool& bError, std::string& Errors) {
    SetEmpty();
    if (feof(fp)) return false;
    char buf[1000];

    bError = false;
    while (fgets(buf, 1000, fp)) {
        line_no++;
        std::string s = convert_from_utf8(buf, m_pWizard->m_Language);
        {
            const char* auth_patt = "//edited by ";
            int qq = s.find(auth_patt);
            if (qq != std::string::npos) {
                m_AuthorStr = s.substr(qq + strlen(auth_patt));
                Trim(m_AuthorStr);
            };
        };
        {
            int qq = s.find("//");
            if (qq != std::string::npos)

                s.erase(qq);

        };
        Trim(s);
        if (s.empty()) continue;
        if (s.substr(0, 5) == "=====") {
            if (m_SlfStr.empty() && m_TypeGrammemsStr.empty() && m_PrefixesStr.empty() && !bError)
                continue;
            else
                break;
        };

        if (s.substr(0, strlen(PrefixesField)) == PrefixesField) {
            int ind = s.find("=");
            if (ind == std::string::npos) {
                Errors += Format("cannot parse %s field at line", PrefixesField, line_no);
                bError = true;
            }
            else {
                m_PrefixesStr = s.substr(ind + 1);
                Trim(m_PrefixesStr);
            };
            continue;
        };

        if (s.substr(0, strlen(TypeGrmField)) == TypeGrmField) {

            int ind = s.find("=");
            if (ind == std::string::npos) {
                Errors += Format("cannot parse %s field at line %i", TypeGrmField, line_no);
                bError = true;
            }
            else {
                m_TypeGrammemsStr = s.substr(ind + 1);
                Trim(m_TypeGrammemsStr);
            };
            continue;
        };

        if (s.substr(0, strlen(SessionField)) == SessionField) {

            int ind = s.find("=");
            if (ind == std::string::npos) {
                Errors += Format("cannot parse %s field at line %i", SessionField, line_no);
                bError = true;
            }
            else {
                std::string SessionStr = s.substr(ind + 1);
                Trim(SessionStr);
                if (!m_Session.ReadFromString(SessionStr)) {
                    Errors += Format("cannot parse %s field at line %i", SessionField, line_no);
                    bError = true;

                };
            };
            continue;
        };

        if (m_FirstSlfLineNo == -1)
            m_FirstSlfLineNo = line_no - 1;
        m_SlfStr += s;
        m_SlfStr += "\r\n";
    };

    return !m_SlfStr.empty();
};

bool CDumpParadigm::SaveHeaderToFile(FILE* fp) const {
    fprintf(fp, "=====\n");

    if (!m_PrefixesStr.empty())
        fprintf(fp, "%s = %s\n", PrefixesField, convert_to_utf8(m_PrefixesStr, m_pWizard->m_Language).c_str());


    if (!m_TypeGrammemsStr.empty())
        fprintf(fp, "%s = %s\n", TypeGrmField, convert_to_utf8(m_TypeGrammemsStr, m_pWizard->m_Language).c_str());

    if (!m_Session.IsEmpty())
        fprintf(fp, "%s = %s\n", SessionField, convert_to_utf8(m_Session.ToString(), m_pWizard->m_Language).c_str());
    return true;
}

bool CDumpParadigm::SaveToFile(FILE* fp) const {
    if (!SaveHeaderToFile(fp))
        return false;
    fprintf(fp, "%s", convert_to_utf8(m_SlfStr, m_pWizard->m_Language).c_str());
    return true;
};

bool CDumpParadigm::ReadNextParadigmFromFile(FILE* fp, int& line_no, bool& bError, std::string& Errors) {
    int start_line_no = line_no;

    if (!ReadFromFile(fp, line_no, bError, Errors)) return false;

    if (!m_pWizard->check_common_grammems(m_TypeGrammemsStr)) {
        Errors += Format("cannot process common grammems in the paradigm at line %i \n", start_line_no);
        bError = true;
    };
    if (!m_pWizard->check_prefixes(m_PrefixesStr)) {
        Errors += Format("cannot process prefixes in the paradigm at  line %i \n", start_line_no);
        bError = true;
    };

    return true;

};
