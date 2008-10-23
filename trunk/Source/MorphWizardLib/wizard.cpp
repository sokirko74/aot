
#pragma warning(disable:4786)

#include "wizard.h"
#include "../common/util_classes.h"
#include "../AgramtabLib/EngGramTab.h"
#include "../AgramtabLib/RusGramTab.h"
#include "../AgramtabLib/GerGramTab.h"

#include "../PCRE/pcre_rml.h"

//----------------------------------------------------------------------------
const char* AnyCommonAncode = " ";

//----------------------------------------------------------------------------
string GetCurrentDate()
{
	time_t ltime;
	time(&ltime);
	struct tm *today = localtime( &ltime );
	char tmpbuf[255];
	strftime( tmpbuf, 255,"%H:%M, %d %B %Y", today );
	return tmpbuf;
}

//==============================================================================
const char FlexModelCommDelim[] = "q//q";
bool CFlexiaModel::ReadFromString(string& s)
{
	int comm = s.rfind(FlexModelCommDelim);
	if (comm != string::npos)
	{
		m_Comments = s.substr(comm+strlen(FlexModelCommDelim));
		Trim(m_Comments);
		s.erase(comm);
		Trim(s);
	}
	else
		m_Comments = "";

	StringTokenizer Tok(s.c_str(), "%");
	m_Flexia.clear();
	while (Tok())
	{
		string OneRecord = Tok.val();
        size_t ast = OneRecord.find('*');
		if (ast == string::npos) return false;
		size_t last_ast = OneRecord.find_last_of('*');
		string  Prefix;
		if (last_ast != ast)
			Prefix = OneRecord.substr(last_ast+1);

		CMorphForm G(OneRecord.substr(ast+1, last_ast-ast-1), OneRecord.substr(0, ast), Prefix);
		m_Flexia.push_back(G);

	};

	return true;
};

string CFlexiaModel::ToString() const
{
	string Result;
	for (size_t i=0; i <m_Flexia.size(); i++)
	{
		Result+="%";
		Result+= m_Flexia[i].m_FlexiaStr;
		Result+="*";
		Result+= m_Flexia[i].m_Gramcode;
		if (!m_Flexia[i].m_PrefixStr.empty())
		{
			Result+="*";
			Result+= m_Flexia[i].m_PrefixStr;
		};
	};
	if (!m_Comments.empty())
		Result += FlexModelCommDelim + m_Comments;
	return Result;
};

string	CFlexiaModel::get_first_flex() const 
{
		assert (!m_Flexia.empty());
		return m_Flexia[0].m_FlexiaStr;
};

string CFlexiaModel::get_first_code() const
{
	assert (!m_Flexia.empty());
	return m_Flexia[0].m_Gramcode;
}
		

bool CFlexiaModel::has_ancode(const string& search_ancode) const 
{
	for (size_t i=0; i <m_Flexia.size(); i++)
	{
		int match = m_Flexia[i].m_Gramcode.find(search_ancode);
		if( (match!=string::npos) && (match%2==0) )
			return true;
	}
	return false;
};

//==============================================================================

bool CMorphSession::operator  == (const  CMorphSession& X) const 
{
	return			(m_UserName == X.m_UserName)
				&&	(m_SessionStart == X.m_SessionStart)
				&&	(m_LastSessionSave == X.m_LastSessionSave);
};

void CMorphSession::SetEmpty()
{
	m_UserName = "";
	m_SessionStart = "";
	m_LastSessionSave = "";
};
bool CMorphSession::IsEmpty() const
{
	return m_UserName.empty();
};

bool	CMorphSession::ReadFromString(const string& s)
{
	StringTokenizer Tok(s.c_str(), ";\r\n");
	if (!Tok()) 
	{
		SetEmpty();
		return false;
	};
	m_UserName =  Tok.val();
	if (!Tok()) 
	{
		SetEmpty();
		return false;
	};
	m_SessionStart =  Tok.val();
	if (!Tok()) 
	{
		SetEmpty();
		return false;
	};
	m_LastSessionSave =  Tok.val();
	return true;
}

string	CMorphSession::ToString() const
{
	return Format ("%s;%s;%s", m_UserName.c_str(), m_SessionStart.c_str(), m_LastSessionSave.c_str());
};

//==============================================================================

bool		CAccentModel::ReadFromString(const string& s)
{
	StringTokenizer Tok(s.c_str(), "; \r\n");
	m_Accents.clear();
	while (Tok())
	{
		string OneRecord = Tok.val();
		if (OneRecord.empty()) return false;
		if (!isdigit(OneRecord[0])) return false;
		m_Accents.push_back(atoi(OneRecord.c_str()));
	};
	return true;
}

string		CAccentModel::ToString() const
{
	string Result;
	for (size_t i=0; i < m_Accents.size(); i++)
	{
		Result += Format("%i;",m_Accents[i]);
	};
	return Result;
};

//==============================================================================
CParadigmInfo::CParadigmInfo() : CLemmaInfo()
{
	m_SessionNo = UnknownSessionNo;
	m_AuxAccent = UnknownAccent;
	m_bToDelete = false;
	m_PrefixSetNo = UnknownPrefixSetNo;
};

CParadigmInfo::CParadigmInfo(WORD ParadigmNo, WORD AccentModelNo, WORD SessionNo, BYTE AuxAccent,  const char* CommonAncode, WORD PrefixSetNo) 
{
	m_FlexiaModelNo = ParadigmNo;
	m_bToDelete = false;
	m_AccentModelNo = AccentModelNo;
	m_SessionNo = SessionNo;
	m_AuxAccent = AuxAccent;
	strncpy(m_CommonAncode, CommonAncode, CommonAncodeSize);
	m_PrefixSetNo = PrefixSetNo;
};

bool CParadigmInfo:: operator  == (const CParadigmInfo& X) const
{
	return		m_FlexiaModelNo == X.m_FlexiaModelNo 
			&&	m_AccentModelNo == X.m_AccentModelNo
			&&  m_AuxAccent == X.m_AuxAccent
			&&	!strncmp(m_CommonAncode, X.m_CommonAncode,CommonAncodeSize)
			&&	m_PrefixSetNo == X.m_PrefixSetNo;
};

//----------------------------------------------------------------------------
// Nick [17/Dec/2003]
//----------------------------------------------------------------------------
CParadigmInfo CParadigmInfo::AnyParadigmInfo()
{
	return CParadigmInfo(AnyParadigmNo, AnyAccentModelNo, AnySessionNo, 
								AnyAccent, AnyCommonAncode, AnyPrefixSetNo );
}

//----------------------------------------------------------------------------
bool CParadigmInfo::IsAnyEqual( const CParadigmInfo& X ) const
{
	return (
		    (m_FlexiaModelNo==AnyParadigmNo		|| 
		  X.m_FlexiaModelNo==AnyParadigmNo		|| 
		     m_FlexiaModelNo==X.m_FlexiaModelNo)
		&&	(m_AccentModelNo==AnyAccentModelNo	||
		  X.m_AccentModelNo==AnyAccentModelNo	|| 
		     m_AccentModelNo==X.m_AccentModelNo)
		&&  (m_AuxAccent==AnyAccent				||
		  X.m_AuxAccent==AnyAccent				|| 
		     m_AuxAccent==X.m_AuxAccent)
	    && (!strncmp(m_CommonAncode,  X.m_CommonAncode,CommonAncodeSize)
		 || !strncmp(m_CommonAncode,	AnyCommonAncode,CommonAncodeSize)
		 || !strncmp(X.m_CommonAncode, AnyCommonAncode,CommonAncodeSize))
		&&	(m_PrefixSetNo==AnyPrefixSetNo		||
		  X.m_PrefixSetNo==AnyPrefixSetNo		|| 
		     m_PrefixSetNo==X.m_PrefixSetNo) 
		   );
}


//==============================================================================
//	Данный класс предназначен для обработки морфологического словаря.
//	Умееет загрузить словарь, найти/отредактировать/удалить/добавить словоформу,
//	сохранить изменения в словаре.


MorphoWizard::MorphoWizard()
:m_bLoaded(false),
m_bWasChanged(false)
{
	m_ReadOnly = true;
	m_bFullTrace = true;
	m_pGramTab = 0;
	m_pMeter = 0;
}

MorphoWizard::~MorphoWizard()
{
	if (m_pGramTab) delete  m_pGramTab;
	MakeReadOnly();
}


string MorphoWizard::get_lock_file_name() const
{
	string s = m_MrdPath;
	int i = s.find_last_of ("/");
	if (i == string::npos)
		i = s.find_last_of ("\\");
	map<string, string>::const_iterator it = m_ProjectFileKeys.find("ProjectsDir");
	string Path;
	if (it != m_ProjectFileKeys.end())
		Path = it->second;
	
	Path += "/";
	if (i != string::npos)
	{
		Path += s.substr(0, i);
		Path += "/";
	}

	Path +="wizard.lck"; 			 
	return Path;
};

string MorphoWizard::get_log_file_name() const
{
	string s = m_MrdPath;
	int i = s.find_last_of ("/");
	if (i == string::npos)
		i = s.find_last_of ("\\");
	map<string, string>::const_iterator it = m_ProjectFileKeys.find("ProjectsDir");
	string Path;
	if (it != m_ProjectFileKeys.end())
		Path = it->second;

	Path += "/";
	if (i != string::npos)
	{
		Path += s.substr(0, i);
		Path += "/";
	}

	Path +="log"; 			 
	return Path;
};

void MorphoWizard::load_string_vector(const string &name, StringVector &res)
{
	string dicts_str = get_value(name);
	StringTokenizer tok(dicts_str.c_str(), ",");
	while(tok()){
		res.push_back(tok.val());
	}
}


//	загружает *.mwz файл.
//	Пример файла:

//# русская морфология
//MRD_FILE 	L:\MORPH.COM\SOURCE\RUS_SRC\morphs.mrd
//LANG	        RUSSIAN
//USERS       user1, user2, user3


const size_t MaxMrdLineLength = 10240;
string GetFullPathByName( string FileName )
{
	string Path;
	#ifdef WIN32
		char absolute_path[255];
		_fullpath(absolute_path, FileName.c_str(), 255);
		Path = GetPathByFile(string(absolute_path));
	#else
		char current_dir [255];
		getcwd(current_dir, 255);
		string add_path =    GetPathByFile(FileName);
		if (!add_path.empty())
			if (add_path[0] != '/')
				Path = string(current_dir)+string("/")+add_path;
			else
				Path = add_path;
		else
			Path =  current_dir;
	#endif
	
	if (!Path.empty())
		if	(		(Path[Path.length()-1] == '\\') 
				||	(Path[Path.length()-1] == '/') 
			)
			Path.erase(Path.length()  - 1);
	return Path;
};

