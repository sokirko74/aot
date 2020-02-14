#include "../common/util_classes.h"
#include "../common/MorphXmlToken.h"


void PrintCookedSentence (const vector<CXmlToken>& Tokens, bool bPrintLemmas)
{
    for (size_t i=0; i<Tokens.size(); i++)
    {
        const CXmlToken& W = Tokens[i];

        if (bPrintLemmas )  {
            printf ("%s", W.m_WordStr.c_str());
            if (W.m_Annots.size() > 0)
                printf ("{%s}", W.m_Annots[0].m_Lemma.c_str());
            printf (" ");
        }
        else {
            printf ("%s ", W.m_WordStr.c_str());
            if (W.m_Type == XmlPunctType)
            {
                if (W.IsImportantPunct())
                    printf ("%s", Tokens[i].m_WordStr.c_str());
                else
                    if (isdigit ((unsigned char)W.m_WordStr[0]) )
                        printf ("NUM");
                    else  
                        printf ("UNK");
            }

            if (W.m_Type == XmlWordType)
            {

                if (W.m_Annots.empty())
                    printf ("UNK");
                else
                {
                    string T = W.m_Annots[0].BuildRusCorpAnnot();
                    if (T.empty())
                        printf ("UNK");
                    else
                        printf ("%s", T.c_str());

                }
            }
        }
        if (W.m_bLastInSentence)
            printf ("\n");
        else
            printf (" ");
    }

}
