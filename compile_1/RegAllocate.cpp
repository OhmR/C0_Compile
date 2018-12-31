#include "pch.h"
#include "RegAllocate.h"


RegAllocate::RegAllocate(string filename)
{
	OptimizationCode optcode(filename);
	middlecode = optcode.ReMiddleCode();
	cur_func = "";
	gen_block_subscript();
	gen_block_flow();
	gen_in_out();
	gen_conflict();
	global_reg_allocate();
//	all_block = optcode.ReAllBlock();
}


RegAllocate::~RegAllocate()
{
}

vector<string> RegAllocate::vectors_different(vector<string> v1, vector<string> v2) {	//�󽻼�
	vector<string> v;
	sort(v1.begin(), v1.end());
	sort(v2.begin(), v2.end());
	set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v));
	return v;
}

vector<string> RegAllocate::vectors_intersection(vector<string> v1, vector<string> v2) {	//�󽻼�
	vector<string> v;
	sort(v1.begin(), v1.end());
	sort(v2.begin(), v2.end());
	set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v));
	return v;
}

vector<string> RegAllocate::vectors_union(vector<string> v1, vector<string> v2) {			//�󲢼�
	vector<string> v;
	sort(v1.begin(), v1.end());
	sort(v2.begin(), v2.end());
	set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v));
	return v;
}

int RegAllocate::inConst(string name) {
	if (cur_func == "")
		return 1;
	if (set.in_FTab(cur_func)) {
		FTable F_t = set.reFTable();
		if (F_t.in_ft(name) || F_t.in_para(name)) {
			return 0;
		}
		else if (set.in_Tab(name)) {	//Ϊȫ�ֳ�����
			return 1;
		}
		else
			return 0;
	}
	else {
		cout << "Error : Can't find func " + name << endl;
		return 0;
	}
}

vector<string> RegAllocate::use(Quaternary quater) {
	Quaternary::opcode op_t = quater.op;
	vector<string> ReVec;
	//src1,src2 : add,sub,mult,div,beq,bne,bls,ble,bgt,bge
	if (op_t == Quaternary::add || op_t == Quaternary::sub || op_t == Quaternary::mult || op_t == Quaternary::div ||
		op_t == Quaternary::beq || op_t == Quaternary::bne || op_t == Quaternary::bls || op_t == Quaternary::ble || op_t == Quaternary::bgt || op_t == Quaternary::bge) {
		if (inConst(quater.src1)) {
			ReVec.push_back(quater.src1);
		}
		if (inConst(quater.src2)) {
			ReVec.push_back(quater.src2);
		}
	}
	//src1 : neg,assign,assigntoarr,push,ret,read,write
	if (op_t == Quaternary::neg|| op_t == Quaternary::assign || op_t == Quaternary::assigntoarr || op_t == Quaternary::push || op_t == Quaternary::ret || op_t == Quaternary::read || op_t == Quaternary::write) {
		if (inConst(quater.src1)) {
			ReVec.push_back(quater.src1);
		}
	}
	return ReVec;
}

vector<string> RegAllocate::def(Quaternary quater) {
	Quaternary::opcode op_t = quater.op;
	vector<string> ReVec;
	//des : add, sub, mult, div, assign, arrassign, assignbyfunc, read
	if (op_t == Quaternary::add || op_t == Quaternary::sub || op_t == Quaternary::mult || op_t == Quaternary::div || op_t == Quaternary::neg ||
		op_t == Quaternary::assign || op_t == Quaternary::assignbyfunc|| op_t == Quaternary::arrassign || op_t == Quaternary::read) {
		if (inConst(quater.des)) {
			ReVec.push_back(quater.des);
		}
	}
	return ReVec;
}

void RegAllocate::gen_block_subscript() {
	for (int i = 0; i < middlecode.size(); i++) {
		if (i == 0) {												//��һ�����Ϊ������
			block_subscript.push_back(i);
		}
		else if (middlecode[i].op == Quaternary::label) {			//����ת�������Ϊ������
			if (*(block_subscript.end() - 1) != i)
				block_subscript.push_back(i);
		}															//��ת����ĵ�һ��ָ��Ҳ����������
		else if (middlecode[i].op == Quaternary::go || (middlecode[i].op >= Quaternary::beq && middlecode[i].op <= Quaternary::bge)) {
			if (*(block_subscript.end() - 1) != i)
				block_subscript.push_back(i + 1);
		}															//��������������ͬ��label���
		else if (middlecode[i].op == Quaternary::intfunc || middlecode[i].op == Quaternary::charfunc || middlecode[i].op == Quaternary::voidfunc || middlecode[i].op == Quaternary::mainfunc) {
			if (*(block_subscript.end() - 1) != i)
				block_subscript.push_back(i);
		}
	}
}

