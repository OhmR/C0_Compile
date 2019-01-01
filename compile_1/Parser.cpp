#include "pch.h"
#include "Parser.h"
#include <iostream>

Parser::Parser(string name)
{
	filename = name;		//最后考虑是否省略filename
	lex = new lexical(filename);
	sign = 1;
	flag_AddSub = 0;
	flag_MultDiv = 0;
	operande = "";
}

Parser::~Parser()
{
	delete lex;
}

void Parser::comparePara(string funcname, int num, string oper) {
	FTable Ftemp;
	info::type typ1, typ2;
	unordered_map<string, info> Mtemp;
	if (set.in_FTab(funcname)) {
		Ftemp = set.reFTable();
	}
	else if (set.curFTab.ReName() == funcname) {
		Ftemp = set.curFTab;
	}
	/*	if(Ftemp.in_ft(oper) || Ftemp.)*/
	Mtemp = Ftemp.ret_para().re_Map();
	if (num >= Ftemp.ret_para().ret_TableSize() / 4) {
		err.ErrorMessage(20, lex->return_linenum(), funcname);
		return;
	}

	auto i = Mtemp.begin();
	for (int j = 0; j < num; i++) {
		j++;
	}
	typ2 = i->second.typ;

	if (isNum(oper)) {
		if (WriteConst == "char") {
			typ1 = info::chars;
		}
		else {
			typ1 = info::ints;
		}
	}
	else if (set.curFTab.in_ft(oper) || set.curFTab.in_para(oper)) {
		typ1 = set.curFTab.ret_ifo().typ;
	}
	else if (set.in_Tab(oper)) {
		typ1 = set.reInfo().typ;
	}
	else {
		typ1 = info::notype;	//为找到该变量，非法
	}

	if (typ1 != typ2) {
		err.ErrorMessage(20, lex->return_linenum(), funcname);
	}
}

void Parser::JumpToChar(char ch) {
	switch (ch)
	{
	case '(': {
		while (lex->return_sy() != lexical::lparent) {
			lex->getsy();
		}
		lex->getsy();
		break;
	}
	case ')': {
		while (lex->return_sy() != lexical::rparent) {
			lex->getsy();
		}
		lex->getsy();
		break;
	}
	case '{': {
		while (lex->return_sy() != lexical::lbrace) {
			lex->getsy();
		}
		lex->getsy();
		break;
	}
	case '}': {
		while (lex->return_sy() != lexical::rbrace) {
			lex->getsy();
		}
		lex->getsy();
		break;
	}
	case ']': {
		while (lex->return_sy() != lexical::rbrack) {
			lex->getsy();
		}
		lex->getsy();
		break;
	}
	case ';': {
		while (lex->return_sy() != lexical::semicolon) {
			lex->getsy();
		}
		lex->getsy();
		break;
	}
	default:
		break;
	}
}

int Parser::isNum(string name) {
	if (name[0] == '-' || name[0] == '+' || name[0] <= '9' && name[0] >= '0')
		return 1;
	else
		return 0;
}

int Parser::transNum(string name) {
	int flag = 1;
	int num = 0;
	if (name[0] == '-')
		flag = -1;
	if (name[0] == '-' || name[0] == '+') {
		for (int i = 1; i < name.size(); i++)
			num += name[i] - '0' + num * 10;
	}
	else {
		for (int i = 0; i < name.size(); i++)
			num += name[i] - '0' + num * 10;
	}
	return num * flag;
}

int Parser::addOperator(){			//加法运算符
	if (lex->return_sy() == lexical::pluscon || lex->return_sy() == lexical::minuscon){
		if (lex->return_sy() == lexical::pluscon)
			flag_AddSub = 1;
		else
			flag_AddSub = 2;
		lex->getsy();
//		cout << "this is addOperator" << endl;
		return flag_AddSub;
	}
	else {
		return 0;
	}
}
int Parser::multOperator() {		//乘法运算符
	if (lex->return_sy() == lexical::divcon || lex->return_sy() == lexical::timescon) {
		if (lex->return_sy() == lexical::timescon)
			flag_MultDiv = 1;
		else
			flag_MultDiv = 2;
		lex->getsy();
//		cout << "this is multOperator" << endl;
		return flag_MultDiv;
	}
	else {
		return 0;
	}
}
int Parser::relateOperator() {		//关系运算符
	if ((lex->return_sy() >= lexical::neq) && (lex->return_sy() <= lexical::lss)) {
		relatesign = lex->return_sy();
		lex->getsy();
//		cout << "this is relateOperator" << endl;
		return 1;
	}
	else {
		return 0;
	}
}
void Parser::isChar() {				//字符
	if (lex->return_sy() == lexical::charcon) {
		charvalue = lex->return_charvalue();
		if (charvalue == '+' || charvalue == '-' || charvalue == '*' || charvalue == '/' || (charvalue <= '9') && (charvalue >= '0') || (charvalue <= 'z') && (charvalue >= 'a') || (charvalue <= 'Z') && (charvalue >= 'A') || charvalue == '_') {
			lex->getsy();
//			cout << "this is char!" << endl;
		}
		else {
			char tmp[10];
			tmp[0] = charvalue;
			string temp = tmp;
			err.ErrorMessage(4, lex->return_linenum(), temp);
		}
	}
	else {
		err.ErrorMessage(5, lex->return_linenum(), "charcon", lex->symbol_string[lex->return_sy()]);
	}
}
void Parser::program() {			//程序：[＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
	lex->getsy();
	if (lex->return_sy() == lexical::constsy) {
		constDec();
	}
	if (lex->return_sy() == lexical::charsy || lex->return_sy() == lexical::intsy) {
		varDec();
	}
	if (lex->return_sy() == lexical::lparent || lex->return_sy() == lexical::lbrace) {
		if (ifo.typ == info::ints) {								//获取函数名称，进入函数处理阶段
			set.curFTab = FTable::FTable(name, FTable::intsy);
			Code.genFuncDef(Quaternary::intfunc, name);
		}
		else {
			set.curFTab = FTable::FTable(name, FTable::charsy);
			Code.genFuncDef(Quaternary::charfunc, name);
		}
	}
	if (lex->return_sy() == lexical::lparent) {	//'('之前分析的var并非变量而是函数，需从括号处继续处理。
		lex->getsy();
		parametersTable();		//参数表
		if (lex->return_sy() == lexical::rparent) {	//')'
			lex->getsy();
		}
		else{
			err.ErrorMessage(6, lex->return_linenum(), ")");
			JumpToChar(')');
		}
		if (lex->return_sy() == lexical::lbrace) {	//'{'
			lex->getsy();
			compoundStatement();		//复合语句
			if (lex->return_sy() == lexical::rbrace) {	//'}'
				set.insert_FTab();		//标志着一个函数分析完成
				Code.genLabel(Quaternary::label, "label_" + set.curFTab.ReName());
				Code.genFuncEnd(Quaternary::funcend);
				lex->getsy();
			}
			else {
				err.ErrorMessage(6, lex->return_linenum(), "}");
				JumpToChar('}');
			}
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "{", ")");
			JumpToChar('}');
		}
	}
	if (lex->return_sy() == lexical::lbrace) {	//'{'
		lex->getsy();
		compoundStatement();		//复合语句
		if (lex->return_sy() == lexical::rbrace) {	//'}'
			set.insert_FTab();
			lex->getsy();
		}
		else {
			err.ErrorMessage(6, lex->return_linenum(), "}");
			JumpToChar('}');
		}
	}
	while (lex->return_sy() == lexical::voidsy || lex->return_sy() == lexical::intsy || lex->return_sy() == lexical::charsy)
	{
		if (lex->return_sy() == lexical::voidsy) {
			lex->getsy();
			if (lex->return_sy() == lexical::mainsy) {
				lex->getsy();
				mainFunc();
			}
			else
				noFuncDef();
		}
		else {
			reFuncDef();
		}
	}
	err_flag = err.err_flag | err_flag;
