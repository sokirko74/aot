#include "../common/util_classes.h"
#include "RusCorpXml.h"
#include "../tinyxml/tinyxml.h"
#include "../tinyxml/xpath_static.h"

CRusCorpXmlFile::CRusCorpXmlFile()
{
    m_CurrSentXml = new TiXmlDocument;
    m_CurrSentXml->SetCondenseWhiteSpace(false);
    m_OutFile = stdout;
    
}

CRusCorpXmlFile::~CRusCorpXmlFile()
{
    delete m_CurrSentXml;
}

bool is_postfix(const string& Body, const string& Postfix)
{
    return      Body.length() >= Postfix.length()
            &&  Body.substr(Body.length() - Postfix.length()) == Postfix;
}

bool CRusCorpXmlFile::ReadNextSentence  (FILE* fp, bool PrintUnparsed)
{
	int ch;
	int len = 0;
    m_SentenceStr.clear();
    const string SentenceStart = "<se>";
	const string SentenceEnd = "</se>";
    if (!fp) throw CExpc ("cannot read input file");
    bool bHasSentStart = false;
	while ((ch = fgetc(fp)) != EOF)
	{
		m_SentenceStr += ch;
		len++;
        if (ch == '>')
        {
		    if (is_postfix(m_SentenceStr, SentenceEnd))
			    break;
            else
            if (is_postfix(m_SentenceStr, SentenceStart))
            {
                size_t i = len-SentenceStart.length();
                if (PrintUnparsed)
                    fprintf (m_OutFile, "%s", m_SentenceStr.substr(0, i).c_str());
                m_SentenceStr.erase (0, i);
                len = SentenceStart.length();
                bHasSentStart = true;
            }
        }

		if (len > 100000000)
			throw CExpc("setence is too long");
	}

    if (!bHasSentStart && PrintUnparsed)
        fprintf (m_OutFile, "%s", m_SentenceStr.c_str());
    return bHasSentStart;
}


string DeleteSingleQuotesAndAsterisks(string& s)
{
	string R;
	for (size_t  i=0;i < s.length(); i++)
		if ( ((BYTE)s[i] != '\'') && ((BYTE)s[i] != '`') && ((BYTE)s[i] != '*'))
			R += s[i];
	s  = R;
	return s;
}


bool CRusCorpXmlFile::CreateGraTable()
{

    m_CurrSentTokens.clear();
    if (!m_CurrSentXml)
        throw CExpc ("m_CurrSentXml is null");
    m_CurrSentXml->Clear();
	//if (m_CurrSentXml->Parse(string("<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n" + m_SentenceStr).c_str()))
    if (m_CurrSentXml->Parse(m_SentenceStr.c_str()))
		throw CExpc (m_CurrSentXml->ErrorDesc());

	size_t CountOfWord = 0;
    string LastReadWord;
    for (TiXmlElement* xml_sent = m_CurrSentXml->FirstChildElement("se"); xml_sent;  xml_sent = xml_sent->NextSiblingElement("se"))
    {
	    for (TiXmlNode* xml_word = xml_sent->FirstChild(); xml_word;  xml_word = xml_word->NextSibling())
	    {
		    
            if (xml_word->Type() == TiXmlNode::TEXT)
            {
                StringTokenizer tok (xml_word->Value(), " \t\r\n");
                while (tok())
                {
                    CXmlToken T;
                    T.m_WordStr = tok.val();
                    Trim(T.m_WordStr);
                    T.m_OrigWordStr = tok.val();
                    T.m_Type = XmlPunctType;
                    m_CurrSentTokens.push_back(T);
                }
                continue;
            }
            if (        (xml_word->Type() != TiXmlNode::ELEMENT) 
                    ||  xml_word->ToElement()->Value() != XmlWordType 
                )
                continue;

            CXmlToken Word;
		    Word.m_Type =  XmlWordType;
            TiXmlNode* xml_word_str= 0;
            for (TiXmlNode* i = xml_word->FirstChild(); i;  i = i->NextSibling())
                if (i->Type() == TiXmlNode::TEXT)
                {
                    xml_word_str = i;
                    break;
                }
		    {
			    if (!xml_word_str)
				    throw CExpc ("cannot get the next word [LastReadWord=%s]", LastReadWord.c_str());						
                Word.m_OrigWordStr = xml_word_str->Value();
			    Word.m_WordStr = xml_word_str->Value();
			    Trim(Word.m_WordStr);
       		    if (Word.m_WordStr.empty())
			        throw CExpc ("empty word after word WordStr=%s\n", LastReadWord.c_str());
		    };

    		
		    for (TiXmlElement* xml_annot = xml_word->FirstChildElement("ana"); xml_annot;  xml_annot = xml_annot->NextSiblingElement("ana"))
		    {
			    CXmlMorphAnnot A;
    			
			    {
				    const char*	q = xml_annot->Attribute("lex");
                    if (!q)
                        throw CExpc ("no lemma for word %s\n", LastReadWord.c_str());
				    A.m_Lemma = q;
			    };
			    {
				    const char* s= xml_annot->Attribute("gr");
                    if (!s)
                        throw CExpc ("no gr for word %s\n", LastReadWord.c_str());
                    A.m_GrammemsStr = s;
			    };
                A.m_XmlRef = xml_annot;
			    Word.m_Annots.push_back(A);
		    };

            if (Word.m_Type == XmlWordType)
            {
                DeleteSingleQuotesAndAsterisks(Word.m_WordStr);
                ConvertJO2Je(Word.m_WordStr);
                if (		
					        !CheckLanguage(Word.m_WordStr,morphRussian)  // ни то ни се
					    &&	!CheckLanguage(Word.m_WordStr,morphEnglish) 
					    &&	!CheckLanguage(Word.m_WordStr,morphDigits) 
					    &&	Word.m_WordStr.length() < 255	
				)
			    {
				    char buffer[256];
				    if (force_to_rus(buffer, Word.m_WordStr.c_str(), Word.m_WordStr.length()))
				    {
    					
					    //fprintf (stderr, "coercing to russian ABC (token %s)\n", Word.m_WordStr.c_str());
					    buffer[Word.m_WordStr.length()] = 0;
					    Word.m_WordStr = buffer;
    				
				    }
			    }

            }
            LastReadWord = Word.m_WordStr; 
		    m_CurrSentTokens.push_back(Word);
		    
	    };
    }

    if (!m_CurrSentTokens.empty())
		m_CurrSentTokens.back().m_bLastInSentence = true;

	return true;
};


