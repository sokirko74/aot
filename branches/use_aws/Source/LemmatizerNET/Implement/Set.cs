using System;
using System.Collections.Generic;
using System.Collections;


namespace LemmatizerNET.Implement {
	internal class Set<T>:ICollection<T> {
		private SortedList<T, T> _list = new SortedList<T, T>();
		public void AddRange(IEnumerable<T> collection) {
			foreach (var item in collection) {
				Add(item);
			}
		}
		#region ICollection<T> Members
		public void Add(T item) {
			if (!_list.ContainsKey(item)){
				_list.Add(item, item);
			}
		}
		public void Clear() {
			_list.Clear();
		}
		public bool Contains(T item) {
			return _list.ContainsKey(item);
		}
		public void CopyTo(T[] array, int arrayIndex) {
			_list.Keys.CopyTo(array, arrayIndex);
		}
		public int Count {
			get {
				return _list.Count;
			}
		}
		public bool IsReadOnly {
			get {
				return false;
			}
		}
		public bool Remove(T item) {
			return _list.Remove(item);
		}
		#endregion
		#region IEnumerable<T> Members
		public IEnumerator<T> GetEnumerator() {
			return _list.Keys.GetEnumerator();
		}
		#endregion
		#region IEnumerable Members
		IEnumerator IEnumerable.GetEnumerator() {
			return GetEnumerator();
		}
		#endregion
	}
}
