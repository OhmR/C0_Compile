#ifndef _REGALLOCATE_H_
#define _REGALLOCTAE_H_

#include "OptimizationCode.h"
#include <algorithm>

class BlockFlow {		//数据流模块
public:
	BlockFlow() { ; }
	Block B;
	vector<string> in;		//in集合
	vector<string> out;		//out集合
	vector<string> use;		//use集合
	vector<string> def;		//def集合
	vector<int> after;		//后继块号
};

class ConflictNode {
public:
	ConflictNode() { reg = ""; }
	string name;
	vector<int> conflictwith;
	string reg;
};

class RegAllocate			//对每个函数进行全局寄存器分配
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
	int searchBlock(string label);			//依据基本块末的跳转label寻找后继块号
	BlockFlow cur_block;					//当前处理的基本块
	string cur_func;
	vector<int> block_subscript;			//基本块下标
	vector<BlockFlow> all_block;			//所有的基本块
	vector<ConflictNode> Conflict;			//冲突图

	//搭建流图及活跃变量分析
	void ResetCurBlock() { cur_block = BlockFlow::BlockFlow(); }
	void gen_block_subscript();				//依据中间代码划分基本块
	void gen_block_flow();					//依据基本块划分构造流图(同时将use/def均处理完成)
	void gen_in_out();
	BlockFlow calculate_in(BlockFlow B);
	BlockFlow calculate_out(BlockFlow B);
	int inConst(string name);
	vector<string> use(Quaternary quater);			//返回相应vector
	vector<string> def(Quaternary quater);			//返回相应vector
	//绘制冲突图
	void gen_conflict();
	int search_in_conflict(string name);			//在冲突图结点中寻找名为name的结点下标值
	void global_reg_allocate();
	void Node_reg_allocate(int index);				//分配一个结点的寄存器
	int isUnallocate(int index);					//返回下标为index的结点是否分配过寄存器
	int conflictnum(int index);						//返回名为name的结点冲突图中相连结点数
};

#endif