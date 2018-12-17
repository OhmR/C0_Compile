#include "pch.h"
#include "SymbolSet.h"
#include <algorithm>

SymbolSet::SymbolSet(){
}

SymbolSet::~SymbolSet()
{
}

int SymbolSet::in_Tab(string name) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	if (Tab.getinfo(name)) {
		ifo = Tab.ret_info();
		return 1;
	}
	return 0;
}

int SymbolSet::in_ATab(string name) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	if (ATab.getinfo(name)) {
		ifo = ATab.ret_info();
		return 1;
	}
	return 0;
}

int SymbolSet::in_FTab(string name) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	for (auto i = FTab.begin(); i != FTab.end(); ++i) {
		if (i->ReName() == name) {
			reFTab = *i;
			return 1;
		}
	}
	return 0;
}

void SymbolSet::PrintSymbolSet() {
	cout << endl;
	cout << "this is SymbolSet const" << endl;
	Tab.PrintTable();
	cout << "this is SymbolSet Array" << endl;
	ATab.PrintTable();
	cout << "this is SymbolSet Function" << endl;
	for (auto i = FTab.begin(); i != FTab.end(); i++) {
		i->PrintTable();
	}
}
/////////////////////////Table/////////////////////////

Table::Table() {
	tablesize = 0;
	adr = 0;
}

int Table::insert_map(string name, info inf) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	for (auto it = m.begin(); it != m.end(); ++it) {
		if (it->first == name) {
			return 0;
		}
	}
	inf.adr = this->adr;
//	cout << "##########################" << adr << endl;
	m.insert({ name, inf });
	adr += inf.size;
	tablesize += inf.size;
	return 1;
}

int Table::getinfo(string name) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	for (auto it = m.begin(); it != m.end(); ++it) {
		if (it->first == name) {
			ifo = it->second;
			return 1;
		}
	}
	return 0;
}

void Table::PrintTable() {
	for (auto i = m.begin(); i != m.end(); i++) {
		if (i->second.typ == info::chars) {
			cout << "char "<< i->first + ":" << i->second.ch << ",size is " << i->second.size << ", adr is: " << i->second.adr << "; ";
		}
		else
			cout << "int " << i->first + ":" + to_string(i->second.value) << ",size is " << i->second.size << ", adr is: " << i->second.adr << "; ";
	}
	cout << endl;
}
////////////////////////Fuction Table///////////////////////////

FTable::FTable(string name, funcType ftyp) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	this->name = name;
	this->functy = ftyp;
}

int FTable::insert_para(string name, info inf) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	return para.insert_map(name, inf);
}

int FTable::insert_arr(string name, info inf) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	return arr.insert_map(name, inf);
}

int FTable::insert_ft(string name, info inf) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	return ft.insert_map(name, inf);
}

int FTable::in_para(string name) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	if (para.getinfo(name)) {
		ifo = para.ret_info();
		return 1;
	}
	return 0;
}

int FTable::in_arr(string name) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	if (arr.getinfo(name)) {
		ifo = arr.ret_info();
		return 1;
	}
	return 0;
}

int FTable::in_ft(string name) {
	transform(name.begin(), name.end(), name.begin(), ::towlower);
	if (ft.getinfo(name)) {
		ifo = ft.ret_info();
		return 1;
	}
	return 0;
}

void FTable::PrintTable() {
	cout << "Function name is : " + this->name;
	cout << ", Fucntion Type is : " << this->functy << endl;
	cout << "Function para is :";
	para.PrintTable();
	cout << "Function arr is :";
	arr.PrintTable();
	cout << "Function ft is : ";
	ft.PrintTable();
	cout << endl;
}