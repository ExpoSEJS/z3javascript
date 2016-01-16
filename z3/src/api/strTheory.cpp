#include "strTheory.h"

#define DEBUGLOG 1
#define DEBUG_LOG 1

FILE * logFile = NULL;
int sLevel = 0;
int searchStart = 0;
int tmpStringVarCount = 0;
int tmpIntVarCount = 0;
int tmpXorVarCount = 0;
int tmpBoolVarCount = 0;
int tmpConcatCount = 0;
bool loopDetected = false;

std::map<std::string, Z3_ast> constStr_astNode_map;
std::map<Z3_ast, Z3_ast> length_astNode_map;
std::map<Z3_ast, Z3_ast> containsReduced_bool_str_map;
std::map<Z3_ast, Z3_ast> containsReduced_bool_subStr_map;
std::map<Z3_ast, int> basicStrVarAxiom_added;
std::map<Z3_ast, Z3_ast> concat_eqc_index;

std::map<std::pair<Z3_ast, Z3_ast>, Z3_ast> star_astNode_map;
std::map<std::pair<Z3_ast, Z3_ast>, Z3_ast> union_astNode_map;
std::map<std::pair<Z3_ast, Z3_ast>, Z3_ast> idxof_astNode_map;
std::map<std::pair<Z3_ast, Z3_ast>, Z3_ast> search_astNode_map;
std::map<std::tuple<Z3_ast, Z3_ast, Z3_ast>, Z3_ast> subStr_astNode_map;
std::map<Z3_ast, Z3_ast> subStr_res_map;
std::map<Z3_ast, Z3_ast> subStr_axiom_map;
std::map<std::tuple<Z3_ast, Z3_ast, Z3_ast>, Z3_ast> repAll_astNode_map;
std::map<Z3_ast, Z3_ast> repAll_res_map;
std::map<Z3_ast, Z3_ast> repAll_axiom_map;
std::map<std::pair<Z3_ast, Z3_ast>, Z3_ast> concat_astNode_map;
std::map<std::pair<Z3_ast, Z3_ast>, Z3_ast> contains_astNode_map;
std::map<std::pair<Z3_ast, Z3_ast>, std::map<int, Z3_ast> > varForBreakConcat;

//----------------------------------------------------------------
// TODO:
int keeptry = true;
int tempPatch = false;
int prop = 1;
int num_try = 5;
int num_prop = 1;

std::map<Z3_ast, int> num_try_map;
std::map<std::pair<Z3_ast, Z3_ast>, bool> reassert_map;

//----------------------------------------------------------------

std::map<Z3_ast, int> inputVarMap;

//----------------------------------------------------------------

std::map<Z3_ast, unsigned int> fvarLenCountMap;
std::map<Z3_ast, std::vector<Z3_ast> > fvarLenTesterMap;
std::map<Z3_ast, Z3_ast> lenTesterFvarMap;

std::map<Z3_ast, std::map<int, std::vector<std::pair<int, Z3_ast> > > > fvarValueTesterMap;
std::map<Z3_ast, std::vector<int> > valRangeMap;
std::map<Z3_ast, Z3_ast> valueTesterFvarMap;

std::map<Z3_ast, std::pair<Z3_ast, Z3_ast>> varToStarMap;

//----------------------------------------------------------------

bool defaultCharSet = true;
char * charSet = NULL;
std::map<char, int> charSetLookupTable;
int charSetSize = 0;

const std::string escapeDict[] = { "\\x00", "\\x01", "\\x02", "\\x03", "\\x04",
		"\\x05", "\\x06", "\\x07", "\\x08", "\\t", "\\n", "\\x0b", "\\x0c",
		"\\r", "\\x0e", "\\x0f", "\\x10", "\\x11", "\\x12", "\\x13", "\\x14",
		"\\x15", "\\x16", "\\x17", "\\x18", "\\x19", "\\x1a", "\\x1b", "\\x1c",
		"\\x1d", "\\x1e", "\\x1f", " ", "!", "\\\"", "#", "$", "%", "&", "'",
		"(", ")", "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", "4", "5",
		"6", "7", "8", "9", ":", ";", "<", "=", ">", "?", "@", "A", "B", "C",
		"D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q",
		"R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\\\", "]", "^", "_",
		"`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
		"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{",
		"|", "}", "~", "\\x7f", "\\x80", "\\x81", "\\x82", "\\x83", "\\x84",
		"\\x85", "\\x86", "\\x87", "\\x88", "\\x89", "\\x8a", "\\x8b", "\\x8c",
		"\\x8d", "\\x8e", "\\x8f", "\\x90", "\\x91", "\\x92", "\\x93", "\\x94",
		"\\x95", "\\x96", "\\x97", "\\x98", "\\x99", "\\x9a", "\\x9b", "\\x9c",
		"\\x9d", "\\x9e", "\\x9f", "\\xa0", "\\xa1", "\\xa2", "\\xa3", "\\xa4",
		"\\xa5", "\\xa6", "\\xa7", "\\xa8", "\\xa9", "\\xaa", "\\xab", "\\xac",
		"\\xad", "\\xae", "\\xaf", "\\xb0", "\\xb1", "\\xb2", "\\xb3", "\\xb4",
		"\\xb5", "\\xb6", "\\xb7", "\\xb8", "\\xb9", "\\xba", "\\xbb", "\\xbc",
		"\\xbd", "\\xbe", "\\xbf", "\\xc0", "\\xc1", "\\xc2", "\\xc3", "\\xc4",
		"\\xc5", "\\xc6", "\\xc7", "\\xc8", "\\xc9", "\\xca", "\\xcb", "\\xcc",
		"\\xcd", "\\xce", "\\xcf", "\\xd0", "\\xd1", "\\xd2", "\\xd3", "\\xd4",
		"\\xd5", "\\xd6", "\\xd7", "\\xd8", "\\xd9", "\\xda", "\\xdb", "\\xdc",
		"\\xdd", "\\xde", "\\xdf", "\\xe0", "\\xe1", "\\xe2", "\\xe3", "\\xe4",
		"\\xe5", "\\xe6", "\\xe7", "\\xe8", "\\xe9", "\\xea", "\\xeb", "\\xec",
		"\\xed", "\\xee", "\\xef", "\\xf0", "\\xf1", "\\xf2", "\\xf3", "\\xf4",
		"\\xf5", "\\xf6", "\\xf7", "\\xf8", "\\xf9", "\\xfa", "\\xfb", "\\xfc",
		"\\xfd", "\\xfe", "\\xff" };
bool avoidLoopCut = true;

//----------------------------------------------------------------
// Data structure for modified algorithm
// backtrack-able cut information
struct T_cut {
	int level;
	std::map<Z3_ast, int> vars;

	T_cut() {
		level = -100;
	}
};

std::map<Z3_ast, std::stack<T_cut *> > cut_VARMap;

void cutVarsMapCopy(std::map<Z3_ast, int> & dest, std::map<Z3_ast, int> & src) {
	std::map<Z3_ast, int>::iterator itor = src.begin();
	for (; itor != src.end(); itor++) {
		dest[itor->first] = 1;
	}
}

void addCutInfoOneNode(Z3_ast baseNode, int slevel, Z3_ast node) {
	if (cut_VARMap.find(baseNode) == cut_VARMap.end()) {
		T_cut * varInfo = new T_cut();
		varInfo->level = slevel;
		varInfo->vars[node] = 1;
		cut_VARMap[baseNode].push(varInfo);
	} else {
		if (cut_VARMap[baseNode].empty()) {
			T_cut * varInfo = new T_cut();
			varInfo->level = slevel;
			varInfo->vars[node] = 1;
			cut_VARMap[baseNode].push(varInfo);
		} else {
			if (cut_VARMap[baseNode].top()->level < slevel) {
				T_cut * varInfo = new T_cut();
				varInfo->level = slevel;
				cutVarsMapCopy(varInfo->vars, cut_VARMap[baseNode].top()->vars);
				varInfo->vars[node] = 1;
				cut_VARMap[baseNode].push(varInfo);
			} else if (cut_VARMap[baseNode].top()->level == slevel) {
				cut_VARMap[baseNode].top()->vars[node] = 1;
			} else {
				printf("should not be here. exit %d\n", __LINE__);
				exit(0);
			}
		}
	}
}

void addCutInfoMerge(Z3_ast destNode, int slevel, Z3_ast srcNode) {
	if (cut_VARMap.find(srcNode) == cut_VARMap.end()) {
		printf("should not be here. exit %d\n", __LINE__);
		exit(0);
	}

	if (cut_VARMap[srcNode].empty()) {
		printf("should not be here. exit %d\n", __LINE__);
		exit(0);
	}

	if (cut_VARMap.find(destNode) == cut_VARMap.end()) {
		T_cut * varInfo = new T_cut();
		varInfo->level = slevel;
		cutVarsMapCopy(varInfo->vars, cut_VARMap[srcNode].top()->vars);
		cut_VARMap[destNode].push(varInfo);
	} else {
		if (cut_VARMap[destNode].empty()
				|| cut_VARMap[destNode].top()->level < slevel) {
			T_cut * varInfo = new T_cut();
			varInfo->level = slevel;
			cutVarsMapCopy(varInfo->vars, cut_VARMap[destNode].top()->vars);
			cutVarsMapCopy(varInfo->vars, cut_VARMap[srcNode].top()->vars);
			cut_VARMap[destNode].push(varInfo);
		} else if (cut_VARMap[destNode].top()->level == slevel) {
			cutVarsMapCopy(cut_VARMap[destNode].top()->vars,
					cut_VARMap[srcNode].top()->vars);
		} else {
			printf("should not be here. exit %d\n", __LINE__);
			exit(0);
		}
	}
}

/*
 *
 */
void checkandInit_cutVAR(Z3_theory t, Z3_ast node) {
	if (cut_VARMap.find(node) != cut_VARMap.end()) {
		return;
	} else {
		if (!isConstStr(t, node)) {
			addCutInfoOneNode(node, -1, node);
		}
	}
}

bool hasSelfCut(Z3_ast n1, Z3_ast n2) {
	if (cut_VARMap.find(n1) == cut_VARMap.end())
		return false;

	if (cut_VARMap.find(n2) == cut_VARMap.end())
		return false;

	if (cut_VARMap[n1].empty() || cut_VARMap[n2].empty())
		return false;

	std::map<Z3_ast, int>::iterator itor = cut_VARMap[n1].top()->vars.begin();
	for (; itor != cut_VARMap[n1].top()->vars.end(); itor++) {
		if (cut_VARMap[n2].top()->vars.find(itor->first)
				!= cut_VARMap[n2].top()->vars.end())
			return true;
	}
	return false;
}

/*
 *
 */

void printCutVAR(Z3_theory t, Z3_ast node) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "\n>> CUT info of [");
	printZ3Node(t, node);
	__debugPrint(logFile, "]\n");

	if (cut_VARMap.find(node) != cut_VARMap.end())
	{
		if (! cut_VARMap[node].empty())
		{
			__debugPrint(logFile, "[%2d] {", cut_VARMap[node].top()->level);
			std::map<Z3_ast, int>::iterator itor = cut_VARMap[node].top()->vars.begin();
			for (; itor != cut_VARMap[node].top()->vars.end(); itor++) {
				printZ3Node(t, itor->first);
				__debugPrint(logFile, ", ");
			}
			__debugPrint(logFile, "}\n");
		}
		else
		{

		}
	}
	__debugPrint(logFile, "------------------------\n\n");
#endif
}

//----------------------------------------------------------------

/*
 *
 */
void setAlphabet() {
	if (defaultCharSet) {
		charSetSize = 256;
		charSet = new char[charSetSize];
		int idx = 0;
		// non-printable - 2
		for (int i = 127; i < 256; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
		// small letters
		for (int i = 97; i < 123; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
		// caps
		for (int i = 65; i < 91; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
		// numbers
		for (int i = 48; i < 58; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
		// printable marks - 1
		for (int i = 32; i < 48; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
		// printable marks - 2
		for (int i = 58; i < 65; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
		// printable marks - 3
		for (int i = 91; i < 97; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
		// printable marks - 4
		for (int i = 123; i < 127; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
		// non-printable - 1
		for (int i = 0; i < 32; i++) {
			charSet[idx] = (char) i;
			charSetLookupTable[charSet[idx]] = 1;
			idx++;
		}
	} else {
		const char setset[] = { 'a', 'b', 'c' };
		int fSize = sizeof(setset) / sizeof(char);

		charSet = new char[fSize];
		charSetSize = fSize;
		for (int i = 0; i < charSetSize; i++) {
			charSet[i] = setset[i];
			charSetLookupTable[setset[i]] = 1;
		}
	}
}

/*
 *
 */
Z3_ast mk_var(Z3_context ctx, const char * name, Z3_sort ty) {
	Z3_symbol s = Z3_mk_string_symbol(ctx, name);
	return Z3_mk_const(ctx, s, ty);
}

/*
 *
 */
Z3_ast mk_bool_var(Z3_context ctx, const char * name) {
	Z3_sort ty = Z3_mk_bool_sort(ctx);
	return mk_var(ctx, name, ty);
}

/*
 *
 */
Z3_ast mk_int_var(Z3_context ctx, const char * name) {
	Z3_sort ty = Z3_mk_int_sort(ctx);
	return mk_var(ctx, name, ty);
}

/*
 *
 */
Z3_ast mk_int(Z3_context ctx, int v) {
	Z3_sort ty = Z3_mk_int_sort(ctx);
	return Z3_mk_int(ctx, v, ty);
}

Z3_sort my_mk_str_sort(Z3_theory t) {
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	return td->String;
}

std::string encodeInputString(Z3_string istr) {
	std::stringstream out;
	char buf[128];
	for (; *istr; istr++) {
		sprintf(buf, "_0x%x ", *istr);
		out << buf;
	}
	printf("ENCODED: %s\n", out.str().c_str());
	return out.str();
}

Z3_ast my_mk_str_value(Z3_theory t, Z3_string str) {

	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData *) Z3_theory_get_ext_data(t);

	// if the empty string is not created, create one
	if (constStr_astNode_map.find("") == constStr_astNode_map.end()) {
		Z3_symbol empty_str_sym = Z3_mk_string_symbol(ctx, "\"\"");
		Z3_ast emptyStrNode = Z3_theory_mk_value(ctx, t, empty_str_sym, td->String);
		constStr_astNode_map[""] = emptyStrNode;
	}

	std::string keyStr = std::string(str);
	// if the str is not created, create one
	if (constStr_astNode_map.find(keyStr) == constStr_astNode_map.end()) {
		Z3_symbol str_sym = Z3_mk_string_symbol(ctx, str);
		Z3_ast strNode = Z3_theory_mk_value(ctx, t, str_sym, td->String);
		constStr_astNode_map[keyStr] = strNode;
	}
	return constStr_astNode_map[keyStr];
}

/*
 *
 */
Z3_ast my_mk_str_var(Z3_theory t, char const * name) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData *) Z3_theory_get_ext_data(t);
	Z3_ast varAst = mk_var(ctx, name, td->String);
	basicStrVarAxiom(t, varAst, __LINE__);
	return varAst;
}

/*
 *
 */
Z3_ast my_mk_internal_string_var(Z3_theory t) {
	std::stringstream ss;
	ss << tmpStringVarCount;
	tmpStringVarCount++;
	std::string name = "_t_str" + ss.str();
	return my_mk_str_var(t, name.c_str());
}

/*
 * Make an integer variable used for intermediated representation
 */
Z3_ast my_mk_internal_int_var(Z3_theory t) {
	Z3_context ctx = Z3_theory_get_context(t);
	std::stringstream ss;
	ss << tmpIntVarCount;
	tmpIntVarCount++;
	std::string name = "_t_int_" + ss.str();
	return mk_int_var(ctx, name.c_str());
}

/*
 *
 */
Z3_ast mk_internal_xor_var(Z3_theory t) {
	Z3_context ctx = Z3_theory_get_context(t);
	std::stringstream ss;
	ss << tmpXorVarCount;
	tmpXorVarCount++;
	std::string name = "_t_xor_" + ss.str();
	return mk_int_var(ctx, name.c_str());
}

/*
 *
 */
Z3_ast my_mk_and(Z3_theory t, Z3_ast * item, int count) {
	Z3_context ctx = Z3_theory_get_context(t);
	if (count == 0)
		return Z3_mk_true(ctx);
	else if (count == 1)
		return item[0];
	else
		return Z3_mk_and(ctx, count, item);
}

/*
 *
 */
Z3_ast mk_2_and(Z3_theory t, Z3_ast and1, Z3_ast and2) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast and_items[2] = { and1, and2 };
	return Z3_mk_and(ctx, 2, and_items);
}

/* ---------------------------------
 * Return the node type in Enum
 * ---------------------------------
 */
int getNodeType(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_ast_kind z3Kind = Z3_get_ast_kind(ctx, n);

	switch (z3Kind) {
		case Z3_NUMERAL_AST: {
			return my_Z3_Num;
		}

		case Z3_APP_AST: {
			Z3_sort s = Z3_get_sort(ctx, n);
			if (Z3_theory_is_value(t, n)) {
				Z3_sort_kind sk = Z3_get_sort_kind(ctx, s);
				Z3_func_decl d = Z3_get_app_decl(ctx, Z3_to_app(ctx, n));
				if (sk == Z3_BOOL_SORT) {
					if (d == td->Contains || d == td->StartsWith || d == td->EndsWith || d == td->In) {
						return my_Z3_Func;
					} else {
						return my_Z3_ConstBool;
					}
				}
				if (sk == Z3_INT_SORT) {
					if (d == td->Length || d == td->Indexof || d == td->Indexof_I || d == td->Search) {
						return my_Z3_Func;
					}
				} else if (sk == Z3_UNKNOWN_SORT) {
					if (s == td->String) {
						if (d == td->Concat || d == td->SubString
								|| d == td->Replace || d == td->Star
								|| d == td->Union || d == td->Star_I
								|| d == td->ReplaceAll) {
							return my_Z3_Func;
						} else {
							return my_Z3_ConstStr;
						}
					}
				}
			} else {
				//Z3 native functions fall into this category
				Z3_sort s = Z3_get_sort(ctx, n);
				if (s == td->String) {
					return my_Z3_Str_Var;
				} else {
					return my_Z3_Func;
				}
			}

			break;
		}

		case Z3_VAR_AST: {
			return my_Z3_Var;
		}

		default: {
			break;
		}
	}

	return my_Z3_Unknown;
}

/* Only focus on star, concat, conststr */
T_myZ3Type getNodeTypeCustom(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);

	Z3_ast_kind z3Kind = Z3_get_ast_kind(ctx, n);
	switch (z3Kind) {
	case Z3_APP_AST: {
		if (Z3_theory_is_value(t, n)) {
			Z3_func_decl d = Z3_get_app_decl(ctx, Z3_to_app(ctx, n));
			if (d == td->Contains || d == td->Indexof || d == td->Indexof_I || d == td->Replace || d == td->In || d == td->Star)
				return my_Z3_Unknown;
			else if (d == td->Star_I)
				return my_Z3_Star;
			else if (d == td->Concat)
				return my_Z3_Concat;
			else if (d == td->ReplaceAll)
				return my_Z3_ReplaceAll;
			else if (d == td->Search)
				return my_Z3_Search;
			else if (d == td->SubString)
				return my_Z3_SubString;
			else
				return my_Z3_ConstStr;
		} else {
			return my_Z3_Unknown;
		}
		break;
	}
	default: {
		break;
	}
	}
	return my_Z3_Unknown;
}

int my_get_node_type(Z3_theory t, Z3_ast n) {
	auto cType = (int) getNodeTypeCustom(t, n);
	return cType != my_Z3_Unknown ? cType : getNodeType(t, n);
}

/*
 *
 */
bool isConstStr(Z3_theory t, Z3_ast node) {
	if (getNodeType(t, node) == my_Z3_ConstStr) {
		return true;
	} else {
		return false;
	}
}

bool my_is_const_str(Z3_theory t, Z3_ast node) {
	return isConstStr(t, node);
}

/*
 *
 */
inline bool isConstNum(Z3_theory t, Z3_ast node) {
	if (getNodeType(t, node) == my_Z3_Num) {
		return true;
	} else {
		return false;
	}
}

/*
 *
 */
Z3_ast mk_1_arg_app(Z3_context ctx, Z3_func_decl f, Z3_ast x) {
	Z3_ast args[1] = { x };
	return Z3_mk_app(ctx, f, 1, args);
}

/*
 *
 *
 */
Z3_ast mk_2_arg_app(Z3_context ctx, Z3_func_decl f, Z3_ast x, Z3_ast y) {
	Z3_ast args[2] = { x, y };
	return Z3_mk_app(ctx, f, 2, args);
}

/*
 *
 *
 */
Z3_ast mk_3_arg_app(Z3_context ctx, Z3_func_decl f, Z3_ast x, Z3_ast y,
		Z3_ast z) {
	Z3_ast args[3] = { x, y, z };
	return Z3_mk_app(ctx, f, 3, args);
}

/*
 *
 */
Z3_ast mk_length(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	if (length_astNode_map.find(n) == length_astNode_map.end()) {
		if (isConstStr(t, n)) {
			length_astNode_map[n] = mk_int(ctx,
					getConstStrValue(t, n).length());
		} else {
			length_astNode_map[n] = mk_1_arg_app(ctx, td->Length, n);
		}
	}
	return length_astNode_map[n];
}

/*
 *
 */
Z3_ast mk_contains(Z3_theory t, Z3_ast n1, Z3_ast n2) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	std::pair<Z3_ast, Z3_ast> containsKey(n1, n2);
	if (contains_astNode_map.find(containsKey) == contains_astNode_map.end()) {
		if (isConstStr(t, n1) && isConstStr(t, n2)) {
			std::string n1Str = getConstStrValue(t, n1);
			std::string n2Str = getConstStrValue(t, n2);
			if (n1Str.find(n2Str) != std::string::npos)
				contains_astNode_map[containsKey] = Z3_mk_true(ctx);
			else
				contains_astNode_map[containsKey] = Z3_mk_false(ctx);
		} else {
			contains_astNode_map[containsKey] = mk_2_arg_app(ctx, td->Contains,
					n1, n2);
		}
	}
	return contains_astNode_map[containsKey];
}

/*
 *
 */
Z3_ast mk_concat(Z3_theory t, Z3_ast n1, Z3_ast n2) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	if (n1 == NULL || n2 == NULL) {
		fprintf(stdout, "> Error: the strings to be concat cannot be NULL (@ %d).\n", __LINE__);
		exit(0);
	} else {
		n1 = get_eqc_value(t, n1);
		n2 = get_eqc_value(t, n2);

		if (isConstStr(t, n1) && isConstStr(t, n2)) {
			return Concat(t, n1, n2);
		} else if (isConstStr(t, n1) && !isConstStr(t, n2)) {
			bool n2_isConcatFunc = isConcatFunc(t, n2);
			if (getConstStrValue(t, n1) == "") {
				return n2;
			}
			if (n2_isConcatFunc) {
				Z3_ast n2_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n2), 0);
				Z3_ast n2_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n2), 1);
				if (isConstStr(t, n2_arg0)) {
					n1 = Concat(t, n1, n2_arg0); // n1 will be a constant
					n2 = n2_arg1;
				}
			}
		} else if (!isConstStr(t, n1) && isConstStr(t, n2)) {
			if (getConstStrValue(t, n2) == "") {
				return n1;
			}

			if (isConcatFunc(t, n1)) {
				Z3_ast n1_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n1), 0);
				Z3_ast n1_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n1), 1);
				if (isConstStr(t, n1_arg1)) {
					n1 = n1_arg0;
					n2 = Concat(t, n1_arg1, n2); // n2 will be a constant
				}
			}
		} else {
			if (isConcatFunc(t, n1) && isConcatFunc(t, n2)) {
				Z3_ast n1_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n1), 0);
				Z3_ast n1_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n1), 1);
				Z3_ast n2_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n2), 0);
				Z3_ast n2_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n2), 1);
				if (isConstStr(t, n1_arg1) && isConstStr(t, n2_arg0)) {
					Z3_ast tmpN1 = n1_arg0;
					Z3_ast tmpN2 = Concat(t, n1_arg1, n2_arg0);
					n1 = mk_concat(t, tmpN1, tmpN2);
					n2 = n2_arg1;
				}
			}
			/* TODO */
			else if (isConcatFunc(t, n1)) {
				Z3_ast n1_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n1), 0);
				Z3_ast n1_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, n1), 1);
				if (isStarFunc(t, n1_arg0)) {
					n1 = n1_arg0;
					n2 = mk_concat(t, n1_arg1, n2);
				}
			}

		}

		//------------------------------------------------------
		// * Z3_ast ast1 = mk_2_arg_app(ctx, td->Concat, n1, n2);
		// * Z3_ast ast2 = mk_2_arg_app(ctx, td->Concat, n1, n2);
		// Z3 treats (ast1) and (ast2) as two different nodes.
		//-------------------------------------------------------
		std::pair<Z3_ast, Z3_ast> concatArgs(n1, n2);
		Z3_ast concatAst = NULL;
		if (concat_astNode_map.find(concatArgs) == concat_astNode_map.end()) {
			concatAst = mk_2_arg_app(ctx, td->Concat, n1, n2);
			//#ifdef DEBUGLOG
			//      __debugPrint(logFile, ">> make_concat: ");
			//      printZ3Node(t, concatAst);
			//      __debugPrint(logFile, "\n\n");
			//#endif
			concat_astNode_map[concatArgs] = concatAst;
			Z3_ast concat_length = mk_length(t, concatAst);
			Z3_ast n1_length = mk_length(t, n1);
			Z3_ast n2_length = mk_length(t, n2);
			Z3_ast addArg[2] = { n1_length, n2_length };
			Z3_ast lenAssert = Z3_mk_eq(ctx, concat_length,
					Z3_mk_add(ctx, 2, addArg));
			addAxiom(t, lenAssert, __LINE__, false);
			//      basicConcatAxiom(t, concatAst, __LINE__);
		} else {
			concatAst = concat_astNode_map[concatArgs];
		}
		return concatAst;
	}
}

Z3_ast mk_union(Z3_theory t, Z3_ast n1, Z3_ast n2) {
	
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);

	if (!n1 || !n2) {
		fprintf(stdout, "> Error: the parameters of UNION cannot be NULL (@ %d).\n", __LINE__);
		exit(0);
	} else {
		n1 = get_eqc_value(t, n1);
		n2 = get_eqc_value(t, n2);

		// TODO: x + x ~> x

		std::pair<Z3_ast, Z3_ast> unionArgs(n1, n2);
		Z3_ast unionAst = NULL;
		if (union_astNode_map.find(unionArgs) == union_astNode_map.end()) {
			unionAst = mk_2_arg_app(ctx, td->Union, n1, n2);
#ifdef DEBUGLOG
			__debugPrint(logFile, "[Union made] ");
			printZ3Node(t, unionAst);
			__debugPrint(logFile, "\n");
#endif
			union_astNode_map[unionArgs] = unionAst;
		} else {
			unionAst = union_astNode_map[unionArgs];
		}

		return unionAst;
	}
}

Z3_ast mk_star(Z3_theory t, Z3_ast n1) {
	
	if (!tempPatch) {
		tempPatch = true;
	}

	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);

	//    Z3_ast n2 = Z3_mk_fresh_const(ctx, "starlen", Z3_mk_int_sort(ctx));
	Z3_ast n2 = my_mk_internal_int_var(t);

	if (!n1 || !n2) {
		fprintf(stdout, "> Error: the parameters of STAR cannot be NULL (@ %d).\n", __LINE__);
		exit(0);
	} else {
		n1 = get_eqc_value(t, n1);

		if (isConstStr(t, n1)) {
			if (isConstNum(t, n2)) {
				std::string result = mk_num_of_str(getConstStrValue(t, n1),
						getConstNumValue(t, n2));
				return my_mk_str_value(t, result.c_str());
			}
			std::string str1 = getConstStrValue(t, n1);
			if (str1 == "") {
				return my_mk_str_value(t, str1.c_str());
			}
		}

		// TODO: n2=0

		std::pair<Z3_ast, Z3_ast> starArgs(n1, n2);
		Z3_ast starAst = NULL;
		if (star_astNode_map.find(starArgs) == star_astNode_map.end()) {
			starAst = mk_2_arg_app(ctx, td->Star_I, n1, n2);
#ifdef DEBUGLOG
			__debugPrint(logFile, "[Star made] ");
			printZ3Node(t, starAst);
			__debugPrint(logFile, "\n");
#endif
			// TODO: length
			star_astNode_map[starArgs] = starAst;
		} else {
			starAst = star_astNode_map[starArgs];
		}
		return starAst;
	}
}

/* Internal Representation of Indexof */
Z3_ast mk_idxof(Z3_theory t, Z3_ast n1, Z3_ast n2) {
	
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	
	if (!n1 || !n2) {
		fprintf(stdout, "> Error: the parameters of INDEX_OF cannot be NULL (@ %d).\n", __LINE__);
		exit(0);
	} else {

		// TODO: Simplified version and Length
		n1 = get_eqc_value(t, n1);
		n2 = get_eqc_value(t, n2);
		std::pair<Z3_ast, Z3_ast> idxofArgs(n1, n2);
		Z3_ast idxofAst = NULL;
		if (idxof_astNode_map.find(idxofArgs) == idxof_astNode_map.end()) {
			idxofAst = mk_2_arg_app(ctx, td->Indexof_I, n1, n2);
#ifdef DEBUGLOG
			__debugPrint(logFile, "[IndexOf made] ");
			printZ3Node(t, idxofAst);
			__debugPrint(logFile, "\n");
#endif
			idxof_astNode_map[idxofArgs] = idxofAst;
		} else {
			idxofAst = idxof_astNode_map[idxofArgs];
		}
		return idxofAst;
	}
}

Z3_ast mk_search(Z3_theory t, Z3_ast n1, Z3_ast n2) {
	
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);

	if (!n1 || !n2) {
		fprintf(stdout, "> Error: the parameters of SEARCH cannot be NULL (@ %d).\n", __LINE__);
		exit(0);
	} else {

		// TODO: Simplified version and Length
		n1 = get_eqc_value(t, n1);
		n2 = get_eqc_value(t, n2);
		std::pair<Z3_ast, Z3_ast> searchArgs(n1, n2);
		Z3_ast searchAst = NULL;
		if (search_astNode_map.find(searchArgs) == search_astNode_map.end()) {
			searchAst = mk_2_arg_app(ctx, td->Search, n1, n2);
#ifdef DEBUGLOG
			__debugPrint(logFile, "[Search made] ");
			printZ3Node(t, searchAst);
			__debugPrint(logFile, "\n");
#endif
			search_astNode_map[searchArgs] = searchAst;
		} else {
			searchAst = search_astNode_map[searchArgs];
		}
		return searchAst;
	}
}

Z3_ast mk_replaceAll(Z3_theory t, Z3_ast arg0, Z3_ast arg1, Z3_ast arg2) {
	
	if (!tempPatch) {
		tempPatch = true;
	}

	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	
	Z3_ast n1 = get_eqc_value(t, arg0);
	Z3_ast n2 = get_eqc_value(t, arg1);
	Z3_ast n3 = get_eqc_value(t, arg2);

	// TODO: Simplified version and Length

	if (isConstStr(t, n1) && isConstStr(t, n2) && isConstStr(t, n3)) {
		std::string arg0Str = getConstStrValue(t, n1);
		std::string arg1Str = getConstStrValue(t, n2);
		std::string arg2Str = getConstStrValue(t, n3);
		std::regex reg(arg1Str);
		std::string res = std::regex_replace(arg0Str, reg, arg2Str);
		__debugPrint(logFile, "arg0: %s\n", arg0Str.c_str());
		__debugPrint(logFile, "arg1: %s\n", arg1Str.c_str());
		__debugPrint(logFile, "arg2: %s\n", arg2Str.c_str());
		__debugPrint(logFile, "Result: %s\n", res.c_str());
		__debugPrint(logFile, "=============================\n");
		return my_mk_str_value(t, res.c_str());
	}

	std::tuple<Z3_ast, Z3_ast, Z3_ast> repAllArgs(n1, n2, n3);

	Z3_ast repAllAst = nullptr;

	if (repAll_astNode_map.find(repAllArgs) == repAll_astNode_map.end()) {
		repAllAst = mk_3_arg_app(ctx, td->ReplaceAll, n1, n2, n3);
		repAll_astNode_map[repAllArgs] = repAllAst;
	} else {
		repAllAst = repAll_astNode_map[repAllArgs];
	}

	return repAllAst;
}

Z3_ast mk_subStr(Z3_theory t, Z3_ast const args[]) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_ast n1 = get_eqc_value(t, args[0]);
	Z3_ast n2 = args[1];
	Z3_ast n3 = args[2];
	// TODO: Simplified version and Length

	if (isConstStr(t, n1) && isConstNum(t, n2) && isConstNum(t, n3)) {
		std::string arg0Str = getConstStrValue(t, n1);
		int arg1Num = getConstNumValue(t, n2);
		int arg2Num = getConstNumValue(t, n3);
		std::string res = arg0Str.substr(arg1Num, arg2Num);
		return my_mk_str_value(t, res.c_str());
	}

	std::tuple<Z3_ast, Z3_ast, Z3_ast> subStrArgs(n1, n2, n3);
	Z3_ast subStrAst = NULL;

	if (subStr_astNode_map.find(subStrArgs) == subStr_astNode_map.end()) {
		subStrAst = mk_3_arg_app(ctx, td->SubString, n1, n2, n3);
		subStr_astNode_map[subStrArgs] = subStrAst;
	} else {
		subStrAst = subStr_astNode_map[subStrArgs];
	}
	return subStrAst;
}

std::string mk_num_of_str(std::string s, int n) {
	if (n == 0) {
		return "";
	} else {
		return s + mk_num_of_str(s, n - 1);
	}
}

/*
 *
 */
bool isTwoStrEqual(std::string str1, std::string str2) {
	return (str1 == str2);
}

/*
 *
 */
void readdAxiom(Z3_theory t, Z3_ast toAssert, int line, bool display) {
#ifdef DEBUGLOG
	if (display) {
		if (searchStart == 1) {
			__debugPrint(logFile, "---------------------\nAxiom ReAdd(@%d, Level %d):\n", line, sLevel);
			printZ3Node(t, toAssert);
			__debugPrint(logFile, "\n---------------------\n\n");
		} else {
			__debugPrint(logFile, "---------------------\nAssertion ReAdd(@%d, Level %d):\n", line, sLevel);
			printZ3Node(t, toAssert);
			__debugPrint(logFile, "\n---------------------\n\n");
		}
	}
#endif

	if (toAssert && searchStart == 1) {
		Z3_theory_assert_axiom(t, toAssert, true);
	}
}

/*
 *
 */
void addAxiom(Z3_theory t, Z3_ast toAssert, int line, bool display) {
#ifdef DEBUGLOG
	if (display) {
		if (searchStart == 1) {
			__debugPrint(logFile, "---------------------\nAxiom Add(@%d, Level %d):\n", line, sLevel);
			printZ3Node(t, toAssert);
			__debugPrint(logFile, "\n---------------------\n\n");
		} else {
			__debugPrint(logFile, "---------------------\nAssertion Add(@%d, Level %d):\n", line, sLevel);
			printZ3Node(t, toAssert);
			__debugPrint(logFile, "\n---------------------\n\n");
		}
	}
#endif

	if (toAssert == NULL) {
		return;
	}

	if (searchStart == 1) {
		Z3_theory_assert_axiom(t, toAssert);
	} else {
		Z3_context ctx = Z3_theory_get_context(t);
		Z3_assert_cnstr(ctx, toAssert);
	}
}

/*
 *
 */
void print_eq_class(Z3_theory t, Z3_ast n) {
#ifdef DEBUGLOG
	__debugPrint(logFile, " EQC={ ");
	Z3_ast curr = n;
	int count = 0;
	do {
		if (count != 0) {
			__debugPrint(logFile, ", ");
		}
		printZ3Node(t, curr);
		curr = Z3_theory_get_eqc_next(t, curr);
		count++;
	}while (curr != n);
	__debugPrint(logFile, " }");
#endif
}

