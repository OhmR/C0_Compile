#ifndef _OPTIMIZATIONCODE_H_
#define _OPTIMIZATIONCODE_H_

#include "SymbolSet.h"
#include "MiddleCode.h"
#include <string>
#include "Parser.h"
#include <map>

using namespace std;

class Block {		//������
public:
	Block() {
		block_begin = 0;
		block_end = 0;
		block_size = 0;
	}
	Block(vector<Quaternary> code, int begin, int end) {
		block_begin = begin;
		block_end = end;
		block_size = end - begin;
		for (int i = begin; i < end; i++) {
			block_code.push_back(code[i]);
		}
	}
	int block_begin;
	int block_end;
	int block_size;
	vector<Quaternary> block_code;
	void PrintMiddleCode() {
		for (auto i = block_code.begin(); i != block_code.end(); i++) {
			cout << MiddleCode::MiddleCode().stropcode[i->op] << ", " << i->src1 << ", " << i->src2 << ", " << i->des << endl;
		}
	}
};

class NodeTable {	//�ڵ��
public:
	NodeTable() { ; }
	~NodeTable() { ; }
	int isTemp(string name) { return (name.size() >= 3 && name[0] == '_' && name[1] == 'T' && name[2] >= '0' && name[2] <= '9'); }
	int isNum(string name) {
		if (name[0] <= '9' && name[0] >= '0')
			return 1;
		else if (name[0] == '-' || name[0] == '+')
			return 1;
		return 0;
	}
	void InsertNode(string name, int index) {
		NodeName.push_back(name);
		NodeIndex.push_back(index);
	}
	int searchNode(string name) {
		for (int i = 0; i < NodeName.size(); i++) {
			if (name == NodeName[i])
				return NodeIndex[i];			//�ҵ����ƶ�Ӧ�Ľ�㣬���ؽ���
		}
		return -1;					//δ�ҵ�����-1
	}
	vector<string> searchNodeName(int index) {	//���ҵ��Ľ�����ư�˳�����vector��
		vector<string> name;
		int flag = 0;
		for (int i = 0; i < NodeIndex.size(); i++) {
			if (index == NodeIndex[i]) {
				if (flag == 0 || !isTemp(NodeName[i]) && !isNum(NodeName[i]))
					name.push_back(NodeName[i]);
				else if (flag == 1 && !isTemp(NodeName[i]))
					name.push_back(NodeName[i]);
				else if (flag == 2 && !isTemp(NodeName[i]) && !isNum(NodeName[i]))
					name.push_back(NodeName[i]);

				if (!isTemp(NodeName[i]) && flag != 2)
					flag = 1;
				if (!isTemp(NodeName[i]) && !isNum(NodeName[i]))
					flag = 2;
			}
		}
		if (flag == 1) {
			while (isTemp(name[0])) {
				name.erase(name.begin());
			}
		}
		else if (flag == 2) {
			while (isTemp(name[0]) || isNum(name[0])) {
				name.erase(name.begin());
			}
		}
		return name;
	}
	void changeNodeIndex(string name, int index) {		//�޸���ֵ
		for (int i = 0; i < NodeName.size(); i++) {
			if (name == NodeName[i]) {
				NodeIndex[i] = index;
				break;
			}
		}
	}
	int ReSize() { return NodeIndex.size(); }

private:
	vector<string> NodeName;
	vector<int> NodeIndex;
};

class DAG_NODE {
public:
	DAG_NODE() { 
		LChild = -1;
		RChild = -1;
	}
	DAG_NODE(string name) {
		this->name = name;
		LChild = -1;
		RChild = -1;
	}
	~DAG_NODE() { ; }
	int LChild;
	int RChild;
	vector<int> Father;
	string name;
	int index;
};

