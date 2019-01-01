#include "pch.h"
#include "Parser.h"
#include <iostream>

Parser::Parser(string name)
{
	filename = name;		//������Ƿ�ʡ��filename
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
		typ1 = info::notype;	//Ϊ�ҵ��ñ������Ƿ�
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

int Parser::addOperator(){			//�ӷ������
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
int Parser::multOperator() {		//�˷������
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
int Parser::relateOperator() {		//��ϵ�����
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
void Parser::isChar() {				//�ַ�
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
void Parser::program() {			//����[������˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}����������
	lex->getsy();
	if (lex->return_sy() == lexical::constsy) {
		constDec();
	}
	if (lex->return_sy() == lexical::charsy || lex->return_sy() == lexical::intsy) {
		varDec();
	}
	if (lex->return_sy() == lexical::lparent || lex->return_sy() == lexical::lbrace) {
		if (ifo.typ == info::ints) {								//��ȡ�������ƣ����뺯������׶�
			set.curFTab = FTable::FTable(name, FTable::intsy);
			Code.genFuncDef(Quaternary::intfunc, name);
		}
		else {
			set.curFTab = FTable::FTable(name, FTable::charsy);
			Code.genFuncDef(Quaternary::charfunc, name);
		}
	}
	if (lex->return_sy() == lexical::lparent) {	//'('֮ǰ������var���Ǳ������Ǻ�����������Ŵ���������
		lex->getsy();
		parametersTable();		//������
		if (lex->return_sy() == lexical::rparent) {	//')'
			lex->getsy();
		}
		else{
			err.ErrorMessage(6, lex->return_linenum(), ")");
			JumpToChar(')');
		}
		if (lex->return_sy() == lexical::lbrace) {	//'{'
			lex->getsy();
			compoundStatement();		//�������
			if (lex->return_sy() == lexical::rbrace) {	//'}'
				set.insert_FTab();		//��־��һ�������������
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
		compoundStatement();		//�������
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
void Parser::constDec() {			//����˵��
	if (lex->return_sy() != lexical::constsy) {
		return;
	}
	while (lex->return_sy() == lexical::constsy) {
		lex->getsy();
		constDef();
		if (lex->return_sy() != lex->semicolon) {		//�жϷֺŽ�β
			err.ErrorMessage(5, lex->return_linenum(), "semicolon", lex->symbol_string[lex->return_sy()]);
			JumpToChar(';');
		}
		else
			lex->getsy();
	}
//	cout << "this is constDec!" << endl;
}
void Parser::constDef() {			//��������:int����ʶ��������������{,����ʶ��������������} | char����ʶ���������ַ���{ ,����ʶ���������ַ��� }		����ʱsyΪ��������һ������
	ifo.reset();
	ifo.obj = info::constsy;		//�ַ�����Ϣ׼������ 
	if (lex->return_sy() == lexical::intsy) {
		do{
			lex->getsy();
			if (identity()) {	
				ifo.typ = info::ints;			//�Դ�Ϊifo��int�ͳ�����Ӧֵ
				if (lex->return_sy() == lexical::assign) {
					lex->getsy();
					Int();
					ifo.value = num;					//numΪint�ͳ�����ֵ
					ifo.size = 4;
					if (!set.isInFunc()) {				//����δ������ǰ�����������ƣ�����Ϊȫ�ֱ���
						if(set.in_Tab(name)) {
							err.ErrorMessage(19, lex->return_linenum(), name);
						}
						else if (set.insert_Tab(name, ifo))
							Code.genConstDef(Quaternary::constdef, name);		//## �м���볣������ ##
					}
					else {
						if(set.curFTab.in_ft(name) || set.curFTab.in_para(name) || set.curFTab.in_arr(name)){
							err.ErrorMessage(19, lex->return_linenum(), name);
						}
						else if (set.curFTab.insert_ft(name, ifo))
							Code.genConstDef(Quaternary::constdef, name);		//���ڵøģ��������峣����ʱ��ô��������
					}
				}
				else {
					err.ErrorMessage(5, lex->return_linenum(), "assign", lex->symbol_string[lex->return_sy()]);
					while (lex->return_sy()!= lexical::semicolon && lex->return_sy() != lexical::comma) {	//�ڶ��źͷֺ�ʱͣס
						lex->getsy();
					}
					err_flag = 1;
				}
			}
			else
			{
				err.ErrorMessage(5, lex->return_linenum(), "indent", lex->symbol_string[lex->return_sy()]);
				while (lex->return_sy() != lexical::semicolon && lex->return_sy() != lexical::comma) {	//�ڶ��źͷֺ�ʱͣס
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
				ifo.typ = info::chars;		//��ע��������Ϊchar
				if (lex->return_sy() == lexical::assign) {
					lex->getsy();
					isChar();	//�ַ�
					ifo.ch = charvalue;		//��ע����ֵΪcharvalue
					ifo.size = 4;			//��ע������СΪ4
					if (!set.isInFunc()) {	//����δ������ǰ�����������ƣ�����Ϊȫ�ֱ���
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
							Code.genConstDef(Quaternary::constdef, name);		//���ڵøģ��������峣����ʱ��ô��������
					}
				}
				else {
					err.ErrorMessage(5, lex->return_linenum(), "eql", lex->symbol_string[lex->return_sy()]);
					while (lex->return_sy() != lexical::semicolon && lex->return_sy() != lexical::comma) {	//�ڶ��źͷֺ�ʱͣס
						lex->getsy();
					}
					err_flag = 1;
				}
			}
			else {
				err.ErrorMessage(5, lex->return_linenum(), "indent", lex->symbol_string[lex->return_sy()]);
				while (lex->return_sy() != lexical::semicolon && lex->return_sy() != lexical::comma) {	//�ڶ��źͷֺ�ʱͣס
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
int Parser::unsignInt() {			//�޷�������
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
void Parser::Int() {				//����
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
int Parser::identity() {			//��ʶ��
	if (lex->return_sy() != lexical::ident) {
		err.ErrorMessage(5, lex->return_linenum(), "ident", lex->symbol_string[lex->return_sy()]);
		return 0;
	}
	else{
		name = lex->return_name();	//��ȡ��ʶ�����ƣ���������ű�
		lex->getsy();
//		cout << "this is identity!" << endl;
	}
	return 1;
}

void Parser::statementHead() {		//����ͷ��
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
void Parser::varDec() {				//����˵��
	ifo.reset();
	while (lex->return_sy() == lex->intsy || lex->return_sy() == lex->charsy)
	{
		if (lex->return_sy() == lex->intsy)		//���������Ϣ
			ifo.typ = info::ints;
		else
			ifo.typ = info::chars;
		varDef();
		if (lex->return_sy() == lex->semicolon) {
			lex->getsy();
		}
		else if (lex->return_sy() == lexical::lbrace || lex->return_sy() == lexical::lparent) {
			return;
		}		//ת������������
		else {
			err.ErrorMessage(7, lex->return_linenum(), "semicolon or lbrace or lparent", "ident");
			return;
		}
	}
//	cout << "this is varDec!" << endl;
}
void Parser::varDef() {				//��������
	while (lex->return_sy() == lex->intsy || lex->return_sy() == lex->charsy)
	{
		do{
			lex->getsy();
			if (identity()) {
				if (lex->return_sy() == lexical::lbrack) {
					ifo.obj = info::arrays;		//��¼Ϊ��������
					lex->getsy();
					if (unsignInt()) {
						ifo.size = num * 4;		//���鳤��Ϊnum������Ԫ�ش�СΪ4
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
				}		//����
				else if (lex->return_sy() == lexical::lparent || lex->return_sy() == lexical::lbrace)	//˵�����Ǳ����������з���ֵ�ĺ�������
					return;
				else {	//�������
					ifo.obj = info::varsy;		//������ı��������Ӧ�ı���
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
int Parser::Type() {				//���ͱ�ʶ��
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
void Parser::reFuncDef() {								//�з��غ�������
	statementHead();
	if (lex->return_sy() == lexical::lparent) {			//'('
//		set.insert_map(name, 0, 0, 0, info::refunc);
		lex->getsy();
		parametersTable();								//������
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
		compoundStatement();							//�������
		if (lex->return_sy() == lexical::rbrace) {	//'}'
			set.insert_FTab();						//��־��һ�������ķ��ű������
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
void Parser::noFuncDef() {			//�޷��غ�������
	if (lex->return_sy() == lexical::voidsy) {
		lex->getsy();
	}
	identity();
	set.curFTab = FTable::FTable(name, FTable::voidsy);
	Code.genFuncDef(Quaternary::voidfunc, name);
	if (lex->return_sy() == lexical::lparent) {	//'('
		lex->getsy();
		parametersTable();		//������
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
		compoundStatement();		//�������
		if (lex->return_sy() == lexical::rbrace) {	//'}'
			set.insert_FTab();						//��־�ŵ�ǰ�������ű������
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

void Parser::compoundStatement() {	//�������
	if(lex->return_sy() == lexical::constsy)
		constDec();
	if(lex->return_sy() == lexical::intsy || lex->return_sy() == lexical::charsy)
		varDec();
	statementColumn();
//	cout << "this is compoundStatement!" << endl;
}
void Parser::parametersTable() {		//�����������ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}
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
//	cout << "this is parametersTable" << endl;	//������ת���ֺŶ���ͣס��
}
void Parser::mainFunc() {			//������
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
					set.insert_FTab();			//���mian�����������
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
void Parser::expr() {				//���ʽ
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
		set.curFTab.insert_ft(T, tmp);		//��ʱ������������Ҫ��������,����׼ȷ��ӡ
		Code.genNagtive(Quaternary::neg, operande, T);
		operande = T;
	}
	else {
		term();
	}
	T1 = operande;
	while ((flag = addOperator()) && flag) {
		if(flag == 1){				//��
			term();
			T2 = operande;
			T3 = Code.GenTempName(temp_count++); 
			info tmp;
			tmp.typ = info::ints;
			set.curFTab.insert_ft(T3, tmp);		//��ʱ���������;�Ϊnotype
			Code.genCalculate(Quaternary::add, T3, T1, T2);
		}
		else if (flag == 2) {		//��
			term();
			T2 = operande;
			T3 = Code.GenTempName(temp_count++);
			info tmp;
			tmp.typ = info::ints;
			set.curFTab.insert_ft(T3, tmp);		//��ʱ���������;�Ϊnotype
			Code.genCalculate(Quaternary::sub, T3, T1, T2);
		}
		T1 = T3;
		operande = T3;
	}

//	cout << "this is expr!" << endl;
}
void Parser::term() {				//��
	string T1, T2, T3;
	int flag = 0;
	do {
		factor();
		T2 = operande;
		if (flag == 1) {			//��
			T3 = Code.GenTempName(temp_count++);
			info tmp;
			tmp.typ = info::ints;
			set.curFTab.insert_ft(T3, tmp);		//��ʱ���������;�Ϊnotype
			Code.genCalculate(Quaternary::mult, T3, T1, T2);
			T1 = T3;
		}
		else if (flag == 2) {		//��
			T3 = Code.GenTempName(temp_count++);
			info tmp;
			tmp.typ = info::ints;
			set.curFTab.insert_ft(T3, tmp);		//��ʱ���������;�Ϊnotype
			Code.genCalculate(Quaternary::div, T3, T1, T2);
			T1 = T3;				
		}
		else {
			T1 = operande;			//��һ�γ˳�������ĵ�һ��Ԫ��
			T3 = operande;			//��ֹ�޳˳�������ֱ�ӷ���
		}
	} while ((flag = multOperator()) && flag);
	operande = T3;
//	cout << "this is term!" << endl;
}
void Parser::factor() {				//����:=����ʶ����������ʶ������[�������ʽ����]��|��(�������ʽ����)������������|���ַ��������з���ֵ����������䣾
	string T;
	string temp_name;
	if (lex->return_sy() == lexical::ident) {				//����ʶ����������ʶ������[�������ʽ����]�������з���ֵ����������䣾
		temp_name = lex->return_name();
		lex->getsy();
		if (lex->return_sy() == lexical::lbrack) {			//����
			lex->getsy();
			expr();

			T = Code.GenTempName(temp_count++);
			info tmp;
			int arraysize;
			if (set.curFTab.in_arr(temp_name)) {
				tmp.typ = set.curFTab.ret_ifo().typ;		//��ʱ�������ͺ����鱣��һ��
				arraysize = set.curFTab.ret_ifo().size / 4;
			}
			else if (set.in_ATab(temp_name)) {
				tmp.typ = set.reInfo().typ;
				arraysize = set.reInfo().size / 4;
			}
			else {
				err.ErrorMessage(1, lex->return_linenum(), temp_name);
			}

			if (isNum(operande)) {		//����Խ�����
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

			set.curFTab.insert_ft(T, tmp);		//��ʱ���������;�Ϊnotype
			Code.genAssignArr(Quaternary::arrassign, T, temp_name, operande);		//��T�и�����Ԫ��ֵ
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
			reFuncCall();									//���з���ֵ����������䣾
		}
		else if (set.in_FTab(temp_name) || set.curFTab.ReName() == temp_name) {
			reFuncCall();
		}
		else {												//��ʶ��
			operande = temp_name;
			if (!set.curFTab.in_ft(temp_name) && !set.curFTab.in_para(temp_name) && !set.in_Tab(temp_name)) {
				err.ErrorMessage(1, lex->return_linenum(), temp_name); 
			}
//			cout << "this is factor" << endl;
			return;
		}
	}
	else if(lex->return_sy() == lexical::lparent){			//(���ʽ)
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
	else if (lex->return_sy() == lexical::charcon) {		//�ַ�
		isChar();
		operande = to_string(int(charvalue));			//���ַ�ת��Ϊassic��
		T = Code.GenTempName(temp_count++);
		WriteConst = "char";
/*		info tmp;
		tmp.typ = info::chars;
		set.curFTab.insert_ft(T, tmp);		//��ʱ���������;�Ϊnotype
		Code.genAssign(Quaternary::assign, T, operande);
		operande = T;*/
	}
	else if(lex->return_sy() == lexical::intcon || lex->return_sy() == lexical::pluscon || lex->return_sy() == lexical::minuscon){	//����
		Int();
		operande = to_string(num);
		T = Code.GenTempName(temp_count++);
		WriteConst = "int";
/*		info tmp;
		tmp.typ = info::ints;
		set.curFTab.insert_ft(T, tmp);		//��ʱ���������;�Ϊnotype
		Code.genAssign(Quaternary::assign, T, operande);
		operande = T;*/
	}
	else {
		err.ErrorMessage(9, lex->return_linenum());
		return;
	}
//	cout << "this is factor!" << endl;
}

void Parser::statement() {			//���
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
		string tmp = lex->return_name();	//֮����name�ж���ʲô���͵ĺ������á�
		lex->getsy();
		if (lex->return_sy() == lexical::assign || lex->return_sy() == lexical::lbrack) {
			assignStatement();
		}
		else if (lex->return_sy() == lexical::lparent) {
			if (set.in_FTab(tmp)){			//����ƥ��ĺ��������ڷ��ű��У�Ҳ�����ǵ�ǰ����ĺ���
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
		}	//���޷���ֵ��������(�в���)
		else {
			if (set.in_FTab(tmp)){			//����ƥ��ĺ��������ڷ��ű��У�Ҳ�����ǵ�ǰ����ĺ���
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
		}	//���޷���ֵ��������(�޲���)
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
	case lexical::semicolon: {	//�����
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

	if (lex->return_sy() == lexical::semicolon) {		//��ĩβ�ķֺ�
		lex->getsy();
//		cout << "this is statement!" << endl;
	}
	else if (flag == 0) {
		err.ErrorMessage(6, lex->return_linenum(), ";");
		JumpToChar(';');
	}
}
void Parser::assignStatement() {		//��ֵ��� ����ʶ�����������ʽ��|����ʶ������[�������ʽ����]��=�����ʽ��
	string tmp = lex->return_name();
	string T;
	if (lex->return_sy() == lexical::lbrack) {		//����
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

		if (isNum(operande)) {		//����Խ�����
			if (transNum(operande) >= arraysize)
				err.ErrorMessage(11, lex->return_linenum(), tmp);
		}
		else if (set.curFTab.in_ft(operande)) {
			if (set.curFTab.ret_ifo().obj == info::constsy) {
				if (set.curFTab.ret_ifo().value >= arraysize)
					err.ErrorMessage(11, lex->return_linenum(), tmp);
			}
		}
		else if (set.curFTab.in_arr(operande)) {	//��Ϊ������������
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
	else if(lex->return_sy() == lexical::assign){		//����
		if (!set.curFTab.in_ft(tmp) && !set.curFTab.in_para(tmp) && !set.in_Tab(tmp)) {
			err.ErrorMessage(1, lex->return_linenum(), tmp);
		}
		if (set.curFTab.in_ft(tmp)) {		//��������ֵ����
			if(set.curFTab.ret_ifo().obj == info::constsy)
				err.ErrorMessage(12, lex->return_linenum(), tmp);
		}
		else if (set.in_Tab(tmp) && set.reInfo().obj == info::constsy)
			err.ErrorMessage(12, lex->return_linenum(), tmp);

		lex->getsy();
		expr();
//		reset_temp();
		Code.genAssign(Quaternary::assign, tmp, operande);
	}	//ǰ��������indent�Ѿ��������
//	cout << "this is assignStatement" << endl;
}
void Parser::ifStatement() {			//������� if ��(������������)������䣾[else����䣾]
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
		if (lex->return_sy() == lexical::elsesy) {	//��ʡ��
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
string Parser::condition() {			//���� �����ʽ������ϵ������������ʽ���������ʽ��
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
		if (type1 == "char") {	//���� 'a' > 'c' һ������
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
void Parser::cycleStatement() {		//ѭ����� while ��(������������)������䣾 |for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
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

				if (set.curFTab.in_ft(name_temp)) {		//��������ֵ����
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
					Code.genJumpUncon(Quaternary::go, label_temp2);		//��ʼֱ�����������ִ��
					Code.genLabel(Quaternary::label, label_temp1);		//�����ж�������label
					lex->getsy();
					con_label = condition();							//����������������ĩβ
					Code.genJumpUncon(Quaternary::go, label_temp2);		//����ִ�����
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
								Code.genCalculate(Quaternary::add, name_temp, name_temp2, to_string(num));		//�Ӳ���
								Code.genJumpUncon(Quaternary::go, label_temp1);									//�����ж�����
							}
							else if (lex->return_sy() == lexical::minuscon) {
								lex->getsy();
								stepSize();
								Code.genLabel(Quaternary::label, label_temp3);
								Code.genCalculate(Quaternary::sub, name_temp, name_temp2, to_string(num));		//������
								Code.genJumpUncon(Quaternary::go, label_temp1);									//�����ж�����
							}
							else {
								err.ErrorMessage(6, lex->return_linenum(), "+ or -");
								while (lex->return_sy() != lexical::minuscon && lex->return_sy() != lexical::pluscon && lex->return_sy() != lexical::rparent)
									lex->getsy();
								if (lex->return_sy() == lexical::pluscon) {
									lex->getsy();
									stepSize();
									Code.genLabel(Quaternary::label, label_temp3);
									Code.genCalculate(Quaternary::add, name_temp, name_temp2, to_string(num));		//�Ӳ���
									Code.genJumpUncon(Quaternary::go, label_temp1);									//�����ж�����
								}
								else if (lex->return_sy() == lexical::minuscon) {
									lex->getsy();
									stepSize();
									Code.genLabel(Quaternary::label, label_temp3);
									Code.genCalculate(Quaternary::sub, name_temp, name_temp2, to_string(num));		//������
									Code.genJumpUncon(Quaternary::go, label_temp1);									//�����ж�����
								}
							}
							if (lex->return_sy() == lexical::rparent) {
								lex->getsy();
								Code.genLabel(Quaternary::label, label_temp2);					//����label
								statement();													//ִ�����
								Code.genJumpUncon(Quaternary::go, label_temp3);					//���������Ӽ�
								Code.genLabel(Quaternary::label, con_label);					//end��label
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

void Parser::stepSize() {			//����
	if (unsignInt()) {
//		cout << "this is step size!" << endl;
	}
}
void Parser::reFuncCall() {			//�з���ֵ�����������
	string temp_name = lex->return_name();		//������
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
		if (set.in_FTab(temp_name)) {			//��ʱ�������ͺͺ������ͱ���һ��
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
	else {		//�޲�����
		Code.genFuncCall(Quaternary::funccall, temp_name);
		T = Code.GenTempName(temp_count++);
		info tmp;
		if (set.in_FTab(temp_name)) {			//��ʱ�������ͺͺ������ͱ���һ��
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
void Parser::noFuncCall() {			//�޷���ֵ�����������
	string temp_name = lex->return_name();		//������
	if (!set.in_FTab(temp_name) && temp_name != set.curFTab.ReName()) {			//�Ƿ���������
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
void Parser::valueParTable(string funcname) {		//ֵ������
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
void Parser::statementColumn() {		//�����
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
void Parser::write() {				//д��� printf ��(�� ���ַ�����,�����ʽ�� ��)��| printf ��(�����ַ����� ��)��| printf ��(�������ʽ����)��
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
void Parser::read() {				//����� scanf ��(������ʶ����{,����ʶ����}��)��
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
void Parser::Return(){				//�������
	if (lex->return_sy() == lexical::returnsy) {
		lex->getsy();
		if (lex->return_sy() == lexical::lparent) {
			if (set.curFTab.ReFuncType() == FTable::voidsy || set.curFTab.ReFuncType() == FTable::mainsy) {
				err.ErrorMessage(14, lex->return_linenum());
			}
			lex->getsy();
			expr();
			
			if (isNum(operande)) {		//�жϷ��ر��������뺯�������Ƿ�ƥ��
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
		else {		//void�����ķ���
			if (set.curFTab.ReFuncType() != FTable::voidsy && set.curFTab.ReFuncType() != FTable::mainsy) {
				err.ErrorMessage(13, lex->return_linenum());
			}
			Code.genJumpUncon(Quaternary::go, "label_" + set.curFTab.ReName());
		}
//		cout << "this is Return!" << endl;
	}
}