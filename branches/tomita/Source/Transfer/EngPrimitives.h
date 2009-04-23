#if !defined(eng_primitives)
	#define eng_primitives

string GetPronounEnglishFormByGrammems(QWORD Grammems, bool self_form);
void TransferTimeGrammems (const CSemNode& RusNode, CEngSemNode& EngNode, const string& CallingFunc);
void TransferNumberGrammems (const CSemNode& RusNode, CEngSemWord& EngWord);
void TransferPersonGrammems (const CSemNode& RusNode,  CEngSemWord& EngWord);
void TransferGrammems (const CSemNode& RusNode, CEngSemNode& EngNode, string CallingFunc);
bool is_past(EngVerbTenseEnum t);
bool is_future(EngVerbTenseEnum t);
bool is_present(EngVerbTenseEnum t);
bool is_would(EngVerbTenseEnum t);
bool is_infinitive(EngVerbTenseEnum t);
bool is_continous(EngVerbTenseEnum t);
bool is_finite_continous(EngVerbTenseEnum t);
bool is_finite_perfect(EngVerbTenseEnum t);
bool is_gerund(EngVerbTenseEnum t);
bool is_not_finite_form(EngVerbTenseEnum t);
EngVerbTenseEnum convert_finite_form_to_infinitive(EngVerbTenseEnum t);
bool node_is_adv(const CEngSemNode& Node);
bool _node_is_adj(const CEngSemNode& Node); 
void set_possessive (CEngSemWord& EngWord);

#endif
