#include "pch.h"
#include "OptimizationCode.h"

OptimizationCode::OptimizationCode(string filename) {
	Parser par(filename);
	par.program();
	middlecode = par.return_middleCode();
	set = par.return_symbolset();
	set.curFTab.func_name("");

	if (par.err_flag == 0) {
		gen_block_subscript();
		gen_all_block();
		Print_all_block();
		cout << "##################" << endl;
		cout << "this is after dag" << endl;
		cout << "##################" << endl;
		for (int i = 0; i < all_block.size(); i++)
			BuildDag(all_block[i]);
		Print_all_block();
		GenOptMiddlecode();
	}
}

OptimizationCode::~OptimizationCode() {

}

void OptimizationCode::gen_block_subscript() {
	for (int i = 0; i < middlecode.size(); i++) {
		if (i == 0) {												//��һ�����Ϊ������
			block_subscript.push_back(i);
		}
		else if (middlecode[i].op == Quaternary::label) {			//����ת�������Ϊ������
			if(*(block_subscript.end()-1) != i)
				block_subscript.push_back(i);
		}															//��ת����ĵ�һ��ָ��Ҳ����������
		else if (middlecode[i].op == Quaternary::go || (middlecode[i].op >= Quaternary::beq && middlecode[i].op <= Quaternary::bge)) {
			if (*(block_subscript.end()-1) != i)
				block_subscript.push_back(i + 1);
		}															//�������á����ء��������Ҳ��ͬ����תָ��
		else if (middlecode[i].op == Quaternary::funccall || middlecode[i].op == Quaternary::funcend || middlecode[i].op == Quaternary::ret) {
			if (*(block_subscript.end()-1) != i)
				block_subscript.push_back(i + 1);
		}															//��������������ͬ��label���
		else if (middlecode[i].op == Quaternary::intfunc || middlecode[i].op == Quaternary::charfunc || middlecode[i].op == Quaternary::voidfunc || middlecode[i].op == Quaternary::mainfunc) {
			if (*(block_subscript.end()-1) != i)
				block_subscript.push_back(i);
		}
	}
//	for (int i = 0; i < block_subscript.size(); i++) {
//		cout << block_subscript[i] << endl;
//	}
}

void OptimizationCode::GenOptMiddlecode() {
	for (int i = 0; i < all_block.size(); i++) {
		for (int j = 0; j < all_block[i].block_code.size(); j++) {
			optimiz_middlecode.push_back(all_block[i].block_code[j]);
		}
	}
}

void OptimizationCode::gen_all_block() {
	int i;
	for (i = 0; i < block_subscript.size() - 1; i++) {
		cur_block = Block(middlecode, block_subscript[i], block_subscript[i + 1]);
		all_block.push_back(cur_block);
	}
	cur_block = Block(middlecode, block_subscript[i], middlecode.size());
	all_block.push_back(cur_block);
}

void OptimizationCode::Print_all_block() {
	for (int i = 0; i < all_block.size(); i++) {
		cout << "this is block" << i << endl;
		all_block[i].PrintMiddleCode();
		cout << "" << endl;
	}
}

void OptimizationCode::Print_MiddleCode() {
	for (auto i = optimiz_middlecode.begin(); i != optimiz_middlecode.end(); i++) {
		cout << MiddleCode::MiddleCode().stropcode[i->op] << ", " << i->src1 << ", " << i->src2 << ", " << i->des << endl;
	}
}

int OptimizationCode::InAssign(string name) {
	for (int i = 0; i < Assign.size(); i++) {
		if (Assign[i] == name)
			return 1;
	}
	return 0;
}

int OptimizationCode::isNum(string name) {
	if (name[0] <= '9' && name[0] >= '0')
		return 1;
	else if (name[0] == '-' || name[0] == '+')
		return 1;
	return 0;
}

int OptimizationCode::isTemp(string name) {
	return (name.size() >= 3 && name[0] == '@' && name[1] == 'T' && name[2] >= '0' && name[2] <= '9');
}

