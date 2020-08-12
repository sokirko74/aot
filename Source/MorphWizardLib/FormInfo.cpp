#include "FormInfo.h"
#include "../common/utilit.h"
#include <fstream>


bool CMorphWizardBase::read_utf8_line(ifstream& inp, std::string& line) const {
    if (!getline(inp, line)) {
        return false;
    }
    line = convert_from_utf(line.c_str(), m_Language);
    return true;
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

void CMorphWizardBase::WriteFlexiaModels(FILE* out_fp) const {
    fprintf(out_fp, "%i\n", m_FlexiaModels.size());
    for (size_t i = 0; i < m_FlexiaModels.size(); i++)
        fprintf(out_fp, "%s\n", m_FlexiaModels[i].ToString().c_str());
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

void CMorphWizardBase::WriteAccentModels(FILE* out_fp) const {
    fprintf(out_fp, "%i\n", m_AccentModels.size());
    for (size_t i = 0; i < m_AccentModels.size(); i++)
        fprintf(out_fp, "%s\n", m_AccentModels[i].ToString().c_str());
};

