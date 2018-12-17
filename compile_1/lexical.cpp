#include "pch.h"
#include "lexical.h"

lexical::lexical(std::string filename)
{
	fin = std::ifstream(filename);
	name = "";
	linenum = 1;
	wordnum = 0;
	next_char();
	reserved_max = 12;
}

lexical::~lexical()
{
	fin.close();
}

int lexical::HandleSpaceAndComment() {				//����ո��в���ȡ��һ����
	while (!fin.eof() && (ch == ' ' || ch == '\t' || ch == '\n')) {
		if (ch == '\n') {
			linenum++;						//λ�ñ�Ƿ�����
			wordnum = -1;
		}
		next_char();
	}
	if (fin.eof()) {
		return -1;
	}
	return 0;
}

void lexical::getname() {			//chΪname��ĵ�һ�ַ�
	clearname();
	if (isLetter()) {
		while (isLetter() || isDigital()) {
			if (ch <= 'Z' && ch >= 'A')
				ch = ch - 'A' + 'a';
			name = name + ch;
			next_char();
		}
	}
}

void lexical::getnum() {				//chΪ���ֺ�ĵ�һ���ַ�
	num = 0;
	while (isDigital()) {
		num = num * 10 + (ch - '0');
		next_char();
	}
	sy = intcon;
}

void lexical::clearname() {
	name = "";
}

void lexical::next_char() {
	ch = fin.get();
	wordnum++;
}

int lexical::getsy() {
	int i;

	if (HandleSpaceAndComment() == -1)
		return -1;

	if (isLetter()) {
		getname();
		string temp = name;
		transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
		for (i = 0; i < reserved_max; i++) {
			if (temp == reserved[i]) {
				sy = syk[i];
				break;
			}
		}
		if (i == reserved_max) {
			sy = ident;					//�û�����ı�ʶ��
		}
	}
	else if (isDigital()) {
		getnum();
	}
	else if (isEql()) {					//chΪ=��==���һ���ַ�
		next_char();
		if (isEql()) {
			sy = eql;
			next_char();
		}
		else
			sy = assign;
	}
	else if (isGtr()) {					//����or���ڵ���
		next_char();
		if (isEql()) {
			sy = geq;
			next_char();
		}
		else
			sy = gtr;
	}
	else if (isLss()) {					//С��orС�ڵ���
		next_char();
		if (isEql()) {
			sy = leq;
			next_char();
		}
		else
			sy = lss;
	}
	else if (isExc()) {					//������
		next_char();
		if (isEql()) {
			sy = neq;
			next_char();
		}
		else{
			char temp[1];
			temp[0] = ch;
			string str(temp, 1);
			error.ErrorMessage(2, linenum, str);
		}
	}
	else if (isPlus()) {				//��
		sy = pluscon;
		next_char();
	}
	else if (isMinus()) {				//��
		sy = minuscon;
		next_char();
	}
	else if (isTimes()) {				//��
		sy = timescon;
		next_char();
	}
	else if (isLpar()) {				//(
		sy = lparent;
		next_char();
	}
	else if (isRpar()) {				//)
		sy = rparent;
		next_char();
	}
	else if (isLbrack()) {				//[
		sy = lbrack;
		next_char();
	}
	else if (isRbrack()) {				//]
		sy = rbrack;
		next_char();
	}
	else if (isLbrace()) {				//{
		sy = lbrace;
		next_char();
	}
	else if (isRbrace()) {				//}
		sy = rbrace;
		next_char();
	}
	else if (isComma()) {				//,
		sy = comma;
		next_char();
	}
	else if (isSemi()) {				//;
		sy = semicolon;
		next_char();
	}
	else if (isDivi()) {				//��
		sy = divcon;
		next_char();
	}
	else if (ch == '"') {				//˫�����ַ���
		strvalue = "";
		next_char();
		while (ch != '"') {
			if (!isIllegal())
				strvalue = strvalue + ch;
			else {
				char temp[1];
				temp[0] = ch;
				string str(temp, 1);
				error.ErrorMessage(1, linenum, str);
			}
			next_char();
		}
		sy = stringcon;
		next_char();
	}
	else if (ch == '\'') {				//�������ַ�
		next_char();
		if (ch == '\'') {
			charvalue = 0;
			sy = charcon;
		}
		else {
			charvalue = ch;
			sy = charcon;
			next_char();
		}

		if (ch != '\'') {
			char temp[1];
			temp[0] = charvalue;
			string str(temp, 1);
			error.ErrorMessage(3, linenum, str);
			while (!fin.eof() && ch != '\'' && ch != '\n')
				next_char();
		}
		if (ch == '\n') {
			linenum++;
			wordnum = 0;
		}
		next_char();					//chΪ��c�����һ���ַ�
	}
	else {
		char temp[1];
		temp[0] = ch;
		string str(temp, 1);
		error.ErrorMessage(0, linenum, str);						//Υ���ַ�
		next_char();
	}
//	printmessage();
	return 0;
}

