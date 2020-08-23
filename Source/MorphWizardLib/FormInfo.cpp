#include "FormInfo.h"
#include "../common/utilit.h"
#include <fstream>


bool CMorphWizardBase::read_utf8_line(std::ifstream& inp, std::string& line) const {
    if (!getline(inp, line)) {
        return false;
    }
    line = convert_from_utf8(line.c_str(), m_Language);
    return true;
}

std::string CMorphWizardBase::str_to_utf8(const std::string& line) const {
    return convert_to_utf8(line, m_Language);
}

static size_t getCount(std::ifstream& mrdFile, const char* sectionName) {
    std::string line;
    if (!getline(mrdFile, line)) {
        throw CExpc("Cannot get size of section  %s", sectionName);
    }
    return atoi(line.c_str());
}

void CMorphWizardBase::ReadFlexiaModels(std::ifstream& mrdFile) {
    m_FlexiaModels.clear();
    size_t count = getCount(mrdFile, "flexia models");

    for (size_t num = 0; num < count; num++) {
        std::string line;
        if (!read_utf8_line(mrdFile, line)) {
            throw CExpc("Cannot read enough flexia models ");
        }
        Trim(line);
        CFlexiaModel M;
        if (!M.ReadFromString(line))
            throw CExpc("Cannot parse paradigm No %i", num + 1);

        m_FlexiaModels.push_back(M);
    }
};

void CMorphWizardBase::WriteFlexiaModels(std::ofstream& outp) const {
    outp << m_FlexiaModels.size() << "\n";
    for (auto f : m_FlexiaModels) {
        outp << str_to_utf8(f.ToString()) << "\n";
    }
};


void CMorphWizardBase::ReadAccentModels(std::ifstream& mrdFile) {
    m_AccentModels.clear();
    size_t count = getCount(mrdFile, "accent models");
    for (size_t num = 0; num < count; num++) {
        std::string line;
        if (!read_utf8_line(mrdFile, line)) {
            throw CExpc("Cannot read enough accent models ");
        }
        Trim(line);

        CAccentModel M;
        if (!M.ReadFromString(line))
            throw CExpc("Cannot parse accent model %s", line.c_str());

        m_AccentModels.push_back(M);

    };
};

void CMorphWizardBase::WriteAccentModels(std::ofstream& outp) const {
    outp << m_AccentModels.size() << "\n";
    for (auto a  : m_AccentModels) {
        outp << str_to_utf8(a.ToString()) << "\n";
    }
};