void OptimizationCode::GenCalculateDag(Quaternary temp) {
	int x_i;
	int y_j;
	int op_k;
	
	x_i = nodetable.searchNode(temp.src1);
	if (x_i >= 0) { ; }		//�ڽڵ����x�����ҵ���¼����i��������dagͼ�д������x���ڽڵ����������x��i��
	else {
		if (isNum(temp.src1))
			x_i = dag.InsertNode(temp.src1);
		else
			x_i = dag.InsertNode(temp.src1 + "0");
		nodetable.InsertNode(temp.src1, x_i);
	}
	
	y_j = nodetable.searchNode(temp.src2);
	if (y_j >= 0) { ; }		//�ڽڵ����y�����ҵ���¼����j��������dagͼ�д������y���ڽڵ����������y��j��
	else {
		if (isNum(temp.src2))
			y_j = dag.InsertNode(temp.src2);
		else
			y_j = dag.InsertNode(temp.src2 + "0");
		nodetable.InsertNode(temp.src2, y_j);
	}

	op_k = dag.searchOp(temp.op, x_i, y_j);
	if (op_k >= 0) { ; }		//��dagͼ���ҷ���������op�����ҵ���¼���ţ����򴴽����
	else {
		op_k = dag.InsertOpNode(temp.op, x_i, y_j);
	}

	if (nodetable.searchNode(temp.des) >= 0) {							//�����������z���޸Ľ��ţ������½����z, op_k��
		ChangeMess.insert(std::pair<int, string>(nodetable.searchNode(temp.des), temp.des));
		nodetable.changeNodeIndex(temp.des, op_k);
	}
	else {
		nodetable.InsertNode(temp.des, op_k);
	}
}

void OptimizationCode::GenAssignDag(Quaternary temp) {
	int x_i;
	int y_j;
	int op_k;
	if (temp.op == Quaternary::assigntoarr || temp.op == Quaternary::arrassign) {	//assigntoarr : des[src1] = src2, arrassign des = src1[src2]
		x_i = nodetable.searchNode(temp.src1);
		if (x_i >= 0) { ; }
		else {
			if (isNum(temp.src1))
				x_i = dag.InsertNode(temp.src1);
			else
				x_i = dag.InsertNode(temp.src1 + "0");
			nodetable.InsertNode(temp.src1, x_i);
		}
		
		y_j = nodetable.searchNode(temp.src2);
		if (y_j >= 0) { ; }				//�ڽڵ����y�����ҵ���¼����j��������dagͼ�д������y���ڽڵ����������y��j��
		else {
			if (isNum(temp.src2))
				y_j = dag.InsertNode(temp.src2);
			else
				y_j = dag.InsertNode(temp.src2 + "0");
			nodetable.InsertNode(temp.src2, y_j);
		}

		op_k = dag.searchOp(temp.op, x_i, y_j);
		if (op_k >= 0) { ; }			//��dagͼ���ҷ���������[]�����ҵ���¼���ţ����򴴽����
		else {
			op_k = dag.InsertOpNode(temp.op, x_i, y_j);
		}
		if (nodetable.searchNode(temp.des) >= 0) {									//�����������z���޸Ľ��ţ������½����des, op_k��
			ChangeMess.insert(std::pair<int, string>(nodetable.searchNode(temp.des), temp.des));
			nodetable.changeNodeIndex(temp.des, op_k);
		}
		else {
			nodetable.InsertNode(temp.des, op_k);
		}
		if (temp.op == Quaternary::assigntoarr) {
			ChangeMess.insert(std::pair<int, string>(op_k, temp.des));
		}
		else if (temp.op == Quaternary::arrassign) {
			ChangeMess.insert(std::pair<int, string>(op_k, temp.src1));
		}
	}
	else if(temp.op == Quaternary::assign){		//z = x	(��Ҫ����xΪ���������) assign
		x_i = nodetable.searchNode(temp.src1);
		if (x_i >= 0) { ; }
		else {
			if (isNum(temp.src1))
				x_i = dag.InsertNode(temp.src1);
			else
				x_i = dag.InsertNode(temp.src1 + "0");
			nodetable.InsertNode(temp.src1, x_i);
		}

		if (nodetable.searchNode(temp.des) >= 0) {							//�����������z���޸Ľ��ţ������½����z, op_k��
			ChangeMess.insert(std::pair<int, string>(nodetable.searchNode(temp.des), temp.des));
			nodetable.changeNodeIndex(temp.des, x_i);
		}
		else {
			nodetable.InsertNode(temp.des, x_i);
		}
	}
	else {		//assignbyfunc
		cur_block.block_code.push_back(temp);
		x_i = nodetable.searchNode(temp.des);		//��Ϊdes = des���з���
		if (x_i >= 0) { ; }
		else {
			if (isNum(temp.des))
				x_i = dag.InsertNode(temp.des);
			else
				x_i = dag.InsertNode(temp.des + "0");
			nodetable.InsertNode(temp.des, x_i);
		}

		if (nodetable.searchNode(temp.des) >= 0) {							//�����������z���޸Ľ��ţ������½����z, op_k��
			ChangeMess.insert(std::pair<int, string>(nodetable.searchNode(temp.des), temp.des));
			nodetable.changeNodeIndex(temp.des, x_i);
		}
		else {
			nodetable.InsertNode(temp.des, x_i);
		}
	}
}