class DAG {			//DAGͼ
public:
	DAG() { ; }
	~DAG() { ; }
	int InsertNode(string name) {
		cur_node = DAG_NODE(name);
		cur_node.index = dagnode.size();
		dagnode.push_back(cur_node);
		return cur_node.index;		//����ͼ�в�������±�
	}
	int InsertOpNode(Quaternary::opcode op, int x_i, int y_j) {
		cur_node = DAG_NODE(MiddleCode::MiddleCode().stropcode[op]);
		cur_node.LChild = x_i;
		cur_node.RChild = y_j;
		cur_node.index = dagnode.size();
		dagnode.push_back(cur_node);
		if(x_i != -1)
			dagnode[x_i].Father.push_back(cur_node.index);
		if(y_j != -1)
			dagnode[y_j].Father.push_back(cur_node.index);
		return cur_node.index;		//����ͼ�в�������±�
	}
	int searchNode(string name) {
		for (int i = 0; i < dagnode.size(); i++) {
			if (name == dagnode[i].name)
				return i;				//�ҵ��󷵻��±�
		}
		return -1;						//δ�ҵ�����-1
	}
	int searchOp(Quaternary::opcode op, int x_i, int y_j) { 
		string opcode = MiddleCode::MiddleCode().stropcode[op];
		for (int i = 0; i < dagnode.size(); i++) {
			if (dagnode[i].name == opcode) {
				if (dagnode[i].LChild == x_i && dagnode[i].RChild == y_j) {
					return i;
				}
			}
		}
		return -1;
	}
	int ReSize() { return dagnode.size(); }
	DAG_NODE ReNode(int index) { return dagnode[index]; }

private:
	DAG_NODE cur_node;
	vector<DAG_NODE> dagnode;
};

class OptimizationCode
{
public:
	OptimizationCode() { ; }
	OptimizationCode(string filename);
	~OptimizationCode();
	void InsertCode(string op, string des, string src1, string src2) { code.push_back(op + des + src1 + src2); }
	void InsertCode(string op, string des, string src1) { code.push_back(op + des + src1); }
	void InsertCode(string op, string des) { code.push_back(op + des); }
	void InsertCode(string op) { code.push_back(op); }
	void ResetCurBlock() { cur_block = Block::Block(); }
	vector<Quaternary> ReMiddleCode() { return optimiz_middlecode; }
	vector<Block> ReAllBlock() { return all_block; }

	void gen_block_subscript();				//�����м���뻮�ֻ�����
	void gen_all_block();					//���ݻ��ֽ�����ɻ����飨����gen_block_subscript()��
	void Print_all_block();					//���δ�ӡ���û������м����
	void Print_MiddleCode();
	void BuildDag(Block &block);			//����һ���������Dagͼ�������ɶ�Ӧ�м����
	int isNum(string name);
	int isTemp(string name);
	int FatherinQue(vector<int> Father, vector<int> Que);
	int NodeinQue(int index, vector<int> Que);
	void GenCalculateDag(Quaternary temp);	//����ԪʽΪz = x op y������䣬ͨ���ú�������DAGͼ��������
	void GenAssignDag(Quaternary temp);
	void GenNegDag(Quaternary temp);
	void TransDagToCode(DAG DagPic);		//����dagͼ��Ϣ���ɶ�Ӧ�м���룬������cur_block��
	void TransNodeToCode(DAG_NODE node);
	void GenOptMiddlecode();
	int InAssign(string name);

private:
	SymbolSet set;
	vector<Quaternary> middlecode;
	vector<Quaternary> temp_code;			//��ʱ�м����
	vector<Quaternary> optimiz_middlecode;	//���������Ż�����м����
	vector<string> code;
	vector<int> block_subscript;			//�������±�
	vector<Block> all_block;				//���еĻ�����
	Block cur_block;						//��ǰ����Ļ�����

	DAG dag;								//dagͼ
	NodeTable nodetable;					//�ڵ��
	map<int, string> ChangeMess;			//��¼dagͼ��arr��ز���������������
	vector<string> Assign;					//��¼nodetable�У�����ֵ�Ľ����
};

#endif