void RegAllocate::gen_block_flow() {
	int flag = 0;
	for (int i = 0; i < block_subscript.size(); i++) {
		ResetCurBlock();		//��ʼ����ǰ��ͼ��
		Quaternary temp = middlecode[block_subscript[i]];
		//�����ȫ�ֳ���������
		if (flag == 0 && temp.op != Quaternary::intfunc && temp.op != Quaternary::charfunc && temp.op != Quaternary::voidfunc && temp.op != Quaternary::mainfunc) {
			cur_block.B = Block(middlecode, block_subscript[i], block_subscript[i + 1]);
			all_block.push_back(cur_block);
		}
		else {
			flag = 1;
		}

		//���뺯������
		if (flag == 1) {
			int j;
			for (j = block_subscript[i]; ; j++) {
				if (j + 1 >= block_subscript.size()) {		//���ֿ��ܵ��ս�������block_subscript[i]Ϊһ��ֽ��߻����һ���ֽ���
					if (j > middlecode.size())
						break;
				}
				else if (j == block_subscript[i + 1]) {
					break;
				}
				//��ǰ�������
				if (middlecode[j].op == Quaternary::intfunc || middlecode[j].op == Quaternary::charfunc || middlecode[j].op == Quaternary::voidfunc) {
					cur_func = middlecode[j].des;
				}
				else if (middlecode[i].op == Quaternary::mainfunc) {
					cur_func = "main";
				}

				vector<string> tp_use = use(middlecode[j]);
				vector<string> tp_def = def(middlecode[j]);
				if (tp_use.size() != 0 && tp_def.size() != 0) {
					if (vectors_intersection(tp_use, tp_def).size() == 0) {
						cur_block.use = vectors_union(tp_use, cur_block.use);
						cur_block.def = vectors_union(tp_def, cur_block.def);
					}
					else {
						cur_block.use = vectors_union(tp_use, cur_block.use);
					}
				}
				else if (tp_use.size() != 0) {
					cur_block.use = vectors_union(tp_use, cur_block.use);
				}
				else if (tp_def.size() != 0) {
					cur_block.def = vectors_union(tp_def, cur_block.def);
				}
			}
			if (j == middlecode.size()) {		//�������һ�������飬�޺�̿�
				cur_block.B = Block(middlecode, block_subscript[i], middlecode.size());
			}
			else if (j == block_subscript[i]) {	//�������������飬������ת�����������̿飬��������תgo�����һ����̿飬��funcend�����һ����̿�
				cur_block.B = Block(middlecode, block_subscript[i], block_subscript[i + 1]);
				Quaternary temp_end = cur_block.B.block_code.back();
				//��ĩβΪ��ת��䣬�򲹳���ͼ
				if (temp_end.op == Quaternary::beq || temp_end.op == Quaternary::ble || temp_end.op == Quaternary::bge || temp_end.op == Quaternary::bls || temp_end.op == Quaternary::bgt || temp_end.op == Quaternary::bne) {
					cur_block.after.push_back(searchBlock(temp_end.des));
					cur_block.after.push_back(j + 1);
				}
				else if (temp_end.op == Quaternary::go) {
					cur_block.after.push_back(searchBlock(temp_end.des));
				}
				else if (temp_end.op != Quaternary::funcend) {
					cur_block.after.push_back(j + 1);;
				}
			}
			all_block.push_back(cur_block);
		}
	}
}

void RegAllocate::gen_in_out() {
	vector<string> in_before;
	vector<string> out_before;
	int flag = 1;
	while (flag == 1) {
		flag = 0;
		for (int i = all_block.size() - 1; i >= 0; i--) {
			in_before = all_block[i].in;
			out_before = all_block[i].out;
			all_block[i] = calculate_out(all_block[i]);
			all_block[i] = calculate_in(all_block[i]);
			if (vectors_intersection(in_before, all_block[i].in).size() != 0 ||			//��in��out���������κ�һ�������仯������Ҫ�ٴμ���
				vectors_intersection(out_before, all_block[i].out).size() != 0) {
				flag = 1;
			}
		}
	}
}

BlockFlow RegAllocate::calculate_in(BlockFlow B) {
	vector<string> in_temp = vectors_different(B.out, B.def);
	B.in = vectors_union(B.use, in_temp);
	return B;
}

