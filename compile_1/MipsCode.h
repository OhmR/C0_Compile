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
	int infunc;			//����Ǵ���ȫ�ֳ��������Ǻ���
	int pushadr;
	int stringnum;		//��¼�ַ�����������������������
	int isNum(string str);
	void LoadInReg(string src1);						//������src1��Ӧ����ֵ�浽�Ĵ�����
	void StoreToRam(string desReg , string src1);		//���Ĵ������ݴ浽����src1��Ӧ���ڴ�ռ���
	void LoadReg();										//��ԭ�Ĵ����ֳ�
	void StoreReg();									//�����Ĵ����ֳ�
	void ResetRegT();
	void EndResetRegT();
	string searchReg(string name);						//��ѯ��Ϊname�ĳ�������ǰ����ļĴ���
	string AllocateNextReg(string name);				//������һ��Ĵ���
	string AllocateReg(string name);					//����name��Ӧ�ļĴ���
	string InReg_T(string name);						//�ж�name�Ƿ��ڼĴ�������
	int is_S(string reg);								//�жϼĴ������Ƿ�ΪS0~S7
	int ReRegIndex(string reg);							//���ؼĴ����±�
	void BeforeAssignS(string name);					//����������assign����������lw
	void BeforeUseS(string name);						//��Ϊ��ֵʱʹ��

	vector<Quaternary> middlecode;
	SymbolSet set;
	vector<string> code;
	vector<string> stringvalue;
	string reg_T[10];									//�Ĵ�����
	string reg_S[8];
	int reg_flag;
};

#endif