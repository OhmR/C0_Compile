#include "pch.h"
#include "MiddleCode.h"


MiddleCode::MiddleCode(){
}

MiddleCode::~MiddleCode(){
}

string MiddleCode::GenTempName(int i) {
	return "_T" + to_string(i);
}

string MiddleCode::GenLabelName(int label) {
	return "label" + to_string(label);
}

string MiddleCode::LastLabelName(int label) {
	if (label == 0)
		return "";
	else
		return "label" + to_string(label-1);
}

void MiddleCode::PrintMiddleCode() {
	cout << "this is MiddleCode" << endl;
	for (auto i = code.begin(); i != code.end(); i++) {
		cout << stropcode[i->op] << ", " << i->src1 << ", " << i->src2 << ", " << i->des<<endl;
	}
}

void MiddleCode::genConstDef(Quaternary::opcode op, string name) {
	insert_code(op, "", "", name);
}

void MiddleCode::genVarDef(Quaternary::opcode op, string name, string type) {
	insert_code(op, type, "", name);
}

void MiddleCode::genMainFunc(Quaternary::opcode op) {
	insert_code(op, "", "", "");
}

void MiddleCode::genAssign(Quaternary::opcode op, string des, string value) {
	insert_code(op, value, "", des);
}

void MiddleCode::genAssignArr(Quaternary::opcode op, string des, string name, string addr) {
	insert_code(op, name, addr, des);
}

void MiddleCode::genAssignToArr(Quaternary::opcode op, string name, string addr, string value){
	insert_code(op, addr, value, name);
}

void MiddleCode::genAssignByFunc(Quaternary::opcode op, string funcname, string des) {
	insert_code(op, funcname, "", des);
}

void MiddleCode::genCalculate(Quaternary::opcode op, string des, string src1, string src2) {
	insert_code(op, src1, src2, des);
}

void MiddleCode::genPush(Quaternary::opcode op, string funcname, string name) {
	insert_code(op, funcname, "", name);
}

void MiddleCode::genFuncCall(Quaternary::opcode op, string name) {
	insert_code(op, "", "", name);
}

void MiddleCode::genFuncDef(Quaternary::opcode op, string name) {
	insert_code(op, "", "", name);
}

void MiddleCode::genPara(Quaternary::opcode op, string name) {
	insert_code(op, "", "", name);
}

void MiddleCode::genReturnSta(Quaternary::opcode op, string des) {
	insert_code(op, "", "", des);
}

void MiddleCode::genScanfSta(Quaternary::opcode op, string des) {
	insert_code(op, "", "", des);
}

void MiddleCode::genPrintString(Quaternary::opcode op, string str) {
	insert_code(op, "", "", str);
}
void MiddleCode::genPrintValue(Quaternary::opcode op, string name, string type) {
	insert_code(op, type, "", name);
}

void MiddleCode::genJumpCondition(Quaternary::opcode op, string op1, string op2, string label){
	insert_code(op, op1, op2, label);
}

void MiddleCode::genJumpUncon(Quaternary::opcode op, string label) {
	insert_code(op, "", "", label);
}

void MiddleCode::genLabel(Quaternary::opcode op, string name) {
	insert_code(op, "", "", name);
}

void MiddleCode::genNagtive(Quaternary::opcode op, string src1, string des) {
	insert_code(op, src1, "", des);
}

void MiddleCode::genFuncEnd(Quaternary::opcode op) {
	insert_code(op, "", "", "");
}
