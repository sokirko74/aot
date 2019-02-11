// =====================================================
// Перевод тайм-групп. Файл создан Алексеем Сокирко 17  ноября 2000 года.
// Документация - в "l:\documents\Описание TimeRoss.lnk"
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#include "StdAfx.h"
#include "EngBinaryTranslate.h"
#include <map>
#include "EngNumerals.h"
#include "EngSemStructure.h"



string get_field(CRossHolder*  RossHolder, WORD UnitNo, string Field, BYTE LeafId, BYTE BracketLeafId, BYTE LevelId)
{
 if (UnitNo ==  ErrUnitNo) return "";
 BYTE FieldNo = RossHolder->GetRoss()->GetFieldNoByFieldStr(Field.c_str());
 if (FieldNo == ErrUChar) return "";

 if (!RossHolder->GetRoss()->IsEmptyArticle(UnitNo))
	for (size_t i = RossHolder->GetRoss()->GetUnitStartPos(UnitNo); i<= RossHolder->GetRoss()->GetUnitEndPos(UnitNo); i++)
	  if (     (RossHolder->GetRoss()->GetCortegeFieldNo(i) == FieldNo) 
		    && (RossHolder->GetRoss()->GetCortegeLeafId(i) == LeafId) 
			&& (RossHolder->GetRoss()->GetCortegeBracketLeafId(i) == BracketLeafId) 
			&& ( (LevelId == ErrUChar) || (RossHolder->GetRoss()->GetCortege(i)->m_LevelId == LevelId) )
		 )
	  {
         return (const char*)RossHolder->GetRoss()->GetDomItemStr(RossHolder->GetRoss()->GetCortegeItem(i,0));
	  };

  return "";
};


string tr_by_ross_simple(CRossHolder*  RossHolder, string UnitStr)
{
	EngRusMakeLower(UnitStr);
	WORD UnitNo = RossHolder->GetRoss()->LocateUnit(UnitStr.c_str(), 1);
	if(UnitNo == ErrUnitNo) return "";
	return get_field(RossHolder,UnitNo, "ENG", 0, 0, ErrUChar);
}

string in_delimiters(const string &word, int delim1, int delim2)
{
	int start = word.find(delim1);
	int stop = word.find(delim2);
	if(start == string::npos || stop == string::npos || stop < start) return "";
	return word.substr(start + 1, stop - start - 1);
}

/*
 в таймгруппу могут входить только однословные термины типа дни недели, месяцы
*/

string tr_by_thesaurus(const CThesaurus* Thes, string TextEntryStr)
{	   
	long TerminId = Thes->GetTerminIdBySingleWord(TextEntryStr.c_str());
	if (TerminId ==-1) return "";
	vector<int> EnglishTranslations;
	Thes->QueryEnglishTranslations(TerminId, EnglishTranslations);
	if( EnglishTranslations.empty()  )
		return "";

	return Thes->m_Termins[EnglishTranslations[0]].m_TerminStr;
};



