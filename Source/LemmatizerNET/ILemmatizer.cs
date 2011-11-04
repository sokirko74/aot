using LemmatizerNET.Files;
namespace LemmatizerNET {
	public interface ILemmatizer {
		bool UseStatistic { get; set; }
		bool MaximalPrediction { set; }
		IParadigmCollection CreateParadigmCollectionFromNorm(string norm, bool capital, bool usePrediction);
		IParadigmCollection CreateParadigmCollectionFromForm(string form, bool capital, bool usePrediction);
		IParadigm CreateParadigmFromID(int id);
		void LoadDictionariesRegistry(FileManager manager);
		void LoadStatisticRegistry(Subject subj,FileManager manager);
		bool CheckABC(string word);
	}
}