void MorphoWizard::load_gramtab()
{
	CAgramtab*	pGramTab;
	switch (m_Language)
	{
		case morphRussian:
			pGramTab = new CRusGramTab;
			break;
		case morphEnglish:	
			pGramTab = new CEngGramTab;
			break;
		case morphGerman:	
			pGramTab = new CGerGramTab;
			break;
		default: throw CExpc("Unknown language: " + GetStringByLanguage(m_Language));
	};

	

	if (!pGramTab->LoadFromRegistry())
	{
			throw CExpc("Cannot load gramtab");
	};

	m_pGramTab = pGramTab;

	// read all poses from  m_pGramTab
	m_PosesList.clear();
	
	for (int i =0; i < m_pGramTab->GetPartOfSpeechesCount(); i++)
		m_PosesList.push_back(m_pGramTab->GetPartOfSpeechStr(i));
	sort(m_PosesList.begin(), m_PosesList.end());

	// read all grammems from  m_pGramTab
	m_GrammemsList.clear();
	for (int i =0; i < m_pGramTab->GetGrammemsCount(); i++)
	{
		m_GrammemsList.push_back( m_pGramTab->GetGrammemStr(i) );
	};
	sort(m_GrammemsList.begin(),m_GrammemsList.end());

	// read all type grammems from  m_pGramTab
	m_TypeGrammemsList.clear();
	string CommonAncodes = m_pGramTab->GetAllPossibleAncodes(UnknownPartOfSpeech,0);
	for (size_t i=0; i < CommonAncodes.length(); i+=2)
	{
		QWORD G;
		m_pGramTab->GetGrammems(CommonAncodes.c_str()+i, G);
		string  q =  m_pGramTab->GrammemsToStr(G);
		m_TypeGrammemsList.push_back( q );
	};
	sort(m_TypeGrammemsList.begin(),m_TypeGrammemsList.end());

	ancode_less.init(m_pGramTab);

};

WORD	MorphoWizard::GetCurrentSessionNo() const
{
	assert (m_SessionNo < m_Sessions.size());
	return m_SessionNo;
}

bool MorphoWizard::StartSession(string user_name)
{
	CMorphSession S;
	S.m_UserName = user_name;
	S.m_SessionStart = GetCurrentDate();
	S.m_LastSessionSave = "no"; 
	m_Sessions.push_back(S);
	m_SessionNo = m_Sessions.size() - 1;
	char msg[128];
	sprintf(msg,"Opened by %s",user_name.c_str());
	log(msg);
	return true;
};
void MorphoWizard::EndSession()
{
	assert (m_SessionNo < m_Sessions.size());
	m_Sessions[m_SessionNo].m_LastSessionSave = GetCurrentDate();
};

void MorphoWizard::StartLastSessionOfUser(string user_name)
{
	if (GetUserName() == user_name) return;
	EndSession();
	for (int i=(int)m_Sessions.size()-1; i >=0;i--)
		if (m_Sessions[i].m_UserName == user_name)
		{
			m_SessionNo = i;
			return;
		};
	StartSession(user_name);
};

bool MorphoWizard::load_static(MorphLanguageEnum langua)
{
	m_Language = langua ;
	if (m_pGramTab) delete m_pGramTab;
	RmlPcreMakeTables(m_PcreCharacterTables, m_Language);
	load_gramtab();
	m_LanguageStr = GetStringByLanguage(langua);
	return true;
}

bool MorphoWizard::load_wizard(const char *path,const char *user_name, bool bCreatePrediction)
{
	m_ProjectFileKeys["ProjectsDir"] = GetFullPathByName(path);
	
	FILE* fp = fopen (path, "r");
	if( !fp ) 
		  throw CExpc("Cannot open file " + string(path));
	
	char buffer[MaxMrdLineLength];
	while(fgets(buffer, MaxMrdLineLength, fp))
	{
		string key, val;
		StringTokenizer tok(buffer, " \t\r\n");
		if(!tok())		continue;
		key = tok.val();
		if(key[0] == '#')	continue;
		if(!tok())	throw CExpc("wrong mwz file");
		val = tok.val();
		if(tok())	throw CExpc("wrong mwz file");
		m_ProjectFileKeys[key] = val;
		
	}

	fclose (fp);

	
	string &lang = get_value("LANG");
	if (m_pGramTab) delete m_pGramTab;
	if (!GetLanguageByString(lang, m_Language))
		throw CExpc("Unknown language: " + lang);

	RmlPcreMakeTables(m_PcreCharacterTables, m_Language);

	load_gramtab();

	// load file
	m_MrdPath = get_value("MRD_FILE");
	m_LanguageStr = get_value("LANG");

	// init users
	load_string_vector("USERS", m_Users);

	bool guest = strcmp(user_name,"guest")==0;
	bool login = false;
	if( !guest )
	{
		for( int i=0; i<m_Users.size(); i++ )
		{
			if( m_Users[i]==user_name )
			{
				login = true;
				break;
			}
		}
	}

	if( !guest && !login )
	{
		throw CExpc("Incorrect login!");
		return false;
	}
	
	load_mrd(guest, bCreatePrediction);

	StartSession(user_name);
	

	m_bLoaded = true;
	return true;
}


void MorphoWizard::check_paradigm(long line_no) 
{
	CFlexiaModel &p = m_FlexiaModels[line_no];
	try 
	{
		for(size_t i=0; i < p.m_Flexia.size(); i++)
			if (get_pos_string(p.m_Flexia[i].m_Gramcode).empty()) 
				goto error_label;
		return;
	}
	catch(...)
	{
	};
error_label:
	 vector<lemma_iterator_t> found_paradigms;
	 find_lemm_by_prdno(line_no,found_paradigms);
	 if (found_paradigms.size() > 0)
	   ErrorMessage (Format ("Flexiamodel No %i has invalid gramcodes",line_no));
	 

};

static void CreateLockFile(const string& LockFileName)
{
	FILE* fp = fopen(LockFileName.c_str(), "wb");
	if( fp!=NULL )
	{
		try 
		{
			string strPath = GetRegistryString("SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName\\ComputerName");
			fprintf (fp, "MachineName = %s \r\n", strPath.c_str());
			fprintf (fp, "Time = %s\n", GetCurrentDate().c_str()); 
		}
		catch (...) 
		{
		}
		fclose(fp);
	}
}

void ReadFlexiaModels(FILE* fp, vector<CFlexiaModel>& FlexiaModels )
{
	char buffer[MaxMrdLineLength];	
	if( !fgets(buffer,  MaxMrdLineLength, fp) )
		throw CExpc("Cannot parse mrd file");

	FlexiaModels.clear();

	int paradigm_count = atoi(buffer);

	for(int num = 0; num < paradigm_count; num++)
	{
		if( !fgets(buffer,  MaxMrdLineLength, fp) )	
			throw CExpc("Too few lines in mrd file");

		string line  = buffer;
		Trim(line);
		CFlexiaModel M;
		if (!M.ReadFromString(line))
			throw CExpc("Cannot parse paradigm No %i", num+1);
		
		FlexiaModels.push_back(M);
	}
};

void WriteFlexiaModels(FILE* out_fp, const vector<CFlexiaModel>& FlexiaModels )
{
	fprintf (out_fp, "%i\n", FlexiaModels.size());
	for(size_t i=0; i < FlexiaModels.size(); i++)
		fprintf (out_fp, "%s\n", FlexiaModels[i].ToString().c_str());
};


void ReadAccentModels (FILE* fp, vector<CAccentModel>&	AccentModels )
{
	AccentModels.clear();

	char buffer[MaxMrdLineLength];	
 
	if( !fgets(buffer,  MaxMrdLineLength, fp) )
		throw CExpc("Cannot read accent models from mrd file");
	
	int count = atoi(buffer);
	for(int b = 0; b < count; b++)
	{
		if( !fgets(buffer,  MaxMrdLineLength, fp) )	
			throw CExpc("Too few lines in mrd file");

		CAccentModel M;
		if (!M.ReadFromString(buffer))
			throw CExpc("Cannot parse line %s", buffer);

		AccentModels.push_back(M);

	};

};

void WriteAccentModels(FILE* out_fp, const vector<CAccentModel>& AccentModels )
{
	fprintf (out_fp, "%i\n", AccentModels.size());
	for(size_t i=0; i < AccentModels.size(); i++)
		fprintf (out_fp, "%s\n", AccentModels[i].ToString().c_str());
};

void MorphoWizard::ReadSessions (FILE* fp)
{
	m_Sessions.clear();

	char buffer[MaxMrdLineLength];	
 
	if( !fgets(buffer,  MaxMrdLineLength, fp) )
		throw CExpc("Cannot read sessions from mrd file");
	
	int count = atoi(buffer);
	for(int b = 0; b < count; b++)
	{
		if( !fgets(buffer,  MaxMrdLineLength, fp) )	
			throw CExpc("Too few lines in mrd file");

		CMorphSession M;
		if (!M.ReadFromString(buffer))
			throw CExpc(Format ("Cannot parse line %s", buffer));

		m_Sessions.push_back(M);

		if( HasMeter() ) 
			GetMeter()->SetFilePos();
	};


};

void MorphoWizard::ReadOnePrefixSet(string PrefixSetStr, set<string>& Result) const
{
	RmlMakeUpper(PrefixSetStr, m_Language);
	Trim(PrefixSetStr);
	for (size_t i=0; i < PrefixSetStr.length(); i++)
		if (		!is_upper_alpha((BYTE)PrefixSetStr[i], m_Language)
				&&	(BYTE)PrefixSetStr[i] != ','
				&&	(BYTE)PrefixSetStr[i] != ' '
			)
			throw CExpc ("Cannot parse the prefix set");

	

	StringTokenizer  tok(PrefixSetStr.c_str(), ", \t\r\n");
	Result.clear();
	while (tok())
	{
		Result.insert(tok.val());
	};
	
};

void MorphoWizard::ReadPrefixSets (FILE* fp)
{
	if( HasMeter() ) 
		GetMeter()->SetInfo("Reading mrd: Prefix Sets...");

	m_PrefixSets.clear();

	char buffer[MaxMrdLineLength];	
 
	if( !fgets(buffer,  MaxMrdLineLength, fp) )
		throw CExpc("Cannot read sessions from mrd file");
	
	int count = atoi(buffer);
	for(int b = 0; b < count; b++)
	{
		if( !fgets(buffer,  MaxMrdLineLength, fp) )	
			throw CExpc("Too few lines in mrd file");

		set<string> PrefixSet;
		ReadOnePrefixSet(buffer, PrefixSet);
		if (PrefixSet.empty())
			throw CExpc("No prefixes found in prefix sets section");

		m_PrefixSets.push_back(PrefixSet);

		if( HasMeter() ) 
			GetMeter()->SetFilePos();
	};


};


static void ReadLemmas (FILE* fp, MorphoWizard& W )
{
	if( W.HasMeter() ) 
		W.GetMeter()->SetInfo("Reading mrd: Lemmas...");

	char buffer[MaxMrdLineLength];	
 
	if( !fgets(buffer,  MaxMrdLineLength, fp) )
		throw CExpc("Cannot read lemmas from mrd file");

	int base_count = atoi(buffer);
	
	for(int b = 0; b < base_count; b++)
	{
		if( !fgets(buffer,  MaxMrdLineLength, fp) )	
			throw CExpc("Too few lines in mrd file");

		char Base[255];
		int ParadigmNo, AccentModelNo, SessionNo, AuxAccent = UnknownAccent;
		WORD PrefixSetNo = UnknownPrefixSetNo;
		char	CommonAncode[100];
		CommonAncode[0] = 0;
		

		char s2[10];
		if (sscanf (buffer, "%s %i %i %i %s %s", Base, &ParadigmNo, &AccentModelNo, &SessionNo, CommonAncode, s2) != 6)
			throw CExpc("Cannot parse line %s", buffer);

		if (!strcmp(CommonAncode,"-"))
			CommonAncode[0] = 0;

		if (strcmp(s2,"-"))
			if ( (strlen(s2) == 0) ||  !isdigit((BYTE)s2[0]) )
				throw CExpc("Cannot parse line %s", buffer);
			else
				PrefixSetNo = atoi(s2);



		string lemm = Base;

		if(lemm == "#") lemm.erase();

		lemm += W.m_FlexiaModels[ParadigmNo].get_first_flex();

		W.m_LemmaToParadigm.insert(make_pair(lemm, CParadigmInfo(ParadigmNo,AccentModelNo, SessionNo, AuxAccent,CommonAncode, PrefixSetNo)));

		if( W.HasMeter() ) 
			W.GetMeter()->SetFilePos();
	}
}