string CEngSemStructure::time_tr_by_ross(long RusNodeNo, string &brack)
{
	const CSemNode& RusNode  = RusStr.GetNode(RusNodeNo);
	bool is_pl = brack == "pl";
	string pl_str = brack == "pl" ? "s" : "";

	string res;
	// если у узла нет частей речи, то его надо переводить просто так
	if ( RusNode.IsWordContainer() && RusNode.GetWord(0).m_Poses == 0)
	{
		for (long i=0; i < RusNode.GetWordsSize(); i++)
			res +=  RusNode.GetWord(i).m_Word + " ";
	}
	else 
	// переводим группу однородных числительных, которые заполняют одно место в поле CONTENT
	if(RusNode.m_NodeType == MNA && RusNode.IsWordContainer())
	{
		vector<long> Nodes;
		RusStr.GetOutcomingNodes(RusNodeNo, Nodes, false);
		for (long i=0; i<Nodes.size(); i++)
		{
			if (!RusStr.GetNode(Nodes[i]).IsPrimitive()) continue;
			const CSemWord&  W = RusStr.GetNode(Nodes[i]).GetWord(0);
			m_Nodes[GetEngNodeByRusNode(Nodes[i])].m_bToDelete = true;
			// W - одно числительное. По-русски оно могло быть написано буквами или цифрами,
			// но семанттика преобразует числа прописью в арабскую запись
			string one_numeral;
			if (W.m_Poses != 0)
			   one_numeral = spellout_number (W.m_Word, W.m_Poses & (1 < NUMERAL));
			else
			   one_numeral = W.m_Word.c_str();
			if (i == Nodes.size() - 1)  // вставка союза, который должен стоять на предпоследнем месте
			{
			    res += tr_by_ross_simple(GetRossHolder(OborRoss), RusNode.GetWord(0).m_Lemma);			
				res += " ";
			}
			res += one_numeral + " ";
		};
		return res;
	}
	else
	if(RusNode.IsPrimitive()){
		string word = RusNode.GetWord(0).m_Lemma;
		EngRusMakeLower(word);

		/*
		 если есть числительное полу, тогда слово должно быть в единственном
		*/
	    if (RusNode.HasRelOperator("ПОЛУ"))
			  pl_str ="";


		res = tr_by_ross_simple(GetRossHolder(TimeRoss), word);
		if(res.size()) 
		{ 
			if (RusNode.HasRelOperator("ПОЛУ"))
			  res = " half "+res;
			return res + pl_str;
		};

		res = tr_by_thesaurus(m_pData->GetThes(OmniThes), word);
		if(res.size()) 
		{ 
			if (RusNode.HasRelOperator("ПОЛУ"))
			  res = " half "+res;
			return res + pl_str;
		};


		res = tr_by_ross_simple(GetRossHolder(Ross), word);

		// сколько времени => how_much time
		if (res.size())
		 if (res.find("_") != -1)
		 {
			WORD UnitNo = GetRoss(EngCollocRoss)->LocateUnit(res.c_str(), 1);
			if(UnitNo == ErrUnitNo) 
				res = "";
			else
			    res = get_field(GetRossHolder(EngCollocRoss), UnitNo, "CONTENT", 0, 0, ErrUChar);
		 };
		if(res.size()) 
		{ 
			if (RusNode.HasRelOperator("ПОЛУ"))
			  res = " half "+res;
			return res + pl_str;
		};

    	if (   RusNode.GetWord(0).HasPOS (NUMERAL) 
			|| RusNode.GetWord(0).HasPOS (NUMERAL_P)
		   )
		  return  spellout_number (RusNode.GetWord(0).m_Word, RusNode.GetWord(0).HasPOS (NUMERAL));

		UINT grammems = 0;
		if(is_pl) grammems |= ePlural;

        long ParadigmId = helper.GetParadigmIdByLemma(morphRussian, RusNode.GetWord(0).m_Lemma,  0);
		if (ParadigmId == -1) return "!!!error!!!";
		vector<long> Ids;
		helper.translate_id(ParadigmId, Ids, 0);
		if (Ids.size() == 0) return "!!!error!!!";
		res = helper.create_form_by_id (Ids[0], grammems);

		if(res.size()) 
		{ 
			if (RusNode.HasRelOperator("ПОЛУ"))
			  res = " half "+res;
			return res + pl_str;
		};
	} 
	return res;
}


