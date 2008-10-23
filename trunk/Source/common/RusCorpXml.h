#include "../common/util_classes.h"
#include "../common/MorphXmlToken.h"
class TiXmlDocument;
class CRusCorpXmlFile{
public:
    FILE*               m_OutFile;
    string              m_SentenceStr;
    TiXmlDocument*      m_CurrSentXml;
    vector<CXmlToken>   m_CurrSentTokens;
    

    CRusCorpXmlFile();
    ~CRusCorpXmlFile();
    bool ReadNextSentence  (FILE* fp, bool PrintUnparsed);
    bool CreateGraTable();
    bool PrintDisambiguatedXmlNodes();
    bool WriteGramXml();
};