//	Загружает *.mrd file.
//---------------------------------------------------
//	Описание формата *.mrd.
	
//	file: paradigm_number
//		paradigm |
//		...	 } paradigm_number times
//		paradigm |
//	      base_number
//		base PNUM |
//		...	  } base_number times
//		base PNUM |
//	paradigm: DICT_TYPE DEPR form ...
//	base: неизменяемая часть слова, или # если неизменяемая часть пустая
//	DICT_TYPE: тип словаря, одна буква
//	DEPR: одна буква (не используется, всегда '#')
//	form: % FLEX * ancode ...
//
//	PNUM - номер парадигиы, начиная с 0
//	
//	FLEX: окончание.
//
//	Первое окончание - окончание нормальной формы.
//	Остальные окончания отсортированы по алфавиту.
//	Внутри одной формы анкоды отсортированы по алфавиту
//	(сначала маленькие буквы, потом большие)

//	Пробела внутри парадигмы нет.
//---------------------------------------------------



void MorphoWizard::load_mrd(bool guest, bool bCreatePrediction)
{
	m_ReadOnly  =  guest || (access(get_lock_file_name().c_str(), 0) != -1);

	// лочим файл
	if (!m_ReadOnly)
	   CreateLockFile(get_lock_file_name());

	string Path = m_MrdPath;
    if (access (Path.c_str(), 04) == -1)
		Path = m_ProjectFileKeys["ProjectsDir"] + "/" + m_MrdPath;

	fprintf (stderr, "Reading mrd-file: %s\n", Path.c_str());


	FILE* fp  = fopen (Path.c_str(), "r");

	if (!fp) throw CExpc("Wrong mrd file : " + m_MrdPath);

	if( !!m_pMeter ) 
		m_pMeter->SetFileMaxPos(fp);

	ReadFlexiaModels(fp, m_FlexiaModels);

	ReadAccentModels(fp, m_AccentModels);

	ReadSessions(fp);

	ReadPrefixSets(fp);

	ReadLemmas(fp, *this);

	fclose (fp);

	if (bCreatePrediction)
		CreatePredictIndex();
}

string &MorphoWizard::get_value(const string &key)
{
	map<string, string>::iterator it = m_ProjectFileKeys.find(key);
	if(it == m_ProjectFileKeys.end()) throw CExpc("No such key: " + key);
	return it->second;
}








//	Сохраняет изменения в mrd file.
void MorphoWizard::save_mrd()
{
	assert(m_bLoaded);
	string Path = m_MrdPath;
	if ( access (Path.c_str(), 4) == -1)
	{
		Path = m_ProjectFileKeys["ProjectsDir"] + "/" + m_MrdPath;
		if ( access (Path.c_str(), 4) == -1)
	     throw CExpc("Can not open mrd file: " + m_MrdPath);
	};
	EndSession();
	
	
	// здесь раньше сохраняли с помощью stream, я заменил на FILE, стало работать
	// в  3 раз быстрее
	
	FILE * out_fp  = fopen (Path.c_str(), "w");
	if(!out_fp) throw CExpc("Error while saving to file. It may be corrupted");

	WriteFlexiaModels(out_fp, m_FlexiaModels );

	WriteAccentModels(out_fp,m_AccentModels);

	fprintf (out_fp, "%i\n", m_Sessions.size());
	for(size_t i=0; i < m_Sessions.size(); i++)
		fprintf (out_fp, "%s\n", m_Sessions[i].ToString().c_str());

	fprintf (out_fp, "%i\n", m_PrefixSets.size());
	for(size_t i=0; i < m_PrefixSets.size(); i++)
			fprintf (out_fp, "%s\n", get_prefix_set_str(i).c_str());

	fprintf (out_fp, "%i\n", m_LemmaToParadigm.size());
	for(lemma_iterator_t b = m_LemmaToParadigm.begin(); b != m_LemmaToParadigm.end(); ++b)
	{
		const CFlexiaModel&p = m_FlexiaModels[b->second.m_FlexiaModelNo];
		int flex_size = p.get_first_flex().size();
		int lemm_size = b->first.size();
		string base = b->first.substr(0, lemm_size - flex_size);
		if(base.empty()) base = "#";
		string s1 = (b->second.m_CommonAncode[0] == 0) ?  "-" :  b->second.GetCommonAncodeIfCan();
		string s2 = (b->second.m_PrefixSetNo == UnknownPrefixSetNo) ?  "-" :  Format("%i",b->second.m_PrefixSetNo);
		fprintf (out_fp, "%s %i %i %i %s %s\n", base.c_str(), b->second.m_FlexiaModelNo, b->second.m_AccentModelNo, b->second.m_SessionNo,s1.c_str(), s2.c_str());

	}
	fclose(out_fp);

	m_bWasChanged = false;

	log( Format("Saved by %s", GetUserName().c_str()) );
};


//----------------------------------------------------------------------------
void MorphoWizard::find_lemm(string lemm, bool bCheckLemmaPrefix, vector<lemma_iterator_t> &res) 
{
	if( !!m_pMeter ) 
	{
		m_pMeter->SetMaxPos(m_LemmaToParadigm.size());
		m_pMeter->SetInfo("Finding lemmas...");
	}

	// search a regular expression 
	if( (lemm.length() > 2) &&	(lemm[0] == '/') &&	(lemm[lemm.length()-1] == '/') )
	{
		string pattern = lemm.substr(1, lemm.length() - 2);
		RML_RE re(pattern, m_PcreCharacterTables);;;
		if (re.error() != "")
		{
			ErrorMessage(re.error());
			return;
		}

		for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
		{
			if ( re.PartialMatch(it->first) )
				res.push_back(it);

			if( !!m_pMeter ) m_pMeter->AddPos();
		}
		return;
	}

	int pos_acc = lemm.rfind('\'');
	if( pos_acc>0 ) 
	{
		lemm.erase(pos_acc,1);
		--pos_acc;
	}
	
	int pos_ast = lemm.find("*");
	if( pos_ast==string::npos )
	{
		string Lemma = lemm;
		//  the user can specify a prefix with '|" for example the input can be "auf|machen",
		// where "auf" is a prefix
		int prefix_pos = lemm.find("|");
		string Prefix;
		if (prefix_pos != string::npos)
		{
			Prefix = Lemma.substr(0,prefix_pos);
			RmlMakeUpper(Prefix, m_Language);
			Lemma.erase(0, prefix_pos+1);
		};
		pair<lemma_iterator_t,lemma_iterator_t> range = m_LemmaToParadigm.equal_range(Lemma);

		if( !!m_pMeter ) 
			m_pMeter->SetMaxPos(distance(range.first,range.second));

		for( lemma_iterator_t it=range.first; it!=range.second; ++it )
		{
			if (pos_acc==string::npos || GetLemmaAccent(it)==pos_acc)  // проверка ударения [17/Dec/2003]
				if	(		(		!Prefix.empty() 
								&& (it->second.m_PrefixSetNo != UnknownPrefixSetNo)
								&& (m_PrefixSets[it->second.m_PrefixSetNo].find(Prefix) != m_PrefixSets[it->second.m_PrefixSetNo].end())	
							)
						||	(		Prefix.empty() 
								&& (		!bCheckLemmaPrefix 
										|| (it->second.m_PrefixSetNo == UnknownPrefixSetNo)
									)
							)
					)
					res.push_back(it);

			if( !!m_pMeter ) m_pMeter->AddPos();
		}
		return;
	}
	// search with right truncation
	else if( pos_ast==lemm.size()-1 )
	{
		string s = lemm.substr(0,lemm.size()-1);
		for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
		{
			int fnd_pos = it->first.find(s);
			if( fnd_pos==0 )
				res.push_back(it);

			if( !!m_pMeter ) m_pMeter->AddPos();
		}
		return;
	}
	// search with left truncation
	else if( pos_ast==0 )
	{
		string s = lemm.substr(1,lemm.size()-1);
		for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
		{
			int fnd_pos = it->first.rfind(s);
			if( fnd_pos!=string::npos && fnd_pos==it->first.size()-s.size() )
				res.push_back(it);

			if( !!m_pMeter ) m_pMeter->AddPos();
		}
		return;
	}
	else
	{
		string s1 = lemm.substr(0,pos_ast);
		string s2 = lemm.substr(pos_ast+1,lemm.size()-pos_ast+1);
		for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
		{
			int fnd_pos1 = it->first.find(s1);
			int fnd_pos2 = it->first.rfind(s2);
			if( fnd_pos1==0 && fnd_pos2!=string::npos && fnd_pos2==it->first.size()-s2.size() )
				res.push_back(it);

			if( !!m_pMeter ) m_pMeter->AddPos();
		}
		return;
	}
}


bool IsLessBySession(const lemma_iterator_t& it1, const lemma_iterator_t& it2)
{
	return it1->second.m_SessionNo < it2->second.m_SessionNo;
};

void MorphoWizard::find_lemm_by_user(string username, vector<lemma_iterator_t> &res)
{
	res.clear();

	set<size_t> Sessions;

	for (size_t i=0; i < m_Sessions.size(); i++)
		if (m_Sessions[i].m_UserName == username)
			Sessions.insert(i);

	for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
		if (Sessions.find(it->second.m_SessionNo) != Sessions.end())
		{
			res.push_back(it);
		};
	sort(res.begin(), res.end(), IsLessBySession);
};


//----------------------------------------------------------------------------
bool simple_match (const string& pattern, const string& word)
{
	int l = pattern.length();
	if (l == 0) return false;

	if (l == 1)
		if (pattern[0] == '*')
			return false;


	if (pattern[0] == '*')
			return		word.length() >= l-1
					&& !strcmp(word.c_str() + word.length() -	l + 1, pattern.c_str() + 1);

	if (pattern[l-1] == '*')
			return		word.length() >= l-1
					&& !strncmp(word.c_str(), pattern.c_str(), l - 1);

	return pattern ==  word;

};

//----------------------------------------------------------------------------
// search a word form in all paradigms
//----------------------------------------------------------------------------
void MorphoWizard::find_wordforms(string wordform, vector<lemma_iterator_t> &res)
{
	if( !!m_pMeter ) 
	{
		m_pMeter->SetMaxPos(m_LemmaToParadigm.size());
		m_pMeter->SetInfo("Finding wordforms...");
	}
	Trim(wordform);

	if (wordform.empty()) return;

	//  if a pure wordfom was given then transform it to the regular  expression syntax
	string pattern;
	if (		(wordform[0] != '/') 
			||	(wordform[wordform.length()-1] != '/') 
			||	(wordform.length() < 3)
		)
		pattern = string("^") + wordform + string("$");
	else
		pattern = wordform.substr(1, wordform.length() -2);
	

		RML_RE re (pattern,  m_PcreCharacterTables);;
		if (re.error() !="")
		{
			ErrorMessage (re.error());
			return;
		}


		StringVector wordforms;
		for (lemma_iterator_t it = m_LemmaToParadigm.begin(); it != m_LemmaToParadigm.end();it++)
		{
			get_wordforms(it, wordforms);
			for (int i = 0; i < wordforms.size(); i++)
			{	
				if ( re.PartialMatch(wordforms[i]) )
					{
						res.push_back(it);
						break;
					}
			}
			if( !!m_pMeter ) m_pMeter->AddPos();
		
		}

}