bool  CEngSemStructure::translate_time_node ( int MainNodeNo) 
{
	CEngSemNode &MainNode = m_Nodes[MainNodeNo];

    /* если однословная тайм-группа  была переведена с помощью АОСС в заглушечную статью,
	 и в самой статье приписан перевод, то нужно взять этот перевод
	 Например, слово "век".
	 Но обязательно надо проверить, что перед нами не часть большой тайм группы (последнее условие)
	*/

	if  (    (MainNode.RusNode != -1)
		  && (RusStr.GetNode(MainNode.RusNode).GetType() == TimeRoss)   
		  && IsPlugArticle(GetRossHolder(Aoss), MainNode.GetUnitNo())
	      && (MainNode.m_Words.size() > 0)
		  && !MainNode.m_Words[0].m_bDoNotChangeForm
		  &&	(			( MainNode.m_Colloc.m_Type != RossType)
					||		( MainNode.m_Colloc.GetRossInterp().m_DictType != TimeRoss)
				)
		)
	{
		string EngField = get_field(GetRossHolder(TimeRoss), RusStr.GetNode(MainNode.RusNode).GetUnitNo(), "ENG", 0, 0, ErrUChar);
		if (EngField == "") return false;
		MainNode.m_Words.resize(1);
		MainNode.m_Words[0].m_Word = EngField;  
		MainNode.m_Words[0].m_Lemma = EngField;  
		EngRusMakeLower(MainNode.m_Words[0].m_Lemma);
		MainNode.m_bReached = true;  
		MainNode.DelAllInterps();
		return true;
	}

	// далее только про многословные тайм-группы
    if		(    !MainNode.IsWordContainer()
			  ||  MainNode.m_bReached
			  || !MainNode.IsMainTimeRossNode()
			)
			return false;
	
	const CRossPartialInterp& MainArticle = MainNode.m_Colloc.GetRossInterp();
		  
	SetNodeToDeleteFalse();

	// артикль, если нужно, будет прописан в словарной статье таймросса, а то получается
	// в прошлом году -> the last year
	// удаляем артикль, который был выставлен внешними процедурами
	MainNode.m_ArticleStr = "";
	MainNode.m_ArticleCauseHistory.push_back(OverwriteArticleForTimeNodes);

	// собираем все узлы словосочетания в один Map 
	map<int , CEngSemNode *> nodes;

	for(int i = 0; i < m_Nodes.size(); i++)
	{
		if(m_Nodes[i].m_CollocId == MainNode.m_CollocId)
		{	 
			nodes[m_Nodes[i].m_Colloc.GetRossInterp().m_ItemNo+1] = (&m_Nodes[i]);
			if (!m_Nodes[i].IsMainTimeRossNode())
			  m_Nodes[i].m_bToDelete = true;
		};
	}
	
    // получаем поле "ENG"  статьи сл-ния и токенизируем его по пробелу
	string EngField = get_field(GetRossHolder(MainArticle.m_DictType), MainArticle.m_UnitNo, "ENG", 0, 0, ErrUChar);
	StringTokenizer eng_item_tok (EngField.c_str(), " ");

	// в переменную res положим результаты перевода 

	string res;
	while( true ) {
		const char* s = eng_item_tok();
		if (s == 0) break;
		string eng_item = s;
		if (eng_item == "")  break;
		//  в квадратных скабках стоит номер в поле состав
	    string PlaceNoStr = in_delimiters(eng_item, '[', ']');
		//  в  круглых скобках стоят граммемы
	    string GrammemsStr = in_delimiters(eng_item, '(', ')');
		string tr;

		if (PlaceNoStr == "")
		{
		  if(eng_item == "NOSP")
			{ 
				while(res.size() && res[res.size() - 1] == ' '){
					res.resize(res.size() - 1);
				}
			}
		  else
			tr = eng_item;
		}
		else
		{
			BYTE PlaceNo = atoi (PlaceNoStr.c_str());
			if (nodes[PlaceNo] == 0)
			{
				string UnitStr =  GetRoss(TimeRoss)->GetEntryStr(MainArticle.m_UnitNo);
				string Mess = "При переводе \""+UnitStr +"\" не найдено  соответствия для " +eng_item;
				MainNode.m_bReached = true;  
				ErrorMessage (Mess);
				return true;
			};
 			

			assert  (nodes[PlaceNo]->RusNode != -1);

			const CSemNode& N = RusStr.GetNode(nodes[PlaceNo]->RusNode);

			if(eng_item.substr(0,2) == "TR")
			{  
				tr = get_field(GetRossHolder(MainArticle.m_DictType), MainArticle.m_UnitNo, "TR", 0, PlaceNo, N.m_Colloc.GetRossInterp().m_LexFetNo+1);
			}
			else if(eng_item[0] == '[')
			{
				/*
				 если элемент поля ENG равен "[i]", а в русском тексте было число,
				 тогда надо взять русский текст как есть. Например,
				 "в 1960-х годах" -> "in 1960s"
				*/
				if (isdigit(N.GetWord(0).m_Word[0]))
					tr = N.GetWord(0).m_Lemma;
				else
	   				tr = time_tr_by_ross(nodes[PlaceNo]->RusNode, GrammemsStr);
			}
			else if(eng_item.substr(0,2) == "SR")
			{
	   			tr = time_tr_by_ross(nodes[PlaceNo]->RusNode, GrammemsStr);
			}
			else if ( eng_item.substr(0,2) == "PR" )
			{
				   tr = get_field(GetRossHolder(MainArticle.m_DictType), MainArticle.m_UnitNo, "PR", 0, PlaceNo, N.m_Colloc.GetRossInterp().m_PrepNoInArticle+1);
			} 
		};


	    res += tr;

	   if(eng_item != "NOSP")
		  res += " ";

	}
    


	MainNode.m_Words.resize(1);
	MainNode.m_Words[0].m_Word = res;  
	MainNode.m_Words[0].m_Lemma = "";
	MainNode.m_Words[0].m_ParadigmId = -1;
	MainNode.m_Words[0].SetFormGrammems(0);
	MainNode.SetInterp(CDictUnitInterp(TimeRoss,MainArticle.m_UnitNo));
	MainNode.m_bReached = true;  
	MainNode.m_Words[0].m_bDoNotChangeForm = true;  

	/*
	 если на отношении, которое  идет в главный узел есть предлог, тогда стираем  его
	 чтобы синтез не написал его два раза,
	 кроме прелдлога "than", который ставит ApplyComparativeRule
	*/
	vector <long> Rels;
	GetIncomingRelations(MainNodeNo,Rels);
	if (Rels.size() == 1)
	{
		vector<CRossInterp>& V = m_Relations[Rels[0]].m_SynReal.m_Preps;
		for (long i=0; i<V.size(); i++)
			if (    (V[i].m_DictType == EngObor) 
				 && (V[i].m_UnitNo != ErrUnitNo)  
				 && (string(GetRoss(EngObor)->GetEntryStr(V[i].m_UnitNo)) == "than")
			   )
			   continue;
			else
			{
				V.erase(V.begin() + i);
				i--;
			};
	};




	// все отношения, идущие к узлам, не вошедшим в эту таум-группу 
	// подвешиваем к главному узлу (узлы вошедшие в группу  помечены m_bToDelete),
	// потому что они потом будут удалены. 
	// Попутно вычисляем самый минимальный WordNo
	PrintNodes();
	long MinWordNo = 10000;
	for (int i=0; i < m_Nodes.size(); i++)
	if  (     (m_Nodes[i].m_CollocId == MainNode.m_CollocId)
		)
	{
		if (m_Nodes[i].m_Words[0].m_WordNo <MinWordNo)
			MinWordNo = m_Nodes[i].m_Words[0].m_WordNo;

		if (!m_Nodes[i].IsMainTimeRossNode()) continue;

		vector <long> Rels;
		GetOutcomingRelations(i,Rels);
	    for (int k=0; k < Rels.size(); k++)
	     if (!m_Nodes[m_Relations[Rels[k]].m_TargetNodeNo].m_bToDelete)
			 m_Relations[Rels[k]].m_SourceNodeNo = MainNodeNo;
	};
	MainNode.m_Words[0].m_WordNo  = MinWordNo;
    //удаляем все узлы, вошедшие в группу
	DelNodesToDelete();
	return true;
}



