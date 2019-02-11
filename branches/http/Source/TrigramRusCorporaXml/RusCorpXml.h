#include "../common/util_classes.h"
#include "../common/MorphXmlToken.h"
class TiXmlDocument;
class CRusCorpXmlFile{
public:
    string              m_SentenceStr;
    TiXmlDocument*      m_CurrSentXml;
    vector<CXmlToken>   m_CurrSentTokens;
    

    CRusCorpXmlFile();
    ~CRusCorpXmlFile();
    bool ReadNextSentence  (std::istream& inputStream);
    bool CreateGraTable();
    bool PrintDisambiguatedXmlNodes(std::ostream& outputStream);
};