/*
//----------------------------------------------------------------------------
void MorphoWizard::find_wordforms(const string &wordform, vector<lemma_iterator_t> &res)
{
	if( !!m_pMeter ) 
	{
		m_pMeter->SetMaxPos(m_LemmaToParadigm.size());
		m_pMeter->SetInfo("Finding wordforms...");
	}

	vector<string> wordforms;
	for (lemma_iterator_t it = m_LemmaToParadigm.begin(); it != m_LemmaToParadigm.end();it++)
	{
		get_wordforms(it, wordforms);
		for (int i = 0; i < wordforms.size(); i++)
		{	
			if (simple_match (wordform, wordforms[i] ) )
			{
				res.push_back(it);
				break;
			}
		}
		if( !!m_pMeter ) m_pMeter->AddPos();
	}
}
*/

//----------------------------------------------------------------------------
// search an ancode string in all paradigms (an ancode string can contain more than one ancode)
//----------------------------------------------------------------------------
void MorphoWizard::find_ancodes(const string& ancodes, vector<lemma_iterator_t> &res) 
{
	if( !!m_pMeter ) 
	{
		m_pMeter->SetMaxPos(m_LemmaToParadigm.size());
		m_pMeter->SetInfo("Finding ancodes...");
	}

	vector<WORD> prdno;

	for(size_t i=0; i < m_FlexiaModels.size(); i++ )
		for (size_t l=0; l<ancodes.size(); l+=2)
			if( m_FlexiaModels[i].has_ancode( ancodes.substr(l, 2) ) )
				prdno.push_back( i );

	sort(prdno.begin(),prdno.end());

	for( lemma_iterator_t i2=m_LemmaToParadigm.begin(); i2!=m_LemmaToParadigm.end(); i2++ )
	{
		WORD pno = i2->second.m_FlexiaModelNo;
		if( binary_search(prdno.begin(),prdno.end(),pno) )
			res.push_back(i2);

		if( !!m_pMeter ) m_pMeter->AddPos();
	}
}

//----------------------------------------------------------------------------
void MorphoWizard::find_lemm_by_grammem(const string &pos_and_grammems, vector<lemma_iterator_t> &res) 
{
	BYTE pos;
	QWORD gra;
	/*{	// processing type grammems for example "С лок | од,
		int u = pos_and_grammems.find("|");
		if (u != string::npos)
		{
			string q = pos_and_grammems.substr(0, u-1);
			Trim(q);
			u = q.find(" ");
			if (u != string::npos)
				q.erase(0, u);
			q = "*" + q;
			if (m_pGramTab->ProcessPOSAndGrammemsIfCan(q.c_str(), &pos, &gra))
			{
				throw CExpc("Wrong type grammem");
			};
		};
	};
	StringTokenizer R (pos_and_grammems.c_str(), "|");*/

	if(		!m_pGramTab->ProcessPOSAndGrammemsIfCan(pos_and_grammems.c_str(), &pos, &gra)
		)
	{
		throw CExpc("Wrong grammem");
	}

	string _codes =  m_pGramTab->GetAllPossibleAncodes(pos, gra);

	if(_codes.empty()) throw CExpc("Cannot find ancode by this morphological pattern");

	find_ancodes(_codes, res);
}

//----------------------------------------------------------------------------
void MorphoWizard::find_lemm_by_accent_model(int AccentModelNo, vector<lemma_iterator_t> &res)
{
	if( !!m_pMeter ) 
	{
		m_pMeter->SetMaxPos(m_LemmaToParadigm.size());
		m_pMeter->SetInfo("Finding lemmas...");
	}
	set<WORD> Models;
	if (AccentModelNo == -1)
	{
		for(size_t k=0; k < m_AccentModels.size(); k++)
			if (find(m_AccentModels[k].m_Accents.begin(), m_AccentModels[k].m_Accents.end(), UnknownAccent) != m_AccentModels[k].m_Accents.end())
				Models.insert(k);
	}
	else
		Models.insert(AccentModelNo);

	for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
	{
		if( Models.find(it->second.m_AccentModelNo) !=  Models.end() )
			res.push_back(it);

		if( !!m_pMeter ) m_pMeter->AddPos();
	}
}


//----------------------------------------------------------------------------
void MorphoWizard::find_lemm_by_prdno(WORD prdno, vector<lemma_iterator_t> &res)
{
	if( !!m_pMeter ) 
	{
		m_pMeter->SetMaxPos(m_LemmaToParadigm.size());
		m_pMeter->SetInfo("Finding lemmas...");
	}

	for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
	{
		if( it->second.m_FlexiaModelNo == prdno )
			res.push_back(it);

		if( !!m_pMeter ) m_pMeter->AddPos();
	}
}

//----------------------------------------------------------------------------
void MorphoWizard::find_lemm_by_prd_info( const CParadigmInfo& info, vector<lemma_iterator_t> &res)
{
	if( !!m_pMeter ) 
	{
		m_pMeter->SetMaxPos(m_LemmaToParadigm.size());
		m_pMeter->SetInfo("Finding lemmas...");
	}

	for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
	{
		if( info.IsAnyEqual(it->second) ) 
			res.push_back(it);

		if( !!m_pMeter ) m_pMeter->AddPos();
	}
}

//----------------------------------------------------------------------------
string MorphoWizard::get_pos_string	(const string &code) const 
{
	return  m_pGramTab->GetPartOfSpeechStr(m_pGramTab->GetPartOfSpeech(code.c_str()));
}

//----------------------------------------------------------------------------
string MorphoWizard::get_pos_string	(const lemma_iterator_t it) const 
{
	return get_pos_string(m_FlexiaModels[it->second.m_FlexiaModelNo].get_first_code());
}

//----------------------------------------------------------------------------
string MorphoWizard::get_grammem_string	(const string &code) const
{
	string res;
	for(int i = 0; i < code.size(); i += 2){
		if(i) res+=";";
		QWORD grams;
		m_pGramTab->GetGrammems(code.substr(i, 2).c_str(), grams);
		res += m_pGramTab->GrammemsToStr (grams);

	}
	return res;
}

//----------------------------------------------------------------------------
string MorphoWizard::get_pos_string_and_grammems	(const string &code) const 
{
	return  get_pos_string(code) + " " + get_grammem_string(code);
}

//----------------------------------------------------------------------------
string MorphoWizard::get_common_grammems_string	(const_lemma_iterator_t it) const
{
	string s = it->second.GetCommonAncodeIfCan();
	if (s.empty()) return "";

	QWORD grams;
	m_pGramTab->GetGrammems(s.c_str(), grams);
	return m_pGramTab->GrammemsToStr (grams);
}

//----------------------------------------------------------------------------
//  get union of type and form grammems of the input lemma
QWORD MorphoWizard::get_all_lemma_grammems	(const_lemma_iterator_t it) const
{
	QWORD grams = 0;
	string s = it->second.GetCommonAncodeIfCan();
	if (!s.empty()) 
		grams = m_pGramTab->GetAllGrammems(s.c_str());


	s = m_FlexiaModels[it->second.m_FlexiaModelNo].get_first_code();
	if (!s.empty()) 
		grams |= m_pGramTab->GetAllGrammems(s.c_str());
	
	return grams;
}


//----------------------------------------------------------------------------
string MorphoWizard::get_grammem_string	(lemma_iterator_t it) const
{
	return get_grammem_string(m_FlexiaModels[it->second.m_FlexiaModelNo].get_first_code());
	
}

//----------------------------------------------------------------------------
string MorphoWizard::get_lemm_string	(const_lemma_iterator_t it) const
{
	return it->first;
}

//----------------------------------------------------------------------------
string MorphoWizard::get_lemm_string_with_accents	(const_lemma_iterator_t it) const
{
	string form = it->first;
	RmlMakeLower(form, m_Language);
	SetAccent(it->second.m_AccentModelNo, it->second.m_AuxAccent, 0, form);
	return form;
}

//----------------------------------------------------------------------------
string MorphoWizard::get_base_string	(const_lemma_iterator_t it) const
{
	const CFlexiaModel& p = m_FlexiaModels[it->second.m_FlexiaModelNo];
	string flex  = p.get_first_flex();
	string lemm = it->first;
	string base = lemm.substr(0, lemm.size() - flex.size());
	return base;
}

const CMorphSession& MorphoWizard::get_session	(int SessionNo) const
{
	return m_Sessions[SessionNo];
}



//----------------------------------------------------------------------------
void MorphoWizard::remove_lemm(lemma_iterator_t it)
{
	WORD paradigm_num = it->second.m_FlexiaModelNo;
	CFlexiaModel &p = m_FlexiaModels[paradigm_num];
	log(it->first, p, false);
	m_LemmaToParadigm.erase(it);
}

string MorphoWizard::get_prefix_set_str(WORD PrefixSetNo) const
{
	string Result;
	const set<string>& PS = m_PrefixSets[PrefixSetNo];
	assert (!PS.empty());
	if (PS.empty()) return "";
	
	for (set<string>::const_iterator it = PS.begin();  it != PS.end(); it++)
	{
		Result += *it;
		Result += ",";
	};
	Result.erase(Result.length() -1);
	return  Result;
};

string MorphoWizard::get_prefix_set(const_lemma_iterator_t it) const
{
	return (it->second.m_PrefixSetNo==UnknownPrefixSetNo) ? "" : get_prefix_set_str(it->second.m_PrefixSetNo);
}


string MorphoWizard::get_slf_string(lemma_iterator_t it, string &common_grammems, string& Prefixes,  int line_size)
{
	const CParadigmInfo& I = it->second;
	const CFlexiaModel& P = m_FlexiaModels[I.m_FlexiaModelNo];
	Prefixes = get_prefix_set(it );
	common_grammems = get_grammem_string(I.GetCommonAncodeIfCan());
	return mrd_to_slf(it->first, P, I.m_AccentModelNo, I.m_AuxAccent, line_size);
}


BYTE  TransferReverseVowelNoToCharNo( const string& form, BYTE AccentCharNo, MorphLanguageEnum Language)
{
	if (AccentCharNo == UnknownAccent) return UnknownAccent;

	assert (AccentCharNo < form.length());
	int CountOfVowels = -1;
	int i=form.length()-1;
	assert ( i < UnknownAccent);
	for (; i>=0;i--)
	{	
		if	(		is_lower_vowel((BYTE)form[i], Language) 
				||	is_upper_vowel((BYTE)form[i], Language) 
			)
			CountOfVowels++;

		if (CountOfVowels == AccentCharNo)
		{
			return i;		
		};
	}
//	assert (false);
	return UnknownAccent;
};



void MorphoWizard::SetAccent(WORD AccentModelNo, BYTE AuxAccent, int FormNo, string& form) const
{
	if (AccentModelNo == UnknownAccentModelNo)  return;
	assert (FormNo < m_AccentModels[AccentModelNo].m_Accents.size());
	int u = TransferReverseVowelNoToCharNo(form, m_AccentModels[AccentModelNo].m_Accents[FormNo], m_Language);
	if (u != UnknownAccent)
	{
		form.insert(u+1,"'"); 
	};
	if (AuxAccent != UnknownAccent)
	{
		assert (AccentModelNo != UnknownAccentModelNo);
		// in  some forms auxiliary and main accents can be the same
		if (form[AuxAccent+1] != '\'') 
			form.insert(AuxAccent+1,"'"); 
			
	};
};