//	cout << "this is program!" << endl;
//	set.PrintSymbolSet();
//	Code.PrintMiddleCode();
}
void Parser::constDec() {			//常量说明
	if (lex->return_sy() != lexical::constsy) {
		return;
	}
	while (lex->return_sy() == lexical::constsy) {
		lex->getsy();
		constDef();
		if (lex->return_sy() != lex->semicolon) {		//判断分号结尾
			err.ErrorMessage(5, lex->return_linenum(), "semicolon", lex->symbol_string[lex->return_sy()]);
			JumpToChar(';');
		}
		else
			lex->getsy();
	}
//	cout << "this is constDec!" << endl;
}
void Parser::constDef() {			//常量定义:int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{ ,＜标识符＞＝＜字符＞ }		结束时sy为定义后的下一个单词
	ifo.reset();
	ifo.obj = info::constsy;		//字符表信息准备工作 
	if (lex->return_sy() == lexical::intsy) {
		do{
			lex->getsy();
			if (identity()) {	
				ifo.typ = info::ints;			//自此为ifo赋int型常量对应值
				if (lex->return_sy() == lexical::assign) {
					lex->getsy();
					Int();
					ifo.value = num;					//num为int型常量数值
					ifo.size = 4;
					if (!set.isInFunc()) {				//若尚未处理到当前函数（无名称），则为全局变量
						if(set.in_Tab(name)) {
							err.ErrorMessage(19, lex->return_linenum(), name);
						}
						else if (set.insert_Tab(name, ifo))
							Code.genConstDef(Quaternary::constdef, name);		//## 中间代码常量声明 ##
					}
					else {
						if(set.curFTab.in_ft(name) || set.curFTab.in_para(name) || set.curFTab.in_arr(name)){
							err.ErrorMessage(19, lex->return_linenum(), name);
						}
						else if (set.curFTab.insert_ft(name, ifo))
							Code.genConstDef(Quaternary::constdef, name);		//后期得改，函数定义常变量时怎么分配容量
					}
				}
				else {
					err.ErrorMessage(5, lex->return_linenum(), "assign", lex->symbol_string[lex->return_sy()]);
					while (lex->return_sy()!= lexical::semicolon && lex->return_sy() != lexical::comma) {	//在逗号和分号时停住
						lex->getsy();
					}
					err_flag = 1;
				}
			}
			else
			{
				err.ErrorMessage(5, lex->return_linenum(), "indent", lex->symbol_string[lex->return_sy()]);
				while (lex->return_sy() != lexical::semicolon && lex->return_sy() != lexical::comma) {	//在逗号和分号时停住
					lex->getsy();
				}
				err_flag = 1;
			}
		} while (lex->return_sy() == lexical::comma);
//		cout << "this is constDef" << endl;
	}
	else if (lex->return_sy() == lexical::charsy) {
		do {
			lex->getsy();
			if(identity()){
				ifo.typ = info::chars;		//标注常量类型为char
				if (lex->return_sy() == lexical::assign) {
					lex->getsy();
					isChar();	//字符
					ifo.ch = charvalue;		//标注常量值为charvalue
					ifo.size = 4;			//标注常量大小为4
					if (!set.isInFunc()) {	//若尚未处理到当前函数（无名称），则为全局变量
						if (set.in_Tab(name)) {
							err.ErrorMessage(19, lex->return_linenum(), name);
						}
						else if (set.insert_Tab(name, ifo))
							Code.genConstDef(Quaternary::constdef, name);
					}
					else {
						if (set.curFTab.in_ft(name) || set.curFTab.in_para(name) || set.curFTab.in_arr(name)) {
							err.ErrorMessage(19, lex->return_linenum(), name);
						}
						else if (set.curFTab.insert_ft(name, ifo))
							Code.genConstDef(Quaternary::constdef, name);		//后期得改，函数定义常变量时怎么分配容量
					}
				}
				else {
					err.ErrorMessage(5, lex->return_linenum(), "eql", lex->symbol_string[lex->return_sy()]);
					while (lex->return_sy() != lexical::semicolon && lex->return_sy() != lexical::comma) {	//在逗号和分号时停住
						lex->getsy();
					}
					err_flag = 1;
				}
			}
			else {
				err.ErrorMessage(5, lex->return_linenum(), "indent", lex->symbol_string[lex->return_sy()]);
				while (lex->return_sy() != lexical::semicolon && lex->return_sy() != lexical::comma) {	//在逗号和分号时停住
					lex->getsy();
				}
				err_flag = 1;
			}
		} while (lex->return_sy() == lexical::comma);
//		cout << "this is constDef" << endl;
	}
	else {
		err.ErrorMessage(5, lex->return_linenum(), "intsy or charsy", lex->symbol_string[lex->return_sy()]);
	}
}
int Parser::unsignInt() {			//无符号整数
	if (lex->return_sy() == lexical::intcon) {
		num = lex->return_num();
		lex->getsy();
//		cout << "this is unsignInt" << endl;
		return 1;
	}
	else {
		err.ErrorMessage(5, lex->return_linenum(), "intcon", lex->symbol_string[lex->return_sy()]);
		return 0;
	}
}
void Parser::Int() {				//整数
	if (lex->return_sy() == lexical::pluscon) {
		sign = 1;
		lex->getsy();
		if (unsignInt()){
			num = sign * num;
//			cout << "this is Int!" << endl;
		}
	}
	else if (lex->return_sy() == lexical::minuscon) {
		sign = -1;
		lex->getsy();
		if (unsignInt()) {
			num = sign * num;
//			cout << "this is Int!" << endl;
		}
	}
	else if (lex->return_sy() == lexical::intcon) {
		unsignInt();
//		cout << "this is Int!" << endl;
	}
	else{
		err.ErrorMessage(5, lex->return_linenum(), "Int", lex->symbol_string[lex->return_sy()]);
		return;
	}
}
int Parser::identity() {			//标识符
	if (lex->return_sy() != lexical::ident) {
		err.ErrorMessage(5, lex->return_linenum(), "ident", lex->symbol_string[lex->return_sy()]);
		return 0;
	}
	else{
		name = lex->return_name();	//获取标识符名称，用于填符号表
		lex->getsy();
//		cout << "this is identity!" << endl;
	}
	return 1;
}