Z3_string my_node_to_string(Z3_theory t, Z3_ast node) {

	Z3_context ctx = Z3_theory_get_context(t);

	if (node) {
		T_myZ3Type nodeType = (T_myZ3Type) getNodeType(t, node);

		char const* result;

		if (nodeType == my_Z3_ConstStr) {
			//TODO: This leaks
			auto strV = getConstStrValue(t, node);
			char* rm = new char[strV.length() + 1];
			strcpy(rm, strV.c_str());
			result = rm;
		} else {
			result = Z3_ast_to_string(ctx, node);
		}

		return result;
	} else {
		return nullptr;
	}
}

/*
 *
 */
void __printZ3Node(Z3_theory t, Z3_ast node) {
#ifdef DEBUGLOG
	Z3_context ctx = Z3_theory_get_context(t);
	if (node == NULL) {
		__debugPrint(logFile, "NULL");
		return;
	}

	T_myZ3Type nodeType = (T_myZ3Type) getNodeType(t, node);
	switch (nodeType) {
		case my_Z3_ConstStr: {
			std::string str = getConstStrValue(t, node);
			__debugPrint(logFile, "\"%s\"", str.c_str());
			break;
		}
		case my_Z3_Func: {
			__debugPrint(logFile, "%s", Z3_ast_to_string(ctx, node));
			break;
		}
		case my_Z3_Num: {
			__debugPrint(logFile, "%s", Z3_ast_to_string(ctx, node));
			break;
		}
		case my_Z3_Var: {
			__debugPrint(logFile, "%s", Z3_ast_to_string(ctx, node));
			break;
		}
		case my_Z3_Str_Var: {
			__debugPrint(logFile, "%s", Z3_ast_to_string(ctx, node));
			break;
		}
		case my_Z3_Quantifier: {
			__debugPrint(logFile, "%s", Z3_ast_to_string(ctx, node));
			break;
		}
		case my_Z3_Unknown: {
			__debugPrint(logFile, "%s", Z3_ast_to_string(ctx, node));
			break;
		}
		default: {
			__debugPrint(logFile, "%s", Z3_ast_to_string(ctx, node));
			break;
		}
	}
#endif
}

/*
 * Look for the equivalent constant for a node "n"
 * Iterate the equivalence class
 * If there is a constant,
 *    return the constant
 * Otherwise,
 *    return n
 */
Z3_ast get_eqc_value(Z3_theory t, Z3_ast n) {
	Z3_ast curr = n;
	do {
		if (Z3_theory_is_value(t, curr)) {
			if (isConstStr(t, curr))
				return curr;
		}
		curr = Z3_theory_get_eqc_next(t, curr);
	} while (curr != n);
	return n;
}

Z3_ast get_eqc_value_or_star(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast curr = n;
	Z3_ast star = NULL;
	do {
		__debugPrint(logFile, "curr = ");
		printZ3Node(t, curr);
		__debugPrint(logFile, "\n");
		if (Z3_theory_is_value(t, curr)) {
			T_myZ3Type typ = getNodeTypeCustom(t, curr);
			if (typ == my_Z3_ConstStr)
				return curr;
			else if (typ == my_Z3_Star && star == NULL) {
				star = curr;
			}
			// TODO:
			else if (typ == my_Z3_Concat && star == NULL) {
				Z3_ast arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, curr), 0);
				if (isStarFunc(t, arg0))
					star = curr;
			}
		}
		curr = Z3_theory_get_eqc_next(t, curr);
	} while (curr != n);

	if (star != NULL)
		return star;
	return n;
}

Z3_ast get_eqc_num(Z3_theory t, Z3_ast n, int debug) {
	//    printf ("DEBUG: %d\n", debug);
	//    Z3_context ctx = Z3_theory_get_context(t);
	//    printf("DEBUG (outside): %s\n", Z3_ast_to_string(ctx,n));
	return Z3_theory_get_value_of_len(t, n);
}

/*
 *
 */
inline bool isConcatFunc(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_func_decl d = Z3_get_app_decl(ctx, Z3_to_app(ctx, n));
	if (d == td->Concat)
		return true;
	else
		return false;
}

inline bool isStarFunc(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_func_decl d = Z3_get_app_decl(ctx, Z3_to_app(ctx, n));
	if (d == td->Star_I)
		return true;
	else
		return false;
}

inline bool isUnionFunc(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_func_decl d = Z3_get_app_decl(ctx, Z3_to_app(ctx, n));
	if (d == td->Union)
		return true;
	else
		return false;
}

inline bool isSearchFunc(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_func_decl d = Z3_get_app_decl(ctx, Z3_to_app(ctx, n));
	if (d == td->Search)
		return true;
	else
		return false;
}

inline bool isReplaceAllFunc(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_func_decl d = Z3_get_app_decl(ctx, Z3_to_app(ctx, n));
	if (d == td->ReplaceAll)
		return true;
	else
		return false;
}

/*
 *
 */
bool isStrInt(std::string & strValue) {
	bool isNum = true;
	if (strValue == "") {
		isNum = false;
	} else {
		std::string::iterator it = strValue.begin();
		if (*it == '-')
			++it;
		while (it != strValue.end()) {
			if (!std::isdigit(*it)) {
				isNum = false;
				break;
			}
			++it;
		}
	}
	return isNum;
}

/*
 *
 */
std::string getConstStrValue(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	std::string strValue;
	if (isConstStr(t, n)) {
		char * str = (char *) Z3_ast_to_string(ctx, n);
		if (strcmp(str, "\"\"") == 0)
			strValue = std::string("");
		else
			strValue = std::string(str);
	} else {
		strValue == std::string("__NotConstStr__");
	}
	return strValue;
}

int getConstNumValue(Z3_theory t, Z3_ast n) {
	Z3_context ctx = Z3_theory_get_context(t);
	int intValue = -1;
	if (isConstNum(t, n)) {
		char * str = (char *) Z3_ast_to_string(ctx, n);
		intValue = atoi(str);
	}
	return intValue;
}

/*
 *
 */
Z3_ast Concat(Z3_theory t, Z3_ast n1, Z3_ast n2) {
	Z3_ast v1 = get_eqc_value(t, n1);
	Z3_ast v2 = get_eqc_value(t, n2);
	if (isConstStr(t, v1) && isConstStr(t, v2)) {
		std::string n1_str = getConstStrValue(t, v1);
		std::string n2_str = getConstStrValue(t, v2);
		std::string result = n1_str + n2_str;
		return my_mk_str_value(t, result.c_str());
	} else if (isConstStr(t, v1) && !isConstStr(t, v2)) {
		if (getConstStrValue(t, v1) == "")
			return n2;
	} else if (!isConstStr(t, v1) && isConstStr(t, v2)) {
		if (getConstStrValue(t, v2) == "")
			return n1;
	}
	return NULL;
}

/*
 * The inputs:
 *    ~ nn: non const node
 *    ~ eq_str: the equivalent constant string of nn
 *  Iterate the parent of all eqc nodes of nn, looking for:
 *    ~ concat node
 *  to see whether some concat nodes can be simplified.
 */
void simplifyConcatStr(Z3_theory t, Z3_ast nn, Z3_ast eq_str) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast n_eqNode = nn;
	std::string eq_strValue = getConstStrValue(t, eq_str);
	do {
		unsigned num_parents = Z3_theory_get_num_parents(t, n_eqNode);
		for (unsigned i = 0; i < num_parents; i++) {
			Z3_ast parent = Z3_theory_get_parent(t, n_eqNode, i);

			if (isConcatFunc(t, parent)) {
				Z3_ast arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, parent), 0);
				Z3_ast arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, parent), 1);
				if (arg0 == n_eqNode) {
#ifdef DEBUGLOG
					__debugPrint(logFile, ">> [simplifyConcatStr 1 @ %d] ", __LINE__);
					printZ3Node(t, parent);
					__debugPrint(logFile, "\n\n");
#endif
					// (Concat n_eqNode arg1) /\ arg1 has eq const
					Z3_ast concatResult = Concat(t, eq_str, arg1);
					if (concatResult != NULL) {
						Z3_ast arg1Value = get_eqc_value(t, arg1);
						Z3_ast implyL = NULL;
						if (arg1Value != arg1) {
							Z3_ast eq_ast1 = Z3_mk_eq(ctx, n_eqNode, eq_str);
							Z3_ast eq_ast2 = Z3_mk_eq(ctx, arg1, arg1Value);
							implyL = mk_2_and(t, eq_ast1, eq_ast2);
						} else {
							implyL = Z3_mk_eq(ctx, n_eqNode, eq_str);
						}

						if (!inSameEqc(t, parent, concatResult)) {
							Z3_ast implyR = Z3_mk_eq(ctx, parent, concatResult);
							Z3_ast implyToAssert = Z3_mk_implies(ctx, implyL,
									implyR);
							addAxiom(t, implyToAssert, __LINE__);
						}
					} else if (isConcatFunc(t, n_eqNode)) {
						Z3_ast simpleConcat = mk_concat(t, eq_str, arg1);
						if (!inSameEqc(t, parent, simpleConcat)) {
							Z3_ast implyL = Z3_mk_eq(ctx, n_eqNode, eq_str);
							Z3_ast implyR = Z3_mk_eq(ctx, parent, simpleConcat);
							Z3_ast implyToAssert = Z3_mk_implies(ctx, implyL,
									implyR);
							addAxiom(t, implyToAssert, __LINE__);
						}
					}
				}

				if (arg1 == n_eqNode) {
#ifdef DEBUGLOG
					__debugPrint(logFile, ">> [simplifyConcatStr 2 @ %d] ", __LINE__);
					printZ3Node(t, parent);
					__debugPrint(logFile, "\n\n");
#endif
					// (Concat arg0 n_eqNode) /\ arg0 has eq const
					Z3_ast concatResult = Concat(t, arg0, eq_str);
					if (concatResult != NULL) {
						Z3_ast arg0Value = get_eqc_value(t, arg0);
						Z3_ast implyL = NULL;
						if (arg0Value != arg0) {
							Z3_ast eq_ast1 = Z3_mk_eq(ctx, arg0, arg0Value);
							Z3_ast eq_ast2 = Z3_mk_eq(ctx, n_eqNode, eq_str);
							implyL = mk_2_and(t, eq_ast1, eq_ast2);
						} else {
							implyL = Z3_mk_eq(ctx, n_eqNode, eq_str);
						}

						if (!inSameEqc(t, parent, concatResult)) {
							Z3_ast implyR = Z3_mk_eq(ctx, parent, concatResult);
							Z3_ast implyToAssert = Z3_mk_implies(ctx, implyL,
									implyR);
							addAxiom(t, implyToAssert, __LINE__);
						}
					}

					else if (isConcatFunc(t, n_eqNode)) {
						Z3_ast simpleConcat = mk_concat(t, arg0, eq_str);
						if (!inSameEqc(t, parent, simpleConcat)) {
							Z3_ast implyL = Z3_mk_eq(ctx, n_eqNode, eq_str);
							Z3_ast implyR = Z3_mk_eq(ctx, parent, simpleConcat);
							Z3_ast implyToAssert = Z3_mk_implies(ctx, implyL,
									implyR);
							addAxiom(t, implyToAssert, __LINE__);
						}
					}
				}

				//---------------------------------------------------------
				// Case (2-1) begin: (Concat n_eqNode (Concat str var))
				if (arg0 == n_eqNode && isConcatFunc(t, arg1)) {
#ifdef DEBUGLOG
					__debugPrint(logFile, ">> [simplifyConcatStr 3 @ %d] ", __LINE__);
					printZ3Node(t, parent);
					__debugPrint(logFile, "\n\n");
#endif
					Z3_ast r_concat_arg0 = Z3_get_app_arg(ctx,
							Z3_to_app(ctx, arg1), 0);
					if (isConstStr(t, r_concat_arg0)) {
						Z3_ast combined_str = Concat(t, eq_str, r_concat_arg0);
						Z3_ast r_concat_arg1 = Z3_get_app_arg(ctx,
								Z3_to_app(ctx, arg1), 1);
						Z3_ast implyL = Z3_mk_eq(ctx, n_eqNode, eq_str);
						Z3_ast simplifiedAst = mk_concat(t, combined_str,
								r_concat_arg1);

						if (!inSameEqc(t, parent, simplifiedAst)) {
							Z3_ast implyR = Z3_mk_eq(ctx, parent,
									simplifiedAst);
							Z3_ast implyToAssert = Z3_mk_implies(ctx, implyL,
									implyR);
							addAxiom(t, implyToAssert, __LINE__);
						}
					}
				}
				// Case (2-1) end: (Concat n_eqNode (Concat str var))
				//---------------------------------------------------------

				//---------------------------------------------------------
				// Case (2-2) begin: (Concat (Concat var str) n_eqNode)
				if (isConcatFunc(t, arg0) && arg1 == n_eqNode) {
#ifdef DEBUGLOG
					__debugPrint(logFile, ">> [simplifyConcatStr 4 @ %d] ", __LINE__);
					printZ3Node(t, parent);
					__debugPrint(logFile, "\n\n");
#endif
					Z3_ast l_concat_arg1 = Z3_get_app_arg(ctx,
							Z3_to_app(ctx, arg0), 1);
					if (isConstStr(t, l_concat_arg1)) {
						Z3_ast combined_str = Concat(t, l_concat_arg1, eq_str);
						Z3_ast l_concat_arg0 = Z3_get_app_arg(ctx,
								Z3_to_app(ctx, arg0), 0);
						Z3_ast implyL = Z3_mk_eq(ctx, n_eqNode, eq_str);
						Z3_ast simplifiedAst = mk_concat(t, l_concat_arg0,
								combined_str);

						if (!inSameEqc(t, parent, simplifiedAst)) {
							Z3_ast implyR = Z3_mk_eq(ctx, parent,
									simplifiedAst);
							Z3_ast implyToAssert = Z3_mk_implies(ctx, implyL,
									implyR);
							addAxiom(t, implyToAssert, __LINE__);
						}
					}
				}
				// Case (2-2) end: (Concat (Concat var str) n_eqNode)
				//---------------------------------------------------------

				// Have to look up one more layer: if the parent of the concat is another concat
				//-------------------------------------------------
				// Case (3-1) begin: (Concat (Concat var n_eqNode) str )
				if (arg1 == n_eqNode) {
					int concat_parent_num = Z3_theory_get_num_parents(t,
							parent);
					for (int j = 0; j < concat_parent_num; j++) {
						Z3_ast concat_parent = Z3_theory_get_parent(t, parent,
								j);
						if (isConcatFunc(t, concat_parent)) {
							Z3_ast concat_parent_arg0 = Z3_get_app_arg(ctx,
									Z3_to_app(ctx, concat_parent), 0);
							Z3_ast concat_parent_arg1 = Z3_get_app_arg(ctx,
									Z3_to_app(ctx, concat_parent), 1);
							if (concat_parent_arg0 == parent
									&& isConstStr(t, concat_parent_arg1)) {
#ifdef DEBUGLOG
								__debugPrint(logFile, "\n\n>> [simplifyConcatStr 5 @ %d] ", __LINE__);
								printZ3Node(t, concat_parent);
								__debugPrint(logFile, "\n");
#endif
								Z3_ast combinedStr = Concat(t, eq_str,
										concat_parent_arg1);
								Z3_ast implyL = Z3_mk_eq(ctx, n_eqNode, eq_str);
								Z3_ast simplifiedAst = mk_concat(t, arg0,
										combinedStr);

								if (!inSameEqc(t, concat_parent,
										simplifiedAst)) {
									Z3_ast implyR = Z3_mk_eq(ctx, concat_parent,
											simplifiedAst);
									Z3_ast implyToAssert = Z3_mk_implies(ctx,
											implyL, implyR);
									addAxiom(t, implyToAssert, __LINE__);
								}
							}
						}
					}
				}
				// Case (3-1) end: (Concat (Concat var n_eqNode) str )

				// Case (3-2) begin: (Concat str (Concat n_eqNode var) )
				if (arg0 == n_eqNode) {
					int concat_parent_num = Z3_theory_get_num_parents(t,
							parent);
					for (int j = 0; j < concat_parent_num; j++) {
						Z3_ast concat_parent = Z3_theory_get_parent(t, parent,
								j);
						if (isConcatFunc(t, concat_parent)) {
							Z3_app parent_app = Z3_to_app(ctx, concat_parent);
							Z3_ast concat_parent_arg0 = Z3_get_app_arg(ctx,
									parent_app, 0);
							Z3_ast concat_parent_arg1 = Z3_get_app_arg(ctx,
									parent_app, 1);
							if (concat_parent_arg1 == parent
									&& isConstStr(t, concat_parent_arg0)) {
#ifdef DEBUGLOG
								__debugPrint(logFile, ">> [simplifyConcatStr 6 @ %d] ", __LINE__);
								printZ3Node(t, concat_parent);
								__debugPrint(logFile, "\n\n");
#endif
								Z3_ast combinedStr = Concat(t,
										concat_parent_arg0, eq_str);
								Z3_ast implyL = Z3_mk_eq(ctx, n_eqNode, eq_str);
								Z3_ast simplifiedAst = mk_concat(t, combinedStr,
										arg1);

								if (!inSameEqc(t, concat_parent,
										simplifiedAst)) {
									Z3_ast implyR = Z3_mk_eq(ctx, concat_parent,
											simplifiedAst);
									Z3_ast implyToAssert = Z3_mk_implies(ctx,
											implyL, implyR);
									addAxiom(t, implyToAssert, __LINE__);
								}
							}
						}
					}
				}
				// Case (3-2) end: (Concat str (Concat n_eqNode var) )
			}
		}
		n_eqNode = Z3_theory_get_eqc_next(t, n_eqNode);
	} while (n_eqNode != nn);
}

/*
 * Check whether Concat(a, b) can equal to a constant string
 */
int canConcatEqStr(Z3_theory t, Z3_ast concat, std::string str) {
	int strLen = str.length();
	if (isConcatFunc(t, concat)) {
		Z3_ast ml_node = getMostLeftNodeInConcat(t, concat);
		if (isConstStr(t, ml_node)) {
			std::string ml_str = getConstStrValue(t, ml_node);
			int ml_len = ml_str.length();
			if (ml_len > strLen)
				return 0;
			int cLen = ml_len;
			if (ml_str != str.substr(0, cLen))
				return 0;
		}

		Z3_ast mr_node = getMostRightNodeInConcat(t, concat);
		if (isConstStr(t, mr_node)) {
			std::string mr_str = getConstStrValue(t, mr_node);
			int mr_len = mr_str.length();
			if (mr_len > strLen)
				return 0;
			int cLen = mr_len;
			if (mr_str != str.substr(strLen - cLen, cLen))
				return 0;
		}
	}
	return 1;
}

/*
 * For two concats "assumed" to be equal by Z3, before having their concrete values:
 * Check whether the two concat can be equal
 */
int canConcatEqConcat(Z3_theory t, Z3_ast concat1, Z3_ast concat2) {
	// make sure left and right are concat functions
	if (isConcatFunc(t, concat1) && isConcatFunc(t, concat2)) {
		{
			// Suppose concat1 = concat(x, y), concat2 = concat(m, n)
			Z3_ast concat1_mostL = getMostLeftNodeInConcat(t, concat1);
			Z3_ast concat2_mostL = getMostLeftNodeInConcat(t, concat2);
			// if both x and m are const strings, check whether they have the same prefix
			if (isConstStr(t, concat1_mostL) && isConstStr(t, concat2_mostL)) {

				std::string concat1_mostL_str = getConstStrValue(t,
						concat1_mostL);
				std::string concat2_mostL_str = getConstStrValue(t,
						concat2_mostL);
				int cLen = std::min(concat1_mostL_str.length(),
						concat2_mostL_str.length());
				if (concat1_mostL_str.substr(0, cLen)
						!= concat2_mostL_str.substr(0, cLen)) {
					return 0;
				}
			}
		}

		{
			Z3_ast concat1_mostR = getMostRightNodeInConcat(t, concat1);
			Z3_ast concat2_mostR = getMostRightNodeInConcat(t, concat2);
			// if both m and n are const strings, check whether they have the same suffix
			if (isConstStr(t, concat1_mostR) && isConstStr(t, concat2_mostR)) {
				std::string concat1_mostR_str = getConstStrValue(t,
						concat1_mostR);
				std::string concat2_mostR_str = getConstStrValue(t,
						concat2_mostR);
				int cLen = std::min(concat1_mostR_str.length(),
						concat2_mostR_str.length());
				if (concat1_mostR_str.substr(concat1_mostR_str.length() - cLen,
						cLen)
						!= concat2_mostR_str.substr(
								concat2_mostR_str.length() - cLen, cLen)) {
					return 0;
				}
			}
		}
	}
	return 1;
}

/*
 * Decide whether two n1 and n2 are ALREADY in a same eq class
 * Or n1 and n2 are ALREADY treated equal by the core
 * BUT, they may or may not be really equal
 */
bool inSameEqc(Z3_theory t, Z3_ast n1, Z3_ast n2) {
	if (n1 == n2)
		return true;

	Z3_ast curr = Z3_theory_get_eqc_next(t, n1);
	while (curr != n1) {
		if (curr == n2)
			return true;
		curr = Z3_theory_get_eqc_next(t, curr);
	}
	return false;
}

/*
 *
 */
bool canTwoNodesEq(Z3_theory t, Z3_ast n1, Z3_ast n2) {
	Z3_ast n1_curr = n1;
	Z3_ast n2_curr = n2;

	// case 0: n1_curr is const string, n2_curr is const string
	if (isConstStr(t, n1_curr) && isConstStr(t, n2_curr)) {
		if (n1_curr != n2_curr) {
			return false;
		}
	}
	// case 1: n1_curr is concat, n2_curr is const string
	else if (isConcatFunc(t, n1_curr) && isConstStr(t, n2_curr)) {
		std::string n2_curr_str = getConstStrValue(t, n2_curr);
		if (canConcatEqStr(t, n1_curr, n2_curr_str) != 1) {
			return false;
		}
	}
	// case 2: n2_curr is concat, n1_curr is const string
	else if (isConcatFunc(t, n2_curr) && isConstStr(t, n1_curr)) {
		std::string n1_curr_str = getConstStrValue(t, n1_curr);
		if (canConcatEqStr(t, n2_curr, n1_curr_str) != 1) {
			return false;
		}
	} else if (isConcatFunc(t, n1_curr) && isConcatFunc(t, n2_curr)) {
		if (canConcatEqConcat(t, n1_curr, n2_curr) != 1) {
			return false;
		}
	}

	return true;
}

bool isMemStrList(std::string str, std::list<std::string> lstr) {
	std::list<std::string>::iterator itor = lstr.begin();
	for (; itor != lstr.end(); itor++) {
		if (str == *itor)
			return true;
	}
	return false;
}

bool isConsumedAll(std::string beconsumed, std::list<std::string> toconsume,
		int toconsumelen) {
	int beconsumedlen = beconsumed.length();
	std::string rem = beconsumed;

	while (toconsumelen <= beconsumedlen) {
		int remStrLen = beconsumedlen - toconsumelen;
		std::string firstPart = beconsumed.substr(0, toconsumelen);
		std::string secondPart = beconsumed.substr(toconsumelen, remStrLen);
		if (!isMemStrList(firstPart, toconsume))
			return false;
		else {
			rem = secondPart;
			beconsumed = rem;
			beconsumedlen = remStrLen;
		}
	}
	if (beconsumedlen == 0)
		return true;
	return false;
}

void solve_star_eq_str(Z3_theory t, Z3_ast starAst, Z3_ast constStr) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "** solve_star_eq_str: ");
	printZ3Node(t, starAst);
	__debugPrint(logFile, " = ");
	printZ3Node(t, constStr);
	__debugPrint(logFile, "\n");
#endif
	Z3_context ctx = Z3_theory_get_context(t);
	if (isStarFunc(t, starAst) && isConstStr(t, constStr)) {
		Z3_ast a1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, starAst), 0);
		Z3_ast a2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, starAst), 1);
		Z3_ast arg1 = get_eqc_value(t, a1);
		Z3_ast arg2 = a2;
		// TODO: if arg2 is int

		Z3_ast orig = Z3_mk_eq(ctx, starAst, constStr);

		// TODO: nested star functions
		if (isConstStr(t, arg1)) {
			std::string const_str = getConstStrValue(t, constStr);
			int constStrLen = const_str.length();
			std::string arg1_str = getConstStrValue(t, arg1);
			int arg1StrLen = arg1_str.length();
			if (constStrLen % arg1StrLen != 0) {
				// negate
				addAxiom(t, Z3_mk_not(ctx, orig), __LINE__);
				return;
			} else {
				int num_repeats = constStrLen / arg1StrLen;
				std::string res = mk_num_of_str(arg1_str, num_repeats);
				if (!isTwoStrEqual(res, const_str)) {
					// negate
					addAxiom(t, Z3_mk_not(ctx, orig), __LINE__);
					return;
				} else {
					// TODO: is that needed?
					addAxiom(t, Z3_mk_eq(ctx, arg2, mk_int(ctx, num_repeats)),
					__LINE__);
					return;
				}
			}
		} else if (isUnionFunc(t, arg1)) {
			Z3_ast au1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, arg1), 0);
			Z3_ast au2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, arg1), 1);
			Z3_ast argu1 = get_eqc_value(t, au1);
			Z3_ast argu2 = get_eqc_value(t, au2);
			// TODO: other cases: star, union of > 2 elements, ...
			if (isConstStr(t, argu1) && isConstStr(t, argu2)) {
				std::string argu1_str = getConstStrValue(t, argu1);
				std::string argu2_str = getConstStrValue(t, argu2);
				unsigned argu1StrLen = argu1_str.length();
				// TODO: other cases
				if (argu1StrLen == argu2_str.length()) {
					std::string const_str = getConstStrValue(t, constStr);
					unsigned constStrLen = const_str.length();
					if (constStrLen % argu1StrLen != 0) {
						// negate
						addAxiom(t, Z3_mk_not(ctx, orig), __LINE__);
						return;
					} else {
						int num_repeats = constStrLen / argu1StrLen;
						std::list < std::string > sl;
						sl.push_back(argu1_str);
						sl.push_back(argu2_str);
						if (!isConsumedAll(const_str, sl, argu1StrLen)) {
							// negate
							addAxiom(t, Z3_mk_not(ctx, orig), __LINE__);
							return;
						} else {
							// TODO: is that needed?
							addAxiom(t,
									Z3_mk_eq(ctx, arg2,
											mk_int(ctx, num_repeats)),
									__LINE__);
							return;
						}
					}
				}
			}
		}
	}
}

void solve_star_eq_var(Z3_theory t, Z3_ast starAst, Z3_ast strVar) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "** solve_star_eq_var: ");
	printZ3Node(t, starAst);
	__debugPrint(logFile, " = ");
	printZ3Node(t, strVar);
	__debugPrint(logFile, "\n");
#endif
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast a1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, starAst), 0);
	Z3_ast a2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, starAst), 1);
	Z3_ast arg1 = get_eqc_value(t, a1);
	Z3_ast arg2 = a2;

	Z3_ast orig = Z3_mk_eq(ctx, starAst, strVar);

	Z3_ast strVarLen = mk_length(t, strVar);

	Z3_ast constLen = get_eqc_num(t, strVarLen, 14);
	if (isConstNum(t, constLen) && isConstStr(t, arg1)) {
		int constStrLen = getConstNumValue(t, constLen);
		std::string arg1_str = getConstStrValue(t, arg1);
		int arg1StrLen = arg1_str.length();
		if (constStrLen % arg1StrLen != 0) {
			// negate
			addAxiom(t, Z3_mk_not(ctx, orig), __LINE__);
			return;
		} else {
			int num_repeats = constStrLen / arg1StrLen;
			std::string res = mk_num_of_str(arg1_str, num_repeats);
			Z3_ast and_items[2];
			and_items[0] = Z3_mk_eq(ctx, arg2, mk_int(ctx, num_repeats));
			and_items[1] = Z3_mk_eq(ctx, strVar,
					my_mk_str_value(t, res.c_str()));
			addAxiom(t, Z3_mk_and(ctx, 2, and_items), __LINE__);
			return;
		}
	}
}

void solve_search_eq_sth(Z3_theory t, Z3_ast searchAst, Z3_ast intVar) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "** solve_search_eq_sth: ");
	printZ3Node(t, searchAst);
	__debugPrint(logFile, " = ");
	printZ3Node(t, intVar);
	__debugPrint(logFile, "\n");
#endif
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast a1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, searchAst), 0);
	Z3_ast arg1 = get_eqc_value(t, a1);
	Z3_ast arg2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, searchAst), 1);

	if (isSearchFunc(t, searchAst)) {
		Z3_ast and_items1[2], and_items2[6];
		and_items1[0] = Z3_mk_gt(ctx, mk_int(ctx, 0), intVar);
		PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
		Z3_ast arb_char = Z3_mk_app(ctx, td->Arb, 0, 0);
		Z3_ast reg = mk_concat(t, mk_concat(t, mk_star(t, arb_char), arg2),
				arb_char);
		and_items1[1] = Z3_mk_not(ctx, reduce_In(t, arg1, reg));

		and_items2[0] = Z3_mk_ge(ctx, intVar, mk_int(ctx, 0));
		Z3_ast unmatch = my_mk_internal_string_var(t);
		Z3_ast match1 = my_mk_internal_string_var(t);
		Z3_ast match2 = my_mk_internal_string_var(t);
		Z3_ast rem = my_mk_internal_string_var(t);
		and_items2[1] = Z3_mk_eq(ctx, arg1,
				mk_concat(t,
						mk_concat(t, unmatch, mk_concat(t, match1, match2)),
						rem));
		and_items2[2] = reduce_In(t, mk_concat(t, match1, match2), arg2);
		and_items2[3] = Z3_mk_eq(ctx, mk_length(t, unmatch), intVar);
		and_items2[4] = Z3_mk_eq(ctx, mk_length(t, match2), mk_int(ctx, 1));
		and_items2[5] = Z3_mk_not(ctx,
				reduce_In(t, mk_concat(t, unmatch, match1), reg));

		Z3_ast or_items[2];
		or_items[0] = Z3_mk_and(ctx, 2, and_items1);
		or_items[1] = Z3_mk_and(ctx, 6, and_items2);

		addAxiom(t, Z3_mk_or(ctx, 2, or_items), __LINE__);
	}
}

void solve_replaceall_eq_sth(Z3_theory t, Z3_ast replaceAllAst, Z3_ast res) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "** solve_replaceall_eq_sth: ");
	printZ3Node(t, replaceAllAst);
	__debugPrint(logFile, " = ");
	printZ3Node(t, res);
	__debugPrint(logFile, "\n");
#endif

	if (isConcatFunc(t, res)) {
		return;
	}

	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast a1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, replaceAllAst), 0);
	Z3_ast arg1 = get_eqc_value(t, a1);
	Z3_ast arg2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, replaceAllAst), 1);
	Z3_ast a3 = Z3_get_app_arg(ctx, Z3_to_app(ctx, replaceAllAst), 2);
	Z3_ast arg3 = get_eqc_value(t, a3);

	if (arg1 == res) {
		return;
	}

	if (isConstStr(t, res) && isConstStr(t, arg3)) {
		if (getConstStrValue(t, res) == "" && getConstStrValue(t, arg3) != "") {
			addAxiom(t, Z3_mk_eq(ctx, arg1, my_mk_str_value(t, "")), __LINE__);
			return;
		}
		//        else {
		//          addAxiom(t, Z3_mk_eq(ctx, arg1, mk_star(t, arg2)), __LINE__);
		//          return;
		//        }
	}

	if (repAll_res_map.find(replaceAllAst) == repAll_res_map.end()) {
		repAll_res_map[replaceAllAst] = res;
	} else {
		if (repAll_res_map[replaceAllAst] != res)
			return;
		else {
			addAxiom(t, repAll_axiom_map[replaceAllAst], __LINE__);
			return;
		}
	}

	if (isConstStr(t, arg1) && isConstStr(t, arg2) && isConstStr(t, arg3)
			&& isConstStr(t, res)) {
		std::string arg1Str = getConstStrValue(t, arg1);
		std::string arg2Str = getConstStrValue(t, arg2);
		std::string arg3Str = getConstStrValue(t, arg3);
		std::string resStr = getConstStrValue(t, res);
		std::regex reg(arg2Str);
		std::string rep = std::regex_replace(arg1Str, reg, arg3Str);
		if (resStr == rep)
			return;
		else {
			// negate
			addAxiom(t, Z3_mk_not(ctx, Z3_mk_eq(ctx, replaceAllAst, res)),
			__LINE__);
			return;
		}
	}

	// TODO: arg2 is star, union ...
	if (isConstStr(t, arg2) && isConstStr(t, arg3)) {
		Z3_ast and_items1[2], and_items2[6];

		Z3_ast para_contains1[2] = { arg1, arg2 };
		and_items1[0] = Z3_mk_not(ctx, reduce_contains2(t, para_contains1));
		and_items1[1] = Z3_mk_eq(ctx, res, arg1);

		Z3_ast unmatch = my_mk_internal_string_var(t);
		Z3_ast rem = my_mk_internal_string_var(t);
		Z3_ast remres = my_mk_internal_string_var(t);
		and_items2[0] = Z3_mk_eq(ctx, arg1,
				mk_concat(t, unmatch, mk_concat(t, arg2, rem)));
		and_items2[1] = Z3_mk_eq(ctx, res,
				mk_concat(t, unmatch, mk_concat(t, arg3, remres)));
		Z3_ast para_contains2[2] = { unmatch, arg2 };
		and_items2[2] = Z3_mk_not(ctx, reduce_contains2(t, para_contains2));
		Z3_ast para[3] = { rem, arg2, arg3 };
		and_items2[3] = Z3_mk_eq(ctx, remres, mk_replaceAll(t, para[0], para[1], para[2]));
		int diff = getConstStrValue(t, arg2).length()
				- getConstStrValue(t, arg3).length();
		Z3_ast para1[2] = { mk_length(t, arg1), mk_length(t, res) };
		Z3_ast para2[2] = { mk_length(t, rem), mk_length(t, remres) };
		Z3_ast p1 = Z3_mk_sub(ctx, 2, para1);
		Z3_ast p2 = Z3_mk_sub(ctx, 2, para2);
		Z3_ast para3[2] = { mk_int(ctx, diff), p2 };
		Z3_ast p3 = Z3_mk_add(ctx, 2, para3);

		if (diff == 0)
			and_items2[4] = Z3_mk_eq(ctx, p1, p2);
		else
			and_items2[4] = Z3_mk_eq(ctx, p1, p3);

		and_items2[5] = reduce_contains2(t, para_contains1);
		Z3_ast or_items[2];
		or_items[0] = Z3_mk_and(ctx, 2, and_items1);
		or_items[1] = Z3_mk_and(ctx, 6, and_items2);

		Z3_ast toAssert = Z3_mk_or(ctx, 2, or_items);
		repAll_axiom_map[replaceAllAst] = toAssert;
		addAxiom(t, toAssert, __LINE__);
	} else {
		Z3_ast and_items1[2], and_items2[6];
		Z3_ast intVar = my_mk_internal_int_var(t);

		and_items1[0] = Z3_mk_gt(ctx, mk_int(ctx, 0), intVar);
		and_items1[1] = Z3_mk_eq(ctx, res, arg1);

		and_items2[0] = Z3_mk_ge(ctx, intVar, mk_int(ctx, 0));
		Z3_ast unmatch = my_mk_internal_string_var(t);
		Z3_ast match = my_mk_internal_string_var(t);
		Z3_ast rem = my_mk_internal_string_var(t);
		and_items2[1] = Z3_mk_eq(ctx, arg1,
				mk_concat(t, unmatch, mk_concat(t, match, rem)));
		Z3_ast remres = my_mk_internal_string_var(t);
		and_items2[2] = Z3_mk_eq(ctx, res,
				mk_concat(t, unmatch, mk_concat(t, arg3, remres)));
		and_items2[3] = reduce_In(t, match, arg2);
		and_items2[4] = Z3_mk_eq(ctx, mk_length(t, unmatch), intVar);
		Z3_ast para[3] = { rem, arg2, arg3 };
		and_items2[5] = Z3_mk_eq(ctx, remres, mk_replaceAll(t, para[0], para[1], para[2]));

		Z3_ast or_items[2];
		or_items[0] = Z3_mk_and(ctx, 2, and_items1);
		or_items[1] = Z3_mk_and(ctx, 6, and_items2);

		Z3_ast and_items[2];
		and_items[0] = Z3_mk_or(ctx, 2, or_items);
		and_items[1] = Z3_mk_eq(ctx, intVar, mk_search(t, arg1, arg2));

		Z3_ast toAssert = Z3_mk_and(ctx, 2, and_items);
		repAll_axiom_map[replaceAllAst] = toAssert;
		addAxiom(t, toAssert, __LINE__);
	}
}