string MorphoWizard::mrd_to_slf(const string &lemm, const CFlexiaModel &p, WORD AccentModelNo, BYTE AuxAccent, int line_size) const
{
	string res;
	string base;
	string lem_code;
	for(size_t n = 0; n<p.m_Flexia.size(); n++)
	{
		string prefix = p.m_Flexia[n].m_PrefixStr;
		if (!prefix.empty()) prefix+= "|";
		string flex = p.m_Flexia[n].m_FlexiaStr;
		string code = p.m_Flexia[n].m_Gramcode;
		if(!n) base = lemm.substr(0, lemm.size() - flex.size());
		if(code.size() % 2 != 0) throw CExpc("Wrong gramm code");
		string form = prefix + base + flex;

		RmlMakeLower(form, m_Language);
		SetAccent(AccentModelNo, AuxAccent, n, form);

	
		for(int i = 0; i < code.size(); i+=2)
		{
			string gramcode  = code.substr(i,2);
			string grammems = get_pos_string_and_grammems(gramcode);

			//  adding word form
			res += form;
			res += " ";
			int n_spaces = line_size - form.size() - grammems.size() - 1;
			while(n_spaces-- >= 0) res += " ";

			//  adding morphological information
			
			res += grammems;
			res += "\r\n";
		};
	}

	return res;
}

void MorphoWizard::get_wordforms(const_lemma_iterator_t it, StringVector& forms) const
{
	const CFlexiaModel &p = m_FlexiaModels[it->second.m_FlexiaModelNo];
	const string &lemm = it->first;
	string base;
	forms.clear();
	for(size_t n = 0; n<p.m_Flexia.size(); n++)
	{
		string flex = p.m_Flexia[n].m_FlexiaStr;
		if(!n) 
			base = lemm.substr(0, lemm.size() - flex.size());
		forms.push_back(base + flex);
	}
}


bool MorphoWizard::slf2ancode(const string slf_line, string& gramcode) const
{
	BYTE pos;
	QWORD gra;
	
	if(		!m_pGramTab->ProcessPOSAndGrammemsIfCan(slf_line.c_str(), &pos, &gra)
		||	!m_pGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan(pos, gra, gramcode)
		)
		return false;
	return true;
};


struct CSlfLineByAncode 
{
	string m_Form;
	BYTE   m_AccentByte;
	string m_Prefix;	
};


//----------------------------------------------------------------------------
// при добавлении парадигмы производится сортировка сначала по граммемам, 
//		при совпадающих граммемах - по формам,
//		при совпадающих формах - по ударениям
// изменено Кецарисом [12/Apr/2004]
//----------------------------------------------------------------------------
struct CSlfLineByAncodeLess
{
	bool operator()( const CSlfLineByAncode& s1, const CSlfLineByAncode& s2 ) const
	{
		int c = s1.m_Form.compare(s2.m_Form);
		if( c!=0 ) return c<0;
		c = s1.m_Prefix.compare(s2.m_Prefix);
		if( c!=0 ) return c<0;
		return s1.m_AccentByte<s2.m_AccentByte;
	}
};

void MorphoWizard::slf_to_mrd(const string &s, string &lemm, CFlexiaModel& FlexiaModel, CAccentModel& AccentModel, BYTE&  AuxAccent, int& line_no_err) const
{
	lemm.empty();

	StringTokenizer lines(s.c_str(), "\r\n");

	string lemm_gramcode;
	typedef set<CSlfLineByAncode,CSlfLineByAncodeLess>	SlfLineSet;
	typedef map<string, SlfLineSet, AncodeLess > Gramcode2SlfLineMap;
	Gramcode2SlfLineMap Paradigm(ancode_less);
	size_t CommonLeftPartSize = lemm.size();
	BYTE LemmaAccentByte;
	AuxAccent = UnknownAccent;

	StringSet TestForFullDublicates;

	//  going through all lines of slf-representation,
	// building all pairs <wordform, gramcode>
	line_no_err = 0;
	int start = 0;
	do
	{
		line_no_err++;

		string line;
		{
			int end = s.find("\n", start);
			if (end == string::npos)
			{
				// no last eoln
				line = s.substr(start);
				start = s.length();
			}
			else
			{
				line = s.substr(start, end-start);
				start = end+1;
			}
		}
		
		Trim(line);
		if (line.empty()) continue;
		
		if (TestForFullDublicates.find(line) == TestForFullDublicates.end() )
			TestForFullDublicates.insert(line);
		else
			continue;

		string form;
		StringTokenizer tok(line.c_str(), "\t \r");
		if(!tok())	throw CExpc("Error! Cannot find a word form");
		form = tok.val();
		if(form.empty() ) throw CExpc("Error! Empty word form");

		string  pos_and_grammems;
		if(!tok())	throw CExpc("Error! Cannot find part of speech");
		pos_and_grammems = tok.val();
		if(tok())  pos_and_grammems += string(" ") + tok.val();

		if(pos_and_grammems.empty())
			throw CExpc("Error! No morphological annotation");
			
		if (tok())
			throw CExpc("Error! Unparsed chars at the end of the line");

		string gramcode;
		if( !slf2ancode(pos_and_grammems, gramcode) )
			throw CExpc("Error! Wrong morphological annotation(%s)", pos_and_grammems.c_str() );


		BYTE AccentByte = UnknownAccent;
		size_t CountOfAccents = 0;
		size_t CountOfVowels = 0;

		for (int k=form.length()-1; k >= 0; k--)
		{
			if (is_lower_vowel((BYTE)form[k], m_Language) )
				CountOfVowels++;

			if (form[k] == '\'') 
			{
				CountOfAccents++;
				if (CountOfAccents > 2)
					throw CExpc("Error! Too many stresses!" );
				if (		(k == 0) 
						||	!is_lower_vowel((BYTE)form[k-1], m_Language)
					)
					throw CExpc("A stress should be put on a vowel ");
				//  we should determine the auxiliary accent, which is permanent for all word forms that's  why
				// we can read it from the first line, but it should differ from the main accent
				// So it should be something like this "aaaAaaaMaaa", where A is the auxiliary 
				//  stress, and M  is the main stress.
				if (AccentByte == UnknownAccent)
					AccentByte = CountOfVowels;
				else
				{
					if (AuxAccent != UnknownAccent)
					{
						if (AuxAccent != k-1)
							throw CExpc("Auxiliary stress should be on the same position");
					}
					else
						AuxAccent = k-1;
				};
				form.erase(k, 1);
			};

		};


		RmlMakeUpper(form, m_Language);
		string Prefix;
		int PrefixInd = form.find("|");
		if (PrefixInd != string::npos)
		{
			Prefix = form.substr(0, PrefixInd);
			form.erase(0, PrefixInd+1);
		};


		if (line_no_err == 1)
		{
			lemm = form;
			lemm_gramcode = gramcode;
			LemmaAccentByte = AccentByte;
			CommonLeftPartSize = form.length();
		}
		else
		{
			CSlfLineByAncode Line;
			Line.m_AccentByte = AccentByte;
			Line.m_Form = form;
			Line.m_Prefix = Prefix;

			SlfLineSet set;
			set.insert(Line);
			pair<Gramcode2SlfLineMap::iterator, bool> p = Paradigm.insert(make_pair(gramcode, set));
			if( !p.second ) 
			{
				p.first->second.insert(Line);
			}

			//  calculating the common left part  of  all wordforms
			size_t i=0;
			for (; i<min(CommonLeftPartSize, form.length()); i++)
				if (form[i] != lemm[i])
					break;

			CommonLeftPartSize = i;
		};
	}
	while( start < s.length());



	if(lemm.empty())
		throw CExpc("Error! Empty paradigm");

	FlexiaModel.m_Flexia.clear();
	AccentModel.m_Accents.clear();

	//  adding lemma, it should be always at the first position
	FlexiaModel.m_Flexia.push_back(CMorphForm(lemm_gramcode, lemm.substr(CommonLeftPartSize), "" ) );
	AccentModel.m_Accents.push_back(LemmaAccentByte);

	//  adding the rest paradigm ordered by ancode
	for(Gramcode2SlfLineMap::const_iterator pit =  Paradigm.begin(); pit != Paradigm.end(); pit++)
	{
		const SlfLineSet& set = pit->second;
		string Gramcode = pit->first;
		for( SlfLineSet::const_iterator it = set.begin(); it != set.end(); it++)
		{
			string Flexia = it->m_Form.substr(CommonLeftPartSize);
			BYTE AccentByte = it->m_AccentByte;
			string Prefix = it->m_Prefix;
			FlexiaModel.m_Flexia.push_back(CMorphForm(Gramcode, Flexia, Prefix) );
			AccentModel.m_Accents.push_back(AccentByte);
		}
	};

}

void MorphoWizard::AncodeLess::init(const CAgramtab* pGramTab)
{
	m_pGramTab = pGramTab;
}

bool MorphoWizard::AncodeLess::operator ()(const string &s1, const string &s2)const
{
	return m_pGramTab->GetSourceLineNo(s1.c_str()) < m_pGramTab->GetSourceLineNo(s2.c_str());
}

string  MorphoWizard::GetUserName() const
{
	if (m_Sessions.empty()) 
		return "guest";
	else
		return m_Sessions.back().m_UserName;
};
void MorphoWizard::log(const string &messg)
{
	if( GetUserName() == "guest" )
		return;
	
	FILE *fp;
	if( (fp=fopen(get_log_file_name().c_str(),"a+t")) == NULL )
		return;

	fprintf (fp, "%s>", GetCurrentDate().c_str()); 
	fprintf (fp, "%s\n", messg.c_str()); 

	fclose(fp);
}

void MorphoWizard::log(const string &lemm, const CFlexiaModel &p, bool is_added)
{
	if (!m_bFullTrace) return;
	log((is_added ? "+ " : "- ") + lemm + " " + p.ToString());
}

WORD AddAccentModel(MorphoWizard& C, const CAccentModel& AccentModel)
{
	WORD AccentModelNo = UnknownAccentModelNo;
	if (!AccentModel.m_Accents.empty())
	{
		vector<CAccentModel>::iterator accent_it = find(C.m_AccentModels.begin(), C.m_AccentModels.end(), AccentModel);
		if(accent_it == C.m_AccentModels.end())
		{
			//  a new accent model should be added
			AccentModelNo = C.m_AccentModels.size();
			if (AccentModelNo == UnknownAccentModelNo)
				throw CExpc ("Too many accent models");

			C.m_AccentModels.push_back(AccentModel);
		}
		else
		{
			AccentModelNo = accent_it -  C.m_AccentModels.begin();
		}
	};
	return AccentModelNo;
};

WORD AddFlexiaModel(MorphoWizard& C, const CFlexiaModel& FlexiaModel)
{
	WORD ParadigmNo;
	// finding Paradigm No
	vector<CFlexiaModel>::iterator pit = find(C.m_FlexiaModels.begin(), C.m_FlexiaModels.end(), FlexiaModel);

	if(pit == C.m_FlexiaModels.end())
	{
		//  a new paradigm should be added
		ParadigmNo = C.m_FlexiaModels.size();
		if (ParadigmNo == 0xffff)
			throw CExpc ("Too many paradigms");

		C.m_FlexiaModels.push_back(FlexiaModel);
	}
	else
	{
		ParadigmNo = pit -  C.m_FlexiaModels.begin();
	}
	return ParadigmNo;
};


