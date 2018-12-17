#ifndef _SYMBOLSET_H_
#define _SYMBOLSET_H_

#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>
using namespace std;

class info {
public:
	info() {
		value = 0;
		ch = 0;
//		level = 0;
		typ = notype;
		obj = noobject;
		adr = 0;
		size = 4;
	}
	void reset(){
		value = 0;
		ch = 0;
//		level = 0;
		typ = notype;
		obj = noobject;
		adr = 0;
		size = 4;
	}
	enum type {ints, chars, notype};
	enum object{constsy, varsy, arrays, noobject};
	int value;
	char ch;
//	int level;
	type typ;
	object obj;
	int adr;		//翻译中间代码时需补充adr
	int size;
};

class Table {
public:
	Table();
	int insert_map(string name, info inf);
	int getinfo(string name);		//若为1，将info存入ifo,否则未找到
	info ret_info() { return ifo; }
	void PrintTable();
	int ret_TableSize() { return tablesize; }
private:
	info ifo;
	int adr;
	int tablesize;
	unordered_map<string, info> m;
};

class FTable {
public:
	enum funcType{intsy, charsy, voidsy, mainsy};
	FTable(string name, funcType ftyp);
	FTable() {}
	int insert_para(string name, info inf);
	int insert_arr(string name, info inf);
	int insert_ft(string name, info inf);
	int in_para(string name);
	int in_arr(string name);
	int in_ft(string name);
	void func_name(string name) { this->name = name; }
	info ret_ifo() { return ifo; }
	string ReName() { return name; }
	funcType ReFuncType() { return functy; }
	void PrintTable();
	int ret_FTableSize() { return (para.ret_TableSize() + arr.ret_TableSize() + ft.ret_TableSize()); }
	Table ret_para() { return para; }
	Table ret_ft() { return ft; }
	Table ret_arr() { return arr; }

private:
	info ifo;		//返回当前所找寻的信息
	funcType functy;
	string name;	//函数名
	Table para;		//参数
	Table arr;		//局部数组
	Table ft;		//局部常变量
};

class SymbolSet
{
public:
	SymbolSet();
	~SymbolSet();
	int insert_Tab(string name, info inf) { return Tab.insert_map(name, inf); }
	int insert_ATab(string name, info inf) { return ATab.insert_map(name, inf); }
	void insert_FTab() { FTab.push_back(curFTab); }

	Table getTab() { return Tab; }
	Table getATab() { return ATab; }
	int in_Tab(string name);
	int in_ATab(string name);
	int in_FTab(string name);
	info reInfo() { return ifo; }
	FTable reFTable() { return reFTab; }
	int isInFunc() { return (curFTab.ReName().length() != 0); }
	void PrintSymbolSet();
	FTable curFTab;			//当前处理的函数,在对每个函数处理完毕后，在结尾时调用ins_Ftab插入FTab

private:
	FTable reFTab;			//用以返回索引后的的函数内容
	info ifo;
	Table Tab;				//全局常变量
	Table ATab;				//全局数组
	vector<FTable> FTab;	//函数（包括main函数）
};

#endif