void solve_subStr_eq_sth(Z3_theory t, Z3_ast subStrAst, Z3_ast res) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "** solve_subStr_eq_sth: ");
	printZ3Node(t, subStrAst);
	__debugPrint(logFile, " = ");
	printZ3Node(t, res);
	__debugPrint(logFile, "\n");
#endif

	if (isConcatFunc(t, res)) {
		return;
	}

	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast a1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, subStrAst), 0);
	Z3_ast arg1 = get_eqc_value(t, a1);
	Z3_ast arg2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, subStrAst), 1);
	Z3_ast arg3 = Z3_get_app_arg(ctx, Z3_to_app(ctx, subStrAst), 2);

	if (isConstStr(t, res) && !isConstStr(t, arg1)) {
		if (getConstStrValue(t, res) == "") {
			addAxiom(t, Z3_mk_eq(ctx, arg1, my_mk_str_value(t, "")), __LINE__);
			return;
		}
	}

	if (subStr_res_map.find(subStrAst) == subStr_res_map.end()) {
		subStr_res_map[subStrAst] = res;
	} else {
		if (subStr_res_map[subStrAst] != res)
			return;
		else {
			addAxiom(t, subStr_axiom_map[subStrAst], __LINE__);
			return;
		}
	}

	if (isConstStr(t, arg1) && isConstNum(t, arg2) && isConstNum(t, arg3)
			&& isConstStr(t, res)) {
		std::string arg1Str = getConstStrValue(t, arg1);
		int arg2Num = getConstNumValue(t, arg2);
		int arg3Num = getConstNumValue(t, arg3);
		std::string resStr = getConstStrValue(t, res);
		std::string sub = arg1Str.substr(arg2Num, arg3Num);
		if (resStr == sub)
			return;
		else {
			// negate
			addAxiom(t, Z3_mk_not(ctx, Z3_mk_eq(ctx, subStrAst, res)),
			__LINE__);
			return;
		}
	}

	Z3_ast af = my_mk_internal_string_var(t);
	if (isConstNum(t, arg2)) {
		int i = getConstNumValue(t, arg2);
		if (i == 0) {
			Z3_ast and_item[2];
			and_item[0] = Z3_mk_eq(ctx, arg1, mk_concat(t, res, af));
			and_item[1] = Z3_mk_eq(ctx, arg3, mk_length(t, res));
			Z3_ast toAssert = Z3_mk_and(ctx, 2, and_item);
			subStr_axiom_map[subStrAst] = toAssert;
			addAxiom(t, toAssert, __LINE__);
			return;
		}
	}
	Z3_ast bf = my_mk_internal_string_var(t);
	Z3_ast and_item[3];
	and_item[0] = Z3_mk_eq(ctx, arg1, mk_concat(t, bf, mk_concat(t, res, af)));
	and_item[1] = Z3_mk_eq(ctx, arg2, mk_length(t, bf));
	and_item[2] = Z3_mk_eq(ctx, arg3, mk_length(t, res));
	Z3_ast toAssert = Z3_mk_and(ctx, 3, and_item);
	subStr_axiom_map[subStrAst] = toAssert;
	addAxiom(t, toAssert, __LINE__);
	return;
}

std::list<Z3_ast> consumeStr_head(std::string toconsume, std::string beconsumed,
		Z3_ast starAst, Z3_ast remAst, Z3_theory t, Z3_context ctx) {
	//#ifdef DEBUGLOG
	//    __debugPrint(logFile, "** consumeStr_head: ");
	//    __debugPrint(logFile, "%s ", toconsume.c_str());
	//    __debugPrint(logFile, "%s \n", beconsumed.c_str());
	//#endif
	int toconsumelen = toconsume.length();
	int beconsumedlen = beconsumed.length();
	std::string rem = beconsumed;
	std::string already_be_consumed = "";
	std::list<Z3_ast> option_items;

	while (toconsumelen <= beconsumedlen) {
		//#ifdef DEBUGLOG
		//    __debugPrint(logFile, "%s ", toconsume.c_str());
		//    __debugPrint(logFile, "%s \n", beconsumed.c_str());
		//#endif
		int remStrLen = beconsumedlen - toconsumelen;
		std::string firstPart = beconsumed.substr(0, toconsumelen);
		std::string secondPart = beconsumed.substr(toconsumelen, remStrLen);
		if (!isTwoStrEqual(toconsume, firstPart))
			break;
		else {
			Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
			Z3_ast tmpStrConst2 = my_mk_str_value(t,
					already_be_consumed.c_str());
			Z3_ast item1[2];
			item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
			item1[1] = Z3_mk_eq(ctx, remAst, tmpStrConst);
			Z3_ast imply = Z3_mk_and(ctx, 2, item1);
			option_items.push_back(imply);
			already_be_consumed += firstPart;
			rem = secondPart;
			beconsumed = rem;
			beconsumedlen = remStrLen;
		}
	}
	Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
	Z3_ast tmpStrConst2 = my_mk_str_value(t, already_be_consumed.c_str());
	Z3_ast item1[2];
	item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
	item1[1] = Z3_mk_eq(ctx, remAst, tmpStrConst);
	Z3_ast imply = Z3_mk_and(ctx, 2, item1);
	option_items.push_back(imply);
	return option_items;
}

std::list<Z3_ast> consumeStr_tail(std::string toconsume, std::string beconsumed,
		Z3_ast starAst, Z3_ast remAst, Z3_theory t, Z3_context ctx) {
	int toconsumelen = toconsume.length();
	int beconsumedlen = beconsumed.length();
	std::string rem = beconsumed;
	std::list<Z3_ast> option_items;
	std::string already_be_consumed = "";
	while (toconsumelen <= beconsumedlen) {
		int remStrLen = beconsumedlen - toconsumelen;
		std::string firstPart = beconsumed.substr(0, remStrLen);
		std::string secondPart = beconsumed.substr(remStrLen, toconsumelen);
		if (!isTwoStrEqual(toconsume, secondPart))
			break;
		else {
			Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
			Z3_ast tmpStrConst2 = my_mk_str_value(t,
					already_be_consumed.c_str());
			Z3_ast item1[2];
			item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
			item1[1] = Z3_mk_eq(ctx, remAst, tmpStrConst);
			Z3_ast imply = Z3_mk_and(ctx, 2, item1);
			option_items.push_back(imply);
			rem = firstPart;
			beconsumed = rem;
			beconsumedlen = remStrLen;
		}
	}
	Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
	Z3_ast tmpStrConst2 = my_mk_str_value(t, already_be_consumed.c_str());
	Z3_ast item1[2];
	item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
	item1[1] = Z3_mk_eq(ctx, remAst, tmpStrConst);
	Z3_ast imply = Z3_mk_and(ctx, 2, item1);
	option_items.push_back(imply);
	return option_items;
}

std::list<Z3_ast> consumeStr(std::string toconsume, std::string beconsumed,
		bool head, Z3_ast starAst, Z3_ast remAst1, Z3_ast remAst2, Z3_theory t,
		Z3_context ctx) {
	if (head)
		return consumeStr_head(toconsume, beconsumed, starAst, remAst1, t, ctx);
	else
		return consumeStr_tail(toconsume, beconsumed, starAst, remAst1, t, ctx);

}

std::list<Z3_ast> consumeStr_head_para(std::list<std::string> toconsume,
		std::string beconsumed, Z3_ast starAst, Z3_ast remAst1, Z3_ast remAst2,
		Z3_theory t, Z3_context ctx) {
	int toconsumelen = toconsume.front().length();
	int beconsumedlen = beconsumed.length();
	std::string rem = beconsumed;
	std::string already_be_consumed = "";
	std::list<Z3_ast> option_items;

	if (remAst2 == NULL) {
		while (toconsumelen <= beconsumedlen) {
			int remStrLen = beconsumedlen - toconsumelen;
			std::string firstPart = beconsumed.substr(0, toconsumelen);
			std::string secondPart = beconsumed.substr(toconsumelen, remStrLen);
			if (!isMemStrList(firstPart, toconsume))
				break;
			else {
				Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
				Z3_ast tmpStrConst2 = my_mk_str_value(t,
						already_be_consumed.c_str());
				Z3_ast item1[2];
				item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
				item1[1] = Z3_mk_eq(ctx, remAst1, tmpStrConst);
				Z3_ast imply = Z3_mk_and(ctx, 2, item1);
				option_items.push_back(imply);
				already_be_consumed += firstPart;
				rem = secondPart;
				beconsumed = rem;
				beconsumedlen = remStrLen;
			}
		}
		Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
		Z3_ast tmpStrConst2 = my_mk_str_value(t, already_be_consumed.c_str());
		Z3_ast item1[2];
		item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
		item1[1] = Z3_mk_eq(ctx, remAst1, tmpStrConst);
		Z3_ast imply = Z3_mk_and(ctx, 2, item1);
		option_items.push_back(imply);
		return option_items;
	} else {
		while (toconsumelen <= beconsumedlen) {
			int remStrLen = beconsumedlen - toconsumelen;
			std::string firstPart = beconsumed.substr(0, toconsumelen);
			std::string secondPart = beconsumed.substr(toconsumelen, remStrLen);
			if (!isMemStrList(firstPart, toconsume))
				break;
			else {
				Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
				Z3_ast tmpStrConst2 = my_mk_str_value(t,
						already_be_consumed.c_str());
				Z3_ast item1[2];
				item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
				item1[1] = Z3_mk_eq(ctx, remAst1,
						mk_concat(t, tmpStrConst, remAst2));
				Z3_ast imply = Z3_mk_and(ctx, 2, item1);
				option_items.push_back(imply);
				already_be_consumed += firstPart;
				rem = secondPart;
				beconsumed = rem;
				beconsumedlen = remStrLen;
			}
		}
		Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
		Z3_ast tmpStrConst2 = my_mk_str_value(t, already_be_consumed.c_str());
		Z3_ast item1[2];
		item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
		item1[1] = Z3_mk_eq(ctx, remAst1, mk_concat(t, tmpStrConst, remAst2));
		Z3_ast imply = Z3_mk_and(ctx, 2, item1);
		option_items.push_back(imply);
		return option_items;
	}
}

std::list<Z3_ast> consumeStr_tail_para(std::list<std::string> toconsume,
		std::string beconsumed, Z3_ast starAst, Z3_ast remAst1, Z3_ast remAst2,
		Z3_theory t, Z3_context ctx) {
	int toconsumelen = toconsume.front().length();
	int beconsumedlen = beconsumed.length();
	std::string rem = beconsumed;
	std::list<Z3_ast> option_items;
	std::string already_be_consumed = "";
	if (remAst2 == NULL) {
		while (toconsumelen <= beconsumedlen) {
			int remStrLen = beconsumedlen - toconsumelen;
			std::string firstPart = beconsumed.substr(0, remStrLen);
			std::string secondPart = beconsumed.substr(remStrLen, toconsumelen);
			if (!isMemStrList(secondPart, toconsume))
				break;
			else {
				Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
				Z3_ast tmpStrConst2 = my_mk_str_value(t,
						already_be_consumed.c_str());
				Z3_ast item1[2];
				item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
				item1[1] = Z3_mk_eq(ctx, remAst1, tmpStrConst);
				Z3_ast imply = Z3_mk_and(ctx, 2, item1);
				option_items.push_back(imply);
				rem = firstPart;
				beconsumed = rem;
				beconsumedlen = remStrLen;
			}
		}
		Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
		Z3_ast tmpStrConst2 = my_mk_str_value(t, already_be_consumed.c_str());
		Z3_ast item1[2];
		item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
		item1[1] = Z3_mk_eq(ctx, remAst1, tmpStrConst);
		Z3_ast imply = Z3_mk_and(ctx, 2, item1);
		option_items.push_back(imply);
		return option_items;
	} else {
		while (toconsumelen <= beconsumedlen) {
			int remStrLen = beconsumedlen - toconsumelen;
			std::string firstPart = beconsumed.substr(0, remStrLen);
			std::string secondPart = beconsumed.substr(remStrLen, toconsumelen);
			if (!isMemStrList(secondPart, toconsume))
				break;
			else {
				Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
				Z3_ast tmpStrConst2 = my_mk_str_value(t,
						already_be_consumed.c_str());
				Z3_ast item1[2];
				item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
				item1[1] = Z3_mk_eq(ctx, remAst1,
						mk_concat(t, tmpStrConst, remAst2));
				Z3_ast imply = Z3_mk_and(ctx, 2, item1);
				option_items.push_back(imply);
				rem = firstPart;
				beconsumed = rem;
				beconsumedlen = remStrLen;
			}
		}
		Z3_ast tmpStrConst = my_mk_str_value(t, rem.c_str());
		Z3_ast tmpStrConst2 = my_mk_str_value(t, already_be_consumed.c_str());
		Z3_ast item1[2];
		item1[0] = Z3_mk_eq(ctx, starAst, tmpStrConst2);
		item1[1] = Z3_mk_eq(ctx, remAst1, mk_concat(t, tmpStrConst, remAst2));
		Z3_ast imply = Z3_mk_and(ctx, 2, item1);
		option_items.push_back(imply);
		return option_items;
	}
}

std::list<Z3_ast> consumeStr_para(std::list<std::string> toconsume,
		std::string beconsumed, bool head, Z3_ast starAst, Z3_ast remAst1,
		Z3_ast remAst2, Z3_theory t, Z3_context ctx) {
	if (head)
		return consumeStr_head_para(toconsume, beconsumed, starAst, remAst1,
				remAst2, t, ctx);
	else
		return consumeStr_tail_para(toconsume, beconsumed, starAst, remAst1,
				remAst2, t, ctx);
}

void solve_star_concat_sth_eq_str_concat_sth(Z3_theory t, Z3_ast starAst,
		Z3_ast remAst1, Z3_ast constStr, Z3_ast remAst2, Z3_ast implyL,
		bool head) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "** solve_star_concat_sth_eq_str_concat_sth: ");
	printZ3Node(t, starAst);
	__debugPrint(logFile, " . ");
	printZ3Node(t, remAst1);
	__debugPrint(logFile, " = ");
	printZ3Node(t, constStr);
	__debugPrint(logFile, "\n");
#endif
	Z3_context ctx = Z3_theory_get_context(t);
	if (isStarFunc(t, starAst) && isConstStr(t, constStr)) {
		Z3_ast a1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, starAst), 0);
		//        Z3_ast a2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, starAst), 1);
		Z3_ast arg1 = get_eqc_value(t, a1);
		//        Z3_ast arg2 = a2;
		// TODO: if arg2 is int
		std::string const_str = getConstStrValue(t, constStr);

		if (isConstStr(t, arg1)) {
			std::string arg1_str = getConstStrValue(t, arg1);
			std::list<Z3_ast> res = consumeStr(arg1_str, const_str, head,
					starAst, remAst1, remAst2, t, ctx);
			//#ifdef DEBUGLOG
			//    __debugPrint(logFile, "%s ", res.second.c_str());
			//    __debugPrint(logFile, "%d \n", res.first);
			//#endif
			int pos1 = res.size();
			Z3_ast * or_items = new Z3_ast[pos1];
			int pos = pos1 - 1;
			std::list<Z3_ast>::iterator itor = res.begin();
			for (; itor != res.end(); itor++) {
				or_items[pos--] = *itor;
			}
			//          strEqLengthAxiom(t, remAst, tmpStrConst, __LINE__);
			Z3_ast implyR = Z3_mk_or(ctx, pos1, or_items);
			addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
			delete or_items;
		} else if (isUnionFunc(t, arg1)) {
			Z3_ast au1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, arg1), 0);
			Z3_ast au2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, arg1), 1);
			Z3_ast argu1 = get_eqc_value(t, au1);
			Z3_ast argu2 = get_eqc_value(t, au2);
			if (isConstStr(t, argu1) && isConstStr(t, argu2)) {
				std::string argu1_str = getConstStrValue(t, argu1);
				std::string argu2_str = getConstStrValue(t, argu2);
				if (argu1_str.length() == argu2_str.length()) {
					std::list < std::string > sl;
					sl.push_back(argu1_str);
					sl.push_back(argu2_str);
					std::list<Z3_ast> res = consumeStr_para(sl, const_str, head,
							starAst, remAst1, remAst2, t, ctx);
					int pos1 = res.size();
					Z3_ast * or_items = new Z3_ast[pos1];
					int pos = pos1 - 1;
					std::list<Z3_ast>::iterator itor = res.begin();
					for (; itor != res.end(); itor++) {
						or_items[pos--] = *itor;
					}
					Z3_ast implyR = Z3_mk_or(ctx, pos1, or_items);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					delete or_items;
				}
			}
		}
	}

}

void solve_star_concat_sth_eq_str(Z3_theory t, Z3_ast starAst, Z3_ast remAst,
		Z3_ast constStr, Z3_ast implyL, bool head) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "** solve_star_concat_sth_eq_str: ");
	printZ3Node(t, starAst);
	__debugPrint(logFile, " . ");
	printZ3Node(t, remAst);
	__debugPrint(logFile, " = ");
	printZ3Node(t, constStr);
	__debugPrint(logFile, "\n");
#endif
	solve_star_concat_sth_eq_str_concat_sth(t, starAst, remAst, constStr, NULL,
			implyL, head);
}

//------------------------------------------------------------
// solve concat of pattern:
//    constStr == Concat( constrStr, xx )
//    constStr == Concat( xx, constrStr )
//------------------------------------------------------------
void solve_concat_eq_str(Z3_theory t, Z3_ast concatAst, Z3_ast constStr) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "** solve_concat_eq_str: ");
	printZ3Node(t, concatAst);
	__debugPrint(logFile, " = ");
	printZ3Node(t, constStr);
	__debugPrint(logFile, "\n");
#endif
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast implyLstar = NULL;
	if (isConcatFunc(t, concatAst) && isConstStr(t, constStr)) {
		std::string const_str = getConstStrValue(t, constStr);
		Z3_ast a1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, concatAst), 0);
		Z3_ast a2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, concatAst), 1);
		// Shortcut
		if (const_str == "") {
			Z3_ast item[2];
			item[0] = Z3_mk_eq(ctx, a1, constStr);
			item[1] = Z3_mk_eq(ctx, a2, constStr);
			addAxiom(t, Z3_mk_and(ctx, 2, item), __LINE__);
			return;
		}
		Z3_ast arg1 = get_eqc_value_or_star(t, a1);
		Z3_ast arg2 = get_eqc_value_or_star(t, a2);
		__debugPrint(logFile, "arg1 = ");
		printZ3Node(t, arg1);
		__debugPrint(logFile, "\n");
		__debugPrint(logFile, "arg2 = ");
		printZ3Node(t, arg2);
		__debugPrint(logFile, "\n");
		Z3_ast newConcat = NULL;
		if (a1 != arg1 || a2 != arg2) {
			int iPos = 0;
			Z3_ast item1[2];
			if (a1 != arg1)
				item1[iPos++] = Z3_mk_eq(ctx, a1, arg1);
			if (a2 != arg2)
				item1[iPos++] = Z3_mk_eq(ctx, a2, arg2);
			Z3_ast implyL1 = NULL;
			if (iPos == 1)
				implyL1 = item1[0];
			else
				implyL1 = Z3_mk_and(ctx, 2, item1);

			newConcat = mk_concat(t, arg1, arg2);

			implyLstar = implyL1;
			bool test = true;
			if (isConcatFunc(t, newConcat)) {
				Z3_ast arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, newConcat), 0);
				Z3_ast arg2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, newConcat), 1);
				test = !isStarFunc(t, arg1) && !isStarFunc(t, arg2);
			}
			if (newConcat != constStr && test) {
				Z3_ast implyR1 = Z3_mk_eq(ctx, concatAst, newConcat);
				addAxiom(t, Z3_mk_implies(ctx, implyL1, implyR1), __LINE__);
			}
		} else {
			newConcat = concatAst;
		}

		if (newConcat == constStr)
			return;

		if (!isConcatFunc(t, newConcat))
			return;

		arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, newConcat), 0);
		arg2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, newConcat), 1);
		if (isStarFunc(t, arg1) && !isConstStr(t, arg2)) {
			Z3_ast implyL = NULL;
			if (implyLstar)
				implyL = implyLstar;
			else
				implyL = Z3_mk_eq(ctx, newConcat, constStr);
			return solve_star_concat_sth_eq_str(t, arg1, arg2, constStr, implyL,
					true);
		} else if (isStarFunc(t, arg2) && !isConstStr(t, arg1)) {
			Z3_ast implyL = NULL;
			if (implyLstar)
				implyL = implyLstar;
			else
				implyL = Z3_mk_eq(ctx, newConcat, constStr);
			return solve_star_concat_sth_eq_str(t, arg2, arg1, constStr, implyL,
					false);
		} else

		//---------------------------------------------------------------------
		// (1) Concat(const_Str, const_Str) = const_Str
		//---------------------------------------------------------------------
		if (isConstStr(t, arg1) && isConstStr(t, arg2)) {
			std::string arg1_str = getConstStrValue(t, arg1);
			std::string arg2_str = getConstStrValue(t, arg2);
			std::string result_str = arg1_str + arg2_str;
			if (result_str != const_str) {
				// negate
				addAxiom(t, Z3_mk_not(ctx, Z3_mk_eq(ctx, concatAst, constStr)),
				__LINE__);
				return;
			}
		}

		//---------------------------------------------------------------------
		// (2) Concat( var, const_Str ) = const_Str
		//---------------------------------------------------------------------
		else if (!isConstStr(t, arg1) && isConstStr(t, arg2)) {
			std::string arg2_str = getConstStrValue(t, arg2);
			int resultStrLen = const_str.length();
			int arg2StrLen = arg2_str.length();
			if (resultStrLen < arg2StrLen) {
				// negate
				addAxiom(t, Z3_mk_not(ctx, Z3_mk_eq(ctx, newConcat, constStr)),
				__LINE__);
				return;
			} else {
				int varStrLen = resultStrLen - arg2StrLen;
				std::string firstPart = const_str.substr(0, varStrLen);
				std::string secondPart = const_str.substr(varStrLen,
						arg2StrLen);
				if (!isTwoStrEqual(arg2_str, secondPart)) {
					// negate
					Z3_ast negateAst = Z3_mk_not(ctx,
							Z3_mk_eq(ctx, newConcat, constStr));
					addAxiom(t, negateAst, __LINE__);
					return;
				} else {
					Z3_ast tmpStrConst = my_mk_str_value(t, firstPart.c_str());
					Z3_ast implyL = Z3_mk_eq(ctx, newConcat, constStr);
					Z3_ast implyR = Z3_mk_eq(ctx, arg1, tmpStrConst);
					strEqLengthAxiom(t, arg1, tmpStrConst, __LINE__);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
				}
			}
		}

		//---------------------------------------------------------------------
		// (3) Concat(const_Str, var) = const_Str
		//---------------------------------------------------------------------
		else if (isConstStr(t, arg1) && !isConstStr(t, arg2)) {
			std::string arg1_str = getConstStrValue(t, arg1);
			int resultStrLen = const_str.length();
			int arg1StrLen = arg1_str.length();
			if (resultStrLen < arg1StrLen) {
				// negate
				addAxiom(t, Z3_mk_not(ctx, Z3_mk_eq(ctx, newConcat, constStr)),
				__LINE__);
				return;
			} else {
				int varStrLen = resultStrLen - arg1StrLen;
				std::string firstPart = const_str.substr(0, arg1StrLen);
				std::string secondPart = const_str.substr(arg1StrLen,
						varStrLen);
				if (!isTwoStrEqual(arg1_str, firstPart)) {
					// negate
					Z3_ast negateAst = Z3_mk_not(ctx,
							Z3_mk_eq(ctx, newConcat, constStr));
					addAxiom(t, negateAst, __LINE__);
					return;
				} else {
					Z3_ast tmpStrConst = my_mk_str_value(t, secondPart.c_str());
					Z3_ast implyL = Z3_mk_eq(ctx, newConcat, constStr);
					Z3_ast implyR = Z3_mk_eq(ctx, arg2, tmpStrConst);
					strEqLengthAxiom(t, arg2, tmpStrConst, __LINE__);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
				}
			}
		}
		//---------------------------------------------------------------------
		// (4) Concat(var, var) = const_Str
		//     Only when arg1 and arg2 do not have eq constant string values
		//---------------------------------------------------------------------
		else {
			if (Concat(t, arg1, arg2) == NULL) {
				Z3_ast implyL = Z3_mk_eq(ctx, concatAst, constStr);

				__debugPrint(logFile, "#  Checkpoint  #\n");

				Z3_ast len_arg1_ast = mk_length(t, arg1);
				len_arg1_ast = get_eqc_num(t, len_arg1_ast, 1);
				printZ3Node(t, len_arg1_ast);
				__debugPrint(logFile, "\n");
				if (isConstNum(t, len_arg1_ast)) {
					int resultStrLen = const_str.length();
					int len_arg1 = getConstNumValue(t, len_arg1_ast);
					if (len_arg1 > resultStrLen) {
						addAxiom(t, Z3_mk_not(ctx, implyL), __LINE__);
						return;
					}

					int remStrLen = resultStrLen - len_arg1;
					std::string firstPart = const_str.substr(0, len_arg1);
					std::string secondPart = const_str.substr(len_arg1,
							remStrLen);
					Z3_ast item1[2];
					item1[0] = Z3_mk_eq(ctx, arg1,
							my_mk_str_value(t, firstPart.c_str()));
					item1[1] = Z3_mk_eq(ctx, arg2,
							my_mk_str_value(t, secondPart.c_str()));
					Z3_ast implyR = Z3_mk_and(ctx, 2, item1);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}

				Z3_ast len_arg2_ast = mk_length(t, arg2);
				len_arg2_ast = get_eqc_num(t, len_arg2_ast, 2);
				printZ3Node(t, len_arg2_ast);
				__debugPrint(logFile, "\n");
				if (isConstNum(t, len_arg2_ast)) {
					int resultStrLen = const_str.length();
					int len_arg2 = getConstNumValue(t, len_arg2_ast);
					if (len_arg2 > resultStrLen) {
						addAxiom(t, Z3_mk_not(ctx, implyL), __LINE__);
						return;
					}

					int remStrLen = resultStrLen - len_arg2;
					std::string firstPart = const_str.substr(0, remStrLen);
					std::string secondPart = const_str.substr(remStrLen,
							len_arg2);
					Z3_ast item1[2];
					item1[0] = Z3_mk_eq(ctx, arg1,
							my_mk_str_value(t, firstPart.c_str()));
					item1[1] = Z3_mk_eq(ctx, arg2,
							my_mk_str_value(t, secondPart.c_str()));
					Z3_ast implyR = Z3_mk_and(ctx, 2, item1);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}

				Z3_ast xorFlag = NULL;
				std::pair<Z3_ast, Z3_ast> key1(arg1, arg2);
				std::pair<Z3_ast, Z3_ast> key2(arg2, arg1);
				if (varForBreakConcat.find(key1) == varForBreakConcat.end()
						&& varForBreakConcat.find(key2)
								== varForBreakConcat.end()) {
					xorFlag = mk_internal_xor_var(t);
					varForBreakConcat[key1][0] = xorFlag;
				} else {
					if (varForBreakConcat.find(key1)
							!= varForBreakConcat.end()) {
						xorFlag = varForBreakConcat[key1][0];
					} else {
						xorFlag = varForBreakConcat[key2][0];
					}
				}

				int concatStrLen = const_str.length();
				int xor_pos = 0;
				int and_count = 1;
				Z3_ast * xor_items = new Z3_ast[concatStrLen + 1];
				Z3_ast * and_items = new Z3_ast[2 * (concatStrLen + 1) + 1];
				Z3_ast arg1_eq = NULL;
				Z3_ast arg2_eq = NULL;
				for (int i = 0; i < concatStrLen + 1; i++) {
					std::string prefixStr = const_str.substr(0, i);
					std::string suffixStr = const_str.substr(i,
							concatStrLen - i);

					// skip invalidate options
					if (isConcatFunc(t, arg1)
							&& canConcatEqStr(t, arg1, prefixStr) == 0) {
						continue;
					}
					if (isConcatFunc(t, arg2)
							&& canConcatEqStr(t, arg2, suffixStr) == 0) {
						continue;
					}

					Z3_ast xorAst = Z3_mk_eq(ctx, xorFlag,
							mk_int(ctx, xor_pos));
					xor_items[xor_pos++] = xorAst;

					Z3_ast prefixAst = my_mk_str_value(t, prefixStr.c_str());
					arg1_eq = Z3_mk_eq(ctx, arg1, prefixAst);
					and_items[and_count++] = Z3_mk_eq(ctx, xorAst, arg1_eq);
					strEqLengthAxiom(t, arg1, prefixAst, __LINE__);

					Z3_ast suffixAst = my_mk_str_value(t, suffixStr.c_str());
					arg2_eq = Z3_mk_eq(ctx, arg2, suffixAst);
					and_items[and_count++] = Z3_mk_eq(ctx, xorAst, arg2_eq);
					strEqLengthAxiom(t, arg2, suffixAst, __LINE__);
				}

				Z3_ast to_double_check = implyL;
				Z3_ast to_double_check2 = Z3_mk_eq(ctx, constStr, concatAst);

				if (!keeptry) {
				}
				if (tempPatch) {
				} else if (num_try_map.find(to_double_check)
						== num_try_map.end()
						&& num_try_map.find(to_double_check2)
								== num_try_map.end()) {

#ifdef DEBUGLOG
					__debugPrint(logFile, "\n");
					__debugPrint(logFile, "#  Will process this equation again  #\n");
#endif

					num_try_map[to_double_check] = 1;
					num_try_map[to_double_check2] = 1;
					reassert_map[std::make_pair(constStr, concatAst)] = true;
					reassert_map[std::make_pair(concatAst, constStr)] = true;
					readdAxiom(t, to_double_check, __LINE__);
					return;
				} else if (num_try_map.find(to_double_check)->second
						< num_try) {

#ifdef DEBUGLOG
					__debugPrint(logFile, "\n");
					__debugPrint(logFile, "#  Will process this equation again  #\n");
#endif

					num_try_map[to_double_check]++;
					num_try_map[to_double_check2]++;
					reassert_map[std::make_pair(constStr, concatAst)] = true;
					reassert_map[std::make_pair(concatAst, constStr)] = true;
					readdAxiom(t, to_double_check, __LINE__);
					return;
				} else
					keeptry = false;

				/**********************/

				Z3_ast implyR1 = NULL;
				if (xor_pos == 0) {
					// negate
					Z3_ast negateAst = Z3_mk_not(ctx, implyL);
					addAxiom(t, negateAst, __LINE__);
				} else {
					if (xor_pos == 1) {
						and_items[0] = xor_items[0];
						implyR1 = Z3_mk_and(ctx, and_count, and_items);
					} else {
						and_items[0] = Z3_mk_or(ctx, xor_pos, xor_items);
						implyR1 = Z3_mk_and(ctx, and_count, and_items);
					}
					Z3_ast implyToAssert = Z3_mk_implies(ctx, implyL, implyR1);
					addAxiom(t, implyToAssert, __LINE__);
				}
				delete[] xor_items;
				delete[] and_items;
			}
		}
	}
}

/*
 * Get constant strings (from left to right) in an AST node and return them in astList
 */
void getconstStrAstsInNode(Z3_theory t, Z3_ast node,
		std::list<Z3_ast> & astList) {
	Z3_context ctx = Z3_theory_get_context(t);
	if (isConstStr(t, node)) {
		astList.push_back(node);
	} else if (getNodeType(t, node) == my_Z3_Func) {
		Z3_app func_app = Z3_to_app(ctx, node);
		int argCount = Z3_get_app_num_args(ctx, func_app);
		for (int i = 0; i < argCount; i++) {
			Z3_ast argAst = Z3_get_app_arg(ctx, func_app, i);
			getconstStrAstsInNode(t, argAst, astList);
		}
	}
}

bool isIntVar(Z3_context ctx, Z3_ast var) {
	Z3_sort s = Z3_get_sort(ctx, var);
	Z3_sort_kind sk = Z3_get_sort_kind(ctx, s);
	return (sk == Z3_INT_SORT);
}

/*
 *
 */
void strEqLengthAxiom(Z3_theory t, Z3_ast varAst, Z3_ast strAst, int line) {
	Z3_context ctx = Z3_theory_get_context(t);

	if (isIntVar(ctx, varAst) || isIntVar(ctx, strAst))
		return;

	if (getNodeType(t, varAst) == my_Z3_Str_Var) {
		std::string vName = std::string(Z3_ast_to_string(ctx, varAst));
		if (vName.length() >= 6
				&& (vName.substr(0, 6) == "_t_len"
						|| vName.substr(0, 6) == "_t_val")) {
			return;
		}
	}

	if (getNodeType(t, strAst) == my_Z3_Str_Var) {
		std::string vName = std::string(Z3_ast_to_string(ctx, strAst));
		if (vName.length() >= 6
				&& (vName.substr(0, 6) == "_t_len"
						|| vName.substr(0, 6) == "_t_val")) {
			return;
		}
	}

	if (isConstStr(t, varAst) && !isConstStr(t, strAst)) {
		Z3_ast tmp = varAst;
		varAst = strAst;
		strAst = tmp;
	}

	Z3_ast implyL = Z3_mk_eq(ctx, varAst, strAst);
	Z3_ast toAssert = NULL;
	if (isConstStr(t, strAst)) {
		std::string str = getConstStrValue(t, strAst);
		if (str == "") {
			if (getNodeType(t, varAst) != my_Z3_Str_Var) {
				Z3_ast lenAst = mk_int(ctx, 0);
				toAssert = Z3_mk_eq(ctx, implyL,
						Z3_mk_eq(ctx, mk_length(t, varAst), lenAst));
			} else {
				// basicStrVarAxiom() already adds above axiom. Not necessary to assert it again
				return;
			}
		} else {
			Z3_ast lenAst = mk_int(ctx, str.length());
			toAssert = Z3_mk_implies(ctx, implyL,
					Z3_mk_eq(ctx, mk_length(t, varAst), lenAst));
		}
	} else {
		Z3_ast lenAst = mk_length(t, strAst);
		toAssert = Z3_mk_implies(ctx, implyL,
				Z3_mk_eq(ctx, mk_length(t, varAst), lenAst));
	}

	if (toAssert != NULL) {
		addAxiom(t, toAssert, line, false);
#ifdef DEBUGLOG
		__debugPrint(logFile, ">> Length added for: ");
		printZ3Node(t, implyL);
		__debugPrint(logFile, " @ %d\n\n", line);
#endif
	}
}

/*
 * Handle two equivalent Concats. nn1 and nn2 are two concat functions
 */
void simplifyConcatEq(Z3_theory t, Z3_ast nn1, Z3_ast nn2, int duplicateCheck) {
	Z3_context ctx = Z3_theory_get_context(t);

#ifdef DEBUGLOG
	__debugPrint(logFile, "\n===============================================\n");
	__debugPrint(logFile, "** simplifyConcatEq():\n");
	__debugPrint(logFile, "   nn1 = ");
	printZ3Node(t, nn1);
	__debugPrint(logFile, "\n");
	__debugPrint(logFile, "   nn2 = ");
	printZ3Node(t, nn2);
	__debugPrint(logFile, "\n===============================================\n");
#endif

	Z3_ast a1_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, nn1), 0);
	Z3_ast a1_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, nn1), 1);
	Z3_ast a2_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, nn2), 0);
	Z3_ast a2_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, nn2), 1);

	//-----------------------------------------
	Z3_ast item[3];
	int ii1 = 0;
	Z3_ast v1_arg0 = get_eqc_value(t, a1_arg0);
	Z3_ast v1_arg1 = get_eqc_value(t, a1_arg1);
	Z3_ast new_nn1 = NULL;
	Z3_ast new_nn1_how = NULL;

	if (a1_arg0 != v1_arg0 || a1_arg1 != v1_arg1) {
		if (a1_arg0 != v1_arg0)
			item[ii1++] = Z3_mk_eq(ctx, a1_arg0, v1_arg0);
		if (a1_arg1 != v1_arg1)
			item[ii1++] = Z3_mk_eq(ctx, a1_arg1, v1_arg1);

		new_nn1 = mk_concat(t, v1_arg0, v1_arg1);
		if (!inSameEqc(t, nn1, new_nn1) && nn1 != new_nn1) {
			Z3_ast implyL = my_mk_and(t, item, ii1);
			Z3_ast implyR = Z3_mk_eq(ctx, nn1, new_nn1);
			new_nn1_how = Z3_mk_implies(ctx, implyL, implyR);
		}
	} else {
		new_nn1 = nn1;
	}
	v1_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, new_nn1), 0);
	v1_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, new_nn1), 1);

	//------------------------------------------------------
	int ii2 = 0;
	Z3_ast v2_arg0 = get_eqc_value(t, a2_arg0);
	Z3_ast v2_arg1 = get_eqc_value(t, a2_arg1);
	Z3_ast new_nn2 = NULL;
	Z3_ast new_nn2_how = NULL;
	if (a2_arg0 != v2_arg0 || a2_arg1 != v2_arg1) {
		if (a2_arg0 != v2_arg0)
			item[ii2++] = Z3_mk_eq(ctx, a2_arg0, v2_arg0);
		if (a2_arg1 != v2_arg1)
			item[ii2++] = Z3_mk_eq(ctx, a2_arg1, v2_arg1);

		new_nn2 = mk_concat(t, v2_arg0, v2_arg1);
		if (!inSameEqc(t, nn2, new_nn2) && nn2 != new_nn2) {
			Z3_ast implyL = my_mk_and(t, item, ii2);
			Z3_ast implyR = Z3_mk_eq(ctx, nn2, new_nn2);
			new_nn2_how = Z3_mk_implies(ctx, implyL, implyR);
		}
	} else {
		new_nn2 = nn2;
	}
	v2_arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, new_nn2), 0);
	v2_arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, new_nn2), 1);

	// --------------------------------------------------

