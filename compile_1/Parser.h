#ifndef _PARSER_H_
#define _PARSER_H_

#include "Error.h"
#include "lexical.h"
#include "SymbolSet.h"
#include "MiddleCode.h"

using namespace std;

class Parser
{
public:
	Parser(string name);
	~Parser();
	void program();				//����
	SymbolSet return_symbolset() { return set; }
	vector<Quaternary> return_middleCode(){ return Code.getcode(); }
	int err_flag;

private:
	string operande;			//�������ʽ���Ĵ����纯���洢��������
	int flag_AddSub;			//�洢�������ڼ�1���2
	int flag_MultDiv;			//�洢�������ڳ�1���2
	int temp_count = 0;
	int label = 0;				//����ѭ����תʹ�ã�������ʹ�������ǣ�
	string op1, op2;
	int relatesign;

	MiddleCode Code;
	info ifo;
	SymbolSet set;
	string name;				//��¼����������������
	string filename;
	lexical *lex;
	Error err;
	string WriteConst;
	int sign;
	int num;					//�洢������ֵ
	char charvalue;				//�洢char��ֵ
	void reset_temp() { temp_count = 0; }
	void JumpToChar(char ch);

	int addOperator();			//�ӷ������
	int multOperator();			//�˷������
	int relateOperator();		//��ϵ�����
//	void isNum();				//����
	void isChar();				//�ַ�
	void constDec();			//����˵��
	void constDef();			//��������
	int unsignInt();			//�޷�������
	void Int();					//����
	int identity();				//��ʶ��
	void statementHead();		//����ͷ��
	void varDec();				//����˵��
	void varDef();				//��������
	int Type();					//���ͱ�ʶ��
	void reFuncDef();			//�з��غ�������
	void noFuncDef();			//�޷��غ�������
	void compoundStatement();	//�������
	void parametersTable();		//������
	void mainFunc();			//������
	void expr();				//���ʽ
	void term();				//��
	void factor();				//����
	void statement();			//���
	void assignStatement();		//��ֵ���
	void ifStatement();			//�������
	string condition();			//����
	void cycleStatement();		//ѭ�����
	void stepSize();			//����
	void reFuncCall();			//�з���ֵ�����������
	void noFuncCall();			//�޷���ֵ�����������
	void valueParTable(string funcname);		//ֵ������
	void statementColumn();		//�����
	void write();				//д���
	void read();				//�����
	void Return();				//�������

	int isNum(string name);		//�ж�name�Ƿ�Ϊ����
	int transNum(string name);	//��stringת��Ϊint
};

#endif