WORD MorphoWizard::AddPrefixSet(string PrefixSetStr)
{
	Trim(PrefixSetStr);
	
	if (PrefixSetStr.empty())
		return UnknownPrefixSetNo;

	set<string> PrefixSet;
	ReadOnePrefixSet(PrefixSetStr,PrefixSet);

	if (PrefixSet.empty())
		throw CExpc ("Cannot add empty prefix set");


	WORD Result;
	vector< set<string> >::iterator pit = find(m_PrefixSets.begin(), m_PrefixSets.end(), PrefixSet);
	if(pit == m_PrefixSets.end())
	{
		//  a new prefix set should be added
		Result = m_PrefixSets.size();
		if (Result == 0xffff)
			throw CExpc ("Too many prefix sets");

		m_PrefixSets.push_back(PrefixSet);
	}
	else
	{
		Result = pit -  m_PrefixSets.begin();
	}

	return Result;
}

//----------------------------------------------------------------------------
// Nick Ketsaris [4/Dec/2003]
//	return: CParadigmInfo
//----------------------------------------------------------------------------
CParadigmInfo MorphoWizard::add_lemma(const string &slf, string common_grammems, const string& prefixes, int& line_no_err, WORD SessionNo)
{
	string lemm;
	CFlexiaModel FlexiaModel; 
	CAccentModel AccentModel;
	BYTE AuxAccent;
	slf_to_mrd(slf, lemm, FlexiaModel, AccentModel, AuxAccent, line_no_err);
	
	string common_gramcode;
	if (!common_grammems.empty())
		if( !slf2ancode("* "+common_grammems, common_gramcode) )
			throw CExpc(Format("Wrong common grammems  \"%s\"", common_grammems.c_str()) );

	
	WORD ParadigmNo = AddFlexiaModel(*this, FlexiaModel);
	WORD AccentModelNo = AddAccentModel(*this, AccentModel);
	WORD PrefixSetNo = AddPrefixSet(prefixes);
	
	if (SessionNo == UnknownSessionNo)
		SessionNo = GetCurrentSessionNo();

	CParadigmInfo NewInfo(ParadigmNo, AccentModelNo, SessionNo, AuxAccent, common_gramcode.c_str() , PrefixSetNo);
	m_LemmaToParadigm.insert(make_pair(lemm, NewInfo));
	log(lemm, FlexiaModel, true);
	m_bWasChanged = true;
	return  NewInfo;
}

//----------------------------------------------------------------------------
void MorphoWizard::set_to_delete_false()
{
	for(lemma_iterator_t i1 = m_LemmaToParadigm.begin(); i1 != m_LemmaToParadigm.end(); ++i1)
	{
		i1->second.m_bToDelete = false;
	}
}

//----------------------------------------------------------------------------
void MorphoWizard::delete_checked_lemms()
{
	lemma_iterator_t i1 = m_LemmaToParadigm.begin();
	while (i1 != m_LemmaToParadigm.end())
		if (i1->second.m_bToDelete)
		{
			m_LemmaToParadigm.erase(i1);
			i1 = m_LemmaToParadigm.begin();
		}
		else
		  i1++;

	m_bWasChanged = true;
};

//----------------------------------------------------------------------------
void MorphoWizard::MakeReadOnly() 
{
	try 
	{
		if( !m_ReadOnly )
		{	 
			m_ReadOnly = true;
			string FileName = get_lock_file_name();
			if (access(FileName.c_str(), 0) != -1)
				remove(FileName.c_str());
		}
	}
	catch (...) 
	{
	}
}

//----------------------------------------------------------------------------
// del_dup_lemm deletes all equal lemmas with the same flexia and accent model
//----------------------------------------------------------------------------
size_t MorphoWizard::del_dup_lemm()
{
	size_t num = 0;
	lemma_iterator_t i1,i2;
	i1 = m_LemmaToParadigm.begin();

AGAIN:
	while( i1!=m_LemmaToParadigm.end() )
	{
		i2 = i1;
		i2++;
		while( i2!=m_LemmaToParadigm.end() )
		{
			if( i1->first!=i2->first )
				break;
			if( i1->second == i2->second )
			{
				string dbg_str = i2->first;
				WORD    dbg_num = i2->second.m_FlexiaModelNo;
				m_LemmaToParadigm.erase(i2);
				num++;
				i1 = m_LemmaToParadigm.begin();
				goto AGAIN;
			}
			i2++;
		}
		i1++;
	}

	if( num )
		m_bWasChanged = true;

	return num;
}




inline string GetSuffix(const string& Lemma, int PrefferedLength)
{
	int SuffLen = Lemma.length() - PrefferedLength;
	if (SuffLen < 0) SuffLen = 0;
	string Suffix = Lemma.substr(SuffLen );
	return Suffix;

};

bool IsLessByLemmaLength(const CPredictSuffix& _X1, const CPredictSuffix& _X2)
{
	return _X1.m_SourceLemma.length()  < _X2.m_SourceLemma.length();
};

void MorphoWizard::CreatePredictIndex()
{
	for (size_t i=0; i < MaxPredictSuffixLength-MinPredictSuffixLength+1; i++)
		m_PredictIndex[i].clear();


	if( !!m_pMeter ) 
	{
		m_pMeter->SetMaxPos(m_LemmaToParadigm.size());
		m_pMeter->SetInfo("Creating Predict Index...");
	}

	// go through all words
	vector<CPredictSuffix> AllLemmas;
	for (lemma_iterator_t lemm_it = m_LemmaToParadigm.begin(); lemm_it!=m_LemmaToParadigm.end(); lemm_it++ )
	{
		const CFlexiaModel& p = m_FlexiaModels[lemm_it->second.m_FlexiaModelNo];
		
		const char* lemma = lemm_it->first.c_str();

		// create predict suffix
		CPredictSuffix S;
		S.m_FlexiaModelNo = lemm_it->second.m_FlexiaModelNo;
		S.m_SourceLemmaAncode = p.get_first_code();
		S.m_SourceCommonAncode = lemm_it->second.GetCommonAncodeIfCan();
		S.m_SourceLemma = lemma;
		S.m_PrefixSetStr = get_prefix_set(lemm_it);
		S.m_Frequence = 1;
		if (S.m_SourceLemma.length() < 3) continue;
		AllLemmas.push_back(S);
	};

	sort(AllLemmas.begin(), AllLemmas.end(), IsLessByLemmaLength);

	// going through all prefix suffixes
	for (size_t i =0;  i<AllLemmas.size(); i++)
	{
		CPredictSuffix& S = AllLemmas[i];
		for (size_t suff_len=MinPredictSuffixLength; suff_len <= MaxPredictSuffixLength; suff_len++)
		{
			predict_container_t& PredictIndex = m_PredictIndex[suff_len-MinPredictSuffixLength];

			S.m_Suffix = GetSuffix(S.m_SourceLemma, suff_len);
	
			pair<predict_container_t::iterator,bool>  bRes	= PredictIndex.insert(S);
			if (!bRes.second)
			{
				bRes.first->m_Frequence++;
			}

		};

		if( !!m_pMeter ) 
			m_pMeter->AddPos();
	}
	

};


void MorphoWizard::predict_lemm(const string &lemm, const int preffer_suf_len, const int minimal_frequence, bool bOnlyMainPartOfSpeeches)
{
	
	m_CurrentPredictedParadigms.clear();
	m_CurrentNewLemma = lemm;
	if (preffer_suf_len < MinPredictSuffixLength) return;
	if (preffer_suf_len > MaxPredictSuffixLength) return;

	try{
		
		const predict_container_t& PredictIndex = m_PredictIndex[preffer_suf_len-MinPredictSuffixLength];

		string Suffix = GetSuffix(lemm, preffer_suf_len);
		
		for (predict_container_t::const_iterator it= PredictIndex.begin(); it !=  PredictIndex.end(); it++)
		{
			const CPredictSuffix& S = *it;

			if(S.m_Suffix != Suffix) continue;
			if (S.m_Frequence < minimal_frequence) 
				continue;

			if ( lemm.find("|") != string::npos)
				if (S.m_PrefixSetStr.empty())
					continue;

			const CFlexiaModel& P = m_FlexiaModels[S.m_FlexiaModelNo];
			string flex = P.get_first_flex();
			if (flex.size() > Suffix.size())
			{
				if (flex.size () >= lemm.size()) continue;
				if (flex != lemm.substr(lemm.length() - flex.size())) continue;
			};

			string pos = get_pos_string(S.m_SourceLemmaAncode);
			if (bOnlyMainPartOfSpeeches)
				if (GetPredictionPartOfSpeech(pos.c_str(), m_Language) == UnknownPartOfSpeech) continue;


			m_CurrentPredictedParadigms.push_back(it);
		};
		
	}catch(...){
		m_CurrentPredictedParadigms.clear();
		ErrorMessage("An exception occurred!");
	}
}


string MorphoWizard::create_slf_from_predicted(int PredictParadigmNo, string& common_grammems, int line_size) const
{

	const CPredictSuffix& S = *m_CurrentPredictedParadigms[PredictParadigmNo];
	const CFlexiaModel& P = m_FlexiaModels[S.m_FlexiaModelNo];

	common_grammems = get_grammem_string(S.m_SourceCommonAncode.c_str());
	string flex = P.get_first_flex();
	string NewLemma = m_CurrentNewLemma.substr(0, m_CurrentNewLemma.length() - flex.size()) + flex;
	if (NewLemma.find("|"))
		NewLemma.erase(0, NewLemma.find("|")+1);
	return mrd_to_slf(NewLemma, P, UnknownAccentModelNo, UnknownAccent, line_size); 

	/*
		// It was commented by Sokirko, because this code does not correctly process prefixes 
	string slf = "\n" + mrd_to_slf(S.m_SourceLemma, P, UnknownAccentModelNo, UnknownAccent, line_size);
	string flex = P.get_first_flex();

	string Base = "\n"+S.m_SourceLemma.substr(0, S.m_SourceLemma.length() - flex.size());
	RmlMakeLower(Base, m_Language);
	
	string NewBase = "\n"+m_CurrentNewLemma.substr(0, m_CurrentNewLemma.length() - flex.size());
	RmlMakeLower(NewBase, m_Language);

	for (int i =0; i <slf.size(); i++)
		if (slf.substr(i, Base.length()) == Base)
		{
			slf.replace(i, Base.length(), NewBase);
			i+= NewBase.size() - 1;
		};
	slf.erase(0, 1);
	return slf;*/

}