#ifdef DEBUGLOG
	__debugPrint(logFile, "[simplifyConcatEq] new_nn1 = ");
	printZ3Node(t, new_nn1);
	__debugPrint(logFile, "\n                   new_nn2 = ");
	printZ3Node(t, new_nn2);
	__debugPrint(logFile, " @ %d\n", __LINE__);
#endif

	if (new_nn1 == new_nn2) {
		__debugPrint(logFile, ">> Eq concats, return.\n");
		return;
	}

	if (!canTwoNodesEq(t, new_nn1, new_nn2)) {
		Z3_ast detected = Z3_mk_not(ctx, Z3_mk_eq(ctx, new_nn1, new_nn2));
		__debugPrint(logFile, "\n");
		__debugPrint(logFile, ">> Inconsistent detected in simplifyConcatEq:\n");
		addAxiom(t, detected, __LINE__);
		__debugPrint(logFile, "\n\n");
		return;
	}

	if (new_nn1_how != NULL) {
		addAxiom(t, new_nn1_how, __LINE__);
	}

	if (new_nn2_how != NULL) {
		addAxiom(t, new_nn2_how, __LINE__);
	}

	bool n1IsConcat = isConcatFunc(t, new_nn1);
	bool n2IsConcat = isConcatFunc(t, new_nn2);

	if (!n1IsConcat && n2IsConcat) {
		if (isConstStr(t, new_nn1)) {
			__debugPrint(logFile,
					">> [simplifyConcatEq] nn1_new is not concat @ %d\n\n",
					__LINE__);
			simplifyConcatStr(t, new_nn2, new_nn1);
		}
		return;
	} else if (n1IsConcat && !n2IsConcat) {
		if (isConstStr(t, new_nn2)) {
			__debugPrint(logFile,
					">> [simplifyConcatEq] nn1_new is not concat @ %d\n\n",
					__LINE__);
			simplifyConcatStr(t, new_nn1, new_nn2);
		}
		return;
	}

	if (!inSameEqc(t, new_nn1, new_nn2) && (nn1 != new_nn1 || nn2 != new_nn2)) {
		int ii4 = 0;
		if (nn1 != new_nn1)
			item[ii4++] = Z3_mk_eq(ctx, nn1, new_nn1);
		if (nn2 != new_nn2)
			item[ii4++] = Z3_mk_eq(ctx, nn2, new_nn2);
		item[ii4++] = Z3_mk_eq(ctx, nn1, nn2);

		Z3_ast implyL = my_mk_and(t, item, ii4);
		Z3_ast implyR = Z3_mk_eq(ctx, new_nn1, new_nn2);
		addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);

		if (new_nn1 != nn1
				&& concat_eqc_index.find(nn1) != concat_eqc_index.end()) {
			concat_eqc_index[new_nn1] = concat_eqc_index[nn1];
		}

		if (new_nn2 != nn2
				&& concat_eqc_index.find(nn2) != concat_eqc_index.end()) {
			concat_eqc_index[new_nn2] = concat_eqc_index[nn2];
		}
	}

	int duplicatedSplit = 0;
	if (duplicateCheck) {
		if (isConcatFunc(t, new_nn1) && isConcatFunc(t, new_nn2)) {
			Z3_ast concatIndex = NULL;
			if (concat_eqc_index.find(new_nn1) != concat_eqc_index.end()
					&& concat_eqc_index.find(new_nn2)
							!= concat_eqc_index.end()) {
				std::map<std::pair<Z3_ast, Z3_ast>, bool>::iterator it =
						reassert_map.find(std::make_pair(new_nn1, new_nn2));
				if (it->second) {
					reassert_map.erase(it);
					reassert_map.erase(
							reassert_map.find(
									std::make_pair(new_nn2, new_nn1)));
				} else {
					__debugPrint(logFile, "DUP 0\n");
					printZ3Node(t, new_nn1);
					__debugPrint(logFile, "\n");
					printZ3Node(t, new_nn2);
					__debugPrint(logFile, "\n");
					std::pair<Z3_ast, Z3_ast> v(new_nn1, new_nn2);
					std::pair<Z3_ast, Z3_ast> key2(new_nn2, new_nn1);
					{
						duplicatedSplit = 1;
					}
				}

			} else if (concat_eqc_index.find(new_nn1) == concat_eqc_index.end()
					&& concat_eqc_index.find(new_nn2)
							!= concat_eqc_index.end()) {
				concatIndex = concat_eqc_index[new_nn2];
				concat_eqc_index[new_nn1] = concatIndex;
			} else if (concat_eqc_index.find(new_nn1) != concat_eqc_index.end()
					&& concat_eqc_index.find(new_nn2)
							== concat_eqc_index.end()) {
				concatIndex = concat_eqc_index[new_nn1];
				concat_eqc_index[new_nn2] = concatIndex;
			} else {
				concatIndex = new_nn1;
				concat_eqc_index[new_nn1] = concatIndex;
				concat_eqc_index[new_nn2] = concatIndex;
			}
		} else {
			__debugPrint(logFile,
					">> Not two concats in simplifyConcatEq @ %d\n\n", __LINE__);
			return;
		}
	}

	Z3_ast implyL = Z3_mk_eq(ctx, new_nn1, new_nn2);

	//*****************************************************
	// check prefix and suffix of new_nn1 and new_nn2
	// Only check one prefix node and suffix node
	//*****************************************************

	//===============================================
	// ** simplifyConcatEq():
	//    nn1 = (Concat var_3 fh)
	//    nn2 = (Concat (Concat var_3 ~) _t_str0)
	//===============================================
	std::vector<Z3_ast> nn1List;
	std::vector<Z3_ast> nn2List;
	getNodesInConcat(t, new_nn1, nn1List);
	getNodesInConcat(t, new_nn2, nn2List);

	// Prefix
	int sidx = 0;
	int eidx1 = nn1List.size() - 1;
	int eidx2 = nn2List.size() - 1;
	if (nn1List[0] == nn2List[0]) {
		sidx = 1;
	}
	if (nn1List[eidx1] == nn2List[eidx2]) {
		eidx1--;
		eidx2--;
	}
	if (sidx != 0 || eidx1 != ((int) nn1List.size() - 1)) {
		Z3_ast nn1_simp = nn1List[sidx];
		Z3_ast nn2_simp = nn2List[sidx];
		for (int i = sidx + 1; i <= eidx1; i++) {
			nn1_simp = mk_concat(t, nn1_simp, nn1List[i]);
		}
		for (int i = sidx + 1; i <= eidx2; i++) {
			nn2_simp = mk_concat(t, nn2_simp, nn2List[i]);
		}

#ifdef DEBUGLOG
		__debugPrint(logFile, "\n");
		__debugPrint(logFile, "#############################\n");
		__debugPrint(logFile, "#  SimplifyConcatEq Type 0  #\n");
		__debugPrint(logFile, "#############################\n");
#endif

		if (!inSameEqc(t, nn1_simp, nn2_simp)) {
			Z3_ast implyR = Z3_mk_eq(ctx, nn1_simp, nn2_simp);
			Z3_ast toAssert = Z3_mk_implies(ctx, implyL, implyR);
			addAxiom(t, toAssert, __LINE__);
			return;
		} else {
			__debugPrint(logFile, ">> [");
			printZ3Node(t, nn1_simp);
			__debugPrint(logFile, "] and [");
			printZ3Node(t, nn2_simp);
			__debugPrint(logFile, "] are in a same eqc. SKIP\n\n");
		}
	}

	//*****************************************************
	// Start to split two Concats
	//*****************************************************

	checkandInit_cutVAR(t, v1_arg0);
	checkandInit_cutVAR(t, v1_arg1);
	checkandInit_cutVAR(t, v2_arg0);
	checkandInit_cutVAR(t, v2_arg1);

	//*************************************************************
	// case 1: concat(x, y) = concat(m, n)
	//*************************************************************
	if (duplicatedSplit == 0) {
		if ((!isConstStr(t, v1_arg0) && !isConstStr(t, v1_arg1)
				&& !isConstStr(t, v2_arg0) && !isConstStr(t, v2_arg1))) {
#ifdef DEBUGLOG
			__debugPrint(logFile, "\n");
			__debugPrint(logFile, "#############################\n");
			__debugPrint(logFile, "#  SimplifyConcatEq Type 1  #\n");
			__debugPrint(logFile, "#############################\n");
#endif
			Z3_ast x = v1_arg0;
			Z3_ast y = v1_arg1;
			Z3_ast m = v2_arg0;
			Z3_ast n = v2_arg1;

			//      if (areTwoNodesInSameEqc(t,x,m) && areTwoNodesInSameEqc(t,y,n)){
			//          return;
			//      }

			Z3_ast len_x = mk_length(t, x);
			Z3_ast len_m = mk_length(t, m);
			Z3_ast len_y = mk_length(t, y);
			Z3_ast len_n = mk_length(t, n);

			//            printf ("get_bound_strlen1\n");
			//#ifdef DEBUGLOG
			//            __debugPrint(logFile, "\nget_bound_strlen1\n");
			//#endif
			//            int lb = -1;
			//            int ub = -1;
			//            Z3_theory_get_bound_strlen(t,len_x,&lb,&ub);

			Z3_ast num = get_eqc_num(t, len_x, 3);
			if (num != len_x) {
				__debugPrint(logFile, "len_x: ");
				printZ3Node(t, num);
				__debugPrint(logFile, "\n");
				if (getConstNumValue(t, num) == 0) {
					__debugPrint(logFile, "len_x: ");
					printZ3Node(t, num);
					__debugPrint(logFile, "\n");
					Z3_ast implyR = Z3_mk_eq(ctx, y, new_nn2);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
			}

			Z3_ast num2 = get_eqc_num(t, len_m, 4);
			if (num2 != len_m) {
				__debugPrint(logFile, "len_m: ");
				printZ3Node(t, num2);
				__debugPrint(logFile, "\n");
				if (getConstNumValue(t, num2) == 0) {
					__debugPrint(logFile, "len_m: ");
					printZ3Node(t, num2);
					__debugPrint(logFile, "\n");
					Z3_ast implyR = Z3_mk_eq(ctx, new_nn1, n);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
			}

			if (num != len_x && num2 != len_m) {
				int l1 = getConstNumValue(t, num);
				int l2 = getConstNumValue(t, num2);
				if (l1 == l2) {
					Z3_ast item[2];
					item[0] = Z3_mk_eq(ctx, x, m);
					item[1] = Z3_mk_eq(ctx, y, n);
					Z3_ast implyR = Z3_mk_and(ctx, 2, item);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
				int r = 0;
				Z3_ast v = NULL;
				Z3_ast lhs1 = NULL, lhs2 = NULL, rhs1 = NULL, rhs2 = NULL;
				if (l1 > l2) {
					__debugPrint(logFile, "l1 > l2\n");
					r = l1 - l2;
					v = my_mk_internal_string_var(t);
					lhs1 = x;
					rhs1 = mk_concat(t, m, v);
					lhs2 = mk_concat(t, v, y);
					rhs2 = n;
				} else {
					__debugPrint(logFile, "l1 < l2\n");
					r = l2 - l1;
					v = my_mk_internal_string_var(t);
					lhs1 = mk_concat(t, x, v);
					rhs1 = m;
					lhs2 = y;
					rhs2 = mk_concat(t, v, n);
				}
				Z3_ast lhs3 = mk_length(t, v);
				Z3_ast rhs3 = mk_int(ctx, r);
				Z3_ast item[3];
				item[0] = Z3_mk_eq(ctx, lhs1, rhs1);
				item[1] = Z3_mk_eq(ctx, lhs2, rhs2);
				item[2] = Z3_mk_eq(ctx, lhs3, rhs3);
				__debugPrint(logFile, "Checkpoint\n");
				Z3_ast implyR = Z3_mk_and(ctx, 3, item);
				addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
				return;
			}

			Z3_ast num3 = get_eqc_num(t, len_y, 5);
			if (num3 != len_y) {
				if (getConstNumValue(t, num3) == 0) {
					__debugPrint(logFile, "len_y: ");
					printZ3Node(t, num3);
					__debugPrint(logFile, "\n");
					Z3_ast implyR = Z3_mk_eq(ctx, x, new_nn2);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
			}

			Z3_ast num4 = get_eqc_num(t, len_n, 6);
			if (num4 != len_n) {
				if (getConstNumValue(t, num4) == 0) {
					__debugPrint(logFile, "len_n: ");
					printZ3Node(t, num4);
					__debugPrint(logFile, "\n");
					Z3_ast implyR = Z3_mk_eq(ctx, new_nn1, m);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
			}

			//        Z3_ast implyR = Z3_theory_get_len_relationship(t,len_x,len_m,x,y,m,n);
			//        if (implyR!=NULL) {
			//            addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
			//            return 1;
			//        }

			if (x == m) {
				if (!inSameEqc(t, y, n)) {
					Z3_ast t_implyR = Z3_mk_eq(ctx, y, n);
					Z3_ast toAssert = Z3_mk_implies(ctx, implyL, t_implyR);
					addAxiom(t, toAssert, __LINE__);
				}
			} else if (y == n) {
				if (!inSameEqc(t, x, m)) {
					Z3_ast t_implyR = Z3_mk_eq(ctx, x, m);
					Z3_ast toAssert = Z3_mk_implies(ctx, implyL, t_implyR);
					addAxiom(t, toAssert, __LINE__);
				}
			} else {
				PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);

				Z3_ast to_double_check = Z3_mk_eq(ctx, nn1, nn2);
				Z3_ast to_double_check2 = Z3_mk_eq(ctx, nn2, nn1);

				if (!keeptry) {
				} else if (tempPatch) {
				} else if (num_try_map.find(to_double_check)
						== num_try_map.end()
						&& num_try_map.find(to_double_check2)
								== num_try_map.end()) {
#ifdef DEBUGLOG
					__debugPrint(logFile, "\n");
					__debugPrint(logFile, "#  Will process this equation again  #\n");
#endif
					num_try_map[to_double_check] = 1;
					num_try_map[to_double_check2] = 1;
					if (prop > 0) {
						Z3_theory_prop_arith_to_len(t, td->Concat);
						prop--;
					}
					reassert_map[std::make_pair(new_nn1, new_nn2)] = true;
					reassert_map[std::make_pair(new_nn2, new_nn1)] = true;
					readdAxiom(t, to_double_check, __LINE__);
					return;
				} else
					keeptry = false;

				if (!tempPatch) {
#ifdef DEBUGLOG
					__debugPrint(logFile, "\n");
					__debugPrint(logFile, "#  Reprocessing the equation # ");
					printZ3Node(t, implyL);
					__debugPrint(logFile, "\n");
#endif
					Z3_ast implyR = Z3_theory_prop_len_to_str(t, len_x, len_m,
							x, y, m, n);
					if (implyR != NULL) {
						addAxiom(t, Z3_mk_implies(ctx, implyL, implyR),
						__LINE__);
						return;
					}
				}

				Z3_ast t1 = NULL;
				Z3_ast t2 = NULL;
				checkandInit_cutVAR(t, t1);
				checkandInit_cutVAR(t, t2);

				Z3_ast xorFlag = NULL;

				std::pair<Z3_ast, Z3_ast> key1(new_nn1, new_nn2);
				std::pair<Z3_ast, Z3_ast> key2(new_nn2, new_nn1);

				if (varForBreakConcat.find(key1) == varForBreakConcat.end()
						&& varForBreakConcat.find(key2)
								== varForBreakConcat.end()) {
					t1 = my_mk_internal_string_var(t);
					t2 = my_mk_internal_string_var(t);
					xorFlag = mk_internal_xor_var(t);

					varForBreakConcat[key1][0] = t1;
					varForBreakConcat[key1][1] = t2;
					varForBreakConcat[key1][2] = xorFlag;

				} else {
					if (varForBreakConcat.find(key1)
							!= varForBreakConcat.end()) {
						t1 = varForBreakConcat[key1][0];
						t2 = varForBreakConcat[key1][1];
						xorFlag = varForBreakConcat[key1][2];

					} else {
						t1 = varForBreakConcat[key2][0];
						t2 = varForBreakConcat[key2][1];
						xorFlag = varForBreakConcat[key2][2];
					}
				}

				Z3_ast * or_item = new Z3_ast[3];
				Z3_ast * and_item = new Z3_ast[20];
				int option = 0;
				int pos = 1;

				//--------------------------------------
				// break option 1: m cut y
				//--------------------------------------
				// if x cut y (meaning x is the cause of split of y)
				// Check whether
				//   suffix(x) ?= VAR(y)
				//   - Yes. Do not cut y again
				//   - NO. OK to proceed
				//--------------------------------------
				if (!avoidLoopCut || !(hasSelfCut(m, y))) {
					// break down option 1-1
					Z3_ast x_t1 = mk_concat(t, x, t1);
					Z3_ast t1_n = mk_concat(t, t1, n);
					or_item[option] = Z3_mk_eq(ctx, xorFlag,
							mk_int(ctx, option));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_eq(ctx, m, x_t1));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_eq(ctx, y, t1_n));

					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_gt(ctx, mk_length(t, m), mk_length(t, x)));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_gt(ctx, mk_length(t, y), mk_length(t, n)));

					option++;

					//"str_eq --> length_eq" constraints
					strEqLengthAxiom(t, m, x_t1, __LINE__);
					strEqLengthAxiom(t, y, t1_n, __LINE__);

					// Cut Info
					addCutInfoMerge(t1, sLevel, m);
					addCutInfoMerge(t1, sLevel, y);
					//                    addCutInfoMerge(x, sLevel, m);
					//                    addCutInfoMerge(n, sLevel, y);
				} else {
					loopDetected = true;
#ifdef DEBUGLOG
					__debugPrint(logFile, "-------------------\n");
					__debugPrint(logFile, "[AVOID Loop] Skip @ %d.\n", __LINE__);
					printCutVAR(t, m);
					printCutVAR(t, y);
					__debugPrint(logFile, "-------------------\n");
#endif
				}
				//--------------------------------------
				// break option 2: x cut n
				//--------------------------------------
				if (!avoidLoopCut || !(hasSelfCut(x, n))) {
					// break down option 1-2
					Z3_ast m_t2 = mk_concat(t, m, t2);
					Z3_ast t2_y = mk_concat(t, t2, y);
					or_item[option] = Z3_mk_eq(ctx, xorFlag,
							mk_int(ctx, option));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_eq(ctx, x, m_t2));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_eq(ctx, n, t2_y));

					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_gt(ctx, mk_length(t, x), mk_length(t, m)));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_gt(ctx, mk_length(t, n), mk_length(t, y)));

					option++;

					strEqLengthAxiom(t, x, m_t2, __LINE__);
					strEqLengthAxiom(t, n, t2_y, __LINE__);

					addCutInfoMerge(t2, sLevel, x);
					addCutInfoMerge(t2, sLevel, n);
					//                    addCutInfoMerge(m, sLevel, x);
					//                    addCutInfoMerge(y, sLevel, n);

				} else {
					loopDetected = true;
#ifdef DEBUGLOG
					__debugPrint(logFile, "-------------------\n");
					__debugPrint(logFile, "[AVOID Looping Cut] Skip @ %d.\n", __LINE__);
					printCutVAR(t, x);
					printCutVAR(t, n);
					__debugPrint(logFile, "-------------------\n");
#endif
				}

				if (canTwoNodesEq(t, x, m) && canTwoNodesEq(t, y, n)) {
					or_item[option] = Z3_mk_eq(ctx, xorFlag,
							mk_int(ctx, option));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_eq(ctx, x, m));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_eq(ctx, y, n));

					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_eq(ctx, mk_length(t, x), mk_length(t, m)));
					and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
							Z3_mk_eq(ctx, mk_length(t, n), mk_length(t, y)));

					option++;
				}

				if (option > 0) {
					if (option == 1) {
						and_item[0] = or_item[0];
					} else {
						and_item[0] = Z3_mk_or(ctx, option, or_item);
					}

					Z3_ast implyR = Z3_mk_and(ctx, pos, and_item);
					Z3_ast toAssert = Z3_mk_implies(ctx, implyL, implyR);

					addAxiom(t, toAssert, __LINE__);
				} else {
					__debugPrint(logFile,
							"\n[STOP @ %d] Should not split two EQ concats:",
							__LINE__);
					__debugPrint(logFile, "\n            ");
					printZ3Node(t, implyL);
					__debugPrint(logFile, "\n");
					return;
				}
				delete[] or_item;
				delete[] and_item;
				return;
			}
			return;
		}
	}

	//*************************************************************
	// case 2: concat(x, y) = concat(m, "str")
	//*************************************************************
	if (duplicatedSplit == 0) {
		if ((!isConstStr(t, v1_arg0) && isConstStr(t, v1_arg1)
				&& !isConstStr(t, v2_arg0) && !isConstStr(t, v2_arg1))
				|| (!isConstStr(t, v2_arg0) && isConstStr(t, v2_arg1)
						&& !isConstStr(t, v1_arg0) && !isConstStr(t, v1_arg1))) {
			__debugPrint(logFile, "\n");
			__debugPrint(logFile, "#############################\n");
			__debugPrint(logFile, "#  SimplifyConcatEq Type 2  #\n");
			__debugPrint(logFile, "#############################\n");

			Z3_ast x = NULL;
			Z3_ast y = NULL;
			Z3_ast strAst = NULL;
			Z3_ast m = NULL;
			Z3_ast xorFlag = NULL;

			if (isConstStr(t, v1_arg1) && !isConstStr(t, v2_arg1)) {
				m = v1_arg0;
				strAst = v1_arg1;
				x = v2_arg0;
				y = v2_arg1;
			} else {
				m = v2_arg0;
				strAst = v2_arg1;
				x = v1_arg0;
				y = v1_arg1;
			}

			Z3_ast len_y = mk_length(t, y);
			Z3_ast len_x = mk_length(t, x);
			Z3_ast len_m = mk_length(t, m);
			printZ3Node(t, x);
			__debugPrint(logFile, "\n");
			printZ3Node(t, y);
			__debugPrint(logFile, "\n");
			printZ3Node(t, m);
			__debugPrint(logFile, "\n");
			printZ3Node(t, strAst);
			__debugPrint(logFile, "\n");
			printZ3Node(t, len_y);
			__debugPrint(logFile, "\n");
			len_y = get_eqc_num(t, len_y, 7);
			printZ3Node(t, len_y);
			__debugPrint(logFile, "\n");

			Z3_ast num = get_eqc_num(t, len_x, 8);
			if (num != len_x) {
				__debugPrint(logFile, "len_x: ");
				printZ3Node(t, num);
				__debugPrint(logFile, "\n");
				if (getConstNumValue(t, num) == 0) {
					__debugPrint(logFile, "len_x: ");
					printZ3Node(t, num);
					__debugPrint(logFile, "\n");
					Z3_ast implyR = Z3_mk_eq(ctx, y, new_nn2);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
			}

			Z3_ast num2 = get_eqc_num(t, len_m, 9);
			if (num2 != len_m) {
				__debugPrint(logFile, "len_m: ");
				printZ3Node(t, num2);
				__debugPrint(logFile, "\n");
				if (getConstNumValue(t, num2) == 0) {
					__debugPrint(logFile, "len_m: ");
					printZ3Node(t, num2);
					__debugPrint(logFile, "\n");
					Z3_ast implyR = Z3_mk_eq(ctx, new_nn1, strAst);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
			}

			//Quick path
			if (getConstStrValue(t, strAst) == "") {
			} else if (isConstNum(t, len_y)) {
				if ((int) getConstStrValue(t, strAst).length()
						== getConstNumValue(t, len_y)) {
					Z3_ast item1[2];
					item1[0] = Z3_mk_eq(ctx, x, m);
					item1[1] = Z3_mk_eq(ctx, y, strAst);
					Z3_ast implyR = Z3_mk_and(ctx, 2, item1);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
			} else {
				//            PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);

				Z3_ast to_double_check = Z3_mk_eq(ctx, nn1, nn2);
				Z3_ast to_double_check2 = Z3_mk_eq(ctx, nn2, nn1);

				if (!keeptry) {
				} else if (tempPatch) {
				} else if (num_try_map.find(to_double_check)
						== num_try_map.end()
						&& num_try_map.find(to_double_check2)
								== num_try_map.end()) {

#ifdef DEBUGLOG
					__debugPrint(logFile, "\n");
					__debugPrint(logFile, "#  Will process this equation again  #\n");
#endif

					num_try_map[to_double_check] = 1;
					num_try_map[to_double_check2] = 1;
					reassert_map[std::make_pair(new_nn1, new_nn2)] = true;
					reassert_map[std::make_pair(new_nn2, new_nn1)] = true;
					readdAxiom(t, to_double_check, __LINE__);
					return;
				} else if (num_try_map.find(to_double_check)->second
						< num_try) {

#ifdef DEBUGLOG
					__debugPrint(logFile, "\n");
					__debugPrint(logFile, "#  Will process this equation again  #\n");
#endif

					num_try_map[to_double_check]++;
					num_try_map[to_double_check2]++;
					reassert_map[std::make_pair(new_nn1, new_nn2)] = true;
					reassert_map[std::make_pair(new_nn2, new_nn1)] = true;
					readdAxiom(t, to_double_check, __LINE__);
					return;
				} else
					keeptry = false;

#ifdef DEBUGLOG
				__debugPrint(logFile, "\n");
				__debugPrint(logFile, "#  Reprocessing the equation #\n");
				printZ3Node(t, implyL);
				__debugPrint(logFile, "\n");
#endif
				Z3_ast implyR = Z3_theory_prop_len_to_str(t, len_x, len_m, x, y,
						m, strAst);
				if (implyR != NULL) {
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}

				Z3_ast temp1 = NULL;

				std::pair<Z3_ast, Z3_ast> key1(new_nn1, new_nn2);
				std::pair<Z3_ast, Z3_ast> key2(new_nn2, new_nn1);
				if (varForBreakConcat.find(key1) == varForBreakConcat.end()
						&& varForBreakConcat.find(key2)
								== varForBreakConcat.end()) {
					temp1 = my_mk_internal_string_var(t);
					xorFlag = mk_internal_xor_var(t);

					varForBreakConcat[key1][0] = temp1;
					varForBreakConcat[key1][1] = xorFlag;
				} else {
					if (varForBreakConcat.find(key1)
							!= varForBreakConcat.end()) {
						temp1 = varForBreakConcat[key1][0];
						xorFlag = varForBreakConcat[key1][1];
					} else if (varForBreakConcat.find(key2)
							!= varForBreakConcat.end()) {
						temp1 = varForBreakConcat[key2][0];
						xorFlag = varForBreakConcat[key2][1];
					}
				}

				std::string strValue = getConstStrValue(t, strAst);
				int optionTotal = 2 + strValue.length();
				Z3_ast * or_item = new Z3_ast[optionTotal];
				Z3_ast * and_item = new Z3_ast[1 + 6
						+ 4 * (strValue.length() + 1)];
				int option = 0;
				int pos = 1;

				Z3_ast temp1_strAst = mk_concat(t, temp1, strAst);
				//--------------------------------------------------------
				// m cut y
				//--------------------------------------------------------
				if (canTwoNodesEq(t, y, temp1_strAst)) {
					if (!avoidLoopCut || !(hasSelfCut(m, y))) {
						// break down option 2-1
						or_item[option] = Z3_mk_eq(ctx, xorFlag,
								mk_int(ctx, option));
						Z3_ast x_temp1 = mk_concat(t, x, temp1);
						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_eq(ctx, m, x_temp1));
						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_eq(ctx, y, temp1_strAst));

						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_gt(ctx, mk_length(t, m),
										mk_length(t, x)));

						option++;

						//"str_eq --> length_eq" constraints
						strEqLengthAxiom(t, m, x_temp1, __LINE__);
						strEqLengthAxiom(t, y, temp1_strAst, __LINE__);

						//Cut Info
						addCutInfoMerge(temp1, sLevel, y);
						addCutInfoMerge(temp1, sLevel, m);
						//                        addCutInfoMerge(x, sLevel, m);
					} else {
						loopDetected = true;
#ifdef DEBUGLOG
						__debugPrint(logFile, "-------------------\n");
						__debugPrint(logFile, "[AVOID Looping Cut] Skip @ %d.\n", __LINE__);
						printCutVAR(t, m);
						printCutVAR(t, y);
						__debugPrint(logFile, "-------------------\n");
#endif
					}
				}

				for (int i = 0; i <= (int) strValue.size(); i++) {
					std::string part1Str = strValue.substr(0, i);
					std::string part2Str = strValue.substr(i,
							strValue.size() - i);
					Z3_ast x_concat = mk_concat(t, m,
							my_mk_str_value(t, part1Str.c_str()));
					Z3_ast cropStr = my_mk_str_value(t, part2Str.c_str());
					if (canTwoNodesEq(t, x, x_concat)
							&& canTwoNodesEq(t, y, cropStr)) {
						// break down option 2-2
						or_item[option] = Z3_mk_eq(ctx, xorFlag,
								mk_int(ctx, option));
						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_eq(ctx, x, x_concat));
						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_eq(ctx, y, cropStr));
						option++;

						//"str_eq --> length_eq" constraints
						strEqLengthAxiom(t, y, cropStr, __LINE__);
						strEqLengthAxiom(t, x, x_concat, __LINE__);
					}
				}

				if (option > 0) {
					if (option == 1)
						and_item[0] = or_item[0];
					else
						and_item[0] = Z3_mk_or(ctx, option, or_item);
					Z3_ast implyR = Z3_mk_and(ctx, pos, and_item);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
				} else {
					__debugPrint(logFile,
							"\n[STOP @ %d] Should not split two EQ concats:",
							__LINE__);
					__debugPrint(logFile, "\n            ");
					printZ3Node(t, implyL);
					__debugPrint(logFile, "\n");
					return;
				}
				delete or_item;
				delete and_item;
				return;
			}
			return;
		}
	}

	//*************************************************************
	// case 3: concat(x, y) = concat("str", n)
	//*************************************************************
	if (duplicatedSplit == 0) {
		if ((isConstStr(t, v1_arg0) && !isConstStr(t, v1_arg1)
				&& !isConstStr(t, v2_arg0) && !isConstStr(t, v2_arg1))
				|| (isConstStr(t, v2_arg0) && !isConstStr(t, v2_arg1)
						&& !isConstStr(t, v1_arg0) && !isConstStr(t, v1_arg1))) {
			__debugPrint(logFile, "\n");
			__debugPrint(logFile, "#############################\n");
			__debugPrint(logFile, "#  SimplifyConcatEq Type 3  #\n");
			__debugPrint(logFile, "#############################\n");

			Z3_ast x = NULL;
			Z3_ast y = NULL;
			Z3_ast strAst = NULL;
			Z3_ast n = NULL;
			Z3_ast xorFlag = NULL;

			if (isConstStr(t, v1_arg0) && !isConstStr(t, v2_arg0)) {
				strAst = v1_arg0;
				n = v1_arg1;
				x = v2_arg0;
				y = v2_arg1;
			} else {
				strAst = v2_arg0;
				n = v2_arg1;
				x = v1_arg0;
				y = v1_arg1;
			}

			Z3_ast len_x = mk_length(t, x);
			printZ3Node(t, len_x);
			__debugPrint(logFile, "\n");
			len_x = get_eqc_num(t, len_x, 10);
			printZ3Node(t, len_x);
			__debugPrint(logFile, "\n");

			//Quick path
			if (getConstStrValue(t, strAst) == "") {

			} else if (isConstNum(t, len_x)) {
				if ((int) getConstStrValue(t, strAst).length()
						== getConstNumValue(t, len_x)) {
					Z3_ast item1[2];
					item1[0] = Z3_mk_eq(ctx, x, strAst);
					item1[1] = Z3_mk_eq(ctx, y, n);
					Z3_ast implyR = Z3_mk_and(ctx, 2, item1);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					return;
				}
			}

			// Split

			else {
				Z3_ast temp1 = NULL;
				std::pair<Z3_ast, Z3_ast> key1(new_nn1, new_nn2);
				std::pair<Z3_ast, Z3_ast> key2(new_nn2, new_nn1);
				if (varForBreakConcat.find(key1) == varForBreakConcat.end()
						&& varForBreakConcat.find(key2)
								== varForBreakConcat.end()) {
					temp1 = my_mk_internal_string_var(t);
					xorFlag = mk_internal_xor_var(t);

					varForBreakConcat[key1][0] = temp1;
					varForBreakConcat[key1][1] = xorFlag;
				} else {
					if (varForBreakConcat.find(key1)
							!= varForBreakConcat.end()) {
						temp1 = varForBreakConcat[key1][0];
						xorFlag = varForBreakConcat[key1][1];
					} else if (varForBreakConcat.find(key2)
							!= varForBreakConcat.end()) {
						temp1 = varForBreakConcat[key2][0];
						xorFlag = varForBreakConcat[key2][1];
					}
				}

				std::string strValue = getConstStrValue(t, strAst);
				int optionTotal = 2 + strValue.length();
				Z3_ast * or_item = new Z3_ast[optionTotal];
				int option = 0;
				Z3_ast * and_item = new Z3_ast[2 + 3 * optionTotal];
				int pos = 1;
				for (int i = 0; i <= (int) strValue.size(); i++) {
					std::string part1Str = strValue.substr(0, i);
					std::string part2Str = strValue.substr(i,
							strValue.size() - i);
					Z3_ast cropStr = my_mk_str_value(t, part1Str.c_str());
					Z3_ast y_concat = mk_concat(t,
							my_mk_str_value(t, part2Str.c_str()), n);

					if (canTwoNodesEq(t, x, cropStr)
							&& canTwoNodesEq(t, y, y_concat)) {
						// break down option 3-1
						Z3_ast x_eq_str = Z3_mk_eq(ctx, x, cropStr);
						or_item[option] = Z3_mk_eq(ctx, xorFlag,
								mk_int(ctx, option));
						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								x_eq_str);
						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_eq(ctx, y, y_concat));
						option++;
						//"str_eq --> length_eq" constraints
						strEqLengthAxiom(t, x, cropStr, __LINE__);
						strEqLengthAxiom(t, y, y_concat, __LINE__);
					}
				}

				Z3_ast strAst_temp1 = mk_concat(t, strAst, temp1);

				//--------------------------------------------------------
				// x cut n
				//--------------------------------------------------------
				if (canTwoNodesEq(t, x, strAst_temp1)) {
					if (!avoidLoopCut || !(hasSelfCut(x, n))) {
						// break down option 3-2
						or_item[option] = Z3_mk_eq(ctx, xorFlag,
								mk_int(ctx, option));

						Z3_ast temp1_y = mk_concat(t, temp1, y);
						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_eq(ctx, x, strAst_temp1));
						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_eq(ctx, n, temp1_y));

						and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
								Z3_mk_gt(ctx, mk_length(t, n),
										mk_length(t, y)));
						option++;

						//"str_eq --> length_eq" constraints
						strEqLengthAxiom(t, x, strAst_temp1, __LINE__);
						strEqLengthAxiom(t, n, temp1_y, __LINE__);

						//--- Cut Info----
						addCutInfoMerge(temp1, sLevel, x);
						addCutInfoMerge(temp1, sLevel, n);
						//                        addCutInfoMerge(y, sLevel, n);
					} else {
						loopDetected = true;
#ifdef DEBUGLOG
						__debugPrint(logFile, "-------------------\n");
						__debugPrint(logFile, "[AVOID Loop] Skip @ %d.\n", __LINE__);
						printCutVAR(t, x);
						printCutVAR(t, n);
						__debugPrint(logFile, "-------------------\n");
#endif
					}
				}

				if (option > 0) {
					if (option == 1)
						and_item[0] = or_item[0];
					else
						and_item[0] = Z3_mk_or(ctx, option, or_item);
					Z3_ast implyR = Z3_mk_and(ctx, pos, and_item);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
				} else {
					__debugPrint(logFile,
							"\n[STOP @ %d] Should not split two EQ concats:",
							__LINE__);
					__debugPrint(logFile, "\n            ");
					printZ3Node(t, implyL);
					__debugPrint(logFile, "\n");
					return;
				}
				delete or_item;
				delete and_item;
				return;
			}
			return;
		}
	}

	//*************************************************************
	//  case 4: concat("str1", y) = concat("str2", n)
	//*************************************************************
	if ((isConstStr(t, v1_arg0) && !isConstStr(t, v1_arg1)
			&& isConstStr(t, v2_arg0) && !isConstStr(t, v2_arg1))) {

		Z3_ast str1Ast = v1_arg0;
		Z3_ast y = v1_arg1;
		Z3_ast str2Ast = v2_arg0;
		Z3_ast n = v2_arg1;
		std::string str1Value = getConstStrValue(t, str1Ast);
		std::string str2Value = getConstStrValue(t, str2Ast);
		int str1Len = str1Value.length();
		int str2Len = str2Value.length();

		__debugPrint(logFile, "\n");
		__debugPrint(logFile, "#############################\n");
		__debugPrint(logFile, "#  SimplifyConcatEq Type 4  #\n");
		__debugPrint(logFile, "#############################\n");

		int commonLen = (str1Len > str2Len) ? str2Len : str1Len;
		if (str1Value.substr(0, commonLen) != str2Value.substr(0, commonLen)) {
			__debugPrint(logFile, ">> [simplifyConcatEq] Conflict: ");
			printZ3Node(t, new_nn1);
			__debugPrint(logFile, " = ");
			printZ3Node(t, new_nn2);
			__debugPrint(logFile, " @ %d\n\n", __LINE__);
			Z3_ast toNegate = Z3_mk_not(ctx, Z3_mk_eq(ctx, new_nn1, new_nn2));
			addAxiom(t, toNegate, __LINE__);
			return;
		} else {
			if (str1Len > str2Len) {
				std::string deltaStr = str1Value.substr(str2Len,
						str1Len - str2Len);
				Z3_ast tmpAst = mk_concat(t,
						my_mk_str_value(t, deltaStr.c_str()), y);
				if (!inSameEqc(t, tmpAst, n)) {
					// break down option 4-1
					Z3_ast implyR = Z3_mk_eq(ctx, n, tmpAst);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					strEqLengthAxiom(t, n, tmpAst, __LINE__);
				}
			} else if (str1Len == str2Len) {
				if (!inSameEqc(t, n, y)) {
					//break down option 4-2
					Z3_ast implyR = Z3_mk_eq(ctx, n, y);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					strEqLengthAxiom(t, n, y, __LINE__);
				}
			} else {
				std::string deltaStr = str2Value.substr(str1Len,
						str2Len - str1Len);
				Z3_ast tmpAst = mk_concat(t,
						my_mk_str_value(t, deltaStr.c_str()), n);
				if (!inSameEqc(t, y, tmpAst)) {
					//break down option 4-3
					Z3_ast implyR = Z3_mk_eq(ctx, y, tmpAst);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					strEqLengthAxiom(t, y, tmpAst, __LINE__);
				}
			}
		}
		return;
	}

	//*************************************************************
	//  case 5: concat(x, "str1") = concat(m, "str2")
	//*************************************************************
	if ((!isConstStr(t, v1_arg0) && isConstStr(t, v1_arg1)
			&& !isConstStr(t, v2_arg0) && isConstStr(t, v2_arg1))) {
		__debugPrint(logFile, "\n");
		__debugPrint(logFile, "#############################\n");
		__debugPrint(logFile, "#  SimplifyConcatEq Type 5  #\n");
		__debugPrint(logFile, "#############################\n");
		Z3_ast x = v1_arg0;
		Z3_ast str1Ast = v1_arg1;
		Z3_ast m = v2_arg0;
		Z3_ast str2Ast = v2_arg1;
		std::string str1Value = getConstStrValue(t, str1Ast);
		std::string str2Value = getConstStrValue(t, str2Ast);
		int str1Len = str1Value.length();
		int str2Len = str2Value.length();

		int cLen = (str1Len > str2Len) ? str2Len : str1Len;
		if (str1Value.substr(str1Len - cLen, cLen)
				!= str2Value.substr(str2Len - cLen, cLen)) {
			__debugPrint(logFile, ">> [simplifyConcatEq] Conflict: ");
			printZ3Node(t, new_nn1);
			__debugPrint(logFile, " = ");
			printZ3Node(t, new_nn2);
			__debugPrint(logFile, " @ %d\n\n", __LINE__);
			Z3_ast toNegate = Z3_mk_not(ctx, Z3_mk_eq(ctx, new_nn1, new_nn2));
			addAxiom(t, toNegate, __LINE__);
			return;
		} else {
			if (str1Len > str2Len) {
				std::string deltaStr = str1Value.substr(0, str1Len - str2Len);
				Z3_ast x_deltaStr = mk_concat(t, x,
						my_mk_str_value(t, deltaStr.c_str()));
				if (!inSameEqc(t, m, x_deltaStr)) {
					Z3_ast implyR = Z3_mk_eq(ctx, m, x_deltaStr);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					strEqLengthAxiom(t, m, x_deltaStr, __LINE__);
				}
			} else if (str1Len == str2Len) {
				// test
				if (!inSameEqc(t, x, m)) {
					Z3_ast implyR = Z3_mk_eq(ctx, x, m);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					strEqLengthAxiom(t, x, m, __LINE__);
				}
			} else {
				std::string deltaStr = str2Value.substr(0, str2Len - str1Len);
				Z3_ast m_deltaStr = mk_concat(t, m,
						my_mk_str_value(t, deltaStr.c_str()));
				if (!inSameEqc(t, x, m_deltaStr)) {
					Z3_ast implyR = Z3_mk_eq(ctx, x, m_deltaStr);
					addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
					strEqLengthAxiom(t, x, m_deltaStr, __LINE__);
				}
			}
		}
		return;
	}
	//*************************************************************
	//  case 6: concat("str1", y) = concat(m, "str2")
	//*************************************************************
	if (duplicatedSplit == 0) {
		if ((isConstStr(t, v1_arg0) && !isConstStr(t, v1_arg1)
				&& !isConstStr(t, v2_arg0) && isConstStr(t, v2_arg1))
				|| (isConstStr(t, v2_arg0) && !isConstStr(t, v2_arg1)
						&& !isConstStr(t, v1_arg0) && isConstStr(t, v1_arg1))) {
			__debugPrint(logFile, "\n");
			__debugPrint(logFile, "#############################\n");
			__debugPrint(logFile, "#  SimplifyConcatEq Type 6  #\n");
			__debugPrint(logFile, "#############################\n");

			Z3_ast str1Ast = NULL;
			Z3_ast y = NULL;
			Z3_ast m = NULL;
			Z3_ast str2Ast = NULL;

			if (isConstStr(t, v1_arg0)) {
				str1Ast = v1_arg0;
				y = v1_arg1;
				m = v2_arg0;
				str2Ast = v2_arg1;
			} else {
				str1Ast = v2_arg0;
				y = v2_arg1;
				m = v1_arg0;
				str2Ast = v1_arg1;
			}

			if (isStarFunc(t, m)) {
				solve_star_concat_sth_eq_str_concat_sth(t, m, str2Ast, str1Ast,
						y, implyL, 1);
				return;
			}

			std::string str1Value = getConstStrValue(t, str1Ast);
			std::string str2Value = getConstStrValue(t, str2Ast);
			//----------------------------------------
			//(a)  |---str1---|----y----|
			//     |--m--|-----str2-----|
			//
			//(b)  |---str1---|----y----|
			//     |-----m----|--str2---|
			//
			//(c)  |---str1---|----y----|
			//     |------m------|-str2-|
			//----------------------------------------
			std::list<int> overlapLen;
			overlapLen.push_back(0);
			int str1Len = str1Value.length();
			int str2Len = str2Value.length();
			for (int i = 1; i <= str1Len && i <= str2Len; i++) {
				if (str1Value.substr(str1Len - i, i) == str2Value.substr(0, i))
					overlapLen.push_back(i);
			}

			//----------------------------------------------------------------
			Z3_ast commonVar = NULL;
			Z3_ast xorFlag = NULL;
			std::pair<Z3_ast, Z3_ast> key1(new_nn1, new_nn2);
			std::pair<Z3_ast, Z3_ast> key2(new_nn2, new_nn1);
			if (varForBreakConcat.find(key1) == varForBreakConcat.end()
					&& varForBreakConcat.find(key2)
							== varForBreakConcat.end()) {
				commonVar = my_mk_internal_string_var(t);
				xorFlag = mk_internal_xor_var(t);
				varForBreakConcat[key1][0] = commonVar;
				varForBreakConcat[key1][1] = xorFlag;
			} else {
				if (varForBreakConcat.find(key1) != varForBreakConcat.end()) {
					commonVar = varForBreakConcat[key1][0];
					xorFlag = varForBreakConcat[key1][1];
				} else {
					commonVar = varForBreakConcat[key2][0];
					xorFlag = varForBreakConcat[key2][1];
				}
			}
			Z3_ast * or_item = new Z3_ast[overlapLen.size() + 1];
			int option = 0;
			Z3_ast * and_item = new Z3_ast[1 + 4 * (overlapLen.size() + 1)];
			int pos = 1;

			if (!avoidLoopCut || !hasSelfCut(m, y)) {
				or_item[option] = Z3_mk_eq(ctx, xorFlag, mk_int(ctx, option));

				Z3_ast str1_commonVar = mk_concat(t, str1Ast, commonVar);
				and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
						Z3_mk_eq(ctx, m, str1_commonVar));
				strEqLengthAxiom(t, m, str1_commonVar, __LINE__);

				Z3_ast commonVar_str2 = mk_concat(t, commonVar, str2Ast);
				and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
						Z3_mk_eq(ctx, y, commonVar_str2));
				strEqLengthAxiom(t, y, commonVar_str2, __LINE__);

				and_item[pos++] = Z3_mk_eq(ctx, or_item[option],
						Z3_mk_gt(ctx, mk_length(t, m), mk_length(t, str1Ast)));
				//                and_item[pos++] = Z3_mk_eq(ctx, or_item[option], Z3_mk_gt(ctx, mk_length(t, y), mk_length(t, str2Ast)));

				option++;
			} else {
				loopDetected = true;
#ifdef DEBUGLOG
				__debugPrint(logFile, "-------------------\n");
				__debugPrint(logFile, "[AVOID Loop] Skip @ %d.\n", __LINE__);
				printCutVAR(t, m);
				printCutVAR(t, y);
				__debugPrint(logFile, "-------------------\n");
#endif
			}

			for (std::list<int>::iterator itor = overlapLen.begin();
					itor != overlapLen.end(); itor++) {
				int overLen = *itor;
				std::string prefix = str1Value.substr(0, str1Len - overLen);
				std::string suffix = str2Value.substr(overLen,
						str2Len - overLen);
				or_item[option] = Z3_mk_eq(ctx, xorFlag, mk_int(ctx, option));

				Z3_ast prefixAst = my_mk_str_value(t, prefix.c_str());
				Z3_ast x_eq_prefix = Z3_mk_eq(ctx, m, prefixAst);
				and_item[pos++] = Z3_mk_eq(ctx, or_item[option], x_eq_prefix);
				strEqLengthAxiom(t, m, prefixAst, __LINE__);

				Z3_ast suffixAst = my_mk_str_value(t, suffix.c_str());
				Z3_ast y_eq_surfix = Z3_mk_eq(ctx, y, suffixAst);
				and_item[pos++] = Z3_mk_eq(ctx, or_item[option], y_eq_surfix);
				strEqLengthAxiom(t, y, suffixAst, __LINE__);

				option++;
			}

			//  case 6: concat("str1", y) = concat(m, "str2")

			and_item[0] = Z3_mk_or(ctx, option, or_item);
			Z3_ast implyR = Z3_mk_and(ctx, pos, and_item);
			addAxiom(t, Z3_mk_implies(ctx, implyL, implyR), __LINE__);
			delete or_item;
			delete and_item;
			return;
		}
	}
}