void Parser::statementHead() {		//声明头部
	if (lex->return_sy() == lexical::charsy) {
		lex->getsy();
		identity();
		set.curFTab = FTable::FTable(name, FTable::charsy);
		Code.genFuncDef(Quaternary::charfunc, name);
	}
	else if(lex->return_sy() == lexical::intsy) {
		lex->getsy();
		identity();
		set.curFTab = FTable::FTable(name, FTable::intsy);
		Code.genFuncDef(Quaternary::intfunc, name);
	}
	else {
		err.ErrorMessage(6, lex->return_linenum(), "char or int");
		return;
	}
//	cout << "this is statementHead!" << endl;
}
void Parser::varDec() {				//变量说明
	ifo.reset();
	while (lex->return_sy() == lex->intsy || lex->return_sy() == lex->charsy)
	{
		if (lex->return_sy() == lex->intsy)		//填入类别信息
			ifo.typ = info::ints;
		else
			ifo.typ = info::chars;
		varDef();
		if (lex->return_sy() == lex->semicolon) {
			lex->getsy();
		}
		else if (lex->return_sy() == lexical::lbrace || lex->return_sy() == lexical::lparent) {
			return;
		}		//转而处理函数定义
		else {
			err.ErrorMessage(7, lex->return_linenum(), "semicolon or lbrace or lparent", "ident");
			return;
		}
	}
//	cout << "this is varDec!" << endl;
}
void Parser::varDef() {				//变量定义
	while (lex->return_sy() == lex->intsy || lex->return_sy() == lex->charsy)
	{
		do{
			lex->getsy();
			if (identity()) {
				if (lex->return_sy() == lexical::lbrack) {
					ifo.obj = info::arrays;		//记录为数组类型
					lex->getsy();
					if (unsignInt()) {
						ifo.size = num * 4;		//数组长度为num，单个元素大小为4
						if (lex->return_sy() == lexical::rbrack) {
							if (!set.isInFunc()){
								if (set.in_Tab(name)) {
									err.ErrorMessage(19, lex->return_linenum(), name);
								}
								else if (set.insert_ATab(name, ifo))
									Code.genVarDef(Quaternary::vardef, name, "array");
							}
							else{
								if (set.curFTab.in_ft(name) || set.curFTab.in_para(name) || set.curFTab.in_arr(name)) {
									err.ErrorMessage(19, lex->return_linenum(), name);
								}
								else if (set.curFTab.insert_arr(name, ifo))
									Code.genVarDef(Quaternary::vardef, name, "array");
							}
							lex->getsy();
						}
						else {
							err.ErrorMessage(7, lex->return_linenum(), "]", "unsign int");
							return;
						}
					}
					else {
						err.ErrorMessage(7, lex->return_linenum(), "unsign int", "[");
						return;
					}
				}		//数组
				else if (lex->return_sy() == lexical::lparent || lex->return_sy() == lexical::lbrace)	//说明不是变量，而是有返回值的函数定义
					return;
				else {	//常规变量
					ifo.obj = info::varsy;		//将常规的变量插入对应的表中
					ifo.size = 4; 
					if (!set.isInFunc()){
						if (set.in_Tab(name)) {
							err.ErrorMessage(19, lex->return_linenum(), name);
						}
						else if (set.insert_Tab(name, ifo))
							Code.genVarDef(Quaternary::vardef, name, "unarray");
					}
					else {
						if (set.curFTab.in_ft(name) || set.curFTab.in_para(name) || set.curFTab.in_arr(name)) {
							err.ErrorMessage(19, lex->return_linenum(), name);
						}
						else if (set.curFTab.insert_ft(name, ifo))
							Code.genVarDef(Quaternary::vardef, name, "unarray");
					}
				}
			}
		} while (lex->return_sy() == lex->comma);
	}
//	cout << "this is varDef!" << endl;
}
int Parser::Type() {				//类型标识符
	if (lex->return_sy() == lexical::intsy || lex->return_sy() == lexical::charsy) {
		if (lex->return_sy() == lexical::intsy)
			ifo.typ = info::ints;
		else
			ifo.typ = info::chars;
		lex->getsy();
//		cout << "this is Type" << endl;
		return 1;
	}
	else {
		err.ErrorMessage(6, lex->return_linenum(), "int or char");
		return 0;
	}
}
void Parser::reFuncDef() {								//有返回函数定义
	statementHead();
	if (lex->return_sy() == lexical::lparent) {			//'('
//		set.insert_map(name, 0, 0, 0, info::refunc);
		lex->getsy();
		parametersTable();								//参数表
		if (lex->return_sy() == lexical::rparent) {		//')'
			lex->getsy();
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), ")", "parrameters table");
			JumpToChar(')');
		}
	}
	if (lex->return_sy() == lexical::lbrace) {			//'{'
		lex->getsy();
		compoundStatement();							//复合语句
		if (lex->return_sy() == lexical::rbrace) {	//'}'
			set.insert_FTab();						//标志着一个函数的符号表建立完成
			Code.genLabel(Quaternary::label, "label_" + set.curFTab.ReName());
			Code.genFuncEnd(Quaternary::funcend);
			lex->getsy();
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "}", "parrameters table");
			JumpToChar('}');
		}
	}