void	MorphoWizard::pack()
{
	map<int, int> OldFlexiaModelsToNewFlexiaModels;
	map<int, int> OldAccentModelsToNewAccentModels;
	map<int, int> OldPrefixSetsToNewPrefixSets;

	if( HasMeter() ) 
	{
		GetMeter()->SetInfo("Packing paradigms...");
		GetMeter()->SetMaxPos(m_LemmaToParadigm.size()+m_LemmaToParadigm.size()/4*2);
	}

	{
		log ("finding all used flexia and accent modleys");
		set<WORD> UsedFlexiaModels;
		set<WORD> UsedAccentModels;
		set<WORD> UsedPrefixSets;
		for (lemma_iterator_t lemm_it = m_LemmaToParadigm.begin(); lemm_it!=m_LemmaToParadigm.end(); lemm_it++)
		{
			UsedFlexiaModels.insert(lemm_it->second.m_FlexiaModelNo);
			UsedAccentModels.insert(lemm_it->second.m_AccentModelNo);
			UsedPrefixSets.insert(lemm_it->second.m_PrefixSetNo);
		};


		log("creating new flexia models without unused items");
		vector<CFlexiaModel> NewParadigms;	
		for(size_t i=0; i < m_FlexiaModels.size(); i++)
			if ( UsedFlexiaModels.find(i) != UsedFlexiaModels.end() )
			{
				
				size_t j=0;

				for (; j < NewParadigms.size(); j++)
					if (m_FlexiaModels[i] == NewParadigms[j])
						break;

				if (j == NewParadigms.size())
				{
					NewParadigms.push_back(m_FlexiaModels[i]);
					OldFlexiaModelsToNewFlexiaModels[i] = NewParadigms.size() - 1;
				}
				else // equal paradigm is already in the list 
				{
					OldFlexiaModelsToNewFlexiaModels[i] = j;
				};
			}
		m_FlexiaModels = NewParadigms;

		log("creating new accent models without unused items");
		vector<CAccentModel> NewAccentModels;	
		for(size_t k=0; k < m_AccentModels.size(); k++)
			if ( UsedAccentModels.find(k) != UsedAccentModels.end() )
			{
				NewAccentModels.push_back(m_AccentModels[k]);
				OldAccentModelsToNewAccentModels[k] = NewAccentModels.size() - 1;
			}
		m_AccentModels = NewAccentModels;

		log("creating new prefix sets");
		vector<set<string> > NewPrefixSets;	
		for(size_t i=0; i < m_PrefixSets.size(); i++)
			if ( UsedPrefixSets.find(i) != UsedPrefixSets.end() )
			{
				NewPrefixSets.push_back(m_PrefixSets[i]);
				OldPrefixSetsToNewPrefixSets[i] = NewPrefixSets.size() - 1;
			}
		m_PrefixSets = NewPrefixSets;

		if( HasMeter() ) GetMeter()->SetPos(m_LemmaToParadigm.size()/4);
	}

	log("fixing index from lemmas to paradigms");
	LemmaMap			NewLemmaToParadigm;
	for (lemma_iterator_t lemm_it = m_LemmaToParadigm.begin(); lemm_it!=m_LemmaToParadigm.end(); lemm_it++)
	{
		map<int, int>::const_iterator flex_it = OldFlexiaModelsToNewFlexiaModels.find(lemm_it->second.m_FlexiaModelNo);
		assert (flex_it != OldFlexiaModelsToNewFlexiaModels.end());

		WORD AccentModelNo = lemm_it->second.m_AccentModelNo;
		if (AccentModelNo != UnknownAccentModelNo)
		{
			map<int, int>::const_iterator accent_it = OldAccentModelsToNewAccentModels.find(lemm_it->second.m_AccentModelNo);
			assert (accent_it != OldAccentModelsToNewAccentModels.end());
			AccentModelNo = accent_it->second;
		};

		WORD PrefixSetNo = lemm_it->second.m_PrefixSetNo;
		if (PrefixSetNo != UnknownPrefixSetNo)
		{
			map<int, int>::const_iterator prefix_set_it = OldPrefixSetsToNewPrefixSets.find(PrefixSetNo);
			assert (prefix_set_it != OldPrefixSetsToNewPrefixSets.end());
			PrefixSetNo = prefix_set_it->second;
		}

		


		CParadigmInfo NewInfo(flex_it->second, 
								AccentModelNo, 
								lemm_it->second.m_SessionNo, 
								lemm_it->second.m_AuxAccent, 
								lemm_it->second.m_CommonAncode,
								PrefixSetNo);

		NewLemmaToParadigm.insert(make_pair(lemm_it->first, NewInfo));

		if( HasMeter() ) GetMeter()->AddPos();
	}
	m_LemmaToParadigm = NewLemmaToParadigm;


	del_dup_lemm();	

	if( HasMeter() ) 
		if (m_LemmaToParadigm.size() > 4)
			GetMeter()->AddPos(m_LemmaToParadigm.size()/4);

	
	CreatePredictIndex();

	

	m_bWasChanged = true;

};

//----------------------------------------------------------------------------
bool MorphoWizard::change_prd_info(CParadigmInfo& I, const string& Lemma, 
		WORD NewFlexiaModelNo, WORD newAccentModelNo, bool keepOldAccents )
{
	if(			NewFlexiaModelNo>=m_FlexiaModels.size() 
			||	(		newAccentModelNo>=m_AccentModels.size() 
					&&	newAccentModelNo!=UnknownAccentModelNo 
				)
			||	(		NewFlexiaModelNo==I.m_FlexiaModelNo 
					&&	newAccentModelNo==I.m_AccentModelNo 
				)
		)
		return false;


	if	(		( newAccentModelNo==UnknownAccentModelNo && !keepOldAccents ) 
			||	(I.m_FlexiaModelNo == UnknownParadigmNo)
		)
		I.m_AccentModelNo = UnknownAccentModelNo;
	else
	{
		/*
		if there is an old accent model, we can build a new accent model from the old one.
		We go through the new flexia model and for each flexia and gramcode we 
		search for the same word form and gramcode in the old flexia model. If the search
		is a success, then we transfer accent from the old wordform to the new one. 
		*/
		string OldBase = Lemma;
		const vector<CMorphForm>&	OldFlexia = m_FlexiaModels[I.m_FlexiaModelNo].m_Flexia;
		OldBase.erase(OldBase.length() - OldFlexia[0].m_FlexiaStr.length());
				
		
		string NewBase = Lemma;
		const vector<CMorphForm>&	NewFlexia = m_FlexiaModels[NewFlexiaModelNo].m_Flexia;
		NewBase.erase(NewBase.length()-NewFlexia[0].m_FlexiaStr.length());

		CAccentModel NewAccents;

		for (size_t i=0; i<NewFlexia.size(); i++)
		{
			string NewWordForm = NewBase+NewFlexia[i].m_FlexiaStr;
			size_t k=0;
			for (; k<OldFlexia.size(); k++)
				if	(		(OldBase+OldFlexia[k].m_FlexiaStr  == NewWordForm) 
						&&	(OldFlexia[k].m_Gramcode  == NewFlexia[i].m_Gramcode)
					)
					break;


			int accOld = _GetReverseVowelNo(NewWordForm,I.m_AccentModelNo,k);
			int accNew = _GetReverseVowelNo(NewWordForm,newAccentModelNo,i);
			int acc;
			if( keepOldAccents ) 
				acc = (accOld==UnknownAccent ? accNew : accOld);
			else
				acc = (accNew==UnknownAccent ? accOld : accNew);

			NewAccents.m_Accents.push_back(acc);
		}

		I.m_AccentModelNo = AddAccentModel(*this, NewAccents);
	}

	I.m_FlexiaModelNo = NewFlexiaModelNo;
	I.m_SessionNo = GetCurrentSessionNo();
	return true;
}

//----------------------------------------------------------------------------
void	MorphoWizard::clear_predicted_paradigms()
{
	m_CurrentPredictedParadigms.clear();
};

//----------------------------------------------------------------------------
string	MorphoWizard::get_predict_src_file_path(int mode) const 
{
	string name = GetPathByFile(m_MrdPath) + "predict.idx";
	if (access(name.c_str() ,mode))
	{
		map<string, string>::const_iterator it = m_ProjectFileKeys.find("ProjectsDir");
		string Path;
		if (it != m_ProjectFileKeys.end())
			Path = it->second;
		name = Path + "/" + name;
	};
	return name;
};

//----------------------------------------------------------------------------
string	MorphoWizard::show_differences_in_two_paradigms(WORD FlexiaModelNo1, WORD FlexiaModelNo2) const 
{
	string s1 = mrd_to_slf("-", m_FlexiaModels[FlexiaModelNo1], UnknownAccentModelNo, UnknownAccent, 79);;
	string s2 = mrd_to_slf("-", m_FlexiaModels[FlexiaModelNo2], UnknownAccentModelNo, UnknownAccent, 79);;

	StringVector V1, V2;

	StringTokenizer t1(s1.c_str(), "\n");
	while (t1()) V1.push_back(t1.val());
	sort(V1.begin(), V1.end());

	StringTokenizer t2(s2.c_str(), "\n");
	while (t2()) V2.push_back(t2.val());
	sort(V2.begin(), V2.end());

	StringVector Missing1(V1.size());
	StringVector::iterator end = set_difference(V1.begin(), V1.end(), V2.begin(), V2.end(), Missing1.begin());
	Missing1.resize(end - Missing1.begin());
	
	StringVector Missing2(V2.size());
	end = set_difference(V2.begin(), V2.end(), V1.begin(), V1.end(), Missing2.begin());
	Missing2.resize(end - Missing2.begin());

	string Result;
	if (!Missing1.empty())
	{
		Result += Format ("missing word forms in %i:\r\n",FlexiaModelNo2);
		for (size_t i=0; i<Missing1.size(); i++)
			Result += Missing1[i] + "\r\n";
	}

	if (!Missing2.empty())
	{
		Result += Format ("\r\nmissing word forms in %i:\r\n",FlexiaModelNo1);
		for (size_t i=0; i<Missing2.size(); i++)
			Result+= Missing2[i] + "\r\n";
	};

	if ( Missing2.empty() && Missing1.empty() )
	{
		Result = "No differences";
	};

	return Result;
};

//----------------------------------------------------------------------------
bool	MorphoWizard::check_common_grammems(string common_grammems) const
{
	Trim(common_grammems);
	if (common_grammems.empty()) return true;
	string common_ancode;
	return slf2ancode("* "+common_grammems, common_ancode);
}

//----------------------------------------------------------------------------
bool	MorphoWizard::check_prefixes(string prefixes) const
{
	Trim(prefixes);
	StringTokenizer tok(prefixes.c_str(), ",");
	while (tok())
	{
		if (strlen(tok.val()) == 0)
			return  false;
		if (!CheckLanguage(tok.val(), m_Language))
			return false;
	};

	return true;
}