/*
 *
 */
void printContext(Z3_theory t) {
#ifdef DEBUGLOG
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast ctxAssign = Z3_get_context_assignment(ctx);
	__debugPrint(logFile, "\n\n== Context ====================================\n");
	printZ3Node(t, ctxAssign);
	__debugPrint(logFile, "\n===============================================\n\n");
#endif
}

/*
 * Process two nodes that are assumed to be equal by Z3
 */
void handleNodesEqual(Z3_theory t, Z3_ast v1, Z3_ast v2) {
	//  __debugPrint(logFile, "\n==================\n>> handleNodesEqual\n==================\n");
	if (v1 == v2)
		return;

	T_myZ3Type v1_Type = getNodeTypeCustom(t, v1);
	T_myZ3Type v2_Type = getNodeTypeCustom(t, v2);
	//**********************************************************
	// Concat(... , ....) = Constant String
	//----------------------------------------------------------
	if (v1_Type == my_Z3_Concat && v2_Type == my_Z3_ConstStr) {
		solve_concat_eq_str(t, v1, v2);
	} else if (v2_Type == my_Z3_Concat && v1_Type == my_Z3_ConstStr) {
		solve_concat_eq_str(t, v2, v1);
	}
	//**********************************************************
	// Concat(... , ....) = Concat(... , ... )
	//----------------------------------------------------------
	else if (v1_Type == my_Z3_Concat && v2_Type == my_Z3_Concat) {
		simplifyConcatEq(t, v1, v2);
	} else if (v1_Type == my_Z3_Star && v2_Type == my_Z3_ConstStr) {
		solve_star_eq_str(t, v1, v2);
	} else if (v2_Type == my_Z3_Star && v1_Type == my_Z3_ConstStr) {
		solve_star_eq_str(t, v2, v1);
	}
	//  else if (v1_Type == my_Z3_Star)
	//  {
	//      solve_star_eq_var(t, v1, v2);
	//  }
	//  else if (v2_Type == my_Z3_Star)
	//  {
	//      solve_star_eq_var(t, v2, v1);
	//  }
	else if (v1_Type == my_Z3_Search) {
		solve_search_eq_sth(t, v1, v2);
	} else if (v2_Type == my_Z3_Search) {
		solve_search_eq_sth(t, v2, v1);
	} else if (v1_Type == my_Z3_ReplaceAll) {
		solve_replaceall_eq_sth(t, v1, v2);
	} else if (v2_Type == my_Z3_ReplaceAll) {
		solve_replaceall_eq_sth(t, v2, v1);
	} else if (v1_Type == my_Z3_SubString) {
		solve_subStr_eq_sth(t, v1, v2);
	} else if (v2_Type == my_Z3_SubString) {
		solve_subStr_eq_sth(t, v2, v1);
	}
}

//==================================================
// Should do equal check among eqc members of nn1 and nn2
// to discover incorrect nn1 = nn2, e.g
//--------------------------------------------------
//** cb_new_eq() : y2 = _t_str3
// * [EQC] : { y2, (Concat ce m2) }, size = 2
// * [EQC] : { _t_str3, (Concat abc x2) }, size = 2
//--------------------------------------------------
// y2 can not be equal to _t_str3.
// Add an assertion: {y2 = (Concat ce m2)} /\ {_t_str3 = (Concat abc x2)} --> y2 != _t_str3
//==================================================
int newEqCheck(Z3_theory t, Z3_ast nn1, Z3_ast nn2) {
	/*  A running example: concat-042
	 *  ===============================================
	 *  ** cb_new_eq():    (Concat _t_str0 _t_str1) = (Concat _t_str0 d)
	 *  ===============================================
	 *    EQC={ (Concat _t_str0 _t_str1), (Concat v1 v2), (Concat v1 b) }
	 *    EQC={ (Concat _t_str0 d) }
	 *  ===============================================
	 */
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast eqc_nn1 = nn1;
	do {
		Z3_ast eqc_nn2 = nn2;
		do {
			if (!canTwoNodesEq(t, eqc_nn1, eqc_nn2)) {
				Z3_ast l_item[3];
				int l_pos = 0;
				if (nn1 != eqc_nn1)
					l_item[l_pos++] = Z3_mk_eq(ctx, nn1, eqc_nn1);
				if (nn2 != eqc_nn2)
					l_item[l_pos++] = Z3_mk_eq(ctx, nn2, eqc_nn2);
				Z3_ast toAssert = NULL;

				l_item[l_pos++] = Z3_mk_eq(ctx, nn1, nn2);
				toAssert = Z3_mk_not(ctx, my_mk_and(t, l_item, l_pos));

				__debugPrint(logFile, "\n");
				__debugPrint(logFile,
						">> Inconsistent detected in newEqCheck:\n");
				addAxiom(t, toAssert, __LINE__);
				__debugPrint(logFile, "\n\n");

				return -1;
			}
			eqc_nn2 = Z3_theory_get_eqc_next(t, eqc_nn2);
		} while (eqc_nn2 != nn2);
		eqc_nn1 = Z3_theory_get_eqc_next(t, eqc_nn1);
	} while (eqc_nn1 != nn1);
	return 0;
}

/*
 * In cb_new_eq, when _t_len_varX = "more", more len tests are needed for varX
 */
void moreLenTests(Z3_theory t, Z3_ast lenTester, std::string lenTesterValue) {
	if (lenTesterFvarMap.find(lenTester) != lenTesterFvarMap.end()) {
		Z3_ast fVar = lenTesterFvarMap[lenTester];
		Z3_context ctx = Z3_theory_get_context(t);
		std::string vName = std::string(Z3_ast_to_string(ctx, fVar));
		bool isStar = false;
		if (vName.length() >= 7) {
			std::string vPrefix = vName.substr(0, 7);
			if (vPrefix == "_t_star")
				isStar = true;
		}
		Z3_ast toAssert = genLenValOptionsForFreeVar(t, fVar, lenTester,
				lenTesterValue, isStar);

		addAxiom(t, toAssert, __LINE__, false);

#ifdef DEBUGLOG
		__debugPrint(logFile, "\n--------moreLenTests-------------\n");
		__debugPrint(logFile, ">> Var: ");
		printZ3Node(t, fVar);
		__debugPrint(logFile," (@%d, Level %d):\n ", __LINE__, sLevel);
		printZ3Node(t, toAssert);
		__debugPrint(logFile, "\n---------------------\n");
#endif
	}
}

/*
 *
 */
void moreValueTests(Z3_theory t, Z3_ast valTester, std::string valTesterValue) {
	Z3_ast fVar = valueTesterFvarMap[valTester];
	int lenTesterCount = fvarLenTesterMap[fVar].size();

	Z3_ast effectiveLenInd = NULL;
	std::string effectiveLenIndiStr = "";
	for (int i = 0; i < lenTesterCount; i++) {
		Z3_ast len_indicator_pre = fvarLenTesterMap[fVar][i];
		Z3_ast len_indicator_value = get_eqc_value(t, len_indicator_pre);
		if (len_indicator_pre != len_indicator_value) {
			std::string len_pIndiStr = getConstStrValue(t, len_indicator_value);
			if (len_pIndiStr != "more") {
				effectiveLenInd = len_indicator_pre;
				effectiveLenIndiStr = len_pIndiStr;
				break;
			}
		}
	}
	Z3_ast valueAssert = genFreeVarOptions(t, fVar, effectiveLenInd,
			effectiveLenIndiStr, valTester, valTesterValue);
	addAxiom(t, valueAssert, __LINE__, false);

#ifdef DEBUGLOG
	__debugPrint(logFile, "\n-----------moreValueTests----------\n");
	__debugPrint(logFile, ">> Var: ");
	printZ3Node(t, fVar);
	__debugPrint(logFile," (@%d, Level %d):\n ", __LINE__, sLevel);
	printZ3Node(t, valueAssert);
	__debugPrint(logFile, "\n---------------------\n");
#endif
}

/*
 *
 */
void cb_new_eq(Z3_theory t, Z3_ast nn1, Z3_ast nn2) {
#ifdef DEBUGLOG

	//    print_All_Eqc(t);

	std::map<Z3_ast, int> nodesMap;
	__debugPrint(logFile, "\n\n===============================================\n");
	__debugPrint(logFile, "** cb_new_eq():    ");
	printZ3Node(t, nn1);
	__debugPrint(logFile, " = ");
	printZ3Node(t, nn2);
	//    __debugPrint(logFile, "\n-----------------------------------------------\n");
	//    print_eq_class(t, nn1);
	//    __debugPrint(logFile, "\n");
	//    print_eq_class(t, nn2);
	__debugPrint(logFile, "\n");
	__debugPrint(logFile, "===============================================\n");
#endif

	Z3_context ctx = Z3_theory_get_context(t);
	if (getNodeType(t, nn1) == my_Z3_Str_Var) {
		std::string vName = std::string(Z3_ast_to_string(ctx, nn1));

		if (vName.length() >= 6) //  && ( == "_t_len" || vName.substr(0, 6) == "_t_val"))
				{
			std::string vPrefix = vName.substr(0, 6);
			if (vPrefix == "_t_len") {
				if (isConstStr(t, nn2)) {
					Z3_ast freeVar = lenTesterFvarMap[nn1];
					Z3_ast len_freeVar = mk_length(t, freeVar);
					Z3_ast num = get_eqc_num(t, len_freeVar, 11);
					if (num != len_freeVar) {
						int i = getConstNumValue(t, num);
						__debugPrint(logFile, "\nKnown Length: %d\n", i);
						Z3_ast toAssert = genValOptions(t, freeVar, i);
						addAxiom(t, toAssert, __LINE__, false);
						return;
					}
					std::string s = getConstStrValue(t, nn2);
					if (s == "more")
						moreLenTests(t, nn1, s);
					else {
						Z3_ast valueAssert = genFreeVarOptions(t, freeVar, nn1,
								s, NULL, "");
						addAxiom(t, valueAssert, __LINE__, false);
					}
				}
				return;
			} else if (vPrefix == "_t_val") {
				if (isConstStr(t, nn2) && "more" == getConstStrValue(t, nn2)) {
					moreValueTests(t, nn1, getConstStrValue(t, nn2));
				}
				return;
			}
		}
	}

	if (getNodeType(t, nn2) == my_Z3_Str_Var) {
		std::string vName = std::string(Z3_ast_to_string(ctx, nn2));
		if (vName.length() >= 6) {
			std::string vPrefix = vName.substr(0, 6);
			if (vPrefix == "_t_len") {
				if (isConstStr(t, nn1)) {
					Z3_ast freeVar = lenTesterFvarMap[nn2];
					Z3_ast len_freeVar = mk_length(t, freeVar);
					Z3_ast num = get_eqc_num(t, len_freeVar, 12);
					if (num != len_freeVar) {
						int i = getConstNumValue(t, num);
						__debugPrint(logFile, "\nKnown Length: %d\n", i);
						Z3_ast toAssert = genValOptions(t, freeVar, i);
						addAxiom(t, toAssert, __LINE__, false);
						return;
					}
					std::string s = getConstStrValue(t, nn1);
					if (s == "more")
						moreLenTests(t, nn2, s);
					else {
						Z3_ast valueAssert = genFreeVarOptions(t, freeVar, nn2,
								s, NULL, "");
						addAxiom(t, valueAssert, __LINE__, false);
					}
				}
				return;
			} else if (vPrefix == "_t_val") {
				if (isConstStr(t, nn1) && "more" == getConstStrValue(t, nn1)) {
					moreValueTests(t, nn2, getConstStrValue(t, nn1));
				}
				return;
			}
		}
	}

	// should do the consistent check first
	if (newEqCheck(t, nn1, nn2) == -1) {
		return;
	}

	//#ifdef DEBUGLOG
	//  __debugPrint(logFile, ">> New_eq Check: PASS\n\n");
	//#endif

	//"str_eq --> length_eq" constraints
	strEqLengthAxiom(t, nn1, nn2, __LINE__);

	Z3_ast eqc_nn1 = nn1;
	Z3_ast eqc_nn2 = nn2;
	do {
		eqc_nn2 = nn2;
		do {
			handleNodesEqual(t, eqc_nn1, eqc_nn2);
			eqc_nn2 = Z3_theory_get_eqc_next(t, eqc_nn2);
		} while (eqc_nn2 != nn2);
		eqc_nn1 = Z3_theory_get_eqc_next(t, eqc_nn1);
	} while (eqc_nn1 != nn1);

	//----------------------------------------------
	// A possible new_eq order:
	//   (1) v1 = "const": use "const" to simplify nodes having v1
	//   (2) v2 = v1:
	//       If we only check whether v1 or v2 is constant, we will miss the chance to
	//       simplify nodes with v2 since eqc(v1)="const"
	// Therefore, let's look at the eqc value of nn1 and nn2.
	//----------------------------------------------
	Z3_ast nn1_value = get_eqc_value(t, nn1);
	if (isConstStr(t, nn1_value) && !isConstStr(t, nn2)) {
		simplifyConcatStr(t, nn2, nn1_value);
	}

	Z3_ast nn2_value = get_eqc_value(t, nn2);
	if (isConstStr(t, nn2_value) && !isConstStr(t, nn1)) {
		simplifyConcatStr(t, nn1, nn2_value);
	}
}

/*
 * Add axioms that are true for any string var
 */
void basicStrVarAxiom(Z3_theory t, Z3_ast vNode, int line) {
	if (basicStrVarAxiom_added.find(vNode) == basicStrVarAxiom_added.end()) {
		Z3_context ctx = Z3_theory_get_context(t);
		Z3_ast lenTerm = mk_length(t, vNode);
		Z3_ast i = Z3_mk_int(ctx, 0, Z3_mk_int_sort(ctx));
		//Z3_ast i = mk_int(ctx, 0);
		/*Z3_ast strlen_ge = Z3_mk_ge(ctx, lenTerm, mk_int(ctx, 0));
		addAxiom(t, strlen_ge, line, false);

		Z3_ast strLen_zero = Z3_mk_eq(ctx, lenTerm, mk_int(ctx, 0));
		Z3_ast str_empty = Z3_mk_eq(ctx, vNode, my_mk_str_value(t, ""));
		Z3_ast str_eq_ast2 = Z3_mk_eq(ctx, strLen_zero, str_empty);
		addAxiom(t, str_eq_ast2, line, false);

		basicStrVarAxiom_added[vNode] = 1;*/
	}
}

/*
 * Add axioms that are true for any string var
 */
void basicConcatAxiom(Z3_theory t, Z3_ast vNode, int line) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast lenAst = mk_length(t, vNode);
	std::list<Z3_ast> astList;
	getconstStrAstsInNode(t, vNode, astList);
	int len = 0;
	std::list<Z3_ast>::iterator itor = astList.begin();
	for (; itor != astList.end(); itor++)
		len += getConstStrValue(t, (*itor)).length();

	if (len != 0)
		addAxiom(t, Z3_mk_ge(ctx, lenAst, mk_int(ctx, len)), __LINE__, false);
}

/*
 * Mark variable appeared in map "varAppearMap"
 */
void classifyAstByType(Z3_theory t, Z3_ast node, std::map<Z3_ast, int> & varMap,
		std::map<Z3_ast, int> & concatMap) {
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_context ctx = Z3_theory_get_context(t);
	T_myZ3Type nodeType = (T_myZ3Type) getNodeType(t, node);

	if (nodeType == my_Z3_Str_Var) {
		std::string vName = std::string(Z3_ast_to_string(ctx, node));
		if (!(vName.length() >= 6
				&& (vName.substr(0, 6) == "_t_len"
						|| vName.substr(0, 6) == "_t_val"))) {
			varMap[node] = 1;
		}
	} else if (getNodeType(t, node) == my_Z3_Func) {
		Z3_app func_app = Z3_to_app(ctx, node);
		Z3_func_decl funcDecl = Z3_get_app_decl(ctx, func_app);

		if (funcDecl == td->Length) {
			return;
		}

		if (funcDecl == td->Concat) {
			if (concatMap.find(node) == concatMap.end())
				concatMap[node] = 1;
		}
		int argCount = Z3_get_app_num_args(ctx, func_app);
		for (int i = 0; i < argCount; i++) {
			Z3_ast argAst = Z3_get_app_arg(ctx, func_app, i);
			classifyAstByType(t, argAst, varMap, concatMap);
		}
	} else {
		//#ifdef DEBUGLOG
		//        __debugPrint(logFile, "   >> [SKIP-CT] ");
		//        printZ3Node(t, node);
		//        __debugPrint(logFile, " @ %d\n", __LINE__);
		//#endif
	}
}

/*
 *
 */
bool isInterestingFuncKind(Z3_decl_kind func_decl) {
	bool result = true;
	switch (func_decl) {
	case Z3_OP_EQ:
		result = true;
		break;
	default:
		result = false;
		//        case Z3_OP_DISTINCT:
		//        case Z3_OP_ITE:
		//        case Z3_OP_AND:
		//        case Z3_OP_OR:
		//        case Z3_OP_IFF:
		//        case Z3_OP_XOR:
		//        case Z3_OP_NOT:
		//        case Z3_OP_IMPLIES:
		//            result = false;
		//            break;
		//        default:
		//            result = true;
	}
	return result;
}

void classifyAstByTypeInPositiveContext(Z3_theory t, Z3_ast node,
		std::map<Z3_ast, int> & varMap, std::map<Z3_ast, int> & concatMap) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast ctxAssign = Z3_get_context_assignment(ctx);

	if (Z3_get_decl_kind(ctx, Z3_get_app_decl(ctx, Z3_to_app(ctx, ctxAssign)))
			!= Z3_OP_AND) {
		if (getNodeType(t, ctxAssign) == my_Z3_Func) {
			Z3_app func_app = Z3_to_app(ctx, ctxAssign);
			Z3_decl_kind func_decl = Z3_get_decl_kind(ctx,
					Z3_get_app_decl(ctx, func_app));
			if (isInterestingFuncKind(func_decl)) {
				classifyAstByType(t, ctxAssign, varMap, concatMap);
			}
		}
	} else {
		int argCount = Z3_get_app_num_args(ctx, Z3_to_app(ctx, ctxAssign));
		for (int i = 0; i < argCount; i++) {
			Z3_ast argAst = Z3_get_app_arg(ctx, Z3_to_app(ctx, ctxAssign), i);
			if (getNodeType(t, argAst) == my_Z3_Func) {
				Z3_app func_app = Z3_to_app(ctx, argAst);
				Z3_decl_kind func_decl = Z3_get_decl_kind(ctx,
						Z3_get_app_decl(ctx, func_app));

				if (isInterestingFuncKind(func_decl)) {
					classifyAstByType(t, argAst, varMap, concatMap);
				}
			}
		}
	}

}

/*
 *
 */
inline Z3_ast getAliasIndexAst(std::map<Z3_ast, Z3_ast> & aliasIndexMap,
		Z3_ast node) {
	if (aliasIndexMap.find(node) != aliasIndexMap.end())
		return aliasIndexMap[node];
	else
		return node;
}

/*
 *
 */

void getNodesInConcat(Z3_theory t, Z3_ast node,
		std::vector<Z3_ast> & nodeList) {
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_context ctx = Z3_theory_get_context(t);
	if (getNodeType(t, node) != my_Z3_Func
			|| (getNodeType(t, node) == my_Z3_Func
					&& Z3_get_app_decl(ctx, Z3_to_app(ctx, node)) != td->Concat)) {
		nodeList.push_back(node);
		return;
	} else {
		Z3_ast leftArg = Z3_get_app_arg(ctx, Z3_to_app(ctx, node), 0);
		Z3_ast rightArg = Z3_get_app_arg(ctx, Z3_to_app(ctx, node), 1);
		getNodesInConcat(t, leftArg, nodeList);
		getNodesInConcat(t, rightArg, nodeList);
	}
}

Z3_ast getMostLeftNodeInConcat(Z3_theory t, Z3_ast node) {
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_context ctx = Z3_theory_get_context(t);

	if (getNodeType(t, node) != my_Z3_Func
			|| (getNodeType(t, node) == my_Z3_Func
					&& Z3_get_app_decl(ctx, Z3_to_app(ctx, node)) != td->Concat))
		return node;
	else {
		Z3_ast concatArgL = Z3_get_app_arg(ctx, Z3_to_app(ctx, node), 0);
		return getMostLeftNodeInConcat(t, concatArgL);
	}
}

/*
 *
 */
Z3_ast getMostRightNodeInConcat(Z3_theory t, Z3_ast node) {
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_context ctx = Z3_theory_get_context(t);

	if (getNodeType(t, node) != my_Z3_Func
			|| (getNodeType(t, node) == my_Z3_Func
					&& Z3_get_app_decl(ctx, Z3_to_app(ctx, node)) != td->Concat))
		return node;
	else {
		Z3_ast concatArgR = Z3_get_app_arg(ctx, Z3_to_app(ctx, node), 1);
		return getMostRightNodeInConcat(t, concatArgR);
	}
}

/*
 *
 */
// simplifyConcat reduce a concat node in a bottom-up approach.
//   * replace variable with its equivalence const string
//   * eliminate concat if possible
Z3_ast simplifyConcat1(Z3_theory t, Z3_ast node) {
	Z3_context ctx = Z3_theory_get_context(t);
	if (!isConcatFunc(t, node)) {
		return node;
	}

	Z3_ast arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, node), 0);
	Z3_ast arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, node), 1);
	Z3_ast r0 = simplifyConcat1(t, arg0);
	Z3_ast r1 = simplifyConcat1(t, arg1);
	// mk_concat will simplify the concat base on the types of r0 and r1
	return mk_concat(t, r0, r1);
}

/*
 *
 */
// simplifyConcat2 reduce a concat node in a bottom-up approach.
//   * de-aliasing varaible
//   * replace variable with its equivalence const string
//   * eliminate concat if possible
Z3_ast simplifyConcat2(Z3_theory t, std::map<Z3_ast, Z3_ast> & aliasMap,
		Z3_ast node) {
	Z3_context ctx = Z3_theory_get_context(t);
	if (!isConcatFunc(t, node)) {
		return getAliasIndexAst(aliasMap, node);
	}

	Z3_ast arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, node), 0);
	Z3_ast arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, node), 1);
	Z3_ast r0 = simplifyConcat2(t, aliasMap, arg0);
	Z3_ast r1 = simplifyConcat2(t, aliasMap, arg1);
	// mk_concat will simplify the concat base on the types of r0 and r1
	return mk_concat(t, r0, r1);
}

/*
 *
 */
bool hasLengthOfNode(Z3_theory t, Z3_ast n, std::map<Z3_ast, int> & wanted) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	Z3_func_decl d = Z3_get_app_decl(ctx, Z3_to_app(ctx, n));
	int argCount = Z3_get_app_num_args(ctx, Z3_to_app(ctx, n));

	if (argCount == 0) {
		return false;
	} else {
		if (d == td->Length) {
			if (wanted.find(Z3_get_app_arg(ctx, Z3_to_app(ctx, n), 0))
					!= wanted.end()) {
				return true;
			}
		} else {
			bool result = false;
			for (int i = 0; i < argCount; i++) {
				Z3_ast argAst = Z3_get_app_arg(ctx, Z3_to_app(ctx, n), i);
				result = result || hasLengthOfNode(t, argAst, wanted);
			}
			return result;
		}
	}
	return false;
}

/*
 *
 */