//	cout << "this is reFuncDef!" << endl;
}
void Parser::noFuncDef() {			//无返回函数定义
	if (lex->return_sy() == lexical::voidsy) {
		lex->getsy();
	}
	identity();
	set.curFTab = FTable::FTable(name, FTable::voidsy);
	Code.genFuncDef(Quaternary::voidfunc, name);
	if (lex->return_sy() == lexical::lparent) {	//'('
		lex->getsy();
		parametersTable();		//参数表
		if (lex->return_sy() == lexical::rparent) {	//')'
			lex->getsy();
		}
		else {
				err.ErrorMessage(6, lex->return_linenum(), ")");
				JumpToChar(')');
		}
	}
	if (lex->return_sy() == lexical::lbrace) {	//'{'
		lex->getsy();
		compoundStatement();		//复合语句
		if (lex->return_sy() == lexical::rbrace) {	//'}'
			set.insert_FTab();						//标志着当前函数符号表建立完成
			Code.genLabel(Quaternary::label, "label_" + set.curFTab.ReName());
			Code.genFuncEnd(Quaternary::funcend);
			lex->getsy();
		}
		else {
			err.ErrorMessage(6, lex->return_linenum(), "}");
			JumpToChar('}');
		}
	}
//	cout << "this is noFuncDef!" << endl;
}

void Parser::compoundStatement() {	//复合语句
	if(lex->return_sy() == lexical::constsy)
		constDec();
	if(lex->return_sy() == lexical::intsy || lex->return_sy() == lexical::charsy)
		varDec();
	statementColumn();
//	cout << "this is compoundStatement!" << endl;
}
void Parser::parametersTable() {		//参数表：＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}
	do {
		ifo.reset();
		if (lex->return_sy() == lexical::comma)
			lex->getsy();
		if (Type()) {
			if (identity()) {
				ifo.size = 4;
				set.curFTab.insert_para(name, ifo);
				Code.genPara(Quaternary::para, name);
			}
			else{
				err.ErrorMessage(6, lex->return_linenum(), "ident");
				while (lex->return_sy() != lexical::rparent && lex->return_sy() != lexical::comma) {
					lex->getsy();
				}
				err_flag = 1;
			}
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "char or int", "(");
			while (lex->return_sy() != lexical::rparent && lex->return_sy() != lexical::comma) {
				lex->getsy();
			}
			err_flag = 1;
		}
	} while (lex->return_sy() == lexical::comma);
//	cout << "this is parametersTable" << endl;	//错误跳转，分号逗号停住。
}
void Parser::mainFunc() {			//主函数
	set.curFTab = FTable::FTable("main", FTable::mainsy);
	Code.genMainFunc(Quaternary::mainfunc);
	if (lex->return_sy() == lexical::lparent) {
		lex->getsy();
		if (lex->return_sy() == lexical::rparent) {
			lex->getsy();
			if (lex->return_sy() == lexical::lbrace) {
				lex->getsy();
				compoundStatement();
				if (lex->return_sy() == lexical::rbrace) {
					set.insert_FTab();			//标记mian函数处理完毕
					Code.genLabel(Quaternary::label, "label_" + set.curFTab.ReName());
//					cout << "this is main!" << endl;
				}
				else {
					err.ErrorMessage(6, lex->return_linenum(), "}");
					JumpToChar('}');
				}
			}
			else {
				err.ErrorMessage(7, lex->return_linenum(), "{", ")");
				JumpToChar('}');
			}
		}
		else {
			err.ErrorMessage(6, lex->return_linenum(), ")");
			JumpToChar('}');
		}
	}
	else {
		err.ErrorMessage(7, lex->return_linenum(), "(", "main");
		return;
	}
}
void Parser::expr() {				//表达式
	string T1, T2, T3;
	int flag;
	flag = addOperator();
	if (flag == 1) {
		term();
	}
	else if (flag == 2) {
		term();
		string T = Code.GenTempName(temp_count++);
		info tmp;
		tmp.typ = info::ints;
		set.curFTab.insert_ft(T, tmp);		//临时变量的类型需要分类讨论,用以准确打印
		Code.genNagtive(Quaternary::neg, operande, T);
		operande = T;
	}
	else {
		term();
	}
	T1 = operande;
	while ((flag = addOperator()) && flag) {
		if(flag == 1){				//加
			term();
			T2 = operande;
			T3 = Code.GenTempName(temp_count++); 
			info tmp;
			tmp.typ = info::ints;
			set.curFTab.insert_ft(T3, tmp);		//临时变量的类型均为notype
			Code.genCalculate(Quaternary::add, T3, T1, T2);
		}
		else if (flag == 2) {		//减
			term();
			T2 = operande;
			T3 = Code.GenTempName(temp_count++);
			info tmp;
			tmp.typ = info::ints;
			set.curFTab.insert_ft(T3, tmp);		//临时变量的类型均为notype
			Code.genCalculate(Quaternary::sub, T3, T1, T2);
		}
		T1 = T3;
		operande = T3;
	}

//	cout << "this is expr!" << endl;
}
void Parser::term() {				//项
	string T1, T2, T3;
	int flag = 0;
	do {
		factor();
		T2 = operande;
		if (flag == 1) {			//乘
			T3 = Code.GenTempName(temp_count++);
			info tmp;
			tmp.typ = info::ints;
			set.curFTab.insert_ft(T3, tmp);		//临时变量的类型均为notype
			Code.genCalculate(Quaternary::mult, T3, T1, T2);
			T1 = T3;
		}
		else if (flag == 2) {		//除
			T3 = Code.GenTempName(temp_count++);
			info tmp;
			tmp.typ = info::ints;
			set.curFTab.insert_ft(T3, tmp);		//临时变量的类型均为notype
			Code.genCalculate(Quaternary::div, T3, T1, T2);
			T1 = T3;				
		}
		else {
			T1 = operande;			//第一次乘除法运算的第一个元素
			T3 = operande;			//防止无乘除法运算直接返回
		}
	} while ((flag = multOperator()) && flag);
	operande = T3;
//	cout << "this is term!" << endl;
}
void Parser::factor() {				//因子:=＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’|‘(’＜表达式＞‘)’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
	string T;
	string temp_name;
	if (lex->return_sy() == lexical::ident) {				//＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜有返回值函数调用语句＞
		temp_name = lex->return_name();
		lex->getsy();
		if (lex->return_sy() == lexical::lbrack) {			//数组
			lex->getsy();
			expr();

			T = Code.GenTempName(temp_count++);
			info tmp;
			int arraysize;
			if (set.curFTab.in_arr(temp_name)) {
				tmp.typ = set.curFTab.ret_ifo().typ;		//临时变量类型和数组保持一致
				arraysize = set.curFTab.ret_ifo().size / 4;
			}
			else if (set.in_ATab(temp_name)) {
				tmp.typ = set.reInfo().typ;
				arraysize = set.reInfo().size / 4;
			}
			else {
				err.ErrorMessage(1, lex->return_linenum(), temp_name);
			}

			if (isNum(operande)) {		//数组越界错误
				if (transNum(operande) >= arraysize)
					err.ErrorMessage(11, lex->return_linenum(), temp_name);
			}
			else if (set.curFTab.in_ft(operande)) {
				if (set.curFTab.ret_ifo().obj == info::constsy) {
					if (set.curFTab.ret_ifo().value >= arraysize)
						err.ErrorMessage(11, lex->return_linenum(), temp_name);
				}
			}
			else if (set.in_Tab(operande) && set.reInfo().obj == info::constsy) {
				if (set.reInfo().value >= arraysize)
					err.ErrorMessage(11, lex->return_linenum(), temp_name);
			}

			set.curFTab.insert_ft(T, tmp);		//临时变量的类型均为notype
			Code.genAssignArr(Quaternary::arrassign, T, temp_name, operande);		//向T中赋数组元素值
			operande = T;
			if (lex->return_sy() == lexical::rbrack) {
				lex->getsy();
			}
			else {
				err.ErrorMessage(6, lex->return_linenum(), "]");
				JumpToChar(']');
			}
		}
		else if (lex->return_sy() == lexical::lparent) {
			reFuncCall();									//＜有返回值函数调用语句＞
		}
		else if (set.in_FTab(temp_name) || set.curFTab.ReName() == temp_name) {
			reFuncCall();
		}
		else {												//标识符
			operande = temp_name;
			if (!set.curFTab.in_ft(temp_name) && !set.curFTab.in_para(temp_name) && !set.in_Tab(temp_name)) {
				err.ErrorMessage(1, lex->return_linenum(), temp_name); 
			}
//			cout << "this is factor" << endl;
			return;
		}
	}
	else if(lex->return_sy() == lexical::lparent){			//(表达式)
		lex->getsy();
		expr();
		if (lex->return_sy() != lexical::rparent) {
			err.ErrorMessage(6, lex->return_linenum(), ")");
			JumpToChar(')');
		}
		else {
			WriteConst = "int";
			lex->getsy();
		}
	}
	else if (lex->return_sy() == lexical::charcon) {		//字符
		isChar();
		operande = to_string(int(charvalue));			//把字符转换为assic码
		T = Code.GenTempName(temp_count++);
		WriteConst = "char";
/*		info tmp;
		tmp.typ = info::chars;
		set.curFTab.insert_ft(T, tmp);		//临时变量的类型均为notype
		Code.genAssign(Quaternary::assign, T, operande);
		operande = T;*/
	}
	else if(lex->return_sy() == lexical::intcon || lex->return_sy() == lexical::pluscon || lex->return_sy() == lexical::minuscon){	//整数
		Int();
		operande = to_string(num);
		T = Code.GenTempName(temp_count++);
		WriteConst = "int";
/*		info tmp;
		tmp.typ = info::ints;
		set.curFTab.insert_ft(T, tmp);		//临时变量的类型均为notype
		Code.genAssign(Quaternary::assign, T, operande);
		operande = T;*/
	}
	else {
		err.ErrorMessage(9, lex->return_linenum());
		return;
	}
//	cout << "this is factor!" << endl;
}

