using System;
using System.IO;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Runtime.Serialization.Formatters.Binary;


namespace LemmatizerNET.Implement.Agramtab {
	internal class Tools {
		private Encoding _encoding;
		private int _codepage;

		public Encoding InternalEncoding(int CodePage/*, [CallerMemberName] string p = null*/)
		{
			if (_codepage != CodePage || _encoding == null)
			{
				_encoding = Encoding.GetEncoding(CodePage);
				_codepage = CodePage;
			}
			return _encoding;
		}
		public static bool ListEquals<T>(IEnumerable<T> l1, IEnumerable<T> l2) {
			return ListEquals(l1, l2, null);
		}
		public static bool ListEquals<T>(IEnumerable<T> l1, IEnumerable<T> l2, IEqualityComparer<T> comparer) {
			if(object.Equals(l1,l2)){
				return true;
			}
			if (comparer == null) {
				comparer = EqualityComparer<T>.Default;
			}
			if (l1 == null) {
				throw new ArgumentNullException("l1");
			}
			if (l2 == null) {
				throw new ArgumentNullException("l2");
			}
			using (var e1 = l1.GetEnumerator()) {
				using (var e2 = l2.GetEnumerator()) {
					while (e1.MoveNext()) {
						if (!e2.MoveNext() || !comparer.Equals(e1.Current, e2.Current)) {
							return false;
						}
					}
					if (e2.MoveNext()) {
						return false;
					}
				}
			}
			return true;

			
		}
		public bool LoadFileToString(Stream stream, out string result, FileManager manager, int CodePage)
		{
			try {
				var file = new StreamReader(stream, InternalEncoding(CodePage));
				result = file.ReadToEnd();
				return true;
			} catch {
				result = null;
				return false;
			}
		}
		public static void LoadList<T>(Stream stream, ICollection<T> list) where T : ILoad, new() {
			while (stream.CanRead) {
				T val = new T();
				if (!val.Load(stream)) {
					return;
				}
				list.Add(val);
			}
		}
		public static string ReadLine(BinaryReader reader) {
			StringBuilder sb = new StringBuilder(80);
			do {
				var ch = reader.ReadChar();
				switch (ch) {
					case '\r':
						//if (reader.ReadChar() != '\n') {
						//	reader.BaseStream.Position--;
						//}
						//return sb.ToString();
						break;						
					case '\n':
						return sb.ToString();
					default:
						sb.Append(ch);
						break;
				}
			} while (true);
		}
		public static string GetStringByLanguage(InternalMorphLanguage langua) {
			switch (langua) {
				case InternalMorphLanguage.morphRussian: return "Russian";
				case InternalMorphLanguage.morphEnglish: return "English";
				case InternalMorphLanguage.morphGerman: return "German";
				case InternalMorphLanguage.morphGeneric: return "Generic";
				case InternalMorphLanguage.morphURL: return "URL_ABC";
				default: return "unk";
			}
		}
		public char GetChar(byte b, int codePage) {
			return InternalEncoding(codePage).GetChars(new[] { b })[0];
		}
		public byte GetByte(char ch, int codePage) {
			return InternalEncoding(codePage).GetBytes(new[] { ch })[0];
		}
		public byte TransferReverseVowelNoToCharNo(string form, byte accentCharNo, InternalMorphLanguage language, int codePage) {
			if (accentCharNo == Constants.UnknownAccent) {
				return Constants.UnknownAccent;
			}
			if (accentCharNo >= form.Length) {
				throw new MorphException("AccentCharNo >= form.Length");
			}
			var countOfVowels = -1;
			var i = form.Length - 1;
			if (i >= Constants.UnknownAccent) {
				throw new MorphException("i >= UnknownAccent");
			}
			for (; i >= 0; i--) {
				if (Lang.is_lower_vowel(GetByte(form[i], codePage), language)
					|| Lang.is_upper_vowel(GetByte(form[i], codePage), language)
				) {
					countOfVowels++;
				}
				if (countOfVowels == accentCharNo) {
					return (byte)i;
				}
			}
			return Constants.UnknownAccent;
		}
	}
}