void print_relevant_length(Z3_theory t, std::map<Z3_ast, int> & wanted) {
#ifdef DEBUGLOG
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast ctxAssign = Z3_get_context_assignment(ctx);

	__debugPrint(logFile, "\n=== Relevant Length ===========================\n");

	std::list<Z3_ast> nodeList;
	if (Z3_get_decl_kind(ctx, Z3_get_app_decl(ctx, Z3_to_app(ctx, ctxAssign))) != Z3_OP_AND) {
		if (hasLengthOfNode(t, ctxAssign, wanted)) {
			nodeList.push_back(ctxAssign);
		}
	} else {
		int argCount = Z3_get_app_num_args(ctx, Z3_to_app(ctx, ctxAssign));
		for (int i = 0; i < argCount; i++) {
			Z3_ast argAst = Z3_get_app_arg(ctx, Z3_to_app(ctx, ctxAssign), i);
			if (hasLengthOfNode(t, argAst, wanted)) {
				nodeList.push_back(argAst);
			}
		}
	}

	Z3_ast result = NULL;
	int pos = 0;
	Z3_ast * items = new Z3_ast[nodeList.size()];
	for (std::list<Z3_ast>::iterator itor = nodeList.begin(); itor != nodeList.end(); itor++) {
		items[pos++] = *itor;
	}
	if (pos == 0)
	result = NULL;
	else if (pos == 1)
	result = items[0];
	else
	result = Z3_mk_and(ctx, pos, items);
	delete[] items;
	if (result != NULL) {
		printZ3Node(t, result);
	}
	__debugPrint(logFile, "\n===============================================\n");
#endif
}

/*
 *
 */
void print_All_Eqc(Z3_theory t) {
#ifdef DEBUGLOG
	std::map<Z3_ast, int> strVarMap;
	std::map<Z3_ast, int> concatMap;
	std::map<Z3_ast, int> printedMap;

	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast ctxAssign = Z3_get_context_assignment(ctx);
	classifyAstByType(t, ctxAssign, strVarMap, concatMap);

	__debugPrint(logFile, "\n\n=== EQC =======================================\n");

	std::map<Z3_ast, int>::iterator itor = strVarMap.begin();
	for (; itor != strVarMap.end(); itor++) {
		if (printedMap.find(itor->first) != printedMap.end())
		continue;

		int count = 0;
		Z3_ast curr = itor->first;
		do {
			count++;
			if (count > 1)
			break;
			curr = Z3_theory_get_eqc_next(t, curr);
		}while (curr != itor->first);

		if (count > 1) {
			if (get_eqc_value(t, itor->first) != itor->first) {
				__debugPrint(logFile, "  > ");
			} else {
				__debugPrint(logFile, "    ");
			}
			__debugPrint(logFile, "{ ");
			Z3_ast curr = itor->first;
			do {
				printedMap[curr] = 1;
				printZ3Node(t, curr);
				__debugPrint(logFile, ",  ");
				curr = Z3_theory_get_eqc_next(t, curr);
			}while (curr != itor->first);
			__debugPrint(logFile, "}\n");
		}
	}

	itor = concatMap.begin();
	for (; itor != concatMap.end(); itor++) {
		if (printedMap.find(itor->first) != printedMap.end())
		continue;

		int count = 0;
		Z3_ast curr = itor->first;
		do {
			count++;
			if (count > 1)
			break;
			curr = Z3_theory_get_eqc_next(t, curr);
		}while (curr != itor->first);

		if (count > 1) {
			if (get_eqc_value(t, itor->first) != itor->first) {
				__debugPrint(logFile, "  > ");
			} else {
				__debugPrint(logFile, "    ");
			}
			__debugPrint(logFile, "{ ");
			Z3_ast curr = itor->first;
			do {
				printedMap[curr] = 1;
				printZ3Node(t, curr);
				__debugPrint(logFile, ",  ");
				curr = Z3_theory_get_eqc_next(t, curr);
			}while (curr != itor->first);
			__debugPrint(logFile, "}\n");
		}
	}
	__debugPrint(logFile, "===============================================\n");
#endif
}

/*
 * Dependence analysis from current context assignment
 */
int ctxDepAnalysis(Z3_theory t, std::map<Z3_ast, int> & strVarMap,
		std::map<Z3_ast, int> & concatMap,
		std::map<Z3_ast, Z3_ast> & aliasIndexMap,
		std::map<Z3_ast, Z3_ast> & var_eq_constStr_map,
		std::map<Z3_ast, std::map<Z3_ast, int> > & var_eq_concat_map,
		std::map<Z3_ast, std::map<Z3_ast, int> > & var_eq_star_map,
		std::map<Z3_ast, Z3_ast> & concat_eq_constStr_map,
		std::map<Z3_ast, std::map<Z3_ast, int> > & concat_eq_concat_map,
		std::map<Z3_ast, int> & freeVarMap,
		std::map<Z3_ast, std::map<Z3_ast, int> > & depMap,
		std::map<std::pair<Z3_ast, Z3_ast>, std::pair<Z3_ast, Z3_ast> > & toBreakMap) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast ctxAssign = Z3_get_context_assignment(ctx);

#ifdef DEBUGLOG
	__debugPrint(logFile, "\n******************************************\n");
	__debugPrint(logFile, "       Dependence Analysis\n");
	__debugPrint(logFile, "------------------------------------------\n");
#endif

	//--------------------------------------------
	// Step 1. get variables/Concat AST appeared in context
	//--------------------------------------------
	//    classifyAstByType(t, ctxAssign, strVarMap, concatMap);

	for (std::map<Z3_ast, int>::iterator it = inputVarMap.begin();
			it != inputVarMap.end(); it++) {
		strVarMap[it->first] = 1;
	}

	classifyAstByTypeInPositiveContext(t, ctxAssign, strVarMap, concatMap);

	//--------------------------------------------
	// Step 2. Collect alias relation
	// e.g EQC={x, y, z}
	//     aliasIndexMap[y] = x
	//     aliasIndexMap[z] = x
	std::map<Z3_ast, int>::iterator varItor = strVarMap.begin();
	for (; varItor != strVarMap.end(); varItor++) {
		if (aliasIndexMap.find(varItor->first) != aliasIndexMap.end())
			continue;

		Z3_ast aRoot = NULL;
		Z3_ast curr = varItor->first;
		do {
			if (getNodeType(t, curr) == my_Z3_Str_Var) {
				if (aRoot == NULL)
					aRoot = curr;
				else
					aliasIndexMap[curr] = aRoot;
			}
			curr = Z3_theory_get_eqc_next(t, curr);
		} while (curr != varItor->first);
	}

	//--------------------------------------------
	// Step 3: Collect interested cases
	varItor = strVarMap.begin();
	for (; varItor != strVarMap.end(); varItor++) {
		Z3_ast deAliasNode = getAliasIndexAst(aliasIndexMap, varItor->first);
		// (1) var_eq_constStr
		//     e.g,  z = "str1"
		//           var_eq_constStr_map[z] = "str1"
		//--------------------------------------------------------------
		if (var_eq_constStr_map.find(deAliasNode)
				== var_eq_constStr_map.end()) {
			Z3_ast nodeValue = get_eqc_value(t, deAliasNode);
			if (deAliasNode != nodeValue)
				var_eq_constStr_map[deAliasNode] = nodeValue;
		}
		// (2) var_eq_concat       : e.g,  z = concat("str1", b) z = concat(c, "str2")
		//-----------------------------------------------------------------
		if (var_eq_concat_map.find(deAliasNode) == var_eq_concat_map.end()
				|| var_eq_star_map.find(deAliasNode) == var_eq_star_map.end()) {
			Z3_ast curr = Z3_theory_get_eqc_next(t, deAliasNode);
			while (curr != deAliasNode) {
				if (isConcatFunc(t, curr)) {
					Z3_ast arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, curr), 0);
					Z3_ast arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, curr), 1);
					Z3_ast arg0_value = get_eqc_value(t, arg0);
					Z3_ast arg1_value = get_eqc_value(t, arg1);
					T_myZ3Type arg0_vType = (T_myZ3Type) getNodeType(t, arg0_value);
					T_myZ3Type arg1_vType = (T_myZ3Type) getNodeType(t, arg1_value);
					bool is_arg0_emptyStr = (arg0_vType == my_Z3_ConstStr)
							&& (getConstStrValue(t, arg0_value) == "");
					bool is_arg1_emptyStr = (arg1_vType == my_Z3_ConstStr)
							&& (getConstStrValue(t, arg1_value) == "");
					if (!is_arg0_emptyStr && !is_arg1_emptyStr) {
						var_eq_concat_map[deAliasNode][curr] = 1;
					}
				}
				if (isStarFunc(t, curr)) {
					Z3_ast arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, curr), 0);
					Z3_ast arg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, curr), 1);
					Z3_ast arg0_value = get_eqc_value(t, arg0);
					T_myZ3Type arg0_vType = (T_myZ3Type) getNodeType(t, arg0_value);
					bool is_arg0_emptyStr = (arg0_vType == my_Z3_ConstStr)
							&& (getConstStrValue(t, arg0_value) == "");
					if (!is_arg0_emptyStr && !isConstNum(t, arg1)) {
						var_eq_star_map[deAliasNode][curr] = 1;
					}
				}

				curr = Z3_theory_get_eqc_next(t, curr);
			}
		}
	}

	std::map<Z3_ast, Z3_ast> concats_eq_Index_map;
	std::map<Z3_ast, int>::iterator concatItor = concatMap.begin();
	for (; concatItor != concatMap.end(); concatItor++) {
		if (concats_eq_Index_map.find(concatItor->first)
				!= concats_eq_Index_map.end())
			continue;

		Z3_ast aRoot = NULL;
		Z3_ast curr = concatItor->first;
		do {
			if (isConcatFunc(t, curr)) {
				if (aRoot == NULL)
					aRoot = curr;
				else
					concats_eq_Index_map[curr] = aRoot;
			}
			curr = Z3_theory_get_eqc_next(t, curr);
		} while (curr != concatItor->first);
	}

	concatItor = concatMap.begin();
	for (; concatItor != concatMap.end(); concatItor++) {
		Z3_ast deAliasConcat = NULL;
		if (concats_eq_Index_map.find(concatItor->first)
				!= concats_eq_Index_map.end())
			deAliasConcat = concats_eq_Index_map[concatItor->first];
		else
			deAliasConcat = concatItor->first;

		// (3) concat_eq_constStr:
		//     e.g,  concat(a,b) = "str1"
		if (concat_eq_constStr_map.find(deAliasConcat)
				== concat_eq_constStr_map.end()) {
			Z3_ast nodeValue = get_eqc_value(t, deAliasConcat);
			if (deAliasConcat != nodeValue)
				concat_eq_constStr_map[deAliasConcat] = nodeValue;
		}

		// (4) concat_eq_concat:
		//     e.g,  concat(a,b) = concat("str1", c) /\ z = concat(a, b) /\ z = concat(e, f)
		if (concat_eq_concat_map.find(deAliasConcat)
				== concat_eq_concat_map.end()) {
			Z3_ast curr = deAliasConcat;
			do {
				if (isConcatFunc(t, curr))
					concat_eq_concat_map[deAliasConcat][curr] = 1;
				curr = Z3_theory_get_eqc_next(t, curr);
			} while (curr != deAliasConcat);
		}
	}

#ifdef DEBUGLOG
	{
		__debugPrint(logFile, "(0) alias: variables\n");
		std::map<Z3_ast, std::map<Z3_ast, int> > aliasSumMap;

		std::map<Z3_ast, Z3_ast>::iterator itor0 = aliasIndexMap.begin();
		for (; itor0 != aliasIndexMap.end(); itor0++)
		aliasSumMap[itor0->second][itor0->first] = 1;

		std::map<Z3_ast, std::map<Z3_ast, int> >::iterator keyItor = aliasSumMap.begin();
		for (; keyItor != aliasSumMap.end(); keyItor++) {
			__debugPrint(logFile, "  * ");
			printZ3Node(t, keyItor->first);
			__debugPrint(logFile, "\t: ");
			std::map<Z3_ast, int>::iterator innerItor = keyItor->second.begin();
			for (; innerItor != keyItor->second.end(); innerItor++) {
				printZ3Node(t, innerItor->first);
				__debugPrint(logFile, ", ");
			}
			__debugPrint(logFile, "\n");
		}
		__debugPrint(logFile, "\n");
	}

	{
		__debugPrint(logFile, "(1) var = constStr:\n");
		std::map<Z3_ast, Z3_ast>::iterator itor1 = var_eq_constStr_map.begin();
		for (; itor1 != var_eq_constStr_map.end(); itor1++) {
			__debugPrint(logFile, "  ");
			printZ3Node(t, itor1->first);
			__debugPrint(logFile, " = ");
			printZ3Node(t, itor1->second);
			if (!inSameEqc(t, itor1->first, itor1->second))
			__debugPrint(logFile, "  (notTrue in ctx)");
			__debugPrint(logFile, "\n");
		}
		__debugPrint(logFile, "\n");
	}

	{
		__debugPrint(logFile, "(2) var = concat:\n");
		std::map<Z3_ast, std::map<Z3_ast, int> >::iterator itor2 = var_eq_concat_map.begin();
		for (; itor2 != var_eq_concat_map.end(); itor2++) {
			__debugPrint(logFile, "  ");
			printZ3Node(t, itor2->first);
			__debugPrint(logFile, " = { ");
			std::map<Z3_ast, int>::iterator i_itor = itor2->second.begin();
			for (; i_itor != itor2->second.end(); i_itor++) {
				printZ3Node(t, i_itor->first);
				__debugPrint(logFile, ", ");
			}
			__debugPrint(logFile, "}\n");
		}
		__debugPrint(logFile, "\n");
	}

	{
		__debugPrint(logFile, "(3) concat = constStr:\n");
		std::map<Z3_ast, Z3_ast>::iterator itor3 = concat_eq_constStr_map.begin();
		for (; itor3 != concat_eq_constStr_map.end(); itor3++) {
			__debugPrint(logFile, "  ");
			printZ3Node(t, itor3->first);
			__debugPrint(logFile, " = ");
			printZ3Node(t, itor3->second);
			__debugPrint(logFile, "\n");

		}
		__debugPrint(logFile, "\n");
	}

	{
		__debugPrint(logFile, "(4) eq concats:\n");
		std::map<Z3_ast, std::map<Z3_ast, int> >::iterator itor4 = concat_eq_concat_map.begin();
		for (; itor4 != concat_eq_concat_map.end(); itor4++) {
			if (itor4->second.size() > 1) {
				std::map<Z3_ast, int>::iterator i_itor = itor4->second.begin();
				__debugPrint(logFile, "  >> ");
				for (; i_itor != itor4->second.end(); i_itor++) {
					printZ3Node(t, i_itor->first);
					__debugPrint(logFile, " , ");
				}
				__debugPrint(logFile, "\n");
			}
		}
	}

#endif

	//*****************************
	// Step 4. Dependence analysis
	//---------------------

	// (1) var = constStr
	for (std::map<Z3_ast, Z3_ast>::iterator itor = var_eq_constStr_map.begin();
			itor != var_eq_constStr_map.end(); itor++) {
		Z3_ast var = getAliasIndexAst(aliasIndexMap, itor->first);
		Z3_ast strAst = itor->second;
		depMap[var][strAst] = 1;
	}

	// (2) var = concat
	for (std::map<Z3_ast, std::map<Z3_ast, int> >::iterator itor =
			var_eq_concat_map.begin(); itor != var_eq_concat_map.end();
			itor++) {
		Z3_ast var = getAliasIndexAst(aliasIndexMap, itor->first);
		for (std::map<Z3_ast, int>::iterator itor1 = itor->second.begin();
				itor1 != itor->second.end(); itor1++) {
			Z3_ast concat = itor1->first;
			std::map<Z3_ast, int> inVarMap;
			std::map<Z3_ast, int> inConcatMap;
			classifyAstByType(t, concat, inVarMap, inConcatMap);
			for (std::map<Z3_ast, int>::iterator itor2 = inVarMap.begin();
					itor2 != inVarMap.end(); itor2++) {
				Z3_ast varInConcat = getAliasIndexAst(aliasIndexMap,
						itor2->first);
				if (!(depMap[var].find(varInConcat) != depMap[var].end()
						&& depMap[var][varInConcat] == 1))
					depMap[var][varInConcat] = 2;
			}
		}
	}

	//(3) concat = constStr
	for (std::map<Z3_ast, Z3_ast>::iterator itor =
			concat_eq_constStr_map.begin();
			itor != concat_eq_constStr_map.end(); itor++) {
		Z3_ast concatAst = itor->first;
		Z3_ast constStr = itor->second;
		std::map<Z3_ast, int> inVarMap;
		std::map<Z3_ast, int> inConcatMap;
		classifyAstByType(t, concatAst, inVarMap, inConcatMap);
		for (std::map<Z3_ast, int>::iterator itor2 = inVarMap.begin();
				itor2 != inVarMap.end(); itor2++) {
			Z3_ast varInConcat = getAliasIndexAst(aliasIndexMap, itor2->first);
			if (!(depMap[varInConcat].find(constStr)
					!= depMap[varInConcat].end()
					&& depMap[varInConcat][constStr] == 1))
				depMap[varInConcat][constStr] = 3;
		}
	}

	// (6) var = star(constStr,intvar)
	for (std::map<Z3_ast, std::map<Z3_ast, int> >::iterator itor =
			var_eq_star_map.begin(); itor != var_eq_star_map.end(); itor++) {
		Z3_ast var = getAliasIndexAst(aliasIndexMap, itor->first);
		for (std::map<Z3_ast, int>::iterator itor1 = itor->second.begin();
				itor1 != itor->second.end(); itor1++) {
			Z3_ast star = itor1->first;
			Z3_ast reg = Z3_get_app_arg(ctx, Z3_to_app(ctx, star), 0);
			Z3_ast intVar = Z3_get_app_arg(ctx, Z3_to_app(ctx, star), 1);
			if (!(depMap[var].find(intVar) != depMap[var].end())
					&& depMap[var].empty()) {
				depMap[var][intVar] = 6;
				freeVarMap[intVar] = 6;
				std::string tmp("_t_star");
				std::string vName = std::string(Z3_ast_to_string(ctx, intVar));
				Z3_ast new_freeVar = my_mk_str_var(t, (tmp + vName).c_str());
				varToStarMap[new_freeVar] = std::make_pair(var, reg);
			}
		}
	}

	//--------------------------------------------------------------
	// (4) equivalent concats
	//     - possiblity 1 : concat("str", v1) = concat(concat(v2, v3), v4) = concat(v5, v6)
	//         ==> v2, v5 are constrainted by "str"
	//     - possiblity 2 : concat(v1, "str") = concat(v2, v3) = concat(v4, v5)
	//         ==> v2, v4 are constrainted by "str"
	//--------------------------------------------------------------
	std::map<Z3_ast, Z3_ast> mostLeftNodes;
	std::map<Z3_ast, Z3_ast> mostRightNodes;

	std::map<Z3_ast, int> mLIdxMap;
	std::map<int, std::set<Z3_ast> > mLMap;
	std::map<Z3_ast, int> mRIdxMap;
	std::map<int, std::set<Z3_ast> > mRMap;
	std::set<Z3_ast> nSet;

	for (std::map<Z3_ast, std::map<Z3_ast, int> >::iterator itor =
			concat_eq_concat_map.begin(); itor != concat_eq_concat_map.end();
			itor++) {
		mostLeftNodes.clear();
		mostRightNodes.clear();

		Z3_ast mLConstParent = NULL;
		Z3_ast mLConst = NULL;
		Z3_ast mRConstParent = NULL;
		Z3_ast mRConst = NULL;

		for (std::map<Z3_ast, int>::iterator itor1 = itor->second.begin();
				itor1 != itor->second.end(); itor1++) {
			Z3_ast concatNode = itor1->first;
			Z3_ast mLNode = getMostLeftNodeInConcat(t, concatNode);
			if (isConstStr(t, mLNode)) {
				if (mLConst == NULL && getConstStrValue(t, mLNode) != "") {
					mLConst = mLNode;
					mLConstParent = concatNode;
				}
			} else {
				mostLeftNodes[mLNode] = concatNode;
			}

			Z3_ast mRNode = getMostRightNodeInConcat(t, concatNode);
			if (isConstStr(t, mRNode)) {
				if (mRConst == NULL && getConstStrValue(t, mRNode) != "") {
					mRConst = mRNode;
					mRConstParent = concatNode;
				}
			} else {
				mostRightNodes[mRNode] = concatNode;
			}
		}

		if (mLConst != NULL) {
			// -------------------------------------------------------------------------------------
			// The left most variable in a concat is constrained by a constant string in eqc concat
			// -------------------------------------------------------------------------------------
			// e.g. Concat(x, ...) = Concat("abc", ...)
			// -------------------------------------------------------------------------------------
			for (std::map<Z3_ast, Z3_ast>::iterator itor1 =
					mostLeftNodes.begin(); itor1 != mostLeftNodes.end();
					itor1++) {
				Z3_ast deVar = getAliasIndexAst(aliasIndexMap, itor1->first);
				if (depMap[deVar].find(mLConst) == depMap[deVar].end()
						|| depMap[deVar][mLConst] != 1) {
					depMap[deVar][mLConst] = 4;
					toBreakMap[std::make_pair(deVar, mLConst)] = std::make_pair(
							itor1->second, mLConstParent);
				}
			}
		}

		{
			// -------------------------------------------------------------------------------------
			// The left most variables in eqc concats are constrained by each other
			// -------------------------------------------------------------------------------------
			// e.g. concat(x, ...) = concat(u, ...) = ...
			//      x and u are constrained by each other
			// -------------------------------------------------------------------------------------
			nSet.clear();
			std::map<Z3_ast, Z3_ast>::iterator itl = mostLeftNodes.begin();
			for (; itl != mostLeftNodes.end(); itl++) {
				if (get_eqc_value(t, itl->first) != itl->first)
					continue;
				Z3_ast deVar = getAliasIndexAst(aliasIndexMap, itl->first);
				nSet.insert(deVar);
			}

			if (nSet.size() > 1) {
				int lId = -1;
				for (std::set<Z3_ast>::iterator itor2 = nSet.begin();
						itor2 != nSet.end(); itor2++) {
					if (mLIdxMap.find(*itor2) != mLIdxMap.end()) {
						lId = mLIdxMap[*itor2];
						break;
					}
				}
				if (lId == -1)
					lId = mLMap.size();
				for (std::set<Z3_ast>::iterator itor2 = nSet.begin();
						itor2 != nSet.end(); itor2++) {
					if (get_eqc_value(t, *itor2) != *itor2)
						continue;
					mLIdxMap[*itor2] = lId;
					mLMap[lId].insert(*itor2);
				}
			}
		}

		if (mRConst != NULL) {
			for (std::map<Z3_ast, Z3_ast>::iterator itor1 =
					mostRightNodes.begin(); itor1 != mostRightNodes.end();
					itor1++) {
				Z3_ast deVar = getAliasIndexAst(aliasIndexMap, itor1->first);
				if (depMap[deVar].find(mRConst) == depMap[deVar].end()
						|| depMap[deVar][mRConst] != 1) {
					depMap[deVar][mRConst] = 5;
					toBreakMap[std::make_pair(deVar, mRConst)] = std::make_pair(
							itor1->second, mRConstParent);
				}
			}
		}

		{
			nSet.clear();
			std::map<Z3_ast, Z3_ast>::iterator itr = mostRightNodes.begin();
			for (; itr != mostRightNodes.end(); itr++) {
				Z3_ast deVar = getAliasIndexAst(aliasIndexMap, itr->first);
				nSet.insert(deVar);
			}
			if (nSet.size() > 1) {
				int rId = -1;
				std::set<Z3_ast>::iterator itor2 = nSet.begin();
				for (; itor2 != nSet.end(); itor2++) {
					if (mRIdxMap.find(*itor2) != mRIdxMap.end()) {
						rId = mRIdxMap[*itor2];
						break;
					}
				}
				if (rId == -1)
					rId = mRMap.size();
				for (itor2 = nSet.begin(); itor2 != nSet.end(); itor2++) {
					if (get_eqc_value(t, *itor2) != *itor2)
						continue;
					mRIdxMap[*itor2] = rId;
					mRMap[rId].insert(*itor2);
				}
			}
		}
	}

#ifdef DEBUGLOG
	__debugPrint(logFile, "\n\n-- Dependence Map -----------------\n");
	for (std::map<Z3_ast, std::map<Z3_ast, int> >::iterator itor = depMap.begin(); itor != depMap.end(); itor++) {
		printZ3Node(t, itor->first);
		__debugPrint(logFile, "\t-->\t");
		for (std::map<Z3_ast, int>::iterator itor1 = itor->second.begin(); itor1 != itor->second.end(); itor1++) {
			printZ3Node(t, itor1->first);
			__debugPrint(logFile, "(%d), ", itor1->second);
		}
		__debugPrint(logFile, "\n");
	}
	__debugPrint(logFile, "-----------------------------------\n\n");

	__debugPrint(logFile, "-- L/R Most Var in eq concat ------\n");
	for (std::map<int, std::set<Z3_ast> >::iterator itor = mLMap.begin(); itor != mLMap.end(); itor++) {
		__debugPrint(logFile, "  >> [L] {");
		for (std::set<Z3_ast>::iterator it1 = itor->second.begin(); it1 != itor->second.end(); it1++) {
			printZ3Node(t, *it1);
			__debugPrint(logFile, ", ");
		}
		__debugPrint(logFile, "}\n");
	}
	for (std::map<int, std::set<Z3_ast> >::iterator itor = mRMap.begin(); itor != mRMap.end(); itor++) {
		__debugPrint(logFile, "  >> [R] {");
		for (std::set<Z3_ast>::iterator it1 = itor->second.begin(); it1 != itor->second.end(); it1++) {
			printZ3Node(t, *it1);
			__debugPrint(logFile, ", ");
		}
		__debugPrint(logFile, "}\n");
	}
	__debugPrint(logFile, "-----------------------------------\n\n");
#endif

	//****************
	// Step 6. Compute free variables based on dependence map.
	// the case dependence map is empty, every var in VarMap is free
	//---------------------------------------------------------------
	// remove L/R most var in eq concat since they are constrained with each other
	std::map<Z3_ast, std::map<Z3_ast, int> > lrConstrainedMap;
	for (std::map<int, std::set<Z3_ast> >::iterator itor = mLMap.begin();
			itor != mLMap.end(); itor++) {
		for (std::set<Z3_ast>::iterator it1 = itor->second.begin();
				it1 != itor->second.end(); it1++) {
			std::set<Z3_ast>::iterator it2 = it1;
			it2++;
			for (; it2 != itor->second.end(); it2++) {
				Z3_ast n1 = *it1;
				Z3_ast n2 = *it2;
				lrConstrainedMap[n1][n2] = 1;
				lrConstrainedMap[n2][n1] = 1;
			}
		}
	}
	for (std::map<int, std::set<Z3_ast> >::iterator itor = mRMap.begin();
			itor != mRMap.end(); itor++) {
		for (std::set<Z3_ast>::iterator it1 = itor->second.begin();
				it1 != itor->second.end(); it1++) {
			std::set<Z3_ast>::iterator it2 = it1;
			it2++;
			for (; it2 != itor->second.end(); it2++) {
				Z3_ast n1 = *it1;
				Z3_ast n2 = *it2;
				lrConstrainedMap[n1][n2] = 1;
				lrConstrainedMap[n2][n1] = 1;
			}
		}
	}

#ifdef DEBUGLOG
	{
		__debugPrint(logFile, "-- L/R constraints ----------------\n");
		for (std::map<Z3_ast, std::map<Z3_ast, int> >::iterator itor = lrConstrainedMap.begin(); itor != lrConstrainedMap.end(); itor++) {
			printZ3Node(t, itor->first);
			__debugPrint(logFile, "\t<->\t{");
			for (std::map<Z3_ast, int>::iterator nit = itor->second.begin(); nit != itor->second.end(); nit++) {
				printZ3Node(t, nit->first);
				__debugPrint(logFile, ", ");
			}
			__debugPrint(logFile, "}\n");
		}
		__debugPrint(logFile, "-----------------------------------\n\n");
	}
