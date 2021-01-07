#pragma once

typedef bool (*IsKeyWordFunction)(const CString& word, COLORREF& C, void* Data);

class CSyntaxColorizer  
{
public:
	CSyntaxColorizer();

	void Colorize(CRichEditCtrl *pCtrl, int index, bool bModified=false );
	void InitializeParser(IsKeyWordFunction Func, void* UserData);
protected:
	CHARFORMAT2 m_cfDefault;
	void*		m_UserData;
	IsKeyWordFunction IsKeyWord;
};


