#ifndef __strTheory_H
#define __strTheory_H 1

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <assert.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <getopt.h>
#include <utility>
#include <limits.h>
#include <string>
#include <regex>
#include <iostream>

#include "z3.h"

#define str2Int_MinInt  -50
#define str2Int_MaxInt   50

#ifdef DEBUGLOG
#define __debugPrint(_fp, _format, ...) { fprintf( (_fp), (_format), ##__VA_ARGS__); fflush( (_fp) ); }
#define printZ3Node(t, n) {__printZ3Node( (t), (n));}
#else
#define __debugPrint(_fp, _format, ...) {}
#define printZ3Node(t, n) {}
#endif

//--------------------------------------------------
// Add your alphabet in freeVarAlphabet[]
//--------------------------------------------------
#define freeVarStep 3

extern char * charSet;
extern int charSetSize;
extern const std::string escapeDict[];

extern bool avoidLoopCut;
extern FILE * logFile;
extern std::string inputFile;
//--------------------------------------------------

/**
 * Theory specific data-structures.
 */
typedef struct _PATheoryData {
	Z3_sort String;
	Z3_func_decl Concat;
	Z3_func_decl Star;
	Z3_func_decl Star_I;
	Z3_func_decl Union;
	Z3_func_decl In;
	Z3_func_decl Length;
	Z3_func_decl SubString;
	Z3_func_decl SubStringExt;
	Z3_func_decl Indexof;
	Z3_func_decl Indexof_I;
	Z3_func_decl StartsWith;
	Z3_func_decl EndsWith;
	Z3_func_decl Contains;
	Z3_func_decl Replace;
	Z3_func_decl ReplaceAll;
	Z3_func_decl ReplaceAllExt;
	Z3_func_decl Search;
	Z3_func_decl Arb;

	//    Z3_func_decl Str2Int;  // assume the argument is always convertible
	//    Z3_func_decl Str2Real;

	//    Z3_func_decl OTHERVALUE;
} PATheoryData;

typedef enum {
	my_Z3_ConstStr = 0,    //
	my_Z3_ConstBool,   //
	my_Z3_Func,        //
	my_Z3_Num,         //
	my_Z3_Var,         //
	my_Z3_Str_Var,     //
	my_Z3_Int_Var,     //
	my_Z3_Quantifier,  //
	my_Z3_Unknown,     //
	my_Z3_Star,        //
	my_Z3_Concat,      //
	my_Z3_Search,      //
	my_Z3_ReplaceAll,  //
	my_Z3_SubString,  //
} T_myZ3Type;

//--------------------------------------------------
// Function Declaration
//--------------------------------------------------
void setAlphabet();

Z3_ast mk_var(Z3_context ctx, const char * name, Z3_sort ty);

Z3_ast mk_bool_var(Z3_context ctx, const char * name);

Z3_ast mk_int_var(Z3_context ctx, const char * name);

Z3_ast mk_int(Z3_context ctx, int v);

Z3_ast my_mk_str_value(Z3_theory t, char const * str);

Z3_ast my_mk_str_var(Z3_theory t, char const * name);

Z3_ast my_mk_internal_string_var(Z3_theory t);

int getNodeType(Z3_theory t, Z3_ast n);

bool isConstStr(Z3_theory t, Z3_ast node);

Z3_ast mk_1_arg_app(Z3_context ctx, Z3_func_decl f, Z3_ast x);

Z3_ast mk_2_arg_app(Z3_context ctx, Z3_func_decl f, Z3_ast x, Z3_ast y);

Z3_ast my_mk_and(Z3_theory t, Z3_ast * item, int count);

Z3_ast mk_2_and(Z3_theory t, Z3_ast and1, Z3_ast and2);

std::string mk_num_of_str(std::string s, int n);

Z3_ast mk_concat(Z3_theory t, Z3_ast n1, Z3_ast n2);

Z3_ast mk_star(Z3_theory t, Z3_ast n1, Z3_ast n2);

Z3_ast mk_replaceAll(Z3_theory t, Z3_ast n1, Z3_ast n2, Z3_ast n3);

bool isTwoStrEqual(std::string str1, std::string str2);

void print_eq_class(Z3_theory t, Z3_ast n);

void __printZ3Node(Z3_theory t, Z3_ast node);

Z3_ast get_eqc_value(Z3_theory t, Z3_ast n);

Z3_ast get_eqc_value_or_star(Z3_theory t, Z3_ast n);

Z3_ast get_eqc_num(Z3_theory t, Z3_ast n);

inline bool isConcatFunc(Z3_theory t, Z3_ast n);

inline bool isStarFunc(Z3_theory t, Z3_ast n);

inline bool isLengthFunc(Z3_theory t, Z3_ast n);

std::string getConstStrValue(Z3_theory t, Z3_ast n);

int getConstNumValue(Z3_theory t, Z3_ast n);

Z3_ast Concat(Z3_theory t, Z3_ast n1, Z3_ast n2);

void simplifyStrWithEqConstStr(Z3_theory t, Z3_ast nn, Z3_ast eq_str);

void solve_concat_eq_str(Z3_theory t, Z3_ast concatAst, Z3_ast constStr);