BlockFlow RegAllocate::calculate_out(BlockFlow B) {
	B.out.clear();
	if (B.after.size() == 1) {
		B.out = all_block[B.after[0]].in;
	}
	else if (B.after.size() == 2) {
		B.out = vectors_union(all_block[B.after[0]].in, all_block[B.after[1]].in);
	}
	return B;
}

int RegAllocate::searchBlock(string label) {
	for (int i = 0; i < all_block.size(); i++) {
		Quaternary temp = all_block[i].B.block_code[0];
		if (temp.op == Quaternary::label && temp.des == label) {			//����Ϊѭ������ת���ж�ָ�����ɵ�label
			return i;
		}
	}
	return -1;	//δ�ҵ�
}
//////////////////////////////conflict////////////////////////////////
int RegAllocate::search_in_conflict(string name) {
	for (int i = 0; i < Conflict.size(); i++) {
		if (Conflict[i].name == name)
			return i;
	}
	return -1;	//δ�ҵ�
}

void RegAllocate::gen_conflict() {
	vector<string> all_name;
	for (int i = 0; i < all_block.size(); i++) {
		all_name = vectors_union(all_name, all_block[i].in);
	}
	for (int i = 0; i < all_name.size(); i++) {		//�������еĽ�㣬ֻ�����ƣ�δ�г�ͻ��ϵ
		ConflictNode temp;
		temp.name = all_name[i];
		Conflict.push_back(temp);
	}
	for (int i = 0; i < all_block.size(); i++) {	//���ɽ��ĳ�ͻ��ϵ
		for (int j = 0; j < all_block[i].in.size(); j++) {
			BlockFlow temp = all_block[i];
			int NodeIndex = search_in_conflict(temp.in[j]);
			for (int k = 0; k < temp.in.size(); k++) {							//����ͬһin�µ����н��
				int ConflictIndex = search_in_conflict(temp.in[k]);
				if(ConflictIndex != NodeIndex) {
					Conflict[NodeIndex].conflictwith.push_back(ConflictIndex);	//����ͻ����±�������
				}
			}
		}
	}

	for (int i = 0; i < Conflict.size(); i++) {			//�����ظ��ĳ�ͻ���
		sort(Conflict[i].conflictwith.begin(), Conflict[i].conflictwith.end());
		auto iter = unique(Conflict[i].conflictwith.begin(), Conflict[i].conflictwith.end());
		if (iter != Conflict[i].conflictwith.end()) {
			Conflict[i].conflictwith.erase(iter, Conflict[i].conflictwith.end());
		}
	}
}

int RegAllocate::isUnallocate(int index) {
	return (Conflict[index].reg == "");
}

int RegAllocate::conflictnum(int index) {
	ConflictNode conflict_temp = Conflict[index];
	int sum = 0;
	for (int i = 0; i < conflict_temp.conflictwith.size(); i++) {
		if (isUnallocate(conflict_temp.conflictwith[i])) { sum++; }
	}
	return sum;
}

void RegAllocate::Node_reg_allocate(int index) {
	ConflictNode conflict_temp = Conflict[index];
	int j;
	for (int i = 0; i < 8; i++) {
		for (j = 0; j < conflict_temp.conflictwith.size(); j++) {
			if (conflict_temp.reg == "$s" + to_string(i))
				break;
		}
		if (j == conflict_temp.conflictwith.size())
			Conflict[index].reg = "$s" + to_string(i);
	}
}

void RegAllocate::global_reg_allocate() {
	vector<int> queue;
	while (1) {
		int flag = 0;
		int i;
		for (i = 0; i < Conflict.size(); i++) {
			if (conflictnum(i) < 8) {
				queue.push_back(i);
				Conflict[i].reg = "InQueue";
				flag = 1;
			}
		}

		if (flag == 0) {
			for (i = 0; i < Conflict.size(); i++) {							//�����н�㶼�޷����ӣ������ֿ���
				if (conflictnum(i) >= 8 && Conflict[i].reg != "Remove") {	//1����Ѱһ�����ɽ��ӽ����ΪRemove
					Conflict[i].reg = "Remove";
					break;
				}
			}
			if (i == Conflict.size()) {										//2�����н��Ҫô�Ѿ�Remove��Ҫô�Ѿ�InQueue������ѭ��
				break;
			}
		}
	}

	while (!queue.empty()) {												//�������еĵ㵹�����Ĵ���
		int index = queue.back();
		queue.pop_back();
		Node_reg_allocate(index);
	}

	for (int i = 0; i < Conflict.size(); i++) {								//���Ĵ���������ͬ����symbolset��
		if (Conflict[i].reg != "Remove") {
			set.SetTabReg(Conflict[i].name, Conflict[i].reg);
		}
	}
}