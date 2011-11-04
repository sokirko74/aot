using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using LemmatizerNET.Implement.Agramtab;
using LemmatizerNET.Files;

namespace LemmatizerNET.Implement {
	internal class MorphAutomat : ABCEncoder {
		private MorphAutomNode[] _nodes;
		private MorphAutomRelation[] _relations;
		private int[] _childrenCache;

		private int FindStringAndPassAnnotChar(string text, int pos) {
			var TextLenth = text.Length;
			var r = 0;
			for (var i = pos; i < TextLenth; i++) {
				var nd = NextNode(r, text[i]);
				if (nd == -1) {
					return -1;
				}
				r = nd;
			}
			return NextNode(r, AnnotChar);
		}
		private void GetAllMorphInterpsRecursive(int nodeNo, string currPath, IList<AutomAnnotationInner> infos) {
			if (_nodes[nodeNo].IsFinal) {
				var a = new AutomAnnotationInner();
				int i = DecodeFromAlphabet(currPath);
				int itemNo;
				int modelNo;
				int prefixNo;
				DecodeMorphAutomatInfo(i, out modelNo, out itemNo, out prefixNo);
				a.ItemNo = (short)itemNo;
				a.ModelNo = (short)modelNo;
				a.PrefixNo = (short)prefixNo;
				infos.Add(a);
			}
			var count = GetChildrenCount(nodeNo);
			var currPathSize = currPath.Length;
			var buff = new char[currPathSize + 1];
			currPath.CopyTo(0, buff, 0, currPathSize);
			for (var i = 0; i < count; i++) {
				var p = GetChildren(nodeNo, i);
				buff[currPathSize] = Tools.GetChar(p.RelationalChar);
				GetAllMorphInterpsRecursive(p.ChildNo, new string(buff), infos);
			}
		}
		private void BuildChildrenCache() {
			var count = Constants.ChildrenCacheSize;
			if (_nodes.Length < Constants.ChildrenCacheSize) {
				count = _nodes.Length;
			}
			int len = count * Constants.MaxAlphabetSize;
			_childrenCache = new int[len];
			for (var i = 0; i < len; i++) {
				_childrenCache[i] = -1;
			}

			for (var NodeNo = 0; NodeNo < count; NodeNo++) {
				var count2 = GetChildrenCount(NodeNo);
				var start = _nodes[NodeNo].ChildrenStart;
				for (var i = 0; i < count2; i++) {
					_childrenCache[NodeNo * Constants.MaxAlphabetSize + _alphabet2Code[_relations[i + start].RelationalChar]] = _relations[i + start].ChildNo;
				}
			}
		}
		private void Clear() {
			_nodes = null;
			_relations = null;
		}
		public MorphAutomat(Lemmatizer lemmatizer,InternalMorphLanguage language, char annotChar)
			: base(lemmatizer,language, annotChar) {
		}
		public int NextNode(int nodeNo, char relationChar) {
			if (nodeNo < Constants.ChildrenCacheSize) {
				int z = _alphabet2Code[Tools.GetByte(relationChar)];
				if (z == -1) {
					return -1;
				}
				return _childrenCache[nodeNo * Constants.MaxAlphabetSize + z];
			} else {
				var num = Tools.GetByte(relationChar);
				var start = _nodes[nodeNo].ChildrenStart;
				var count = GetChildrenCount(nodeNo);
				for (int i = 0; i < count; i++) {
					if (num == _relations[i + start].RelationalChar) {
						return _relations[i + start].ChildNo;
					}
				}
			}
			return -1;
		}
		public MorphAutomNode GetNode(int nodeNo) {
			return _nodes[nodeNo];
		}
		public void GetInnerMorphInfos(string text, int textPos, IList<AutomAnnotationInner> infos) {
			infos.Clear();
			var r = FindStringAndPassAnnotChar(text, textPos);
			if (r == -1) {
				return;
			}
			GetAllMorphInterpsRecursive(r, "", infos);
		}
		public int GetChildrenCount(int nodeNo) {
			if (nodeNo + 1 == _nodes.Length) {
				return _relations.Length - _nodes[nodeNo].ChildrenStart;
			} else {
				return _nodes[nodeNo + 1].ChildrenStart - _nodes[nodeNo].ChildrenStart;
			}
		}
		public bool Load(string grammarFileName,FileManager manager) {
			Clear();
			using (var file = manager.GetFile(Lemmatizer.Registry, grammarFileName)) {
				string line = null;
				var reader = new BinaryReader(file, Tools.InternalEncoding);
				line = Tools.ReadLine(reader);
				if (line == null) {
					return false;
				}
				int nodeCount;
				int.TryParse(line, out nodeCount);
				if (nodeCount <= 0) {
					return false;
				}
				if (_nodes != null) {
					throw new MorphException("_nodes != null");
				}
				var len = nodeCount * sizeof(int);
				var buffer = reader.ReadBytes(len);
				if (buffer.Length != len) {
					return false;
				}
				var bufferInt = new int[nodeCount];
				Buffer.BlockCopy(buffer, 0, bufferInt, 0, buffer.Length);
				_nodes = new MorphAutomNode[nodeCount];
				for (var i = 0; i < nodeCount; i++) {
					_nodes[i] = new MorphAutomNode { Data = bufferInt[i] }; ;
				}
				line = Tools.ReadLine(reader);
				if (line == null) {
					return false;
				}
				int.TryParse(line, out nodeCount);
				len = nodeCount * sizeof(int);
				buffer = reader.ReadBytes(len);
				if (buffer.Length != len) {
					return false;
				}
				bufferInt = new int[nodeCount];
				Buffer.BlockCopy(buffer, 0, bufferInt, 0, buffer.Length);
				_relations = new MorphAutomRelation[nodeCount];
				for (int i = 0; i < nodeCount; i++) {
					_relations[i] = new MorphAutomRelation { Data = bufferInt[i] };
				}
				len = Constants.AlphabetSize * sizeof(int);
				buffer = reader.ReadBytes(len);
				if (buffer.Length != len) {
					return false;
				}
				var buf = new int[Constants.AlphabetSize];
				Buffer.BlockCopy(buffer, 0, buf, 0, buffer.Length);
				if (!Tools.ListEquals(buf, _alphabet2Code)) {
					throw new MorphException(Tools.GetStringByLanguage(Language) + "alphabet has changed; cannot load morph automat");
				}
			}
			BuildChildrenCache();
			return true;
		}
		public MorphAutomRelation GetChildren(int nodeNo,int index) {
			return _relations[_nodes[nodeNo].ChildrenStart+index];
		}
		public int EncodeMorphAutomatInfo(int modelNo, int itemNo, int prefixNo) {
				return (int)((((uint)((ushort)((uint)(modelNo) & 0xffff))) << 18)
					| (((uint)((ushort)((uint)(itemNo) & 0xffff))) << 9)
					| prefixNo);
		}
		public void DecodeMorphAutomatInfo(int info, out int modelNo, out int itemNo, out int prefixNo) {
			modelNo = info >> 18;
			itemNo = (0x3FFFF & info) >> 9;
			prefixNo = (0x1FF & info);
		}
		public string GetFirstResult(string text) {
			var nodeNo = FindStringAndPassAnnotChar(text, 0);
			if (nodeNo == -1) {
				return "";
			}
			var res = "";
			while (!_nodes[nodeNo].IsFinal) {
				var p = GetChildren(nodeNo,0);
				res += p.RelationalChar;
				nodeNo = p.ChildNo;
			}
			return res;
		}
		//private void DumpAllStringsRecursive(Stream fp, int NodeNo, string CurrPath);
		//internal bool DumpAllStrings(string FileName);
		//internal bool Save(string GrammarFileName); //TODO Future
	}
}
