class CMorphologyHolder;
extern string GetParadigm(string WordForm, int LemmaNo, const CMorphologyHolder* Holder);
extern string Lemmatize(string WordForm, const CMorphologyHolder* Holder, int& LemmasCount);