void OptimizationCode::GenNegDag(Quaternary temp) {
	int x_i;
	int op_k;
	if (temp.op == Quaternary::neg) {	//assigntoarr : des = -src1
		x_i = nodetable.searchNode(temp.src1);
		if (x_i >= 0) { ; }
		else {
			if (isNum(temp.src1))
				x_i = dag.InsertNode(temp.src1);
			else
				x_i = dag.InsertNode(temp.src1 + "0");
			nodetable.InsertNode(temp.src1, x_i);
		}
		op_k = dag.searchOp(temp.op, x_i, -1);
		if (op_k >= 0) { ; }			//��dagͼ���ҷ���������[]�����ҵ���¼���ţ����򴴽����
		else {
			op_k = dag.InsertOpNode(temp.op, x_i, -1);
		}
		if (nodetable.searchNode(temp.des) >= 0) {									//�����������z���޸Ľ��ţ������½����des, op_k��
			ChangeMess.insert(std::pair<int, string>(nodetable.searchNode(temp.des), temp.des));
			nodetable.changeNodeIndex(temp.des, op_k);
		}
		else {
			nodetable.InsertNode(temp.des, op_k);
		}
	}
}

int OptimizationCode::FatherinQue(vector<int> Father, vector<int> Que) {		//�жϸ��ڵ��Ƿ�ȫ������
	if (Father.size() == 0)
		return 1;
	if (Que.size() == 0)
		return 0;

	int i,j;
	for (i = 0; i < Father.size(); i++) {
		for (j = 0; j < Que.size(); j++) {
			if (Father[i] == Que[j])
				break;
		}
		if (j == Que.size())		//��ʾ��������father[i]
			return 0;
	}
	return 1;
}

