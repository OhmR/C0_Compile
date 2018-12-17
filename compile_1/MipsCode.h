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
	void LoadInReg(string desReg, string src1);			//������src1��Ӧ����ֵ�浽desReg�Ĵ����£����߽���ֵsrc1�����Ĵ���desReg
	void StoreToRam(string desReg , string src1);		//���Ĵ������ݴ浽����src1��Ӧ���ڴ�ռ���
	void LoadReg();								//��ԭ�Ĵ����ֳ�
	void StoreReg();							//�����Ĵ����ֳ�

	vector<Quaternary> middlecode;
	SymbolSet set;
	vector<string> code;
	vector<string> stringvalue;
};

#endif