void lexical::printmessage() {			//���ݹ����ӡ��Ϣ
	//while (getsy() == 0)
	//{
		switch (sy)
		{
		case stringcon: {
			cout << symbol_string[sy] << ' ' << str << endl;
			break;
		}
		case intcon: {
			cout << symbol_string[sy] << ' ' << num << endl;
			break;
		}
		case charcon: {
			cout << symbol_string[sy] << ' ' << charvalue << endl;
			break;
		}
		case pluscon: {
			cout << symbol_string[sy] << ' ' << '+' << endl;
			break;
		}
		case minuscon: {
			cout << symbol_string[sy] << ' ' << '-' << endl;
			break;
		}
		case timescon: {
			cout << symbol_string[sy] << ' ' << '*' << endl;
			break;
		}
		case divcon: {
			cout << symbol_string[sy] << ' ' << '/' << endl;
			break;
		}
		case lparent: {
			cout << symbol_string[sy] << ' ' << '(' << endl;
			break;
		}
		case rparent: {
			cout << symbol_string[sy] << ' ' << ')' << endl;
			break;
		}
		case lbrace: {
			cout << symbol_string[sy] << ' ' << '{' << endl;
			break;
		}
		case rbrace: {
			cout << symbol_string[sy] << ' ' << '}' << endl;
			break;
		}
		case lbrack: {
			cout << symbol_string[sy] << ' ' << '[' << endl;
			break;
		}
		case rbrack: {
			cout << symbol_string[sy] << ' ' << ']' << endl;
			break;
		}
		case comma: {
			cout << symbol_string[sy] << ' ' << ',' << endl;
			break;
		}
		case semicolon: {
			cout << symbol_string[sy] << ' ' << ';' << endl;
			break;
		}
		case neq: {
			cout << symbol_string[sy] << ' ' << "!=" << endl;
			break;
		}
		case eql: {
			cout << symbol_string[sy] << ' ' << "==" << endl;
			break;
		}
		case geq: {
			cout << symbol_string[sy] << ' ' << ">=" << endl;
			break;
		}
		case leq: {
			cout << symbol_string[sy] << ' ' << "<=" << endl;
			break;
		}
		case gtr: {
			cout << symbol_string[sy] << ' ' << '>' << endl;
			break;
		}
		case lss: {
			cout << symbol_string[sy] << ' ' << '<' << endl;
			break;
		}
		case assign: {
			cout << symbol_string[sy] << ' ' << '=' << endl;
			break;
		}
		case ident: {
			cout << symbol_string[sy] << ' ' << name << endl;
			break;
		}
		default:
			if (sy <= 11)
				cout << symbol_string[sy] << ' ' << name << endl;
		}
//	}
}

int lexical::isDigital() {
	return (ch <= '9' && ch >= '0');
}
int lexical::isLetter() {
	return (ch <= 'z' && ch >= 'a' || ch <= 'Z' && ch >= 'A' || ch == '_');
}
int lexical::isEql() {
	return (ch == '=');
}
int lexical::isGtr() {
	return (ch == '>');
}
int lexical::isLss() {
	return (ch == '<');
}
int lexical::isExc() {
	return (ch == '!');
}
int lexical::isPlus() {
	return (ch == '+');
}
int lexical::isMinus() {
	return (ch == '-');
}
int lexical::isTimes() {
	return (ch == '*');
}
int lexical::isDivi() {
	return (ch == '/');
}
int lexical::isLpar() {
	return (ch == '(');
}
int lexical::isRpar() {
	return (ch == ')');
}
int lexical::isLbrack() {
	return (ch == '[');
}
int lexical::isRbrack() {
	return (ch == ']');
}
int lexical::isLbrace() {
	return (ch == '{');
}
int lexical::isRbrace() {
	return (ch == '}');
}
int lexical::isComma() {
	return (ch == ',');
}
int lexical::isSemi() {
	return (ch == ';');
}
int lexical::isIllegal() {
	return !((ch <= 126 && ch >= 35) || ch == 32 || ch == 33);
}

int lexical::return_sy() {
	return sy;
}
int lexical::return_charvalue() {
	return charvalue;
}
int lexical::return_num(){
	return num;
}
string lexical::return_name() {
	return name;
}
int lexical::return_linenum() {
	return linenum;
}