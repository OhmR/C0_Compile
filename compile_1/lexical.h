#ifndef _LEXICAL_H_
#define _LEXICAL_H_

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "Error.h"

using namespace std;

class lexical
{
public:
	lexical(std::string filename);
	~lexical();
	enum symbol
	{
		constsy, intsy, charsy, voidsy, mainsy, ifsy, elsesy, whilesy, forsy, scanfsy, printfsy, returnsy,
		stringcon, intcon, charcon, pluscon, minuscon, timescon, divcon, lparent, rparent, comma, lbrack, rbrack,
		lbrace, rbrace, semicolon, neq, eql, geq, leq, gtr, lss, ident, assign, err					//eql为判断是否相等==，assign为赋值=
	};
	std::string reserved[12] =
	{
		"const", "int", "char", "void", "main", "if",
		"else", "while", "for",	"scanf", "printf", "return"
	};
	std::string symbol_string[35] =
	{
		"constsy", "intsy", "charsy", "voidsy", "mainsy", "ifsy", "elsesy", "whilesy", "forsy",
		"scanfsy", "printfsy", "returnsy", "stringcon", "intcon", "charcon", "pluscon", "minuscon",
		"timescon", "divcon", "lparent", "rparent", "comma", "lbrack", "rbrack", "lbrace",
		"rbrace", "semicolon", "neq", "eql", "geq", "leq", "gtr", "lss", "ident", "assign"
	};
	void printmessage();
	string return_strvalue() { return strvalue; }
	int return_sy();
	int return_charvalue();
	int return_num();
	string return_name();
	int return_linenum();
	int getsy();

private:
	Error error;
	symbol sy;
	char ch;
	int num;
	string name;
	ifstream fin;
	int linenum;			//记录当前文件位置
	int wordnum;
	string str;
	string strvalue;		//记录字符串变量值
	char charvalue;
	int reserved_max;
	
	symbol syk[28] = {
		constsy, intsy, charsy, voidsy, mainsy, ifsy, elsesy,
		whilesy, forsy, scanfsy, printfsy, returnsy
	};
	int HandleSpaceAndComment();

	int isDigital();
	int isLetter();
	int isEql();
	int isGtr();		//大于
	int isLss();		//小于
	int isExc();		//感叹号
	int isPlus();
	int isMinus();
	int isTimes();
	int isDivi();
	int isLpar();
	int isRpar();
	int isLbrack();
	int isRbrack();
	int isLbrace();
	int isRbrace();
	int isComma();
	int isSemi();
	int isIllegal();

	void next_char();
	void clearname();
	void getname();
	void getnum();
};

#endif