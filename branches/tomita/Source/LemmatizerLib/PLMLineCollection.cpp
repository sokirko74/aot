// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko, Andrey Putrin

#include "StdMorph.h"
#include "Lemmatizers.h"
#include "PLMLineCollection.h"
#include "../common/Graspace.h"
#include	 "../GraphanLib/GraphmatFile.h"

#ifdef DETECT_MEMORY_LEAK
	#ifdef _DEBUG
	#define new DEBUG_NEW

	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#endif
#endif


CPlmLineCollection::CPlmLineCollection()
{
	m_pLemmatizer = NULL;
};

CPlmLineCollection::~CPlmLineCollection()
{
}





bool CPlmLineCollection::ProcessPlmLines(const CGraphmatFile* piGraphmatFile)
{
	int LineNo = 0;
	try
	{
		if (!m_pLemmatizer)
			return false;

		ClearVector(m_Items);

        vector<CFormInfo> results;
		results.reserve(5);

		vector<CFormInfo> results1;
		results1.reserve(5);

		vector<CFormInfo> results2;
		results2.reserve(5);
		
		const CGraphmatFile* Gr = piGraphmatFile;
		bool bInFixedExpression = false;
		
		for (LineNo = 0; LineNo < Gr->GetTokensCount(); LineNo++)
		{
			// take a line from Graphematics
			char buffer[CriticalGraphemLineLength];
			Gr->GetGraphematicalLine(buffer, LineNo);
		 	string strProcess = buffer;

			//=====   do not lemmatize oborots with EXPR=Fixed!
			if (Gr->StartsFixedOborot(LineNo))
			{
				bInFixedExpression = true;
			};

			if (bInFixedExpression)
			{
				m_Items.push_back(strProcess);
				if (Gr->HasDescr(LineNo, OEXPR2))	
					bInFixedExpression = false;
				continue;
			};
			//=====   



			if (m_pLemmatizer->GetLanguage() == Gr->GetTokenLanguage(LineNo))
			{
				string InputWordStr = Gr->GetToken(LineNo);

                m_pLemmatizer->CreateParadigmCollection(false, InputWordStr, !Gr->HasDescr(LineNo, OLw), true, results);

				if (results.empty() )
					m_Items.push_back(strProcess+ Format(" -?? %s ?? -1 0",Gr->GetUppercaseToken(LineNo)) );
				else
					for( int i=0; i<results.size(); i++ )
					{
						string Line;
						if (i > 0) Line = 	"  ";
                        Line +=  strProcess+" "+results[i].FormatAsInPlmLine();
						m_Items.push_back(Line);
					}
			}
			else
			{
				m_Items.push_back(strProcess);
			}
		}
	}
	catch (...)
	{

		return false;
	}
	return true;
}




bool CPlmLineCollection::SaveToFile(string filename) const
{
	try
	{
		FILE* fp = fopen (filename.c_str(),"wb");
		if (!fp) return false;
		for (int i = 0; i < m_Items.size(); i++)
			fprintf (fp, "%s\n",m_Items[i].c_str());
		fclose (fp);
	}
	catch(...)
	{
		return false;
	}
	return true;
}