void Parser::statement() {			//语句
	int flag = 0;
	switch (lex->return_sy())
	{
	case lexical::ifsy: {
		flag = 1;
		lex->getsy();
		ifStatement();
		break;
	}
	case lexical::whilesy: {
		flag = 1;
		cycleStatement();
		break;
	}
	case lexical::forsy: {
		flag = 1;
		cycleStatement();
		break;
	}
	case lexical::lbrace: {
		flag = 1;
		lex->getsy();
		statementColumn();
		if (lex->return_sy() == lexical::rbrace) {
			lex->getsy();
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "}", "statement column");
			JumpToChar('}');
		}
		break;
	}
	case lexical::ident: {
		string tmp = lex->return_name();	//之后用name判断是什么类型的函数调用。
		lex->getsy();
		if (lex->return_sy() == lexical::assign || lex->return_sy() == lexical::lbrack) {
			assignStatement();
		}
		else if (lex->return_sy() == lexical::lparent) {
			if (set.in_FTab(tmp)){			//名称匹配的函数可能在符号表中，也可能是当前处理的函数
				if (set.reFTable().ReFuncType() == FTable::voidsy)
					noFuncCall();
				else
					reFuncCall();
			}
			else if(set.curFTab.ReName() == tmp){
				if (set.reFTable().ReFuncType() == FTable::voidsy)
					noFuncCall();
				else
					reFuncCall();
			}
		}	//有无返回值函数调用(有参数)
		else {
			if (set.in_FTab(tmp)){			//名称匹配的函数可能在符号表中，也可能是当前处理的函数
				if (set.reFTable().ReFuncType() == FTable::voidsy)
					noFuncCall();
				else
					reFuncCall();
			}
			else if (set.curFTab.ReName() == tmp) {
				if (set.reFTable().ReFuncType() == FTable::voidsy)
					noFuncCall();
				else
					reFuncCall();
			}
			else {
				cout << "unknowen ident " + tmp << endl;
				JumpToChar(';');
				return;
			}
		}	//有无返回值函数调用(无参数)
		break;
	}
	case lexical::scanfsy: {
		read();
		break;
	}
	case lexical::printfsy: {
		write();
		break;
	}
	case lexical::semicolon: {	//空语句
		break;
	}
	case lexical::returnsy: {
		Return();
		break;
	}
	default: {
		err.ErrorMessage(8, lex->return_linenum());
		JumpToChar(';');
		return;
		break;
	}
	}

	if (lex->return_sy() == lexical::semicolon) {		//读末尾的分号
		lex->getsy();
//		cout << "this is statement!" << endl;
	}
	else if (flag == 0) {
		err.ErrorMessage(6, lex->return_linenum(), ";");
		JumpToChar(';');
	}
}
void Parser::assignStatement() {		//赋值语句 ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
	string tmp = lex->return_name();
	string T;
	if (lex->return_sy() == lexical::lbrack) {		//数组
		int arraysize = 0;
		if (!set.curFTab.in_arr(tmp) && !set.in_ATab(tmp)) {
			err.ErrorMessage(1, lex->return_linenum(), tmp);
		}
		if (set.curFTab.in_arr(tmp)) {
			arraysize = set.curFTab.ret_ifo().size /4;
		}
		else if (set.in_ATab(tmp)) {
			arraysize = set.reInfo().size /4;
		}

		lex->getsy();
		expr();
//		reset_temp();

		if (isNum(operande)) {		//数组越界错误
			if (transNum(operande) >= arraysize)
				err.ErrorMessage(11, lex->return_linenum(), tmp);
		}
		else if (set.curFTab.in_ft(operande)) {
			if (set.curFTab.ret_ifo().obj == info::constsy) {
				if (set.curFTab.ret_ifo().value >= arraysize)
					err.ErrorMessage(11, lex->return_linenum(), tmp);
			}
		}
		else if (set.curFTab.in_arr(operande)) {	//若为函数参数名称
			;
		}
		else if (set.in_Tab(operande) && set.reInfo().obj == info::constsy) {
			if (set.reInfo().value >= arraysize)
				err.ErrorMessage(11, lex->return_linenum(), tmp);
		}

		T = operande;
;		if (lex->return_sy() == lexical::rbrack) {
			lex->getsy();
			if (lex->return_sy() == lexical::assign) {
				lex->getsy();
				expr();
//				reset_temp();
				Code.genAssignToArr(Quaternary::assigntoarr, tmp, T, operande);
			}
			else {
				err.ErrorMessage(7, lex->return_linenum(), "=", "]");
				return;
			}
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "]", "expr");
			return;
		}
	}
	else if(lex->return_sy() == lexical::assign){		//变量
		if (!set.curFTab.in_ft(tmp) && !set.curFTab.in_para(tmp) && !set.in_Tab(tmp)) {
			err.ErrorMessage(1, lex->return_linenum(), tmp);
		}
		if (set.curFTab.in_ft(tmp)) {		//给常量赋值错误
			if(set.curFTab.ret_ifo().obj == info::constsy)
				err.ErrorMessage(12, lex->return_linenum(), tmp);
		}
		else if (set.in_Tab(tmp) && set.reInfo().obj == info::constsy)
			err.ErrorMessage(12, lex->return_linenum(), tmp);

		lex->getsy();
		expr();
//		reset_temp();
		Code.genAssign(Quaternary::assign, tmp, operande);
	}	//前面描述的indent已经处理过了
