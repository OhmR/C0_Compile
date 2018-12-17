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
	int adr;		//�����м����ʱ�貹��adr
	int size;
};

class Table {
public:
	Table();
	int insert_map(string name, info inf);
	int getinfo(string name);		//��Ϊ1����info����ifo,����δ�ҵ�
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
	info ifo;		//���ص�ǰ����Ѱ����Ϣ
	funcType functy;
	string name;	//������
	Table para;		//����
	Table arr;		//�ֲ�����
	Table ft;		//�ֲ�������
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
	FTable curFTab;			//��ǰ����ĺ���,�ڶ�ÿ������������Ϻ��ڽ�βʱ����ins_Ftab����FTab

private:
	FTable reFTab;			//���Է���������ĵĺ�������
	info ifo;
	Table Tab;				//ȫ�ֳ�����
	Table ATab;				//ȫ������
	vector<FTable> FTab;	//����������main������
};

#endif