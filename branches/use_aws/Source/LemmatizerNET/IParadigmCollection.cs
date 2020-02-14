namespace LemmatizerNET {
	public interface IParadigmCollection {
		int Count { get; }
		IParadigm this[int index] { get; }
	}
}