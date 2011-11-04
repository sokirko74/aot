using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement.Agramtab;
using System.IO;
using LemmatizerNET.Files;

namespace LemmatizerNET.Implement {


	internal class Statistic {
		private class Homonode : ILoad {
			public int X1;
			public int X2;
			public int X3;
			#region ILoad Members
			public bool Load(Stream stream) {
				var len = sizeof(int);
				var buffer = new byte[len];
				if (stream.Read(buffer, 0, len) != len) {
					return false;
				}
				X1 = BitConverter.ToInt32(buffer, 0);
				if (stream.Read(buffer, 0, len) != len) {
					return false;
				}
				X2 = BitConverter.ToInt32(buffer, 0);
				if (stream.Read(buffer, 0, len) != len) {
					return false;
				}
				X3 = BitConverter.ToInt32(buffer, 0);
				return true;
			}
			#endregion
		}
		private class Statnode : ILoad {
			public int X1;
			public int X2;
			#region ILoad Members
			public bool Load(Stream stream) {
				var len = sizeof(int);
				var buffer = new byte[len];
				if (stream.Read(buffer, 0, len) != len) {
					return false;
				}
				X1 = BitConverter.ToInt32(buffer, 0);
				if (stream.Read(buffer, 0, len) != len) {
					return false;
				}
				X2 = BitConverter.ToInt32(buffer, 0);
				return true;
			}
			#endregion
		}
		private class HomonodeComparer : IComparer<Homonode> {
			#region IComparer<_homonode_t> Members
			public int Compare(Homonode x, Homonode y) {
				return (x.X1 == y.X1
				? x.X2.CompareTo(y.X2)
					: x.X1.CompareTo(y.X1));
			}
			#endregion
		}
		private class StatnodeComparer : IComparer<Statnode> {
			#region IComparer<_statnode_t> Members
			public int Compare(Statnode x, Statnode y) {
				return x.X1.CompareTo(y.X1);
			}
			#endregion
		}
		private static HomonodeComparer _homonodeComparer = new HomonodeComparer();
		private static StatnodeComparer _statnodeComparer = new StatnodeComparer();

		private List<Statnode> _wordWeights=new List<Statnode>();
		private List<Homonode> _homoWeights=new List<Homonode>();

		public void Load(Lemmatizer lemmatizer, string prefix, FileManager manager) {
			using (var file = manager.GetFile(lemmatizer.Registry, prefix + Constants.HomoweightBinPath)) {
				Tools.LoadList(file, _homoWeights);
			}
			using (var file = manager.GetFile(lemmatizer.Registry, prefix + Constants.WordweightBinPath)) {
				Tools.LoadList(file, _wordWeights);
			}
		}
		public int GetHomoWeight(int paradigmid, int form) {
			Homonode item = new Homonode {
				X1 = paradigmid,
				X2 = form
			};
			int index = _homoWeights.BinarySearch(item, _homonodeComparer);
			if (index >= 0 && index < _homoWeights.Count) {
				return _homoWeights[index].X3;
			}
			return 0;
		}
		public int GetWordWeight(int paradigmid) {
			Statnode item = new Statnode { X1 = paradigmid };
			int index = _wordWeights.BinarySearch(item, _statnodeComparer);
			if (index >= 0 && index < _wordWeights.Count) {
				return _wordWeights[index].X2;
			}
			return 0;
		}
	}
}
