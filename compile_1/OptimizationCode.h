#ifndef _OPTIMIZATIONCODE_H_
#define _OPTIMIZATIONCODE_H_

#include "SymbolSet.h"
#include "MiddleCode.h"
#include <string>
#include "Parser.h"
#include <map>

using namespace std;

class Block {		//基本块
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

class NodeTable {	//节点表
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
				return NodeIndex[i];			//找到名称对应的结点，返回结点号
		}
		return -1;					//未找到返回-1
	}
	vector<string> searchNodeName(int index) {	//将找到的结点名称按顺序放入vector中
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
	void changeNodeIndex(string name, int index) {		//修改数值
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

class DAG {			//DAG图
public:
	DAG() { ; }
	~DAG() { ; }
	int InsertNode(string name) {
		cur_node = DAG_NODE(name);
		cur_node.index = dagnode.size();
		dagnode.push_back(cur_node);
		return cur_node.index;		//返回图中插入结点的下标
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
		return cur_node.index;		//返回图中插入结点的下标
	}
	int searchNode(string name) {
		for (int i = 0; i < dagnode.size(); i++) {
			if (name == dagnode[i].name)
				return i;				//找到后返回下标
		}
		return -1;						//未找到返回-1
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

	void gen_block_subscript();				//依据中间代码划分基本块
	void gen_all_block();					//依据划分结果生成基本块（依赖gen_block_subscript()）
	void Print_all_block();					//依次打印所用基本块中间代码
	void Print_MiddleCode();
	void BuildDag(Block &block);			//构建一个基本块的Dag图，并生成对应中间代码
	int isNum(string name);
	int isTemp(string name);
	int FatherinQue(vector<int> Father, vector<int> Que);
	int NodeinQue(int index, vector<int> Que);
	void GenCalculateDag(Quaternary temp);	//若四元式为z = x op y类型语句，通过该函数进行DAG图及结点表建立
	void GenAssignDag(Quaternary temp);
	void GenNegDag(Quaternary temp);
	void TransDagToCode(DAG DagPic);		//依据dag图信息生成对应中间代码，并插入cur_block后
	void TransNodeToCode(DAG_NODE node);
	void GenOptMiddlecode();
	int InAssign(string name);

private:
	SymbolSet set;
	vector<Quaternary> middlecode;
	vector<Quaternary> temp_code;			//临时中间代码
	vector<Quaternary> optimiz_middlecode;	//基本块内优化后的中间代码
	vector<string> code;
	vector<int> block_subscript;			//基本块下标
	vector<Block> all_block;				//所有的基本块
	Block cur_block;						//当前处理的基本块

	DAG dag;								//dag图
	NodeTable nodetable;					//节点表
	map<int, string> ChangeMess;			//记录dag图中arr相关操作结点的数组名称
	vector<string> Assign;					//记录nodetable中，被赋值的结点名
};

#endif