#endif

	if (depMap.size() == 0) {
		std::map<Z3_ast, int>::iterator itor = strVarMap.begin();
		for (; itor != strVarMap.end(); itor++) {
			Z3_ast var = getAliasIndexAst(aliasIndexMap, itor->first);
			if (lrConstrainedMap.find(var) == lrConstrainedMap.end()) {
				freeVarMap[var] = 1;
			} else {
				int lrConstainted = 0;
				std::map<Z3_ast, int>::iterator lrit = freeVarMap.begin();
				for (; lrit != freeVarMap.end(); lrit++) {
					if (lrConstrainedMap[var].find(lrit->first)
							!= lrConstrainedMap[var].end()) {
						lrConstainted = 1;
						break;
					}
				}
				if (lrConstainted == 0) {
					freeVarMap[var] = 1;
				}
			}
		}
	} else {
		// if the keys in aliasIndexMap are not contained in keys in depMap, they are free
		// e.g.,  x= y /\ x = z /\ t = "abc"
		//        aliasIndexMap[y]= x, aliasIndexMap[z] = x
		//        depMap        t ~ "abc"(1)
		//        x should be free
		std::map<Z3_ast, int>::iterator itor2 = strVarMap.begin();
		for (; itor2 != strVarMap.end(); itor2++) {
			if (aliasIndexMap.find(itor2->first) != aliasIndexMap.end()) {
				Z3_ast var = aliasIndexMap[itor2->first];
				if (depMap.find(var) == depMap.end()) {
					if (lrConstrainedMap.find(var) == lrConstrainedMap.end()) {
						freeVarMap[var] = 1;
					} else {
						int lrConstainted = 0;
						std::map<Z3_ast, int>::iterator lrit =
								freeVarMap.begin();
						for (; lrit != freeVarMap.end(); lrit++) {
							if (lrConstrainedMap[var].find(lrit->first)
									!= lrConstrainedMap[var].end()) {
								lrConstainted = 1;
								break;
							}
						}
						if (lrConstainted == 0) {
							freeVarMap[var] = 1;
						}
					}
				}
			} else if (aliasIndexMap.find(itor2->first)
					== aliasIndexMap.end()) {
				// if a variable is not in aliasIndexMap and not in depMap, it's free
				if (depMap.find(itor2->first) == depMap.end()) {
					Z3_ast var = itor2->first;
					if (lrConstrainedMap.find(var) == lrConstrainedMap.end()) {
						freeVarMap[var] = 1;
					} else {
						int lrConstainted = 0;
						std::map<Z3_ast, int>::iterator lrit =
								freeVarMap.begin();
						for (; lrit != freeVarMap.end(); lrit++) {
							if (lrConstrainedMap[var].find(lrit->first)
									!= lrConstrainedMap[var].end()) {
								lrConstainted = 1;
								break;
							}
						}
						if (lrConstainted == 0) {
							freeVarMap[var] = 1;
						}
					}
				}
			}
		}

		std::map<Z3_ast, std::map<Z3_ast, int> >::iterator itor =
				depMap.begin();
		for (; itor != depMap.end(); itor++) {
			for (std::map<Z3_ast, int>::iterator itor1 = itor->second.begin();
					itor1 != itor->second.end(); itor1++) {
				if (getNodeType(t, itor1->first) == my_Z3_Str_Var) {
					Z3_ast var = getAliasIndexAst(aliasIndexMap, itor1->first);
					// if a var is dep on itself and all dependence are type 2, it's a free variable
					// e.g {y --> x(2), y(2), m --> m(2), n(2)} y,m are free
					{
						if (depMap.find(var) == depMap.end()) {
							if (freeVarMap.find(var) == freeVarMap.end()) {
								if (lrConstrainedMap.find(var)
										== lrConstrainedMap.end()) {
									freeVarMap[var] = 1;
								} else {
									int lrConstainted = 0;
									std::map<Z3_ast, int>::iterator lrit =
											freeVarMap.begin();
									for (; lrit != freeVarMap.end(); lrit++) {
										if (lrConstrainedMap[var].find(
												lrit->first)
												!= lrConstrainedMap[var].end()) {
											lrConstainted = 1;
											break;
										}
									}
									if (lrConstainted == 0) {
										freeVarMap[var] = 1;
									}
								}

							} else {
								freeVarMap[var] = freeVarMap[var] + 1;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

/*
 *
 */
Z3_ast my_mk_internal_lenTest_var(Z3_theory t, Z3_ast node, int lTries) {
	Z3_context ctx = Z3_theory_get_context(t);
	std::stringstream ss;
	ss << "_t_len_" << Z3_ast_to_string(ctx, node) << "_" << lTries;
	std::string name = ss.str();
	return my_mk_str_var(t, name.c_str());
}

/*
 *
 */
Z3_ast my_mk_internal_ValTest_var(Z3_theory t, Z3_ast node, int len,
		int vTries) {
	Z3_context ctx = Z3_theory_get_context(t);
	std::stringstream ss;
	ss << "_t_val_" << Z3_ast_to_string(ctx, node) << "_" << len << "_"
			<< vTries;
	std::string name = ss.str();
	return my_mk_str_var(t, name.c_str());
}

/*
 *
 */
inline std::string intToString(int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}

/*
 *
 */
inline std::string longLongToString(long long i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}

/*
 *
 */
inline bool isIndicator(Z3_theory t, Z3_ast node) {
	Z3_context ctx = Z3_theory_get_context(t);
	if (getNodeType(t, node) == my_Z3_Str_Var) {
		std::string vName = std::string(Z3_ast_to_string(ctx, node));
		if (vName.length() >= 7) {
			std::string pprefix = vName.substr(0, 7);
			if (pprefix == "_t_len_" || pprefix == "_t_val_")
				return true;
			else
				return false;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

/*
 *
 */
inline bool isFreevarIndicator(Z3_theory t, Z3_ast node,
		std::string freeVarName) {
	Z3_context ctx = Z3_theory_get_context(t);
	if (getNodeType(t, node) == my_Z3_Str_Var) {
		std::string vName = std::string(Z3_ast_to_string(ctx, node));
		unsigned int indiFvLen = 7 + freeVarName.length();
		if (vName.length() >= indiFvLen) {
			std::string pprefix = vName.substr(0, indiFvLen);
			std::string lenIndi = "_t_len_" + freeVarName;
			std::string valIndi = "_t_val_" + freeVarName;
			if (pprefix == lenIndi || pprefix == valIndi)
				return true;
			else
				return false;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

/*
 *
 */
Z3_ast genLenTestOptions(Z3_theory t, Z3_ast freeVar, Z3_ast indicator,
		int tries, bool isStar) {
	Z3_context ctx = Z3_theory_get_context(t);
	std::vector<Z3_ast> orList;

	int distance = 1;
	int l = (tries - 1) * distance;
	int h = tries * distance;

	if (isStar) {
		for (int i = l; i < h; i++) {
			if (i == 0)
				orList.push_back(
						Z3_mk_eq(ctx, varToStarMap[freeVar].first,
								my_mk_str_value(t, "")));
			else {
				std::pair<Z3_ast, Z3_ast> p = varToStarMap[freeVar];
				std::string star = mk_num_of_str(getConstStrValue(t, p.second),
						i);
				orList.push_back(
						Z3_mk_eq(ctx, p.first,
								my_mk_str_value(t, star.c_str())));
			}
		}
		orList.push_back(Z3_mk_eq(ctx, indicator, my_mk_str_value(t, "more")));

		Z3_ast * or_items = new Z3_ast[orList.size()];
		for (int i = 0; i < (int) orList.size(); i++) {
			or_items[i] = orList[i];
		}
		return Z3_mk_or(ctx, orList.size(), or_items);
	}

	for (int i = l; i < h; i++) {
		if (i == 0)
			orList.push_back(Z3_mk_eq(ctx, freeVar, my_mk_str_value(t, "")));
		else {
			Z3_ast and_items[2];
			and_items[0] = Z3_mk_eq(ctx, indicator,
					my_mk_str_value(t, intToString(i).c_str()));
			and_items[1] = Z3_mk_eq(ctx, mk_length(t, freeVar), mk_int(ctx, i));

			orList.push_back(Z3_mk_and(ctx, 2, and_items));
		}
	}
	Z3_ast and_items2[2];
	and_items2[0] = Z3_mk_eq(ctx, indicator, my_mk_str_value(t, "more"));
	and_items2[1] = Z3_mk_gt(ctx, mk_length(t, freeVar), mk_int(ctx, h - 1));
	orList.push_back(Z3_mk_and(ctx, 2, and_items2));

	Z3_ast * or_items = new Z3_ast[orList.size()];
	for (int i = 0; i < (int) orList.size(); i++) {
		or_items[i] = orList[i];
	}
	return Z3_mk_or(ctx, orList.size(), or_items);

	/*
	 Z3_context ctx = Z3_theory_get_context(t);
	 Z3_ast freeVarLen = mk_length(t, freeVar);

	 std::vector<Z3_ast> orList;
	 std::vector<Z3_ast> andList;

	 int distance = 3;
	 int l = (tries - 1) * distance;
	 int h = tries * distance;
	 for (int i = l; i < h; i++) {
	 orList.push_back(Z3_mk_eq(ctx, indicator, my_mk_str_value(t, intToString(i).c_str())));
	 andList.push_back(Z3_mk_eq(ctx, orList[orList.size() - 1], Z3_mk_eq(ctx, freeVarLen, mk_int(ctx, i))));
	 }
	 orList.push_back(Z3_mk_eq(ctx, indicator, my_mk_str_value(t, "more")));
	 andList.push_back(Z3_mk_eq(ctx, orList[orList.size() - 1], Z3_mk_ge(ctx, freeVarLen, mk_int(ctx, h))));

	 Z3_ast * or_items = new Z3_ast[orList.size()];
	 Z3_ast * and_items = new Z3_ast[andList.size() + 1];
	 for (int i = 0; i < (int) orList.size(); i++) {
	 or_items[i] = orList[i];
	 }
	 //  for (int i = 0; i < (int) orList.size(); i++) {
	 //    printZ3Node(t, or_items[i]);
	 //    __debugPrint(logFile, "\n");
	 //  }
	 and_items[0] = Z3_mk_or(ctx, orList.size(), or_items);
	 for (int i = 0; i < (int) andList.size(); i++) {
	 and_items[i + 1] = andList[i];
	 }
	 Z3_ast lenTestAssert = Z3_mk_and(ctx, andList.size() + 1, and_items);
	 delete[] or_items;
	 delete[] and_items;

	 Z3_ast assertL = NULL;
	 int testerCount = tries - 1;          //fvarLenTesterMap[freeVar].size() - 1;
	 if (testerCount > 0) {
	 Z3_ast * and_items_LHS = new Z3_ast[testerCount];
	 Z3_ast moreAst = my_mk_str_value(t, "more");
	 for (int i = 0; i < testerCount; i++) {
	 and_items_LHS[i] = Z3_mk_eq(ctx, fvarLenTesterMap[freeVar][i], moreAst);
	 }
	 if (testerCount == 1)
	 assertL = and_items_LHS[0];
	 else
	 assertL = Z3_mk_and(ctx, testerCount, and_items_LHS);
	 delete[] and_items_LHS;
	 }

	 if (assertL != NULL)
	 lenTestAssert = Z3_mk_implies(ctx, assertL, lenTestAssert);
	 return lenTestAssert;
	 */
}

/*
 *
 */
std::string genValString(int len, std::vector<int> & encoding) {
	if (charSetSize <= 0) {
		fprintf(stdout,
				"> Error: Character size smaller than or equal to 0. Exit.\n");
		fflush (stdout);
		exit(0);
	}

	std::string re = std::string(len, charSet[0]);
	for (int i = 0; i < (int) encoding.size() - 1; i++) {
		int idx = encoding[i];
		re[len - 1 - i] = charSet[idx];
	}
	return re;
}

/*
 *
 */
void printVectorInt(std::vector<int> & e) {
#ifdef DEBUGLOG
	__debugPrint(logFile, "{");
	for (int i = 0; i < (int) e.size(); i++) {
		__debugPrint(logFile, " %d, ", e[i]);
	}
	__debugPrint(logFile, "}\n");
#endif
}

/*
 * The return value means whether we covered the search space
 *   - If the next encoding is valid, return false
 *   - Otherwise, return true
 */
bool getNextValEncode(std::vector<int> & base, std::vector<int> & next) {
	int s = 0;
	int carry = 0;
	next.clear();

	for (int i = 0; i < (int) base.size(); i++) {
		if (i == 0) {
			s = base[i] + 1;
			carry = s / charSetSize;
			s = s % charSetSize;
			next.push_back(s);
		} else {
			s = base[i] + carry;
			carry = s / charSetSize;
			s = s % charSetSize;
			next.push_back(s);
		}
	}
	if (next[next.size() - 1] > 0) {
		next.clear();
		return true;
	} else {
		return false;
	}
}

/*
 *
 */
Z3_ast genValOptions(Z3_theory t, Z3_ast freeVar, int len) {
	Z3_context ctx = Z3_theory_get_context(t);
	int distance = 128;

	std::vector < std::vector<int> > options;
	std::vector<int> base;

	base = std::vector<int>(len + 1, 0);

	long long l = 0;
	long long h = l;
	for (int i = 0; i < distance; i++) {
		options.push_back(base);
		h++;
	}

	// ----------------------------------------------------------------------------------------

	std::vector<Z3_ast> orList;
	std::vector<Z3_ast> andList;

	for (long long i = l; i < h; i++) {
		std::string aStr = genValString(len, options[i - l]);
		Z3_ast strAst = my_mk_str_value(t, aStr.c_str());
		orList.push_back(Z3_mk_eq(ctx, freeVar, strAst));
	}

	Z3_ast * or_items = new Z3_ast[orList.size()];
	for (int i = 0; i < (int) orList.size(); i++) {
		or_items[i] = orList[i];
	}

	Z3_ast valTestAssert = Z3_mk_or(ctx, orList.size(), or_items);
	;
	delete[] or_items;
	return valTestAssert;
}

/*
 *
 */
Z3_ast genValOptions(Z3_theory t, Z3_ast freeVar, Z3_ast len_indicator,
		Z3_ast val_indicator, std::string lenStr, int tries) {
	Z3_context ctx = Z3_theory_get_context(t);
	int distance = 128;

	// ----------------------------------------------------------------------------------------
	// generate value options encoding
	// encoding is a vector of size (len + 1)
	// e.g, len = 2,
	//      encoding {1, 2, 0} means the value option is "charSet[2]"."charSet[1]"
	//      the last item in the encoding indicates whether the whole space is covered
	//      for example, if the charSet = {a, b}. All valid encodings are
	//        {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0}
	//      if add 1 to the last one, we get
	//        {0, 0, 1}
	//      the last item "1" shows this is not a valid encoding, and we have covered all space
	// ----------------------------------------------------------------------------------------
	int len = atoi(lenStr.c_str());
	bool coverAll = false;
	std::vector < std::vector<int> > options;
	std::vector<int> base;

	if (tries == 0) {
		base = std::vector<int>(len + 1, 0);
		coverAll = false;
	} else {
		Z3_ast lastestValIndi =
				fvarValueTesterMap[freeVar][len][tries - 1].second;

		__debugPrint(logFile, ">> Last Value Tester = ");
		printZ3Node(t, lastestValIndi);
		__debugPrint(logFile, "\n\n");

		coverAll = getNextValEncode(valRangeMap[lastestValIndi], base);
	}

	long long l = (tries) * distance;
	long long h = l;
	for (int i = 0; i < distance; i++) {
		if (coverAll)
			break;
		options.push_back(base);
		h++;
		coverAll = getNextValEncode(options[options.size() - 1], base);
	}
	valRangeMap[val_indicator] = options[options.size() - 1];

	__debugPrint(logFile, ">> Value Tester Encoding = ");
	printVectorInt(valRangeMap[val_indicator]);
	__debugPrint(logFile, "\n");

	// ----------------------------------------------------------------------------------------

	std::vector<Z3_ast> orList;
	std::vector<Z3_ast> andList;

	for (long long i = l; i < h; i++) {
		orList.push_back(
				Z3_mk_eq(ctx, val_indicator,
						my_mk_str_value(t, longLongToString(i).c_str())));
		std::string aStr = genValString(len, options[i - l]);
		Z3_ast strAst = my_mk_str_value(t, aStr.c_str());
		andList.push_back(
				Z3_mk_eq(ctx, orList[orList.size() - 1],
						Z3_mk_eq(ctx, freeVar, strAst)));
	}
	if (!coverAll) {
		orList.push_back(
				Z3_mk_eq(ctx, val_indicator, my_mk_str_value(t, "more")));
	}

	Z3_ast * or_items = new Z3_ast[orList.size()];
	Z3_ast * and_items = new Z3_ast[andList.size() + 1];
	for (int i = 0; i < (int) orList.size(); i++) {
		or_items[i] = orList[i];
	}
	if (orList.size() > 1)
		and_items[0] = Z3_mk_or(ctx, orList.size(), or_items);
	else
		and_items[0] = or_items[0];

	for (int i = 0; i < (int) andList.size(); i++) {
		and_items[i + 1] = andList[i];
	}
	Z3_ast valTestAssert = Z3_mk_and(ctx, andList.size() + 1, and_items);
	delete[] or_items;
	delete[] and_items;

	// ---------------------------------------
	// IF the new value tester is _t_val_x_16_i
	// Should add (_t_len_x_j = 16) /\ (_t_val_x_16_i = "more")
	// ---------------------------------------
	andList.clear();
	andList.push_back(
			Z3_mk_eq(ctx, len_indicator, my_mk_str_value(t, lenStr.c_str())));
	for (int i = 0; i < tries; i++) {
		Z3_ast vTester = fvarValueTesterMap[freeVar][len][i].second;
		if (vTester != val_indicator)
			andList.push_back(
					Z3_mk_eq(ctx, vTester, my_mk_str_value(t, "more")));
	}
	Z3_ast assertL = NULL;
	if (andList.size() == 1) {
		assertL = andList[0];
	} else {
		Z3_ast * and_items = new Z3_ast[andList.size()];
		for (int i = 0; i < (int) andList.size(); i++) {
			and_items[i] = andList[i];
		}
		assertL = Z3_mk_and(ctx, andList.size(), and_items);
	}

	valTestAssert = Z3_mk_implies(ctx, assertL, valTestAssert);
	return valTestAssert;
}

/*
 *
 */
void printValueTesterList(Z3_theory t,
		std::vector<std::pair<int, Z3_ast> > & testerList, int lineNum) {
#ifdef DEBUGLOG
	int ss = testerList.size();
	__debugPrint(logFile, ">> valueTesterList @ %d = { ", lineNum);
	for (int i = 0; i < ss; i++) {
		if (i % 4 == 0)
		__debugPrint(logFile, "\n    ");
		__debugPrint(logFile, "(%d, ", testerList[i].first);
		printZ3Node(t, testerList[i].second);
		__debugPrint(logFile, "), ");
	}
	__debugPrint(logFile, "\n   }\n\n");
#endif
}

/*
 *
 */
Z3_ast genFreeVarOptions(Z3_theory t, Z3_ast freeVar, Z3_ast len_indicator,
		std::string len_valueStr, Z3_ast valTesterInCbEq,
		std::string valTesterValueStr) {
	int len = atoi(len_valueStr.c_str());

	if (fvarValueTesterMap[freeVar].find(len)
			== fvarValueTesterMap[freeVar].end()) {
		int tries = 0;
		Z3_ast val_indicator = my_mk_internal_ValTest_var(t, freeVar, len,
				tries);
		valueTesterFvarMap[val_indicator] = freeVar;
		fvarValueTesterMap[freeVar][len].push_back(
				std::make_pair(sLevel, val_indicator));
		printValueTesterList(t, fvarValueTesterMap[freeVar][len], __LINE__);
		return genValOptions(t, freeVar, len_indicator, val_indicator,
				len_valueStr, tries);
	} else {
		// go through all previous value testers
		// If some doesn't have an eqc value, add its assertion again.
		int testerTotal = fvarValueTesterMap[freeVar][len].size();
		int i = 0;
		for (; i < testerTotal; i++) {
			Z3_ast aTester = fvarValueTesterMap[freeVar][len][i].second;

			if (aTester == valTesterInCbEq) {
				break;
			}

			Z3_ast anEqc = get_eqc_value(t, aTester);
			if (aTester == anEqc) {
#ifdef DEBUGLOG
				__debugPrint(logFile, "\n>> Value tester: ");
				printZ3Node(t, aTester);
				__debugPrint(logFile, " doesn't have eqc value.\n");
#endif

				Z3_ast makeupAssert = genValOptions(t, freeVar, len_indicator,
						aTester, len_valueStr, i);

#ifdef DEBUGLOG
				__debugPrint(logFile, "\n----------genFreeVarOptions-----------\n");
				__debugPrint(logFile, ">> Var: ");
				printZ3Node(t, freeVar);
				__debugPrint(logFile," (@%d, Level %d):\n ", __LINE__, sLevel);
				printZ3Node(t, makeupAssert);
				__debugPrint(logFile, "\n---------------------\n");
#endif
				addAxiom(t, makeupAssert, __LINE__, false);
			}
		}

		if (valTesterValueStr == "more") {
			Z3_ast valTester = NULL;
			if (i + 1 < testerTotal) {
				valTester = fvarValueTesterMap[freeVar][len][i + 1].second;
			} else {
				valTester = my_mk_internal_ValTest_var(t, freeVar, len, i + 1);
				valueTesterFvarMap[valTester] = freeVar;
				fvarValueTesterMap[freeVar][len].push_back(
						std::make_pair(sLevel, valTester));
				printValueTesterList(t, fvarValueTesterMap[freeVar][len],
				__LINE__);
			}
			Z3_ast nextAssert = genValOptions(t, freeVar, len_indicator,
					valTester, len_valueStr, i + 1);
			return nextAssert;
		}

		return NULL;
	}
}

/*
 *
 */
Z3_ast genLenValOptionsForFreeVar(Z3_theory t, Z3_ast freeVar,
		Z3_ast lenTesterInCbEq, std::string lenTesterValue, bool isStar) {
	// -----------------------------------------------------------------------------------------------------
	// True branch will be taken in final_check:
	//   - When we discover a variable is "free" for the first time
	//     lenTesterInCbEq = NULL
	//     lenTesterValue = ""
	// False branch will be taken when invoked by cb_new_eq.
	//   - After we set up length tester for a "free" var in final_check,
	//     when the tester is assigned to some value (e.g. "more" or "4"),
	//     lenTesterInCbEq != NULL, and its value will be passed by lenTesterValue
	// The difference is that in cb_new_eq, lenTesterInCbEq and its value have NOT been put into a same eqc
	// -----------------------------------------------------------------------------------------------------

	__debugPrint(logFile, "\n=============================");
	__debugPrint(logFile, "\n>> genLenValOptionsForFreeVar");
	__debugPrint(logFile, "\n=============================\n");

	// no length assertions for this free variable has ever been added.
	if (fvarLenCountMap.find(freeVar) == fvarLenCountMap.end()) {
		fvarLenCountMap[freeVar] = 1;
		unsigned int testNum = fvarLenCountMap[freeVar];
		Z3_ast indicator = my_mk_internal_lenTest_var(t, freeVar, testNum);
		fvarLenTesterMap[freeVar].push_back(indicator);
		lenTesterFvarMap[indicator] = freeVar;

		Z3_ast lenTestAssert = genLenTestOptions(t, freeVar, indicator, testNum,
				isStar);
		return lenTestAssert;
	} else {
		Z3_ast effectiveLenInd = NULL;
		std::string effectiveLenIndiStr = "";
		int lenTesterCount = (int) fvarLenTesterMap[freeVar].size();

		int i = 0;
		for (; i < lenTesterCount; i++) {
			Z3_ast len_indicator_pre = fvarLenTesterMap[freeVar][i];
			Z3_ast len_indicator_value = get_eqc_value(t, len_indicator_pre);

			if (len_indicator_pre != len_indicator_value) {
				std::string len_pIndiStr = getConstStrValue(t,
						len_indicator_value);
				if (len_pIndiStr != "more") {
					effectiveLenInd = len_indicator_pre;
					effectiveLenIndiStr = len_pIndiStr;
					break;
				}
			} else {
				if (lenTesterInCbEq != len_indicator_pre) {
#ifdef DEBUGLOG
					__debugPrint(logFile, "\n>> *Warning*: length indicator: ");
					printZ3Node(t, len_indicator_pre);
					__debugPrint(logFile, " doesn't have an EQC value. i = %d, lenTesterCount = %d\n", i , lenTesterCount);
#endif
					if (i > 0) {
						effectiveLenInd = fvarLenTesterMap[freeVar][i - 1];
						if (effectiveLenInd == lenTesterInCbEq) {
							effectiveLenIndiStr = lenTesterValue;
						} else {
							effectiveLenIndiStr = getConstStrValue(t,
									get_eqc_value(t, effectiveLenInd));
						}
					}
					break;
				}
			}
		}

		if (effectiveLenIndiStr == "more" || effectiveLenIndiStr == "") {
			Z3_ast indicator = NULL;
			unsigned int testNum = 0;

			__debugPrint(logFile,
					"\n>> effectiveLenIndiStr = %s, i = %d,lenTesterCount = %d\n",
					effectiveLenIndiStr.c_str(), i, lenTesterCount);

			if (i == lenTesterCount) {
				fvarLenCountMap[freeVar] = fvarLenCountMap[freeVar] + 1;
				testNum = fvarLenCountMap[freeVar];
				indicator = my_mk_internal_lenTest_var(t, freeVar, testNum);
				fvarLenTesterMap[freeVar].push_back(indicator);
				lenTesterFvarMap[indicator] = freeVar;
			} else {
				indicator = fvarLenTesterMap[freeVar][i];
				testNum = i + 1;
			}

			Z3_ast lenTestAssert = genLenTestOptions(t, freeVar, indicator,
					testNum, isStar);
			return lenTestAssert;
		} else {
			// length is fixed
			Z3_ast valueAssert = genFreeVarOptions(t, freeVar, effectiveLenInd,
					effectiveLenIndiStr, NULL, "");
			return valueAssert;
		}
	}
}

/*
 *
 */
Z3_bool cb_final_check(Z3_theory t) {
	if (num_prop > 0) {
		PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
		Z3_theory_prop_arith_to_len(t, td->Concat);
		num_prop--;
		//        prop = 1;
		keeptry = true;
	}

	Z3_context ctx = Z3_theory_get_context(t);

#ifdef DEBUGLOG
	__debugPrint(logFile, "\n\n\n");
	__debugPrint(logFile, "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
	__debugPrint(logFile, "                cb_final_check @ Level [%d] \n", sLevel);
	__debugPrint(logFile, "=============================================================\n");
#endif

	//----------------------------------------------------------------------------------
	//run dependence analysis, find free string vars
	std::map<Z3_ast, int> varAppearInAssign;
	std::map<Z3_ast, int> concatMap;
	std::map<Z3_ast, Z3_ast> aliasIndexMap;
	std::map<Z3_ast, Z3_ast> var_eq_constStr_map;
	std::map<Z3_ast, Z3_ast> concatAstEqConst_map;
	std::map<Z3_ast, std::map<Z3_ast, int> > var_eq_concat_map;
	std::map<Z3_ast, std::map<Z3_ast, int> > var_eq_star_map;
	std::map<Z3_ast, Z3_ast> concat_eq_constStr_map;
	std::map<Z3_ast, std::map<Z3_ast, int> > concat_eq_concat_map;
	std::map<Z3_ast, std::map<Z3_ast, int> > depMap;
	std::map<Z3_ast, int> freeVar_map;
	std::map<std::pair<Z3_ast, Z3_ast>, std::pair<Z3_ast, Z3_ast> > toBreakMap;

	int conflictInDep = ctxDepAnalysis(t, varAppearInAssign, concatMap,
			aliasIndexMap, var_eq_constStr_map, var_eq_concat_map,
			var_eq_star_map, concat_eq_constStr_map, concat_eq_concat_map,
			freeVar_map, depMap, toBreakMap);

	if (conflictInDep == -1) {
		__debugPrint(logFile, "\n\n###########################################################\n\n");
		return Z3_TRUE;
	}

	//**************************************************************
	// double check depdence map for unbroken deps, e.g
	//    * (Concat ef y2) , ..., (Concat _t_str2 abc)
	//      [1]  y2  --> "abc"(5),
	//      [2]  _t_str2 --> "ef"(4),
	//**************************************************************
	Z3_ast nodeToSplitRoot = NULL;
	std::map<Z3_ast, std::map<Z3_ast, int> >::iterator depListItor =
			depMap.begin();
	for (; depListItor != depMap.end(); depListItor++) {
		Z3_ast depRoot = depListItor->first;
		int unbroken = 1;
		std::map<Z3_ast, int>::iterator itor = depListItor->second.begin();
		for (; itor != depListItor->second.end(); itor++) {
			int depType = itor->second;
			if (depType != 4 && depType != 5) {
				unbroken = 0;
				break;
			}
		}
		if (unbroken == 1) {
			nodeToSplitRoot = depRoot;
			break;
		}
	}

	if (nodeToSplitRoot != NULL) {
		// depMap[nodeToSplitRoot] contains type-4 and type-5 dependence only
		Z3_ast nodeDepOn = depMap[nodeToSplitRoot].begin()->first;
		std::pair<Z3_ast, Z3_ast> pp = toBreakMap[std::make_pair(
				nodeToSplitRoot, nodeDepOn)];

#ifdef DEBUGLOG
		__debugPrint(logFile, "\n************************************************\n");
		__debugPrint(logFile, "Further split needed for: ");
		printZ3Node(t, nodeToSplitRoot);
		__debugPrint(logFile, " with ");
		printZ3Node(t, nodeDepOn);
		__debugPrint(logFile, ", [depType = %d]\n\n", depMap[nodeToSplitRoot].begin()->second);
		printZ3Node(t, pp.first);
		__debugPrint(logFile, " = ");
		printZ3Node(t, pp.second);
		__debugPrint(logFile, "\n************************************************\n");
#endif

		Z3_ast toBreak1 = pp.first;
		Z3_ast toBreak2 = pp.second;
		if (toBreak1 != NULL && toBreak2 != NULL) {
#ifdef DEBUGLOG
			__debugPrint(logFile, "* toBreak1: ");
			printZ3Node(t, toBreak1);
			__debugPrint(logFile, "\n* toBreak2: ");
			printZ3Node(t, toBreak2);
			__debugPrint(logFile, "\n");
#endif
			// disable duplicate check when reducing eq concat
			simplifyConcatEq(t, toBreak1, toBreak2, 0);
		}
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n###########################################################\n\n");
#endif
		return Z3_TRUE;
	}

	//**************************************************************
	// Check whether variables appeared have eq string constants
	// If yes, all input variables are all assigned.
	//         we don't need to instantiate them as free var
	// If no, need to go ahead and assign free variables
	//**************************************************************
	int needToAssignFreeVar = 0;
	std::map<Z3_ast, int>::iterator itor = varAppearInAssign.begin();
	for (; itor != varAppearInAssign.end(); itor++) {
		std::string vName = std::string(Z3_ast_to_string(ctx, itor->first));
		if (vName.length() >= 3 && vName.substr(0, 3) == "_t_")
			continue;

		Z3_ast vNode = get_eqc_value(t, itor->first);
		if (itor->first == vNode) {
			needToAssignFreeVar = 1;
			break;
		}
	}
	if (needToAssignFreeVar == 0) {
		doubleCheckForNotContain(t);
		__debugPrint(logFile,
				"\n * All non-internal variables are assigned. Done!\n");
		__debugPrint(logFile,
				"###########################################################\n\n");
		return Z3_TRUE;
	}

	// Assign free variables
#ifdef DEBUGLOG
	{
		std::map<Z3_ast, int>::iterator freeVarItor1 = freeVar_map.begin();
		__debugPrint(logFile, "* Free Variables:\n----------------------------------\n");
		int varPrintedCount = 0;
		for (; freeVarItor1 != freeVar_map.end(); freeVarItor1++)
		{
			Z3_ast freeVar = freeVarItor1->first;
			printZ3Node(t, freeVar);
			__debugPrint(logFile, "(%d), ", freeVarItor1->second);
			varPrintedCount++;
			if (varPrintedCount % 5 == 0)
			__debugPrint(logFile, "\n");
		}
		__debugPrint(logFile, "\n----------------------------------\n");
	}
#endif

	std::map<Z3_ast, int>::iterator freeVarItor = freeVar_map.begin();
	for (; freeVarItor != freeVar_map.end(); freeVarItor++) {
		Z3_ast toAssert = NULL;
		Z3_ast freeVar = freeVarItor->first;
		std::string vName = std::string(Z3_ast_to_string(ctx, freeVar));
		if (vName.length() >= 6) {
			std::string vPrefix = vName.substr(0, 6);
			if (vPrefix == "_t_int") {
				std::string tmp("_t_star");
				Z3_ast new_freeVar = my_mk_str_var(t, (tmp + vName).c_str());
				toAssert = genLenValOptionsForFreeVar(t, new_freeVar, NULL, "",
						true);
				addAxiom(t, toAssert, __LINE__, false);

#ifdef DEBUGLOG
				__debugPrint(logFile, "\n---------------------\n");
				__debugPrint(logFile, "Assertion for free int var: ");
				printZ3Node(t, freeVar);
				__debugPrint(logFile," (@%d, Level %d):\n ", __LINE__, sLevel);
				printZ3Node(t, toAssert);
				__debugPrint(logFile, "\n---------------------\n");
#endif
				continue;
			}
		}
		Z3_ast len_freeVar = mk_length(t, freeVar);
		Z3_ast num = get_eqc_num(t, len_freeVar, 13);
		if (num != len_freeVar) {
			int i = getConstNumValue(t, num);
			__debugPrint(logFile, "\nKnown Length: %d\n", i);
			toAssert = genValOptions(t, freeVar, i);
		} else
			toAssert = genLenValOptionsForFreeVar(t, freeVar, NULL, "", false);

		addAxiom(t, toAssert, __LINE__, false);

#ifdef DEBUGLOG
		__debugPrint(logFile, "\n---------------------\n");
		__debugPrint(logFile, "Assertion for free var: ");
		printZ3Node(t, freeVar);
		__debugPrint(logFile," (@%d, Level %d):\n ", __LINE__, sLevel);
		printZ3Node(t, toAssert);
		__debugPrint(logFile, "\n---------------------\n");
#endif

	}
	__debugPrint(logFile,
			"\n###########################################################\n\n");
	return Z3_TRUE;
}

/*
 *
 */
//void strReplaceAll(std::string & str, const std::string & from, const std::string & to) {
//  if (from.empty())
//    return;
//  size_t start_pos = 0;
//  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
//    str.replace(start_pos, from.length(), to);
//    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
//  }
//}
/****************************************
 *  Z3 input parser doesn't understand constant string
 *  So, we pass const string by adding a special mark "$",
 * --------------------------------------
 * "__cOnStStR__x23_x5c_x6e_x5c_x22_x53"
 ****************************************/
inline bool isValidHexDigit(char c) {
	if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f')
			|| ('A' <= c && c <= 'F')) {
		return true;
	}
	return false;
}

/*
 *
 */
inline int hexDigitToInt(char a) {
	if ('0' <= a && a <= '9')
		return a - '0';
	else if ('a' <= a && a <= 'f')
		return 10 + a - 'a';
	else if ('A' <= a && a <= 'F')
		return 10 + a - 'A';
	else
		return 0;
}

/*
 *
 */
inline int twoHexDigitToChar(char a, char b) {
	return (hexDigitToInt(a) * 16 + hexDigitToInt(b));
}

/*
 *
 */
std::string convertInputTrickyConstStr(std::string inputStr) {
	std::string outputStr = "";
	std::string innerStr = inputStr.substr(11, inputStr.length() - 11);
	int innerStrLen = innerStr.length();
	if (innerStrLen % 4 != 0) {
		fprintf(stdout, "> Error: Constant string conversion error. Exit.\n");
		fprintf(stdout, "         Input encoding: %s\n", inputStr.c_str());
		fflush (stdout);
		exit(0);
	}
	for (int i = 0; i < (innerStrLen / 4); i++) {
		std::string cc = innerStr.substr(i * 4, 4);
		if (cc[0] == '_' && cc[1] == 'x' && isValidHexDigit(cc[2])
				&& isValidHexDigit(cc[3])) {
			char dc = twoHexDigitToChar(cc[2], cc[3]);
			// Check whether the input character in the charSet
			if (charSetLookupTable.find(dc) == charSetLookupTable.end()) {
				fprintf(stdout,
						"> Error: Character '%s' in a constant string is not in the system alphabet.\n",
						encodeToEscape(dc).c_str());
				fprintf(stdout,
						"         Please set the character set accordingly.\n");
				fflush (stdout);
				exit(0);
			}
			outputStr = outputStr + std::string(1, dc);
		}
	}
	return outputStr;
}

/*
 *
 */
inline std::string encodeToEscape(char c) {
	int idx = (unsigned char) c;
	if (0 <= idx && idx <= 255) {
		return escapeDict[idx];
	} else {
		printf("> Error: should not get here @ %d.\n", __LINE__);
		exit(0);
	}
}

/*
 *
 */
Z3_bool cb_reduce_eq(Z3_theory t, Z3_ast s1, Z3_ast s2, Z3_ast * r) {
	Z3_context ctx = Z3_theory_get_context(t);
	std::string s1_str = std::string(Z3_ast_to_string(ctx, s1));
	std::string s2_str = std::string(Z3_ast_to_string(ctx, s2));
	Z3_ast s1_new = s1;
	Z3_ast s2_new = s2;

	// Convert the tricky "string" representation to string constant
	if (s1_str.length() >= 11 && s1_str.substr(0, 11) == "__cOnStStR_")
		s1_new = my_mk_str_value(t, convertInputTrickyConstStr(s1_str).c_str());
	if (s2_str.length() >= 11 && s2_str.substr(0, 11) == "__cOnStStR_")
		s2_new = my_mk_str_value(t, convertInputTrickyConstStr(s2_str).c_str());

	if (s2_new != s2 || s1_new != s1) {
		*r = Z3_mk_eq(ctx, s1_new, s2_new);
#ifdef DEBUGLOG
		__debugPrint(logFile, ">> cb_reduce_eq: ");
		printZ3Node(t, *r);
		__debugPrint(logFile, "\n\n");
#endif
		if (!isConstStr(t, s1_new) && isConstStr(t, s2_new))
			strEqLengthAxiom(t, s1_new, s2_new, __LINE__);
		if (isConstStr(t, s1_new) && !isConstStr(t, s2_new))
			strEqLengthAxiom(t, s2_new, s1_new, __LINE__);
		return Z3_TRUE;
	} else {
		return Z3_FALSE;
	}
}

/*
 *
 */
void my_get_vars_in_input(Z3_theory t, Z3_ast node) {
	Z3_context ctx = Z3_theory_get_context(t);
	T_myZ3Type nodeType = (T_myZ3Type) getNodeType(t, node);

	if (nodeType == my_Z3_Str_Var) {
		std::string vName = std::string(Z3_ast_to_string(ctx, node));
		if (vName.length() >= 11 && vName.substr(0, 11) == "__cOnStStR_") {
			return;
		}
		inputVarMap[node] = 1;
	} else if (getNodeType(t, node) == my_Z3_Func) {
		Z3_app func_app = Z3_to_app(ctx, node);
		int argCount = Z3_get_app_num_args(ctx, func_app);
		for (int i = 0; i < argCount; i++) {
			Z3_ast argAst = Z3_get_app_arg(ctx, func_app, i);
			my_get_vars_in_input(t, argAst);
		}
	}
}

/*
 *
 */
void cb_init_search(Z3_theory t) {
#ifdef DEBUGLOG
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast ctxAssign = Z3_get_context_assignment(ctx);
	__debugPrint(logFile, "\n\n");
	__debugPrint(logFile, "***********************************************\n");
	__debugPrint(logFile, "*               Starting Search               *\n");
	__debugPrint(logFile, "-----------------------------------------------\n");
	printZ3Node(t, ctxAssign);
	__debugPrint(logFile, "\n");
	__debugPrint(logFile, "***********************************************\n");
#endif
	searchStart = 1;

	__debugPrint(logFile, ">> Input Var Set: ");
	for (std::map<Z3_ast, int>::iterator it = inputVarMap.begin();
			it != inputVarMap.end(); it++) {
		printZ3Node(t, it->first);
		__debugPrint(logFile, ", ");
	}
	__debugPrint(logFile, "\n");

	/* initialize random seed: */
	srand (time(NULL));}

	/*
	 *  Reduce contains to concat & length
	 */
void getBoolAssignmentFromCtx(Z3_theory t,
		std::map<Z3_ast, bool> & boolAssignMap) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast ctxAssign = Z3_get_context_assignment(ctx);
	if (Z3_get_decl_kind(ctx, Z3_get_app_decl(ctx, Z3_to_app(ctx, ctxAssign)))
			!= Z3_OP_AND) {
		if (Z3_get_decl_kind(ctx,
				Z3_get_app_decl(ctx, Z3_to_app(ctx, ctxAssign))) == Z3_OP_NOT) {
			Z3_ast arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, ctxAssign), 0);
			boolAssignMap[arg0] = false;
		} else {
			boolAssignMap[ctxAssign] = true;
		}
	} else {
		int argCount = Z3_get_app_num_args(ctx, Z3_to_app(ctx, ctxAssign));
		for (int i = 0; i < argCount; i++) {
			Z3_ast itemAssign = Z3_get_app_arg(ctx, Z3_to_app(ctx, ctxAssign),
					i);
			if (Z3_get_decl_kind(ctx,
					Z3_get_app_decl(ctx, Z3_to_app(ctx, itemAssign)))
					== Z3_OP_NOT) {
				Z3_ast arg0 = Z3_get_app_arg(ctx, Z3_to_app(ctx, itemAssign),
						0);
				boolAssignMap[arg0] = false;
			} else {
				boolAssignMap[itemAssign] = true;
			}
		}
	}
}

/*
 *
 */
void doubleCheckForNotContain(Z3_theory t) {
	if (containsReduced_bool_str_map.size() == 0) {
		return;
	} else {
		std::map<Z3_ast, bool> boolAssignMap;
		getBoolAssignmentFromCtx(t, boolAssignMap);

		std::map<Z3_ast, Z3_ast>::iterator strItor =
				containsReduced_bool_str_map.begin();
		for (; strItor != containsReduced_bool_str_map.end(); strItor++) {
			Z3_ast boolVar = strItor->first;
			Z3_ast strVar = strItor->second;
			Z3_ast subStrVar = containsReduced_bool_subStr_map[boolVar];
			bool boolVarValue = boolAssignMap[boolVar];
			if (!boolVarValue) {
#ifdef DEBUGLOG
				__debugPrint(logFile, " >> Bool var: { ");
				printZ3Node(t, boolVar);
				if ( boolVarValue )
				{
					__debugPrint(logFile, " =  TRUE}. Check Contains( ");
				}
				else
				{
					__debugPrint(logFile, " =  FALSE}. Check ! Contains( ");
				}
				printZ3Node(t, strVar);
				__debugPrint(logFile, ", ");
				printZ3Node(t, subStrVar);
				__debugPrint(logFile, ") for conflict...\n");
#endif
				Z3_ast strValue = get_eqc_value(t, strVar);
				Z3_ast substrValue = get_eqc_value(t, subStrVar);
				if (isConstStr(t, strValue) && isConstStr(t, substrValue)) {
					std::string strConst = getConstStrValue(t, strValue);
					std::string subStrConst = getConstStrValue(t, substrValue);

					if (!boolVarValue) {
						if (strConst.find(subStrConst) != std::string::npos) {
							Z3_context ctx = Z3_theory_get_context(t);
							int pos = 0;
							Z3_ast l_set[2];
							if (strValue != strVar)
								l_set[pos++] = Z3_mk_eq(ctx, strVar, strValue);
							if (subStrVar != substrValue)
								l_set[pos++] = Z3_mk_eq(ctx, subStrVar,
										substrValue);

							Z3_ast r_imply = boolVar;
							Z3_ast toAssert = NULL;
							if (pos == 0) {
								toAssert = r_imply;
							} else if (pos == 1) {
								toAssert = Z3_mk_implies(ctx, l_set[0],
										r_imply);
							} else {
								Z3_ast l_imply = Z3_mk_and(ctx, 2, l_set);
								toAssert = Z3_mk_implies(ctx, l_imply, r_imply);
							}
							addAxiom(t, toAssert, __LINE__);
						}
					}
				}
			}
		}
	}
}

/*
 *
 */
Z3_ast reduce_contains2(Z3_theory t, Z3_ast const args[]) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast reduceAst = NULL;
	if (isConstStr(t, args[0]) && isConstStr(t, args[1])) {
		std::string arg0Str = getConstStrValue(t, args[0]);
		std::string arg1Str = getConstStrValue(t, args[1]);
		if (arg0Str.find(arg1Str) != std::string::npos)
			reduceAst = Z3_mk_true(ctx);
		else
			reduceAst = Z3_mk_false(ctx);
	} else {
		reduceAst = mk_contains(t, args[0], args[1]);
		//--------------------------------------------------
		//* Current model can not rule out the possibility: {contains(x, "efg") /\ ! contains(x, "ef")}
		//* So, in final_check, double check such cases.
		//* Remember reduced bool and str searched for, used to check whether args[0] contains args[1]
		//--------------------------------------------------
		containsReduced_bool_str_map[reduceAst] = args[0];
		containsReduced_bool_subStr_map[reduceAst] = args[1];
	}
	return reduceAst;
}

/*
 *
 */
Z3_ast reduce_contains(Z3_theory t, Z3_ast const args[]) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast reduceAst = NULL;
	if (isConstStr(t, args[0]) && isConstStr(t, args[1])) {
		std::string arg0Str = getConstStrValue(t, args[0]);
		std::string arg1Str = getConstStrValue(t, args[1]);
		if (arg0Str.find(arg1Str) != std::string::npos)
			reduceAst = Z3_mk_true(ctx);
		else
			reduceAst = Z3_mk_false(ctx);
	} else {
		Z3_ast ts0 = my_mk_internal_string_var(t);
		Z3_ast ts1 = my_mk_internal_string_var(t);
		reduceAst = Z3_mk_eq(ctx, args[0],
				mk_concat(t, ts0, mk_concat(t, args[1], ts1)));
		//--------------------------------------------------
		//* Current model can not rule out the possibility: {contains(x, "efg") /\ ! contains(x, "ef")}
		//* So, in final_check, double check such cases.
		//* Remember reduced bool and str searched for, used to check whether args[0] contains args[1]
		//--------------------------------------------------
		containsReduced_bool_str_map[reduceAst] = args[0];
		containsReduced_bool_subStr_map[reduceAst] = args[1];
	}
	return reduceAst;
}

/*
 *
 */
Z3_ast reduce_startswith(Z3_theory t, Z3_ast const args[]) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast reduceAst = NULL;
	if (isConstStr(t, args[0]) && isConstStr(t, args[1])) {
		std::string arg0Str = getConstStrValue(t, args[0]);
		std::string arg1Str = getConstStrValue(t, args[1]);
		if (arg0Str.length() < arg1Str.length()) {
			reduceAst = Z3_mk_false(ctx);
		} else {
			if (arg0Str.substr(0, arg1Str.length()) == arg1Str) {
				reduceAst = Z3_mk_true(ctx);
			} else {
				reduceAst = Z3_mk_false(ctx);
			}
		}
	} else {
		Z3_ast ts0 = my_mk_internal_string_var(t);
		reduceAst = Z3_mk_eq(ctx, args[0], mk_concat(t, args[1], ts0));
	}
	return reduceAst;
}

/*
 *
 */
Z3_ast reduce_endswith(Z3_theory t, Z3_ast const args[]) {
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_ast reduceAst = NULL;
	if (isConstStr(t, args[0]) && isConstStr(t, args[1])) {
		std::string arg0Str = getConstStrValue(t, args[0]);
		std::string arg1Str = getConstStrValue(t, args[1]);
		if (arg0Str.length() < arg1Str.length()) {
			reduceAst = Z3_mk_false(ctx);
		} else {
			if (arg0Str.substr(arg0Str.length() - arg1Str.length(),
					arg1Str.length()) == arg1Str) {
				reduceAst = Z3_mk_true(ctx);
			} else {
				reduceAst = Z3_mk_false(ctx);
			}
		}
	} else {
		Z3_ast ts0 = my_mk_internal_string_var(t);
		reduceAst = Z3_mk_eq(ctx, args[0], mk_concat(t, ts0, args[1]));
	}
	return reduceAst;
}

/*
 *
 */
Z3_ast reduce_indexof(Z3_theory t, Z3_ast const args[],
		Z3_ast & breakdownAssert) {
	if (!tempPatch)
		tempPatch = true;
	Z3_context ctx = Z3_theory_get_context(t);
	if (isConstStr(t, args[0]) && isConstStr(t, args[1])) {
		std::string arg0Str = getConstStrValue(t, args[0]);
		std::string arg1Str = getConstStrValue(t, args[1]);
		if (arg0Str.find(arg1Str) != std::string::npos) {
			int index = arg0Str.find(arg1Str);
			return mk_int(ctx, index);
		} else {
			return mk_int(ctx, -1);
		}
	} else {
		Z3_ast x1 = my_mk_internal_string_var(t);
		Z3_ast x2 = my_mk_internal_string_var(t);
		Z3_ast indexAst = my_mk_internal_int_var(t);

		Z3_ast and_items1[3], and_items2[2];
		and_items1[0] = Z3_mk_eq(ctx, indexAst, mk_length(t, x1));
		and_items1[1] = Z3_mk_eq(ctx, args[0],
				mk_concat(t, x1, mk_concat(t, args[1], x2)));
		and_items1[2] = Z3_mk_not(ctx, mk_contains(t, x1, args[1]));
		and_items2[0] = Z3_mk_eq(ctx, indexAst, mk_int(ctx, -1));
		and_items2[1] = Z3_mk_not(ctx, mk_contains(t, args[0], args[1]));

		Z3_ast or_items[2];
		or_items[0] = Z3_mk_and(ctx, 3, and_items1);
		or_items[1] = Z3_mk_and(ctx, 2, and_items2);

		Z3_ast and_items[2];
		and_items[0] = Z3_mk_or(ctx, 2, or_items);
		and_items[1] = Z3_mk_eq(ctx, indexAst, mk_idxof(t, args[0], args[1]));

		breakdownAssert = Z3_mk_and(ctx, 2, and_items);
		return indexAst;
	}
}

/*
 *
 */
Z3_ast reduce_replace(Z3_theory t, Z3_ast const args[],
		Z3_ast & breakdownAssert) {
	Z3_context ctx = Z3_theory_get_context(t);
	if (isConstStr(t, args[0]) && isConstStr(t, args[1])
			&& isConstStr(t, args[2])) {
		std::string arg0Str = getConstStrValue(t, args[0]);
		std::string arg1Str = getConstStrValue(t, args[1]);
		std::string arg2Str = getConstStrValue(t, args[2]);
		if (arg0Str.find(arg1Str) != std::string::npos) {
			int index1 = arg0Str.find(arg1Str);
			int index2 = index1 + arg1Str.length();
			std::string substr0 = arg0Str.substr(0, index1);
			std::string substr2 = arg0Str.substr(index2);
			std::string replaced = substr0 + arg2Str + substr2;
			return my_mk_str_value(t, replaced.c_str());
		} else {
			return args[0];
		}
	} else {
		Z3_ast x1 = my_mk_internal_string_var(t);
		Z3_ast x2 = my_mk_internal_string_var(t);
		Z3_ast x3 = my_mk_internal_string_var(t);
		Z3_ast indexAst = my_mk_internal_int_var(t);
		Z3_ast result = my_mk_internal_string_var(t);

		int pos = 0;
		Z3_ast and_items[8];

		and_items[pos++] = Z3_mk_eq(ctx, args[0],
				mk_concat(t, x1, mk_concat(t, x2, x3)));
		Z3_ast i_minus_one = Z3_mk_eq(ctx, indexAst, mk_int(ctx, -1));
		Z3_ast i_ge_zero = Z3_mk_ge(ctx, indexAst, mk_int(ctx, 0));
		and_items[pos++] = Z3_mk_xor(ctx, i_minus_one, i_ge_zero);
		//-------------------------------------------
		and_items[pos++] = Z3_mk_iff(ctx, i_minus_one,
				Z3_mk_not(ctx, mk_contains(t, args[0], args[1])));
		and_items[pos++] = Z3_mk_iff(ctx, i_minus_one,
				Z3_mk_eq(ctx, result, args[0]));
		//-------------------------------------------
		and_items[pos++] = Z3_mk_iff(ctx, i_ge_zero,
				Z3_mk_eq(ctx, indexAst, mk_length(t, x1)));
		and_items[pos++] = Z3_mk_iff(ctx, i_ge_zero,
				Z3_mk_eq(ctx, x2, args[1]));
		and_items[pos++] = Z3_mk_iff(ctx, i_ge_zero,
				Z3_mk_not(ctx, mk_contains(t, x1, args[1])));
		and_items[pos++] = Z3_mk_eq(ctx, result,
				mk_concat(t, x1, mk_concat(t, args[2], x3)));
		//-------------------------------------------
		breakdownAssert = Z3_mk_and(ctx, pos, and_items);
		return result;
	}
}

/*
 *
 */
Z3_ast reduce_search(Z3_theory t, Z3_ast const args[]) {
	__debugPrint(logFile, "=============================\n");
	__debugPrint(logFile, "reduce_search:\n");
	Z3_context ctx = Z3_theory_get_context(t);
	if (isConstStr(t, args[0]) && isConstStr(t, args[1])) {
		std::string arg0Str = getConstStrValue(t, args[0]);
		std::string arg1Str = getConstStrValue(t, args[1]);
		__debugPrint(logFile, "arg0: %s\n", arg0Str.c_str());
		__debugPrint(logFile, "arg1: %s\n", arg1Str.c_str());

		std::regex reg(arg1Str);
		std::smatch m;
		std::regex_search(arg0Str, m, reg);
		int res = (m.size() > 0) ? m.position(0) : (-1);
		__debugPrint(logFile, "Result: %d\n", res);
		__debugPrint(logFile, "=============================\n");
		return mk_int(ctx, res);
	} else {
		return mk_search(t, args[0], args[1]);
	}
}

/*
 *
 */
Z3_ast reduce_In(Z3_theory t, Z3_ast lhs, Z3_ast rhs) {
	Z3_context ctx = Z3_theory_get_context(t);
	__debugPrint(logFile, ">> reduce_In(): In( ");
	printZ3Node(t, lhs);
	__debugPrint(logFile, " , ");
	printZ3Node(t, rhs);
	__debugPrint(logFile, " )\n\n");

	if (isConstStr(t, rhs) || isStarFunc(t, rhs)) {
		return Z3_mk_eq(ctx, lhs, rhs);
	} else if (isUnionFunc(t, rhs)) {
		Z3_ast uarg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, rhs), 0);
		Z3_ast uarg2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, rhs), 1);
		Z3_ast and_items[2];
		and_items[0] = reduce_In(t, lhs, uarg1);
		and_items[1] = reduce_In(t, lhs, uarg2);
		return Z3_mk_and(ctx, 2, and_items);
	} else if (isConcatFunc(t, rhs)) {
		PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
		Z3_ast carg1 = Z3_get_app_arg(ctx, Z3_to_app(ctx, rhs), 0);
		Z3_ast carg2 = Z3_get_app_arg(ctx, Z3_to_app(ctx, rhs), 1);
		Z3_ast v1 = my_mk_internal_string_var(t);
		Z3_ast v2 = my_mk_internal_string_var(t);
		Z3_ast newconcat = mk_2_arg_app(ctx, td->Concat, v1, v2);
		Z3_ast and_items[3];
		and_items[0] = reduce_In(t, v1, carg1);
		and_items[1] = reduce_In(t, v2, carg2);
		and_items[2] = Z3_mk_eq(ctx, lhs, newconcat);
		return Z3_mk_and(ctx, 3, and_items);
	}
	return NULL;
}

/*
 *
 */
Z3_bool cb_reduce_app(Z3_theory t, Z3_func_decl d, unsigned n,
		Z3_ast const * args, Z3_ast * result) {
	Z3_context ctx = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);

	// Convert the tricky "string" representation to string constant
	int convertedFlag = 0;
	Z3_ast * convertedArgs = new Z3_ast[n];
	for (int i = 0; i < (int) n; i++) {
		std::string symbolStr = std::string(Z3_ast_to_string(ctx, args[i]));
		if (symbolStr.length() >= 11
				&& symbolStr.substr(0, 11) == "__cOnStStR_") {
			convertedFlag = 1;
			convertedArgs[i] = my_mk_str_value(t,
					convertInputTrickyConstStr(symbolStr).c_str());
		} else {
			convertedArgs[i] = args[i];
		}
	}

	//---------------------------------
	// reduce app: Concat
	//---------------------------------
	if (d == td->Concat) {
		Z3_ast result_ast = Concat(t, convertedArgs[0], convertedArgs[1]);
		if (result_ast != 0) {
			*result = result_ast;
#ifdef DEBUGLOG
			__debugPrint(logFile, ">> cb_reduce_app(): concat( ");
			printZ3Node(t, convertedArgs[0]);
			__debugPrint(logFile, " , ");
			printZ3Node(t, convertedArgs[1]);
			__debugPrint(logFile, " )\n\n");
#endif
			delete[] convertedArgs;
			return Z3_TRUE;
		} else {
			*result = mk_concat(t, convertedArgs[0], convertedArgs[1]);
			delete[] convertedArgs;
			return Z3_TRUE;
		}
	}
	//---------------------------------
	// reduce app: Union
	//---------------------------------
	if (d == td->Union) {
		*result = mk_union(t, convertedArgs[0], convertedArgs[1]);
		delete[] convertedArgs;
		return Z3_TRUE;
	}
	//---------------------------------
	// reduce app: Star
	//---------------------------------
	if (d == td->Star) {
		*result = mk_star(t, convertedArgs[0]);
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//------------------------------------------
	// Reduce app: In
	//------------------------------------------
	if (d == td->In) {
		*result = reduce_In(t, convertedArgs[0], convertedArgs[1]);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): In(");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, ")");
		__debugPrint(logFile, "  =>  ");
		printZ3Node(t, *result);
		__debugPrint(logFile, "\n===================\n");
#endif
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//---------------------------------
	// reduce app: Length
	//---------------------------------
	if (d == td->Length) {
		if (isConstStr(t, convertedArgs[0])) {
			int size = getConstStrValue(t, convertedArgs[0]).size();
			*result = mk_int(ctx, size);
#ifdef DEBUGLOG
			__debugPrint(logFile, ">> cb_reduce_app(): Length( ");
			printZ3Node(t, convertedArgs[0]);
			__debugPrint(logFile, " ) = ");
			__debugPrint(logFile, "%d\n\n", size);
#endif
			delete[] convertedArgs;
			return Z3_TRUE;
		} else {
			if (convertedFlag == 1) {
				*result = mk_1_arg_app(ctx, d, convertedArgs[0]);
				delete[] convertedArgs;
				return Z3_TRUE;
			} else {
				delete[] convertedArgs;
				return Z3_FALSE;
			}
		}
	}

	//---------------------------------
	// reduce app: SubString
	//---------------------------------
	if (d == td->SubStringExt) {
		*result = mk_subStr(t, convertedArgs);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): SubString(");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[2]);
		__debugPrint(logFile, ")  =>  ");
		printZ3Node(t, *result);
		__debugPrint(logFile, "\n===================\n");
#endif
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//------------------------------------------
	// Reduce app: Contains
	//------------------------------------------
	if (d == td->Contains) {
		*result = reduce_contains(t, convertedArgs);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): Contains( ");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, " )");
		__debugPrint(logFile, "  =>  ");
		printZ3Node(t, *result);
		__debugPrint(logFile, "\n===================\n");
#endif
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//------------------------------------------
	// Reduce app: StartsWith
	//------------------------------------------
	if (d == td->StartsWith) {
		*result = reduce_startswith(t, convertedArgs);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): StartsWith( ");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, " )");
		__debugPrint(logFile, "  =>  ");
		printZ3Node(t, *result);
		__debugPrint(logFile, "\n===================\n");
#endif
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//------------------------------------------
	// Reduce app: EndsWith
	//------------------------------------------
	if (d == td->EndsWith) {
		*result = reduce_endswith(t, convertedArgs);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): StartsWith( ");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, " )");
		__debugPrint(logFile, "  =>  ");
		printZ3Node(t, *result);
		__debugPrint(logFile, "\n===================\n");
#endif
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	if (d == td->Indexof_I) {
		*result = mk_idxof(t, convertedArgs[0], convertedArgs[1]);
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//------------------------------------------
	// Reduce app: Indexof
	//------------------------------------------
	if (d == td->Indexof) {
		Z3_ast breakDownAst = NULL;
		*result = reduce_indexof(t, convertedArgs, breakDownAst);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): Indexof(");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, ")");
		__debugPrint(logFile, "  =>  ");
		printZ3Node(t, *result);
		if( breakDownAst != NULL )
		{
			__debugPrint(logFile, "\n-- ADD(@%d): \n", __LINE__);
			printZ3Node(t, breakDownAst);
		}
		__debugPrint(logFile, "\n===================\n");
#endif
		// when quick path is taken, breakDownAst == NULL;
		if (breakDownAst != NULL)
			Z3_assert_cnstr(ctx, breakDownAst);
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//------------------------------------------
	// Reduce app: Replace
	//------------------------------------------
	if (d == td->Replace) {
		Z3_ast breakDownAst = NULL;
		*result = reduce_replace(t, convertedArgs, breakDownAst);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): Replace(");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[2]);
		__debugPrint(logFile, ")");
		__debugPrint(logFile, "  =>  ");
		printZ3Node(t, *result);
		if( breakDownAst != NULL )
		{
			__debugPrint(logFile, "\n-- ADD(@%d): \n", __LINE__);
			printZ3Node(t, breakDownAst);
		}
		__debugPrint(logFile, "\n===================\n");
#endif
		if (breakDownAst != NULL)
			Z3_assert_cnstr(ctx, breakDownAst);
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//------------------------------------------
	// Reduce app: ReplaceAllExt
	//------------------------------------------
	if (d == td->ReplaceAllExt) {
		*result = mk_replaceAll(t, convertedArgs[0], convertedArgs[1], convertedArgs[2]);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): ReplaceAllExt(");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[2]);
		__debugPrint(logFile, ")");
		__debugPrint(logFile, "  =>  ");
		printZ3Node(t, *result);
		__debugPrint(logFile, "\n===================\n");
#endif
		delete[] convertedArgs;
		return Z3_TRUE;
	}

	//------------------------------------------
	// Reduce app: Search
	//------------------------------------------
	if (d == td->Search) {
		*result = reduce_search(t, convertedArgs);
#ifdef DEBUGLOG
		__debugPrint(logFile, "\n===================\n");
		__debugPrint(logFile, "** cb_reduce_app(): Search(");
		printZ3Node(t, convertedArgs[0]);
		__debugPrint(logFile, ", ");
		printZ3Node(t, convertedArgs[1]);
		__debugPrint(logFile, ")");
		__debugPrint(logFile, "  =>  ");
		printZ3Node(t, *result);
		__debugPrint(logFile, "\n===================\n");
#endif
		delete[] convertedArgs;
		return Z3_TRUE;
	}
	//======================================

	delete[] convertedArgs;
	return Z3_FALSE; // failed to simplify
}

/*
 *
 */
void cb_push(Z3_theory t) {
	sLevel++;
	__debugPrint(logFile, "\n*******************************************\n");
	__debugPrint(logFile, "[PUSH]: Level = %d", sLevel);
	__debugPrint(logFile, "\n*******************************************\n");
}

/*
 *
 */
void cb_pop(Z3_theory t) {
	sLevel--;
	__debugPrint(logFile, "\n*******************************************\n");
	__debugPrint(logFile, "[POP]: Level = %d", sLevel);
	__debugPrint(logFile, "\n*******************************************\n");

	//    std::map<Z3_ast, std::stack<T_cut *> >::iterator sfxItor = cut_SuffixMap.begin();
	//    while (sfxItor != cut_SuffixMap.end())
	//    {
	//        while ((sfxItor->second.size() > 0) && (sfxItor->second.top()->level != 0)
	//                && (sfxItor->second.top()->level >= sLevel))
	//        {
	//            T_cut * aCut = sfxItor->second.top();
	//            sfxItor->second.pop();
	//            delete aCut;
	//        }
	//        if (sfxItor->second.size() == 0)
	//            cut_SuffixMap.erase(sfxItor++);
	//        else
	//            sfxItor++;
	//    }

	std::map<Z3_ast, std::stack<T_cut *> >::iterator varItor =
			cut_VARMap.begin();
	while (varItor != cut_VARMap.end()) {
		while ((varItor->second.size() > 0)
				&& (varItor->second.top()->level != 0)
				&& (varItor->second.top()->level >= sLevel)) {
			T_cut * aCut = varItor->second.top();
			varItor->second.pop();
			delete aCut;
		}
		if (varItor->second.size() == 0)
			cut_VARMap.erase(varItor++);
		else
			varItor++;
	}

}

/*
 *
 */
void cb_reset(Z3_theory t) {
	__debugPrint(logFile, "\n** Reset():\n");
}

/*
 *
 */
void cb_restart(Z3_theory t) {
	__debugPrint(logFile, "\n** Restart():\n");
}

/*
 *
 */
void cb_new_relevant(Z3_theory t, Z3_ast n) {
	if (getNodeType(t, n) == my_Z3_Str_Var) {
		basicStrVarAxiom(t, n, __LINE__);
	}
}

/*
 *
 */
void cb_delete(Z3_theory t) {
	__debugPrint(logFile, "\n** Delete()\n");
	PATheoryData * td = (PATheoryData *) Z3_theory_get_ext_data(t);
	free(td);
}

/*
 *
 */
void display_symbol(Z3_context c, FILE * out, Z3_symbol s) {
	switch (Z3_get_symbol_kind(c, s)) {
	case Z3_INT_SYMBOL:
		fprintf(out, "#%d", Z3_get_symbol_int(c, s));
		break;
	case Z3_STRING_SYMBOL:
		fprintf(out, "%s", Z3_get_symbol_string(c, s));
		break;
	default:
		break;
	}
}

/*
 *
 */
void display_sort(Z3_theory t, FILE * out, Z3_sort ty) {
	Z3_context c = Z3_theory_get_context(t);
	PATheoryData * td = (PATheoryData*) Z3_theory_get_ext_data(t);
	switch (Z3_get_sort_kind(c, ty)) {
	case Z3_UNINTERPRETED_SORT: {
		display_symbol(c, out, Z3_get_sort_name(c, ty));
		break;
	}
	case Z3_BOOL_SORT: {
		fprintf(out, "bool");
		break;
	}
	case Z3_INT_SORT:
		fprintf(out, "int");
		break;
	case Z3_REAL_SORT: {
		fprintf(out, "real");
		break;
	}
	case Z3_BV_SORT: {
		fprintf(out, "bv%d", Z3_get_bv_sort_size(c, ty));
		break;
	}
	case Z3_ARRAY_SORT: {
		fprintf(out, "[");
		display_sort(t, out, Z3_get_array_sort_domain(c, ty));
		fprintf(out, "->");
		display_sort(t, out, Z3_get_array_sort_range(c, ty));
		fprintf(out, "]");
		break;
	}
	case Z3_DATATYPE_SORT: {
		if (Z3_get_datatype_sort_num_constructors(c, ty) != 1) {
			fprintf(out, "%s", Z3_sort_to_string(c, ty));
			break;
		}

		unsigned num_fields = Z3_get_tuple_sort_num_fields(c, ty);
		unsigned i;
		fprintf(out, "(");
		for (i = 0; i < num_fields; i++) {
			Z3_func_decl field = Z3_get_tuple_sort_field_decl(c, ty, i);
			if (i > 0) {
				fprintf(out, ", ");
			}
			display_sort(t, out, Z3_get_range(c, field));
		}
		fprintf(out, ")");
		break;
	}
	default: {
		if (ty == td->String) {
			fprintf(out, "string");
			break;
		} else {
			fprintf(out, "unknown[");
			display_symbol(c, out, Z3_get_sort_name(c, ty));
			fprintf(out, "]");
		}
		break;
	}
	}
}

/*
 *
 */
void display_ast(Z3_theory t, FILE * out, Z3_ast v) {
	Z3_context c = Z3_theory_get_context(t);
	switch (Z3_get_ast_kind(c, v)) {
	case Z3_NUMERAL_AST: {
		fprintf(out, "%s", Z3_get_numeral_string(c, v));
		break;
	}
	case Z3_APP_AST: {
		if (isConstStr(t, v)) {
			std::string str = getConstStrValue(t, v);
			std::string escapedStr = "";
			for (unsigned int i = 0; i < str.length(); i++) {
				escapedStr = escapedStr + encodeToEscape(str[i]);
			}
			fprintf(out, "\"%s\"", escapedStr.c_str());
		} else {
			fprintf(out, "%s", Z3_ast_to_string(c, v));
		}
		break;
	}
	default: {
		fprintf(out, "> Error: Cannot print the value for %s\nExit.", Z3_ast_to_string(c, v));
		exit(0);
	}
	}
}

/*
 *
 */
void display_model(Z3_theory t, FILE * out, Z3_model m) {
	Z3_context c = Z3_theory_get_context(t);
	unsigned num_constants;
	unsigned i;

	num_constants = Z3_get_model_num_constants(c, m);
	for (i = 0; i < num_constants; i++) {
		Z3_func_decl cnst = Z3_get_model_constant(c, m, i);
		Z3_symbol name = Z3_get_decl_name(c, cnst);
		Z3_ast a = Z3_mk_app(c, cnst, 0, 0);
		Z3_ast v = a;
		Z3_eval(c, m, a, &v);
		Z3_sort v_sort = Z3_get_sort(c, v);

		display_symbol(c, out, name);
		fprintf(out, " : ");
		display_sort(t, out, v_sort);

		fprintf(out, " -> ");
		display_ast(t, out, v);
		fprintf(out, "\n");
	}
}

Z3_model cachedModel = nullptr;
int loud = 0;

void my_set_loud(int level) {
		loud = level;
}

/*
 *
 */
int my_check(Z3_theory t) {
	int isSAT = -1;
	Z3_context ctx = Z3_theory_get_context(t);
	Z3_lbool result = Z3_check_and_get_model(ctx, &cachedModel);

	if (loud) {
		__debugPrint(logFile, "\n*****************************\n");
		printf("************************\n>> ");
	}

	switch (result) {
		case Z3_L_FALSE: {
			isSAT = -1;
			if (loud) {
				if (loopDetected) {
					printf("UNKNOWN\n");
					__debugPrint(logFile, "UNKNOWN\n");
				} else {
					printf("UNSAT\n");
					__debugPrint(logFile, "UNSAT\n");
				}
			}
			break;
		}
		case Z3_L_UNDEF: {
			isSAT = 0;
			if (loud) {
				__debugPrint(logFile, "UNKNOWN\n ");
				__debugPrint(logFile, "POSSIBLE MODEL:\n");
				__debugPrint(logFile, "-----------------------------\n");
				__debugPrint(logFile, "%s", Z3_model_to_string(ctx, cachedModel));
				printf("UNKNOWN\n");
				printf("POSSIBLE MODEL:\n");
				printf("------------------------\n");
				printf("%s", Z3_model_to_string(ctx, cachedModel));
			}
			break;
		}
		case Z3_L_TRUE: {
			isSAT = 1;
			if (loud) {
				std::string modelStr = std::string(Z3_model_to_string(ctx, cachedModel));
				printf("SAT\n");
				printf("-----------------------------\n");
				printf("%s", modelStr.c_str());
				printf("SAT\n");
				printf("------------------------\n");
				display_model(t, stdout, cachedModel);
			}
			break;
		}
	}

	if (loud) {
		__debugPrint(logFile, "*****************************\n");
		printf("************************\n");
	}

	return isSAT;
}

Z3_model my_get_model() {
	return cachedModel;
}

void my_free_model(Z3_context ctx) {
	if (cachedModel) {
		Z3_del_model(ctx, cachedModel);
		cachedModel = nullptr;
	}
}

/*
 *Procedural attachment theory example.
 */
Z3_theory mk_pa_theory(Z3_context ctx) {

	PATheoryData * td = (PATheoryData *) malloc(sizeof(PATheoryData));
	Z3_theory Th = Z3_mk_theory(ctx, "StringAttachment", td);
	Z3_sort BoolSort = Z3_mk_bool_sort(ctx);
	Z3_sort IntSort = Z3_mk_int_sort(ctx);
	Z3_symbol string_name = Z3_mk_string_symbol(ctx, "String");
	td->String = Z3_theory_mk_sort(ctx, Th, string_name);

	Z3_symbol arb_char_name = Z3_mk_string_symbol(ctx, "Dot");
	td->Arb = Z3_theory_mk_func_decl(ctx, Th, arb_char_name, 0, 0, td->String);
	//---------------------------
	Z3_symbol concat_name = Z3_mk_string_symbol(ctx, "Concat");
	Z3_sort concat_domain[2];
	concat_domain[0] = td->String;
	concat_domain[1] = td->String;
	td->Concat = Z3_theory_mk_func_decl(ctx, Th, concat_name, 2, concat_domain,
			td->String);
	//---------------------------
	Z3_symbol union_name = Z3_mk_string_symbol(ctx, "Union");
	Z3_sort union_domain[2];
	union_domain[0] = td->String;
	union_domain[1] = td->String;
	td->Union = Z3_theory_mk_func_decl(ctx, Th, union_name, 2, union_domain,
			td->String);
	//---------------------------
	Z3_symbol star_name = Z3_mk_string_symbol(ctx, "Star");
	Z3_sort star_domain[1];
	star_domain[0] = td->String;
	td->Star = Z3_theory_mk_func_decl(ctx, Th, star_name, 1, star_domain,
			td->String);
	//---------------------------
	Z3_symbol star_i_name = Z3_mk_string_symbol(ctx, "Star_I");
	Z3_sort star_i_domain[2];
	star_i_domain[0] = td->String;
	star_i_domain[1] = IntSort;
	td->Star_I = Z3_theory_mk_func_decl(ctx, Th, star_i_name, 2, star_i_domain,
			td->String);
	//---------------------------
	Z3_symbol in_name = Z3_mk_string_symbol(ctx, "In");
	Z3_sort in_domain[2];
	in_domain[0] = td->String;
	in_domain[1] = td->String;
	td->In = Z3_theory_mk_func_decl(ctx, Th, in_name, 2, in_domain, BoolSort);
	//---------------------------
	Z3_symbol length_name = Z3_mk_string_symbol(ctx, "Length");
	Z3_sort length_domain[1];
	length_domain[0] = td->String;
	td->Length = Z3_theory_mk_func_decl(ctx, Th, length_name, 1, length_domain,
			IntSort);
	//---------------------------
	Z3_symbol substring_name = Z3_mk_string_symbol(ctx, "Substring");
	Z3_sort substring_domain[3];
	substring_domain[0] = td->String;
	substring_domain[1] = IntSort;
	substring_domain[2] = IntSort;
	td->SubStringExt = Z3_theory_mk_func_decl(ctx, Th, substring_name, 3,
			substring_domain, td->String);
	//---------------------------
	Z3_symbol substr_name = Z3_mk_string_symbol(ctx, "Substr");
	td->SubString = Z3_theory_mk_func_decl(ctx, Th, substr_name, 3,
			substring_domain, td->String);
	//---------------------------
	Z3_symbol indexof_name = Z3_mk_string_symbol(ctx, "Indexof");
	Z3_sort indexof_domain[2];
	indexof_domain[0] = td->String;
	indexof_domain[1] = td->String;
	td->Indexof = Z3_theory_mk_func_decl(ctx, Th, indexof_name, 2,
			indexof_domain, IntSort);
	//---------------------------
	Z3_symbol indexof_I_name = Z3_mk_string_symbol(ctx, "Indexof_I");
	td->Indexof_I = Z3_theory_mk_func_decl(ctx, Th, indexof_I_name, 2,
			indexof_domain, IntSort);
	//---------------------------
	Z3_symbol contains_name = Z3_mk_string_symbol(ctx, "Contains");
	Z3_sort contains_domain[2];
	contains_domain[0] = td->String;
	contains_domain[1] = td->String;
	td->Contains = Z3_theory_mk_func_decl(ctx, Th, contains_name, 2,
			contains_domain, BoolSort);
	//---------------------------
	Z3_symbol startsWith_name = Z3_mk_string_symbol(ctx, "StartsWith");
	Z3_sort startsWith_domain[2];
	startsWith_domain[0] = td->String;
	startsWith_domain[1] = td->String;
	td->StartsWith = Z3_theory_mk_func_decl(ctx, Th, startsWith_name, 2,
			startsWith_domain, BoolSort);
	//---------------------------
	Z3_symbol endsWith_name = Z3_mk_string_symbol(ctx, "EndsWith");
	Z3_sort endsWith_domain[2];
	endsWith_domain[0] = td->String;
	endsWith_domain[1] = td->String;
	td->EndsWith = Z3_theory_mk_func_decl(ctx, Th, endsWith_name, 2,
			endsWith_domain, BoolSort);
	//---------------------------
	Z3_symbol replace_name = Z3_mk_string_symbol(ctx, "Replace");
	Z3_sort replace_domain[3];
	replace_domain[0] = td->String;
	replace_domain[1] = td->String;
	replace_domain[2] = td->String;
	td->Replace = Z3_theory_mk_func_decl(ctx, Th, replace_name, 3,
			replace_domain, td->String);
	//---------------------------
	Z3_sort replaceAll_domain[3];
	replaceAll_domain[0] = td->String;
	replaceAll_domain[1] = td->String;
	replaceAll_domain[2] = td->String;
	Z3_symbol replaceAll_name = Z3_mk_string_symbol(ctx, "RepAll");
	td->ReplaceAll = Z3_theory_mk_func_decl(ctx, Th, replaceAll_name, 3,
			replaceAll_domain, td->String);
	//---------------------------
	Z3_symbol replaceAllExt_name = Z3_mk_string_symbol(ctx, "ReplaceAll");
	td->ReplaceAllExt = Z3_theory_mk_func_decl(ctx, Th, replaceAllExt_name, 3,
			replaceAll_domain, td->String);
	//---------------------------
	Z3_symbol search_name = Z3_mk_string_symbol(ctx, "Search");
	Z3_sort search_domain[2];
	search_domain[0] = td->String;
	search_domain[1] = td->String;
	td->Search = Z3_theory_mk_func_decl(ctx, Th, search_name, 2, search_domain,
			IntSort);
	//---------------------------
	Z3_set_delete_callback(Th, cb_delete);
	Z3_set_new_eq_callback(Th, cb_new_eq);
	Z3_set_final_check_callback(Th, cb_final_check);
	Z3_set_init_search_callback(Th, cb_init_search);
	Z3_set_push_callback(Th, cb_push);
	Z3_set_pop_callback(Th, cb_pop);
	Z3_set_reset_callback(Th, cb_reset);
	Z3_set_restart_callback(Th, cb_restart);
	Z3_set_new_relevant_callback(Th, cb_new_relevant);
	Z3_set_reduce_eq_callback(Th, cb_reduce_eq);
	Z3_set_reduce_app_callback(Th, cb_reduce_app);

	if (!charSet) {
		setAlphabet();
	}

	return Th;
}

/*
 *
 */
void throw_z3_error(Z3_context ctx, Z3_error_code c) {}

/*
 *
 */
Z3_context mk_context_custom(Z3_config cfg) {
	Z3_context ctx;
	Z3_set_param_value(cfg, "MODEL", "true");
	ctx = Z3_mk_context(cfg);
	Z3_set_error_handler(ctx, throw_z3_error);
	return ctx;
}

/*
 *
 */
Z3_context mk_my_context() {
	Z3_config cfg = Z3_mk_config();
	Z3_context ctx;
	ctx = mk_context_custom(cfg);
	Z3_del_config(cfg);
	return ctx;
}

/*
void pa_theory_example() {
	if (inputFile == "") {
		printf("No input file is provided.\n");
		return;
	}
	Z3_context ctx = mk_my_context();
	Z3_theory Th = mk_pa_theory(ctx);
	ctx = Z3_theory_get_context(Th);
	setAlphabet();

	// load cstr from inputFile
	Z3_ast fs = Z3_parse_smtlib2_file(ctx, inputFile.c_str(), 0, 0, 0, 0, 0, 0);

	getVarsInInput(Th, fs);

#ifdef DEBUGLOG
	__debugPrint(logFile, "\nInput loaded:\n-----------------------------------------------\n");
	printZ3Node(Th, fs);
	__debugPrint(logFile, "\n-----------------------------------------------\n\n");
#endif

	Z3_assert_cnstr(ctx, fs);
	check(Th);

	// clean up
	Z3_del_context(ctx);
}*/
