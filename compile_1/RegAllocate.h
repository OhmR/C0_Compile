#ifndef _REGALLOCATE_H_
#define _REGALLOCTAE_H_

#include "OptimizationCode.h"
#include <algorithm>

class BlockFlow {		//������ģ��
public:
	BlockFlow() { ; }
	Block B;
	vector<string> in;		//in����
	vector<string> out;		//out����
	vector<string> use;		//use����
	vector<string> def;		//def����
	vector<int> after;		//��̿��
};

class ConflictNode {
public:
	ConflictNode() { reg = ""; }
	string name;
	vector<int> conflictwith;
	string reg;
};

class RegAllocate			//��ÿ����������ȫ�ּĴ�������
{
public:
	RegAllocate(string filename);
	~RegAllocate();
	vector<string> vectors_union(vector<string> v1, vector<string> v2);
	vector<string> vectors_different(vector<string> v1, vector<string> v2);
	vector<string> vectors_intersection(vector<string> v1, vector<string> v2);

private:
	SymbolSet set;
	vector<Quaternary> middlecode;
	int searchBlock(string label);			//���ݻ�����ĩ����תlabelѰ�Һ�̿��
	BlockFlow cur_block;					//��ǰ����Ļ�����
	string cur_func;
	vector<int> block_subscript;			//�������±�
	vector<BlockFlow> all_block;			//���еĻ�����
	vector<ConflictNode> Conflict;			//��ͻͼ

	//���ͼ����Ծ��������
	void ResetCurBlock() { cur_block = BlockFlow::BlockFlow(); }
	void gen_block_subscript();				//�����м���뻮�ֻ�����
	void gen_block_flow();					//���ݻ����黮�ֹ�����ͼ(ͬʱ��use/def���������)
	void gen_in_out();
	BlockFlow calculate_in(BlockFlow B);
	BlockFlow calculate_out(BlockFlow B);
	int inConst(string name);
	vector<string> use(Quaternary quater);			//������Ӧvector
	vector<string> def(Quaternary quater);			//������Ӧvector
	//���Ƴ�ͻͼ
	void gen_conflict();
	int search_in_conflict(string name);			//�ڳ�ͻͼ�����Ѱ����Ϊname�Ľ���±�ֵ
	void global_reg_allocate();
	void Node_reg_allocate(int index);				//����һ�����ļĴ���
	int isUnallocate(int index);					//�����±�Ϊindex�Ľ���Ƿ������Ĵ���
	int conflictnum(int index);						//������Ϊname�Ľ���ͻͼ�����������
};

#endif