int OptimizationCode::NodeinQue(int index, vector<int> Que) {
	for (int i = 0; i < Que.size(); i++) {
		if (index == Que[i])
			return 1;
	}
	return 0;
}
/*
"arrassign", "assigntoarr"			//arrassign =[], assigntoarr []=
"add", "sub", "div", "mult", "neg",
"beq", "bne", "bls", "ble", "bgt", "bge"
*/
void OptimizationCode::TransNodeToCode(DAG_NODE node) {
	Quaternary quater;
	string src1, src2, des;
	int index = node.index;
	vector<string> desArr = nodetable.searchNodeName(index);
	int flag = 0;
	if (desArr.size() >0 && !isTemp(desArr[0])) {
		flag = 1;
	}
	if(desArr.size() > 0 && node.name != "assigntoarr")
		quater.des = desArr[0];
	if (node.name == "add" || node.name == "sub" || node.name == "mult" || node.name == "div" || node.name == "assigntoarr") {
		if (nodetable.searchNodeName(node.LChild).size() > 0)		//�������ڽ������޶�Ӧ�˵���������޸Ĺ������޸���Ϣ��������������
			quater.src1 = nodetable.searchNodeName(node.LChild)[0];
		else
			quater.src1 = ChangeMess[node.LChild];

		if (nodetable.searchNodeName(node.RChild).size() > 0)		//���ҽ���ڽ������޶�Ӧ�˵���������޸Ĺ������޸���Ϣ��������������
			quater.src2 = nodetable.searchNodeName(node.RChild)[0];
		else
			quater.src1 = ChangeMess[node.RChild];
	}
	else if (node.name == "arrassign") {
//		quater.src1 = ChangeMess[node.index];
//		quater.src2 = nodetable.searchNodeName(node.RChild)[0];
		if (nodetable.searchNodeName(node.LChild).size() > 0)		//�������ڽ������޶�Ӧ�˵���������޸Ĺ������޸���Ϣ��������������
			quater.src1 = nodetable.searchNodeName(node.LChild)[0];
		else
			quater.src1 = ChangeMess[node.LChild];

		if (nodetable.searchNodeName(node.RChild).size() > 0)		//���ҽ���ڽ������޶�Ӧ�˵���������޸Ĺ������޸���Ϣ��������������
			quater.src2 = nodetable.searchNodeName(node.RChild)[0];
		else
			quater.src2 = ChangeMess[node.RChild];
	}
	else if (node.name == "neg") {
		if (nodetable.searchNodeName(node.LChild).size() > 0)		//�������ڽ������޶�Ӧ�˵���������޸Ĺ������޸���Ϣ��������������
			quater.src1 = nodetable.searchNodeName(node.LChild)[0];
		else
			quater.src1 = ChangeMess[node.LChild];
	}
		
	if (node.name == "add") {
		quater.op = Quaternary::add;
		cur_block.block_code.push_back(quater);
		Assign.push_back(quater.des);
	}
	else if (node.name == "sub") {
		quater.op = Quaternary::sub;
		cur_block.block_code.push_back(quater);
		Assign.push_back(quater.des);
	}
	else if (node.name == "mult") {
		quater.op = Quaternary::mult;
		cur_block.block_code.push_back(quater);
		Assign.push_back(quater.des);
	}
	else if (node.name == "div") {
		quater.op = Quaternary::div;
		cur_block.block_code.push_back(quater);
		Assign.push_back(quater.des);
	}
	else if (node.name == "arrassign") {
		quater.op = Quaternary::arrassign;
		cur_block.block_code.push_back(quater);
		Assign.push_back(quater.des);
	}
	else if (node.name == "assigntoarr") {
		quater.op = Quaternary::assigntoarr;
		quater.des = ChangeMess[node.index];
		cur_block.block_code.push_back(quater);
	}
	else if (node.name == "neg") {
		quater.op = Quaternary::neg;
		cur_block.block_code.push_back(quater);
		Assign.push_back(quater.des);
	}

	if (flag == 1 && desArr.size() > 1) {
		quater.src1 = desArr[0];
		quater.src2 = "";
		for (int i = 1; i < desArr.size(); i++) {
			quater.op = Quaternary::assign;
			quater.des = desArr[i];
			if (!isNum(quater.des)) {
				cur_block.block_code.push_back(quater);
				Assign.push_back(quater.des);
			}
		}
	}
}

//�ڸú����ڣ�Ƶ������TransNodeToCode��ʵ��ת��
void OptimizationCode::TransDagToCode(DAG DagPic) {
	vector<int> temp_queue;
	int curnode;
	int i;
	while (1) {
		for (i = DagPic.ReSize() - 1; i >= 0; i--) {		//�ҵ���һ��������еĵ�(������dagͼ��ʱ�䵹�����)
			DAG_NODE temp = DagPic.ReNode(i);
			if (!NodeinQue(i, temp_queue) && (temp.LChild != -1 || temp.RChild != -1) && FatherinQue(temp.Father, temp_queue)) {		//���ڵ��������еķ�Ҷ���
				temp_queue.push_back(i);
				curnode = i;
				break;
			}
		}
		if (i == -1) {		//��ֹ����
			break;
		}
		DAG_NODE temp = DagPic.ReNode(curnode);
		while (temp.LChild != -1 || temp.RChild != -1) {
			if (temp.LChild != -1) {
				curnode = temp.LChild;
				temp = DagPic.ReNode(curnode);
				if (!NodeinQue(i, temp_queue) && (temp.LChild != -1 || temp.RChild != -1) && FatherinQue(temp.Father, temp_queue)) {		//���ڵ��������еķ�Ҷ���
					temp_queue.push_back(i);
				}
			}
			else if (temp.RChild != -1) {
				curnode = temp.RChild;
				temp = DagPic.ReNode(curnode);
				if (!NodeinQue(i, temp_queue) && (temp.LChild != -1 || temp.RChild != -1) && FatherinQue(temp.Father, temp_queue)) {		//���ڵ��������еķ�Ҷ���
					temp_queue.push_back(i);
				}
			}
		}
	}

	//�������ɶ����еĴ���
	while (!temp_queue.empty()) {
		int node = temp_queue.back();
		DAG_NODE temp = DagPic.ReNode(node);
		TransNodeToCode(temp);
		temp_queue.pop_back();
	}
}