bool CRusCorpXmlFile::PrintDisambiguatedXmlNodes()
{
    set <void*> GoodAnnots;
    for (size_t i =0; i <m_CurrSentTokens.size(); i++)
        for (size_t j=0; j <m_CurrSentTokens[i].m_Annots.size(); j++)
            GoodAnnots.insert(m_CurrSentTokens[i].m_Annots[j].m_XmlRef);


    for (TiXmlElement* xml_sent = m_CurrSentXml->FirstChildElement("se"); xml_sent;  xml_sent = xml_sent->NextSiblingElement("se"))
    {
	    for (TiXmlNode* xml_word = xml_sent->FirstChild("w"); xml_word;  xml_word = xml_word->NextSibling("w"))
	    {
            bool bRemoved;
            do {
                bRemoved = false;
                for (TiXmlElement* xml_annot = xml_word->FirstChildElement("ana"); xml_annot;  xml_annot = xml_annot->NextSiblingElement("ana"))
		        {
                    if (GoodAnnots.find(xml_annot) == GoodAnnots.end())
                    {
                        xml_word->RemoveChild(xml_annot);
                        bRemoved = true; // I do not know how to remove in the cycle
                        break;
                    }
                }
            }while (bRemoved);
            if (xml_word->NoChildren()) throw CExpc ("all annots were deleted");
        }
    }
    m_CurrSentXml->Print(m_OutFile, 2);

    return true;
}

bool CRusCorpXmlFile::WriteGramXml()
{
    fprintf (m_OutFile, "<se>\n");
    for (size_t i=0; i < m_CurrSentTokens.size(); i++)
    {
        const CXmlToken& W = m_CurrSentTokens[i];
        if (W.m_Type == XmlPunctType) 
        {
            fprintf  (m_OutFile, "%s", W.m_OrigWordStr.c_str());
            continue;
        }

        fprintf  (m_OutFile, "<w>%s\n", W.m_OrigWordStr.c_str());

        for (size_t j=0; j < W.m_Annots.size(); j++)
        {
            const CXmlMorphAnnot& A = W.m_Annots[j];
            fprintf (m_OutFile, "\t<ana lex=\"%s\" gr=\"%s\" />\n", A.m_Lemma.c_str(), A.m_GrammemsStr.c_str());
        }
        fprintf  (m_OutFile, "</w>\n");
    }
    fprintf (m_OutFile, "</se>\n");
    return true;
}
