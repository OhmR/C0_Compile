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
	void LoadInReg(string src1);						//将变量src1对应的数值存到寄存器下
	void StoreToRam(string desReg , string src1);		//将寄存器数据存到变量src1对应的内存空间中
	void LoadReg();										//还原寄存器现场
	void StoreReg();									//保留寄存器现场
	void ResetRegT();
	void EndResetRegT();
	string searchReg(string name);						//查询名为name的常变量当前分配的寄存器
	string AllocateNextReg(string name);				//分配下一块寄存器
	string AllocateReg(string name);					//返回name对应的寄存器
	string InReg_T(string name);						//判断name是否在寄存器池中
	int is_S(string reg);								//判断寄存器名是否为S0~S7
	int ReRegIndex(string reg);							//返回寄存器下标
	void BeforeAssignS(string name);					//仅仅适用于assign，不适用于lw
	void BeforeUseS(string name);						//不为赋值时使用

	vector<Quaternary> middlecode;
	SymbolSet set;
	vector<string> code;
	vector<string> stringvalue;
	string reg_T[10];									//寄存器池
	string reg_S[8];
	int reg_flag;
};

#endif