void OptimizationCode::BuildDag(Block &block) {
	int i;
	ResetCurBlock();
	for (i = 0; i < block.block_code.size(); i++) {
		if (!Quaternary::Quaternary().isAssign(block.block_code[i].op)) {			//����Ϊ��ֵ��������䣬ֱ�ӷ���cur_block������
			cur_block.block_code.push_back(block.block_code[i]);
			//			cur_block.block_size += 4;
		}
		else break;
	}
	if (i == block.block_code.size())		//�������������ϣ�����
		return;

	while (i < block.block_code.size()) {
		dag = DAG::DAG();
		ChangeMess.clear();
		nodetable = NodeTable::NodeTable();
		for (; i < block.block_code.size(); i++) {		//������б��ʽ����dagͼ��������
			Quaternary temp = block.block_code[i];
			if (temp.isAssign(temp.op)) {
				switch (temp.op)
				{
				case Quaternary::assign: {
					GenAssignDag(temp);
					break;
				}
				case Quaternary::arrassign: {
					GenAssignDag(temp);
					break;
				}
				case Quaternary::assigntoarr: {
					GenAssignDag(temp);
					break;
				}
				case Quaternary::assignbyfunc: {
					GenAssignDag(temp);
					break;
				}
				case Quaternary::add: {
					GenCalculateDag(temp);
					break;
				}
				case Quaternary::sub: {
					GenCalculateDag(temp);
					break;
				}
				case Quaternary::div: {
					GenCalculateDag(temp);
					break;
				}
				case Quaternary::mult: {
					GenCalculateDag(temp);
					break;
				}
				case Quaternary::neg: {
					GenNegDag(temp);
					break;
				}
				default:
					break;
				}
			}
			else {
				break;
			}
		}

		TransDagToCode(dag);						//��dagͼת��Ϊ�м����

		//������Ҷ�ڵ�ı������и�ֵ����
		for (int i = 0; i < dag.ReSize(); i++) {
			DAG_NODE temp_node = dag.ReNode(i);
			Quaternary Quater_temp;
			Quater_temp.op = Quaternary::assign;
			if (temp_node.LChild == -1 && temp_node.RChild == -1) {
				if (isNum(temp_node.name)) {		//Ҷ�ڵ�Ϊ����ʱ
					vector<string> VarName = nodetable.searchNodeName(i);
					Quater_temp.src1 = temp_node.name;
					Quater_temp.src2 = "";
					if (VarName.size() > 0 && !isTemp(VarName[0]) && !isNum(VarName[0]))
						for (int j = 0; j < VarName.size(); j++) {
							Quater_temp.des = VarName[j];
							if (!isNum(Quater_temp.des))
								cur_block.block_code.push_back(Quater_temp);
						}
				}
				else {								//Ҷ���Ϊ����ʱ
					vector<string> VarName = nodetable.searchNodeName(i);
					Quater_temp.src1 = temp_node.name;
					Quater_temp.src1.pop_back();			//ȥ������ĩβ��0
					Quater_temp.src2 = "";
					if (VarName.size() > 0 && !isTemp(VarName[0]) && !isNum(VarName[0]))		//����������Ϊ����������ʱ����
						for (int j = 0; j < VarName.size(); j++) {
							Quater_temp.des = VarName[j];
							if ((Quater_temp.des + "0") != temp_node.name && !isNum(Quater_temp.des))
								cur_block.block_code.push_back(Quater_temp);
						}
				}
			}
		}

		for (; i < block.block_code.size(); i++) {			//�����ʽ����Ĳ������ݴ����м����
			Quaternary temp = block.block_code[i];
			if (temp.isAssign(temp.op))						//���ֶ��븳ֵ��������䣬�ٴι���dagͼ
				break;
		/*	if (temp.src1 != "" && !InAssign(temp.src1) && isTemp(temp.src1)) {
				int index = nodetable.searchNode(temp.src1);
				vector<string> name_temp = nodetable.searchNodeName(index);
				temp.src1 = name_temp[0];
			}
			if (temp.src2 != "" && !InAssign(temp.src2) && isTemp(temp.src2)) {
				int index = nodetable.searchNode(temp.src2);
				vector<string> name_temp = nodetable.searchNodeName(index);
				temp.src2 = name_temp[0];
			}
			if (temp.des != "" && !InAssign(temp.des) && isTemp(temp.des)) {
				int index = nodetable.searchNode(temp.des);
				vector<string> name_temp = nodetable.searchNodeName(index);
				temp.des = name_temp[0];
			}	*/
			cur_block.block_code.push_back(temp);
		}
	}
	block = cur_block;
}