void solve_star_eq_str(Z3_theory t, Z3_ast starAst, Z3_ast constStr);

void getconstStrAstsInNode(Z3_theory t, Z3_ast node,
		std::list<Z3_ast> & astList);

bool inSameEqc(Z3_theory t, Z3_ast n1, Z3_ast n2);

bool canTwoNodesEq(Z3_theory t, Z3_ast n1, Z3_ast n2);

void simplifyConcatEq(Z3_theory t, Z3_ast nn1, Z3_ast nn2, int duplicateCheck = 1);

int newEqCheck(Z3_theory t, Z3_ast nn1, Z3_ast nn2);

void cb_new_eq(Z3_theory t, Z3_ast n1, Z3_ast n2);

Z3_ast genFreeVarOptions(Z3_theory t, Z3_ast freeVar, Z3_ast len_indicator,
		std::string indicatorStr, Z3_ast valTesterInCbEq,
		std::string valTesterValueStr);

Z3_ast genLenValOptionsForFreeVar(Z3_theory t, Z3_ast freeVar,
		Z3_ast lenTesterInCbEq, std::string lenTesterValue, bool isStar);

Z3_ast genValOptions(Z3_theory t, Z3_ast freeVar, int len);

Z3_bool cb_final_check(Z3_theory t);

inline std::string encodeToEscape(char c);

Z3_bool cb_reduce_eq(Z3_theory t, Z3_ast s_1, Z3_ast s_2, Z3_ast * r);

Z3_ast reduce_In(Z3_theory t, Z3_ast n1, Z3_ast n2);

Z3_ast reduce_contains(Z3_theory t, Z3_ast const * args);

Z3_ast reduce_contains2(Z3_theory t, Z3_ast const * args);

void getVarsInInput(Z3_theory t, Z3_ast node);

void cb_init_search(Z3_theory t);

Z3_bool cb_reduce_app(Z3_theory t, Z3_func_decl d, unsigned n,
		Z3_ast const * args, Z3_ast * result);

void cb_push(Z3_theory t);

void cb_pop(Z3_theory t);

void cb_reset(Z3_theory t);

void cb_restart(Z3_theory t);

void cb_new_relevant(Z3_theory t, Z3_ast n);

void cb_delete(Z3_theory t);

int my_check(Z3_theory t);

Z3_theory mk_pa_theory(Z3_context ctx);

void throw_z3_error(Z3_context ctx, Z3_error_code c);

Z3_context mk_context_custom(Z3_config cfg);

Z3_context mk_my_context();

void classifyAstByType(Z3_theory t, Z3_ast node, std::map<Z3_ast, int> & varMap,
		std::map<Z3_ast, int> & concatMap);

void basicConcatAxiom(Z3_theory t, Z3_ast vNode, int line);

void getNodesInConcat(Z3_theory t, Z3_ast node, std::vector<Z3_ast> & nodeList);

Z3_ast getMostLeftNodeInConcat(Z3_theory t, Z3_ast node);

Z3_ast getMostRightNodeInConcat(Z3_theory t, Z3_ast node);

Z3_ast simplifyConcat1(Z3_theory t, Z3_ast node);

Z3_ast simplifyConcat2(Z3_theory t, std::map<Z3_ast, Z3_ast> & aliasMap,
		Z3_ast node);

void print_relevant_length(Z3_theory t, std::map<Z3_ast, int> & wanted);

void print_All_Eqc(Z3_theory t);

int ctxDepAnalysis(Z3_theory t, std::map<Z3_ast, int> & varAppearMap,
		std::map<Z3_ast, int> & concatMap,
		std::map<Z3_ast, Z3_ast> & aliasIndexMap,
		std::map<Z3_ast, Z3_ast> & var_eq_constStr_map,
		std::map<Z3_ast, std::map<Z3_ast, int> > & var_eq_concat_map,
		std::map<Z3_ast, Z3_ast> & concat_eq_constStr_map,
		std::map<Z3_ast, std::map<Z3_ast, int> > & concat_eq_concat_map,
		std::map<Z3_ast, int> & freeVarMap,
		std::map<Z3_ast, std::map<Z3_ast, int> > & depMap,
		std::map<std::pair<Z3_ast, Z3_ast>, std::pair<Z3_ast, Z3_ast> > & toBreakMap);

Z3_ast mk_length(Z3_theory t, Z3_ast n);

void strEqLengthAxiom(Z3_theory t, Z3_ast varAst, Z3_ast strAst, int line);

void addAxiom(Z3_theory t, Z3_ast toAssert, int line, bool display = true);

void readdAxiom(Z3_theory t, Z3_ast toAssert, int line, bool display = true);

void basicStrVarAxiom(Z3_theory t, Z3_ast vNode, int line);

void handleNodesEqual(Z3_theory t, Z3_ast v1, Z3_ast v2);

int canConcatEqStr(Z3_theory t, Z3_ast concat, std::string str);

int canConcatEqConcat(Z3_theory t, Z3_ast concat1, Z3_ast concat2);

void doubleCheckForNotContain(Z3_theory t);

//void pa_theory_example();

#endif

