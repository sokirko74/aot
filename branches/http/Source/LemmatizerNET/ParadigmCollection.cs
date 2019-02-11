using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement;

namespace LemmatizerNET {
	internal class ParadigmCollection:IParadigmCollection {
		private List<FormInfo> _list;
		internal ParadigmCollection(List<FormInfo> list) {
			_list = list;
		}
		#region IParadigmCollection Members
		public int Count {
			get {
				return _list.Count;
			}
		}
		public IParadigm this[int index] {
			get {
				return _list[index];
			}
		}
		#endregion
	}
}