//----------------------------------------------------------------------------
//  This function converts all paradigms with prefixes which are ascribed to particular forms
//  to paradigms without prefixes.  
//  1. We find a CMorphForm, which has at least one  prefix.
//  2. We go through the list of lemmas of  this CMorphForm.
//  3. Let A be a slf-representation of one paradigm with prefixes
//  3. Delete  A in the dictionary.
//  4. Delete all '|' (prefix delimiter)   in A.
//  5. Insert A to the dictionary.
// For example, if se have  a paradigm
// aaa NOUN
// bb|aaa NOUN
// cc|aaa NOUN
// will be converted to 
// aaa NOUN
// bbaaa NOUN
// ccaaa NOUN
//----------------------------------------------------------------------------
bool MorphoWizard::attach_form_prefixes_to_bases()
{
	bool bFound = false;
	fprintf (stderr,"   processing.... \n");
	DwordVector ModelsWithPrefixes;

	//  finding all models with prefixes
	for (int ModelNo = 0; ModelNo < m_FlexiaModels.size(); ModelNo++)
	{
		for (size_t k=0; k< m_FlexiaModels[ModelNo].m_Flexia.size(); k++)
			if (!m_FlexiaModels[ModelNo].m_Flexia[k].m_PrefixStr.empty())
			{
				ModelsWithPrefixes.push_back(ModelNo);
				break;
			};
	};

	if (ModelsWithPrefixes.empty())
		return true;

	size_t Count = 0;
	size_t Size = m_LemmaToParadigm.size();
	for( lemma_iterator_t it=m_LemmaToParadigm.begin(); it!=m_LemmaToParadigm.end(); it++ )
	{
		Count++;
		if (!(Count% 1000))
			printf ("%i/%i  \r", Count,Size);
	
		if (binary_search(ModelsWithPrefixes.begin(), ModelsWithPrefixes.end(), it->second.m_FlexiaModelNo))
		{
				bFound = true;

				string type_grm, Prefixes;
				string slf = get_slf_string(it, type_grm, Prefixes);
				
				{	
					
					assert(slf.find("|") != string::npos);
					string new_slf;
					for (size_t i=0; i < slf.length(); i++)
						if (slf[i] != '|')
							new_slf += slf[i];
					slf = new_slf;
				}

				lemma_iterator_t to_delete = it;
				it--;
				remove_lemm(to_delete);

				
				try {
					int line_no_err;
					add_lemma(slf, type_grm, Prefixes,line_no_err);
					
				}
				catch (...)
				{
					fprintf (stderr, "cannot add lemma \"%s\"!\n", it->first.c_str()); 
					fprintf (stderr, "Stopping the process!\n"); 
					return false;
				};

		}
	}

	printf ("%i/%i\n", Count,Size);

	if (!bFound) return true;

	
	fprintf (stderr,"   packing.... \n");
	pack();


	fprintf (stderr,"   checking.... \n");

	for (int ModelNo = 0; ModelNo < m_FlexiaModels.size(); ModelNo++)
	{
		for (size_t k=0; k< m_FlexiaModels[ModelNo].m_Flexia.size(); k++)
			if (!m_FlexiaModels[ModelNo].m_Flexia[k].m_PrefixStr.empty())
			{
				fprintf (stderr, "FlexModelNo=%i still has prefixes  !\n", ModelNo); 
				fprintf (stderr, "We cannot go further!\n"); 
				return false;
			};
	};
	return true;
}


//----------------------------------------------------------------------------
// эта функция приводит русскую морфологию к виду, который она должна  иметь в Диалинге:
// 1. удаление "ё"
// 2. ...
//----------------------------------------------------------------------------
bool	MorphoWizard::prepare_for_RML()
{
	if (m_Language != morphRussian) return true;

	// перевод "ё"  в "е"
	for (int ModelNo = 0; ModelNo < m_FlexiaModels.size(); ModelNo++)
		for (size_t k=0; k< m_FlexiaModels[ModelNo].m_Flexia.size(); k++)
		{
				ConvertJO2Je(m_FlexiaModels[ModelNo].m_Flexia[k].m_PrefixStr);
				ConvertJO2Je(m_FlexiaModels[ModelNo].m_Flexia[k].m_FlexiaStr);
		};

	for (lemma_iterator_t lemm_it = m_LemmaToParadigm.begin(); lemm_it!=m_LemmaToParadigm.end(); )
	{
		string Lemma = lemm_it->first;
		ConvertJO2Je(Lemma);
		lemma_iterator_t next_lemm_it =  lemm_it;
		next_lemm_it++;
		if (Lemma != lemm_it->first)
		{
			CParadigmInfo P = lemm_it->second;
			m_LemmaToParadigm.erase(lemm_it);
			m_LemmaToParadigm.insert(make_pair(Lemma, P));
		};
		lemm_it = next_lemm_it;
	};

	// checking
	for (lemma_iterator_t lemm_it = m_LemmaToParadigm.begin(); lemm_it!=m_LemmaToParadigm.end(); lemm_it++)
	{
		string Lemma = lemm_it->first;
		ConvertJO2Je(Lemma);
		if (Lemma != lemm_it->first)
			return false;
	};
	return true;
}

//----------------------------------------------------------------------------
bool MorphoWizard::HasUnknownAccents( lemma_iterator_t it ) const
{
	if( it->second.m_AccentModelNo==UnknownAccentModelNo ) 
		return true;

	CAccentModel accModel = m_AccentModels[it->second.m_AccentModelNo];
	for( int i=0; i<accModel.m_Accents.size(); ++i ) 
	{
		if( accModel.m_Accents[i]==UnknownAccent ) return true;
	}
	return false;
}

//----------------------------------------------------------------------------
bool MorphoWizard::IsPartialAccented( lemma_iterator_t it ) const
{
	if( it->second.m_AccentModelNo==UnknownAccentModelNo ) 
		return false;

	CAccentModel accModel = m_AccentModels[it->second.m_AccentModelNo];
	int count=0;
	for( int i=0; i<accModel.m_Accents.size(); ++i ) 
	{
		if( accModel.m_Accents[i]==UnknownAccent ) 
			++count;
	}
	return count<accModel.m_Accents.size();
}

//----------------------------------------------------------------------------
BYTE MorphoWizard::GetLemmaAccent( const_lemma_iterator_t it ) const
{
	if( it->second.m_AccentModelNo == UnknownAccentModelNo )
		return UnknownAccent;

	return TransferReverseVowelNoToCharNo(it->first, 
		m_AccentModels[it->second.m_AccentModelNo].m_Accents[0], m_Language);
}

//----------------------------------------------------------------------------
BYTE MorphoWizard::_GetReverseVowelNo( const string& form, WORD accentModelNo, WORD formInd ) const
{
	if( accentModelNo==UnknownAccentModelNo || accentModelNo>=m_AccentModels.size()
		|| formInd>=m_AccentModels[accentModelNo].m_Accents.size() ) 
		return UnknownAccent;

	BYTE vowelNo = m_AccentModels[accentModelNo].m_Accents[formInd];
	return TransferReverseVowelNoToCharNo(form,vowelNo,m_Language)==UnknownAccent 
		? UnknownAccent : vowelNo;
}

bool  PrintAllForms(const char* MrdFile, string OutFile)
{
	MorphoWizard					Wizard;
	if (!Wizard.load_wizard(MrdFile, "guest",false))
	{	
		fprintf (stderr,"Cannot load mrd-file : %s\n", MrdFile);
		return false;
	};

	fprintf (stderr,"attach_form_prefixes_to_bases \n");
	if (!Wizard.attach_form_prefixes_to_bases())
		return false;

	fprintf (stderr,"prepare_for_RML\n");
	if (!Wizard.prepare_for_RML())
		return false;

	
	FILE* fp = fopen (OutFile.c_str(), "wb");
	if (!fp) return false;
	for( lemma_iterator_t it=Wizard.m_LemmaToParadigm.begin(); it!=Wizard.m_LemmaToParadigm.end(); it++ )
	{
		size_t ModelNo = it->second.m_FlexiaModelNo;
		if (ModelNo  > Wizard.m_FlexiaModels.size())
		{
			fprintf (stderr,"Bad flexia model  : %s\n", Wizard.get_lemm_string_with_accents(it).c_str());
			return false;
		};

		string base  = Wizard.get_base_string(it);
		const CFlexiaModel&p = Wizard.m_FlexiaModels[ModelNo];
		for (size_t i=0; i <p.m_Flexia.size(); i++)
		{
			
			string WordForm = base;
			fprintf (fp, "%s\n",WordForm.c_str());
		};

		
	};
	fclose (fp);

	

	return true;
};


void	CDumpParadigm::SetEmpty()
{
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
bool	CDumpParadigm::ReadFromFile(FILE* fp, int& line_no, bool& bError, string& Errors)
{
	SetEmpty();
	if (feof(fp)) return  false;
	char buf[1000];

	bError = false;
	while (fgets(buf, 1000, fp)) 
	{
		line_no++;
		string s = buf;
		{
			const  char* auth_patt = "//edited by ";
			int qq = s.find(auth_patt);
			if (qq != string::npos)
			{
				m_AuthorStr = s.substr(qq+strlen(auth_patt));
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
		if (s.substr(0, 5) == "=====")
		{
			if (m_SlfStr.empty() && m_TypeGrammemsStr.empty() && m_PrefixesStr.empty() && !bError)
				continue;
			else
				break; 
		};

		if (s.substr(0, strlen(PrefixesField)) == PrefixesField)
		{
			int ind = s.find("=");
			if (ind == std::string::npos)
			{
				Errors += Format("cannot parse %s field at line", PrefixesField, line_no);
				bError = true;
			}
			else
			{
				m_PrefixesStr = s.substr(ind+1);
				Trim(m_PrefixesStr);
			};
			continue;
		};

		if (s.substr(0, strlen(TypeGrmField)) == TypeGrmField)
		{
			
			int ind = s.find("=");
			if (ind == std::string::npos)
			{
				Errors += Format("cannot parse %s field at line %i", TypeGrmField, line_no);
				bError = true;
			}
			else
			{
				m_TypeGrammemsStr = s.substr(ind+1);
				Trim(m_TypeGrammemsStr);
			};
			continue;
		};

		if (s.substr(0, strlen(SessionField)) == SessionField)
		{
			
			int ind = s.find("=");
			if (ind == std::string::npos)
			{
				Errors += Format("cannot parse %s field at line %i", SessionField, line_no);
				bError = true;
			}
			else
			{
				string  SessionStr = s.substr(ind+1);
				Trim(SessionStr);
				if (!m_Session.ReadFromString(SessionStr))
				{
					Errors += Format("cannot parse %s field at line %i", SessionField, line_no);
					bError = true;

				};
			};
			continue;
		};

		if (m_FirstSlfLineNo == -1)
			m_FirstSlfLineNo = line_no-1;
		m_SlfStr += s;
		m_SlfStr += "\r\n";
	};

	return !m_SlfStr.empty(); 
};

bool	CDumpParadigm::SaveHeaderToFile(FILE* fp) const
{
	fprintf (fp, "=====\n");

	if (!m_PrefixesStr.empty())
		fprintf (fp, "%s = %s\n", PrefixesField, m_PrefixesStr.c_str() );


	if (!m_TypeGrammemsStr.empty())
		fprintf (fp, "%s = %s\n", TypeGrmField, m_TypeGrammemsStr.c_str() );

	if ( !m_Session.IsEmpty() )
		fprintf (fp, "%s = %s\n", SessionField, m_Session.ToString().c_str());
	return true;
}

bool	CDumpParadigm::SaveToFile(FILE* fp) const
{
	if (!SaveHeaderToFile(fp))
		return false;
	fprintf (fp, "%s", m_SlfStr.c_str());
	return true;
};

WORD MorphoWizard::RegisterSession(const CMorphSession& S)
{
	if (S.IsEmpty()) return UnknownSessionNo;

	vector<CMorphSession>::const_iterator it = find(m_Sessions.begin(), m_Sessions.end(), S);
	if (it == m_Sessions.end())
	{
		m_Sessions.push_back(S);
		return m_Sessions.size() - 1;
		
	}
	else
		return it - m_Sessions.begin();
};

//----------------------------------------------------------------------------
bool MorphoWizard::ReadNextParadigmFromFile(FILE* fp, CDumpParadigm& P, int& line_no, bool& bError, string& Errors) const
{
	int start_line_no = line_no;

	if (!P.ReadFromFile(fp, line_no,  bError, Errors)) return false;
	if (!check_common_grammems(P.m_TypeGrammemsStr))
	{
		Errors += Format("cannot process common grammems in the paradigm at line %i \n", start_line_no);
		bError = true;
	};
	if (!check_prefixes(P.m_PrefixesStr))
	{
		Errors += Format("cannot process prefixes in the paradigm at  line %i \n", start_line_no);
		bError = true;
	};
	
	return true; 

};