//	cout << "this is assignStatement" << endl;
}
void Parser::ifStatement() {			//条件语句 if ‘(’＜条件＞‘)’＜语句＞[else＜语句＞]
	string label_temp;
	string con_label;
	if (lex->return_sy() == lexical::lparent) {
		lex->getsy();
		con_label = condition();
		if (lex->return_sy() == lexical::rparent) {
			lex->getsy();
		}
		else {
			err.ErrorMessage(6, lex->return_linenum(), ")");
			JumpToChar(')');
		}
		statement();
		if (lex->return_sy() == lexical::elsesy) {	//可省略
			label_temp = Code.GenLabelName(label++);
			Code.genJumpUncon(Quaternary::go, label_temp);
			Code.genLabel(Quaternary::label, con_label);
			lex->getsy();
			statement();
			Code.genLabel(Quaternary::label, label_temp);
		}
		else {
			Code.genLabel(Quaternary::label, con_label);
		}
//		cout << "this is if statement!" << endl;
	}
	else {
		err.ErrorMessage(7, lex->return_linenum(), "(", "if");
		JumpToChar('}');
	}
}
string Parser::condition() {			//条件 ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
	string label_temp;
	string type1, type2;
	expr();
//	reset_temp();
	op1 = operande;
	if (isNum(op1)) {
		type1 = WriteConst;
	}
	else if (set.curFTab.in_ft(op1) || set.curFTab.in_para(op1)) {
		if (set.curFTab.ret_ifo().typ == info::ints)
			type1 = "int";
		else
			type1 = "char";
	}
	else if (set.in_Tab(op1)) {
		if (set.reInfo().typ == info::ints)
			type1 = "int";
		else
			type1 = "char";
	}
	else {
		err.ErrorMessage(1, lex->return_linenum(), op1);
	}

	if (relateOperator()) {
		expr();
//		reset_temp();
		op2 = operande;
		if (isNum(op2)) {
			type2 = WriteConst;
		}
		else if (set.curFTab.in_ft(op2) || set.curFTab.in_para(op2)) {
			if (set.curFTab.ret_ifo().typ == info::ints)
				type2 = "int";
			else
				type2 = "char";
		}
		else if (set.in_Tab(op2)) {
			if (set.reInfo().typ == info::ints)
				type2 = "int";
			else
				type2 = "char";
		}
		else {
			err.ErrorMessage(1, lex->return_linenum(), op2);
		}

		if (type1 != type2) {
			err.ErrorMessage(10, lex->return_linenum());
		}
		if (type1 == "char") {	//出现 'a' > 'c' 一类的情况
			err.ErrorMessage(15, lex->return_linenum(), op1);
		}
		else if (type2 == "char") {
			err.ErrorMessage(15, lex->return_linenum(), op2);
		}

		label_temp = Code.GenLabelName(label++);
		switch (relatesign)
		{
		case 27: Code.genJumpCondition(Quaternary::beq, op1, op2, label_temp); break;
		case 28: Code.genJumpCondition(Quaternary::bne, op1, op2, label_temp); break;
		case 29: Code.genJumpCondition(Quaternary::bls, op1, op2, label_temp); break;
		case 30: Code.genJumpCondition(Quaternary::bgt, op1, op2, label_temp); break;
		case 31: Code.genJumpCondition(Quaternary::ble, op1, op2, label_temp); break;
		case 32: Code.genJumpCondition(Quaternary::bge, op1, op2, label_temp); break;
		default:break;
		}
	}
	else {
		label_temp = Code.GenLabelName(label++);
		Code.genJumpCondition(Quaternary::beq, "0", op1, label_temp);
	}
//	cout << "this is condition!" << endl;
	return label_temp;
}
void Parser::cycleStatement() {		//循环语句 while ‘(’＜条件＞‘)’＜语句＞ |for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
	string label_temp1, label_temp2, label_temp3;
	string con_label;
	if (lex->return_sy() == lexical::whilesy) {
		lex->getsy();
		if (lex->return_sy() == lexical::lparent) {
			label_temp1 = Code.GenLabelName(label++);
			Code.genLabel(Quaternary::label, label_temp1);
			lex->getsy();
			con_label = condition();
			if (lex->return_sy() == lexical::rparent) {
				lex->getsy();
			}
			else {
				err.ErrorMessage(6, lex->return_linenum(), ")");
				JumpToChar(')');
			}
			statement();
			Code.genJumpUncon(Quaternary::go, label_temp1);
			Code.genLabel(Quaternary::label, con_label);
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "(", "while");
			JumpToChar('}');
		}
	}
	else if (lex->return_sy() == lexical::forsy) {
		lex->getsy();
		if (lex->return_sy() == lexical::lparent) {
			lex->getsy();
			identity();
			string name_temp = name;
			string name_temp2;
			if (lex->return_sy() == lexical::assign) {
				lex->getsy();
				expr();
//				reset_temp();

				if (set.curFTab.in_ft(name_temp)) {		//给常量赋值错误
					if (set.curFTab.ret_ifo().obj == info::constsy)
						err.ErrorMessage(12, lex->return_linenum(), name_temp);
				}
				else if (set.in_Tab(name_temp) && set.reInfo().obj == info::constsy)
					err.ErrorMessage(12, lex->return_linenum(), name_temp);

				Code.genAssign(Quaternary::assign, name_temp, operande);
				if (lex->return_sy() == lexical::semicolon) {
					label_temp1 = Code.GenLabelName(label++);
					label_temp2 = Code.GenLabelName(label++);
					label_temp3 = Code.GenLabelName(label++);
					Code.genJumpUncon(Quaternary::go, label_temp2);		//开始直接跳至语句内执行
					Code.genLabel(Quaternary::label, label_temp1);		//生成判断条件的label
					lex->getsy();
					con_label = condition();							//不满足条件就跳至末尾
					Code.genJumpUncon(Quaternary::go, label_temp2);		//跳至执行语句
					if (lex->return_sy() == lexical::semicolon) {
						lex->getsy();
						identity();
						name_temp = name;
						if (lex->return_sy() == lexical::assign) {
							lex->getsy();
							identity();
							name_temp2 = name;
							if (lex->return_sy() == lexical::pluscon) {
								lex->getsy();
								stepSize();
								Code.genLabel(Quaternary::label, label_temp3);
								Code.genCalculate(Quaternary::add, name_temp, name_temp2, to_string(num));		//加步长
								Code.genJumpUncon(Quaternary::go, label_temp1);									//跳至判断条件
							}
							else if (lex->return_sy() == lexical::minuscon) {
								lex->getsy();
								stepSize();
								Code.genLabel(Quaternary::label, label_temp3);
								Code.genCalculate(Quaternary::sub, name_temp, name_temp2, to_string(num));		//减步长
								Code.genJumpUncon(Quaternary::go, label_temp1);									//跳至判断条件
							}
							else {
								err.ErrorMessage(6, lex->return_linenum(), "+ or -");
								while (lex->return_sy() != lexical::minuscon && lex->return_sy() != lexical::pluscon && lex->return_sy() != lexical::rparent)
									lex->getsy();
								if (lex->return_sy() == lexical::pluscon) {
									lex->getsy();
									stepSize();
									Code.genLabel(Quaternary::label, label_temp3);
									Code.genCalculate(Quaternary::add, name_temp, name_temp2, to_string(num));		//加步长
									Code.genJumpUncon(Quaternary::go, label_temp1);									//跳至判断条件
								}
								else if (lex->return_sy() == lexical::minuscon) {
									lex->getsy();
									stepSize();
									Code.genLabel(Quaternary::label, label_temp3);
									Code.genCalculate(Quaternary::sub, name_temp, name_temp2, to_string(num));		//减步长
									Code.genJumpUncon(Quaternary::go, label_temp1);									//跳至判断条件
								}
							}
							if (lex->return_sy() == lexical::rparent) {
								lex->getsy();
								Code.genLabel(Quaternary::label, label_temp2);					//语句的label
								statement();													//执行语句
								Code.genJumpUncon(Quaternary::go, label_temp3);					//跳至步长加减
								Code.genLabel(Quaternary::label, con_label);					//end的label
							}
						}
						else {
							err.ErrorMessage(6, lex->return_linenum(), "=");
							JumpToChar('}');
						}
					}
					else {
						err.ErrorMessage(6, lex->return_linenum(), ";");
						JumpToChar('}');
					}
				}
				else {
					err.ErrorMessage(6, lex->return_linenum(), ";");
					JumpToChar('}');
				}
			}
			else{
				err.ErrorMessage(6, lex->return_linenum(), "=");
				JumpToChar('}');
			}
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "(", "for");
			JumpToChar('}');
		}
	}
