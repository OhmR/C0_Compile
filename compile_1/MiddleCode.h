#ifndef _MIDDLECODE_H_
#define _MIDDLECODE_H_

#include "SymbolSet.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;

//extern int label = 0;		//生成label的指示符，每次生成一个label，该值加1

class Quaternary {
public:
	enum opcode {
		funccall, voidfunc, intfunc, charfunc, mainfunc, funcend,
		assign, arrassign, assigntoarr, assignbyfunc,			//arrassign =[], assigntoarr []=
		add, sub, div, mult, neg, 
		beq, bne, bls, ble, bgt, bge, label, 
		push, para, ret, go,
		read, write, writestring, constdef, vardef
	};
	//ret对应函数返回，voidfunc, intfunc, charfunc, mainfunc对应函数定义，call对应函数调用
	//bnz满足条件跳转，bz不满足条件跳转，go直接跳转
	Quaternary() { ; }
	Quaternary(opcode op, string str1, string str2, string str3) {
		this->op = op; 
		src1 = str1; 
		src2 = str2; 
		des = str3; 
	}
	int isAssign(opcode temp_op) {			//若为赋值类型opcode返回1，构建dag图时使用
		if (/*temp_op == assign ||*/ temp_op == arrassign || temp_op == assigntoarr/* || temp_op == assignbyfunc*/ || temp_op == add || temp_op == sub || temp_op == div || temp_op == mult || temp_op == neg)
			return 1;
		return 0;
	}
	opcode op;
	string src1, src2, des;
};


class MiddleCode
{
public:
	MiddleCode();
	~MiddleCode();

	string stropcode[50]{
		"funccall", "voidfunc", "intfunc", "charfunc", "mainfunc", "funcend",
		"assign", "arrassign", "assigntoarr", "assignbyfunc",			//arrassign =[], assigntoarr []=
		"add", "sub", "div", "mult", "neg",
		"beq", "bne", "bls", "ble", "bgt", "bge", "label",
		"push", "para", "ret", "go",
		"read", "write", "writestring", "constdef", "vardef"
	};

	vector<Quaternary> getcode() { return code; }
	void insert_code(Quaternary q) { code.push_back(q); }
	void insert_code(Quaternary::opcode op, string str1, string str2, string str3){
		Quaternary q(op, str1, str2, str3);
		code.push_back(q);
	}
	string GenTempName(int i);
	string GenLabelName(int label);
	string LastLabelName(int label);

	void genConstDef(Quaternary::opcode op, string name);
	void genVarDef(Quaternary::opcode op, string name, string type);	//type标记是否为数组
	void genMainFunc(Quaternary::opcode op);
	void genAssign(Quaternary::opcode op, string des, string value);
	void genAssignArr(Quaternary::opcode op, string des, string name, string addr);
	void genAssignToArr(Quaternary::opcode op, string name, string addr, string value);
	void genAssignByFunc(Quaternary::opcode op, string funcname, string des);				//将函数返回值赋给des
	void genCalculate(Quaternary::opcode op, string des, string src1, string src2);
	void genPush(Quaternary::opcode op, string funcname, string name);
	void genPara(Quaternary::opcode op, string name);
	void genFuncCall(Quaternary::opcode op, string name);
	void genFuncDef(Quaternary::opcode op, string name);
	void genReturnSta(Quaternary::opcode op, string des);
	void genScanfSta(Quaternary::opcode op, string des);
	void genPrintString(Quaternary::opcode op, string str);
	void genPrintValue(Quaternary::opcode op, string name, string type);
	void genJumpCondition(Quaternary::opcode op, string op1, string op2, string label);
	void genJumpUncon(Quaternary::opcode op, string label);
	void genLabel(Quaternary::opcode op, string name);
	void genNagtive(Quaternary::opcode op, string src1, string des);
	void genFuncEnd(Quaternary::opcode op);

	void PrintMiddleCode();
private:
	vector<Quaternary> code;
	info inf;
};

#endif