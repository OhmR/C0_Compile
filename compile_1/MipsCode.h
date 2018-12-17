#ifndef _MIPSCODE_H_
#define _MIPSCODE_H_

#include "Parser.h"
#include <sstream>

class MipsCode
{
public:
	MipsCode(string filename, vector<Quaternary> midcode);
	~MipsCode();
	void insertCode(string op, string des, string src1, string src2);
	void insertString(string des);
	void genMipsCode();
	void PrintCode();
	void PrintString();
	void PrintMiddleCodeText();

private:
	vector<string> para_name;
	int infunc;			//标记是处理全局常变量还是函数
	int pushadr;
	int stringnum;		//记录字符串个数，用以生成引用名
	int isNum(string str);
	void LoadInReg(string desReg, string src1);			//将变量src1对应的数值存到desReg寄存器下，或者将数值src1赋给寄存器desReg
	void StoreToRam(string desReg , string src1);		//将寄存器数据存到变量src1对应的内存空间中
	void LoadReg();								//还原寄存器现场
	void StoreReg();							//保留寄存器现场

	vector<Quaternary> middlecode;
	SymbolSet set;
	vector<string> code;
	vector<string> stringvalue;
};

#endif