//	cout << "this is cycleStatement!" << endl;
}

void Parser::stepSize() {			//步长
	if (unsignInt()) {
//		cout << "this is step size!" << endl;
	}
}
void Parser::reFuncCall() {			//有返回值函数调用语句
	string temp_name = lex->return_name();		//函数名
	string T;
	if (!set.in_FTab(temp_name) && temp_name != set.curFTab.ReName()) {
		err.ErrorMessage(18, lex->return_linenum(), temp_name);
	}

	if (lex->return_sy() == lexical::lparent) {
		lex->getsy();
		valueParTable(temp_name);
		Code.genFuncCall(Quaternary::funccall, temp_name);
		T = Code.GenTempName(temp_count++);
		info tmp;
		if (set.in_FTab(temp_name)) {			//临时变量类型和函数类型保持一致
			if (set.reFTable().ReFuncType() == FTable::intsy)
				tmp.typ = info::ints;
			else
				tmp.typ = info::chars;
		}
		else if (temp_name == set.curFTab.ReName()) {
			if (set.curFTab.ReFuncType() == FTable::intsy)
				tmp.typ = info::ints;
			else
				tmp.typ = info::chars;
		}
		else {
			err.ErrorMessage(1, lex->return_linenum(), temp_name);
		}
		set.curFTab.insert_ft(T, tmp);
		Code.genAssignByFunc(Quaternary::assignbyfunc, temp_name, T);
		operande = T;
		if (lex->return_sy() == lexical::rparent) {
			lex->getsy();
		}
		else {
			err.ErrorMessage(6, lex->return_linenum(), ")");
			JumpToChar(')');
		}
	}
	else {		//无参数表
		Code.genFuncCall(Quaternary::funccall, temp_name);
		T = Code.GenTempName(temp_count++);
		info tmp;
		if (set.in_FTab(temp_name)) {			//临时变量类型和函数类型保持一致
			if (set.reFTable().ReFuncType() == FTable::intsy)
				tmp.typ = info::ints;
			else
				tmp.typ = info::chars;
		}
		else if(temp_name == set.curFTab.ReName()){
			if (set.curFTab.ReFuncType() == FTable::intsy)
				tmp.typ = info::ints;
			else
				tmp.typ = info::chars;
		}
		else {
			err.ErrorMessage(1, lex->return_linenum(), temp_name);
		}
		set.curFTab.insert_ft(T, tmp);
		Code.genAssignByFunc(Quaternary::assignbyfunc, temp_name, T);
		operande = T;
	}
//	cout << "this is reFuncCall!" << endl;
}
void Parser::noFuncCall() {			//无返回值函数调用语句
	string temp_name = lex->return_name();		//函数名
	if (!set.in_FTab(temp_name) && temp_name != set.curFTab.ReName()) {			//非法函数名称
		err.ErrorMessage(18, lex->return_linenum(), temp_name);
	}
	else if (lex->return_sy() == lexical::lparent) {
		lex->getsy();
		valueParTable(temp_name); 
		Code.genFuncCall(Quaternary::funccall, temp_name);
		if (lex->return_sy() == lexical::rparent) {
			lex->getsy();
		}
		else {
			err.ErrorMessage(6, lex->return_linenum(), ")");
			JumpToChar(')');
		}
	}
	else {
		Code.genFuncCall(Quaternary::funccall, temp_name);
	}
//	cout << "this is noFuncCall!" << endl;
}
void Parser::valueParTable(string funcname) {		//值参数表
	int num = 0;
	do {
		if (lex->return_sy() == lexical::comma)
			lex->getsy();
		expr();
//		reset_temp();
		Code.genPush(Quaternary::push, funcname, operande);
		comparePara(funcname, num, operande);
		num++;
	} while (lex->return_sy() == lexical::comma);
//	cout << "this is valueParTable" << endl;
}
void Parser::statementColumn() {		//语句列
	while ( lex->return_sy() == lexical::ifsy ||
			lex->return_sy() == lexical::whilesy ||
			lex->return_sy() == lexical::forsy ||
			lex->return_sy() == lexical::lbrace ||
			lex->return_sy() == lexical::ident ||
			lex->return_sy() == lexical::scanfsy ||
			lex->return_sy() == lexical::printfsy ||
			lex->return_sy() == lexical::semicolon ||
			lex->return_sy() == lexical::returnsy) {
		statement();
	}
//	cout << "this is statementColumn!" << endl;
}
void Parser::write() {				//写语句 printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞ ‘)’| printf ‘(’＜表达式＞‘)’
	string temp;
	if (lex->return_sy() == lexical::printfsy) {
		lex->getsy();
		if (lex->return_sy() == lexical::lparent) {
			lex->getsy();
			if (lex->return_sy() == lexical::stringcon) {
				temp = lex->return_strvalue();
				Code.genPrintString(Quaternary::writestring, temp);
				lex->getsy();
				if (lex->return_sy() == lexical::comma) {
					lex->getsy();
					expr();
//					reset_temp();
					if (isNum(operande))
						Code.genPrintValue(Quaternary::write, operande, WriteConst);
					else
						Code.genPrintValue(Quaternary::write, operande, "");
				}
			}
			else {
				expr();
//				reset_temp();
				if (isNum(operande))
					Code.genPrintValue(Quaternary::write, operande, WriteConst);
				else
					Code.genPrintValue(Quaternary::write, operande, "");
			}
			if (lex->return_sy() == lexical::rparent) {
//				cout << "this is write!" << endl;
				lex->getsy();
			}
			else {
				err.ErrorMessage(6, lex->return_linenum(), ")");
				JumpToChar(')');
			}
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "(", "printf");
			return;
		}
	}
}
void Parser::read() {				//读语句 scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
	string temp_name;
	if (lex->return_sy() == lexical::scanfsy) {
		lex->getsy();
		if (lex->return_sy() == lexical::lparent) {
			lex->getsy();
			do {
				if (lex->return_sy() == lexical::comma) {
					lex->getsy();
				}
				identity();
				Code.genScanfSta(Quaternary::read, name);
			} while (lex->return_sy() == lexical::comma);
			if (lex->return_sy() == lexical::rparent) {
				lex->getsy();
//				cout << "this is read!" << endl;
			}
			else {
				err.ErrorMessage(6, lex->return_linenum(), ")");
				JumpToChar(')');
			}
		}
		else {
			err.ErrorMessage(7, lex->return_linenum(), "(", "scanf");
			return;
		}
	}
}
void Parser::Return(){				//返回语句
	if (lex->return_sy() == lexical::returnsy) {
		lex->getsy();
		if (lex->return_sy() == lexical::lparent) {
			if (set.curFTab.ReFuncType() == FTable::voidsy || set.curFTab.ReFuncType() == FTable::mainsy) {
				err.ErrorMessage(14, lex->return_linenum());
			}
			lex->getsy();
			expr();
			
			if (isNum(operande)) {		//判断返回变量类型与函数类型是否匹配
				if (WriteConst == "char" && set.curFTab.ReFuncType() != FTable::charsy)
					err.ErrorMessage(16, lex->return_linenum(), set.curFTab.ReName());
				else if (WriteConst == "int" && set.curFTab.ReFuncType() != FTable::intsy)
					err.ErrorMessage(17, lex->return_linenum(), set.curFTab.ReName());
			}
			else if (set.curFTab.in_ft(operande) || set.curFTab.in_para(operande)) {
				if (set.curFTab.ret_ifo().typ == info::ints && set.curFTab.ReFuncType() != FTable::intsy)
					err.ErrorMessage(17, lex->return_linenum(), set.curFTab.ReName());
				else if (set.curFTab.ret_ifo().typ == info::chars && set.curFTab.ReFuncType() != FTable::charsy)
					err.ErrorMessage(16, lex->return_linenum(), set.curFTab.ReName());
			}
			else if (set.in_Tab(operande)) {
				if (set.reInfo().typ == info::ints && set.curFTab.ReFuncType() != FTable::intsy)
					err.ErrorMessage(17, lex->return_linenum(), set.curFTab.ReName());
				else if (set.reInfo().typ == info::chars && set.curFTab.ReFuncType() != FTable::charsy)
					err.ErrorMessage(16, lex->return_linenum(), set.curFTab.ReName());
			}

//			reset_temp();
			Code.genReturnSta(Quaternary::ret, operande);
			if (lex->return_sy() != lexical::rparent) {
				err.ErrorMessage(6, lex->return_linenum(), ")");
				err_flag = 1;
				while (lex->return_sy() != lexical::rparent && lex->return_sy() != lexical::semicolon) {
					lex->getsy();
				}
				if (lex->return_sy() == lexical::rparent)
					lex->getsy();
			}
			else {
				lex->getsy();
			}
		}
		else {		//void函数的返回
			if (set.curFTab.ReFuncType() != FTable::voidsy && set.curFTab.ReFuncType() != FTable::mainsy) {
				err.ErrorMessage(13, lex->return_linenum());
			}
			Code.genJumpUncon(Quaternary::go, "label_" + set.curFTab.ReName());
		}
//		cout << "this is Return!" << endl;
	}
}