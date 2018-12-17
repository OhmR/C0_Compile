#include "pch.h"
#include "MipsCode.h"


MipsCode::MipsCode(string filename, vector<Quaternary> midcode){
	infunc = 0;
	pushadr = 0;
	stringnum = 0;
	Parser par(filename);
	par.program();
	middlecode = midcode;
	set = par.return_symbolset();
	set.curFTab.func_name("");	//当前函数名称清空
	insertCode(".text", "", "", "");	//开启代码段
	insertString(".data");				//开辟空间存字符串
	this->genMipsCode();		//生成目标代码
}


MipsCode::~MipsCode()
{
}

int MipsCode::isNum(string str) {
	if (str[0] <= '9' && str[0] >= '0')
		return 1;
	else if (str[0] == '-' || str[0] == '+')
		return 1;
	return 0;
}

void MipsCode::LoadReg() {
	insertCode("lw ", "$a0, ", "0", "($sp)");
	insertCode("lw ", "$a1, ", "4", "($sp)");
	insertCode("lw ", "$a2, ", "8", "($sp)");
	insertCode("lw ", "$a3, ", "12", "($sp)");
	insertCode("lw ", "$t0, ", "16", "($sp)");
	insertCode("lw ", "$t1, ", "20", "($sp)");
	insertCode("lw ", "$t2, ", "24", "($sp)");
	insertCode("lw ", "$t3, ", "28", "($sp)");
	insertCode("lw ", "$t4, ", "32", "($sp)");
	insertCode("lw ", "$t5, ", "36", "($sp)");
	insertCode("lw ", "$t6, ", "40", "($sp)");
	insertCode("lw ", "$t7, ", "44", "($sp)");
/*	insertCode("lw ", "$s0, ", "56", "($sp)");
	insertCode("lw ", "$s1, ", "60", "($sp)");
	insertCode("lw ", "$s2, ", "64", "($sp)");
	insertCode("lw ", "$s3, ", "68", "($sp)");
	insertCode("lw ", "$s4, ", "72", "($sp)");
	insertCode("lw ", "$s5, ", "76", "($sp)");
	insertCode("lw ", "$s6, ", "80", "($sp)");
	insertCode("lw ", "$s7, ", "84", "($sp)");*/
	insertCode("lw ", "$t8, ", "48", "($sp)");
	insertCode("lw ", "$t9, ", "52", "($sp)");
	insertCode("lw ", "$ra, ", "56", "($sp)");
	insertCode("addi ", "$sp, ", "$sp, ", "60");
}

void MipsCode::StoreReg() {
	insertCode("addi ", "$sp, ", "$sp, ", "-60");
	insertCode("sw ", "$a0, ", "0", "($sp)");
	insertCode("sw ", "$a1, ", "4", "($sp)");
	insertCode("sw ", "$a2, ", "8", "($sp)");
	insertCode("sw ", "$a3, ", "12", "($sp)");
	insertCode("sw ", "$t0, ", "16", "($sp)");
	insertCode("sw ", "$t1, ", "20", "($sp)");
	insertCode("sw ", "$t2, ", "24", "($sp)");
	insertCode("sw ", "$t3, ", "28", "($sp)");
	insertCode("sw ", "$t4, ", "32", "($sp)");
	insertCode("sw ", "$t5, ", "36", "($sp)");
	insertCode("sw ", "$t6, ", "40", "($sp)");
	insertCode("sw ", "$t7, ", "44", "($sp)");
/*	insertCode("sw ", "$s0, ", "56", "($sp)");
	insertCode("sw ", "$s1, ", "60", "($sp)");
	insertCode("sw ", "$s2, ", "64", "($sp)");
	insertCode("sw ", "$s3, ", "68", "($sp)");
	insertCode("sw ", "$s4, ", "72", "($sp)");
	insertCode("sw ", "$s5, ", "76", "($sp)");
	insertCode("sw ", "$s6, ", "80", "($sp)");
	insertCode("sw ", "$s7, ", "84", "($sp)");*/
	insertCode("sw ", "$t8, ", "48", "($sp)");
	insertCode("sw ", "$t9, ", "52", "($sp)");
	insertCode("sw ", "$ra, ", "56", "($sp)");
}

void MipsCode::LoadInReg(string desReg, string src1) {				//将数据src1存入寄存器desReg中，src1可以是数字、常变量
	if (isNum(src1)) {
		insertCode("li ", desReg+", ", src1, "");
	}
	else if(set.curFTab.ReName() != "" && set.curFTab.in_ft(src1)){
		insertCode("lw ", desReg + ", ", to_string(set.curFTab.ret_para().ret_TableSize() + set.curFTab.ret_ifo().adr), "($sp)");		//函数常变量(存于函数参数后)
	}
	else if (set.curFTab.ReName() != "" && set.curFTab.in_para(src1)) {
		insertCode("lw ", desReg + ", ", to_string(set.curFTab.ret_ifo().adr), "($sp)");		//函数参数
	}
	else if (set.in_Tab(src1)) {
		insertCode("lw ", desReg + ", ", to_string(set.reInfo().adr), "($gp)");		//全局变量
	}
}

void MipsCode::StoreToRam(string desReg, string src1) {
	if (set.curFTab.ReName()!="" && set.curFTab.in_ft(src1)) {
		insertCode("sw ", desReg + ", ", to_string(set.curFTab.ret_para().ret_TableSize() + set.curFTab.ret_ifo().adr), "($sp)");		//函数常变量(存于函数参数后)
	}
	else if (set.curFTab.ReName() != "" && set.curFTab.in_para(src1)) {
		insertCode("sw ", desReg + ", ", to_string(set.curFTab.ret_ifo().adr), "($sp)");		//函数参数
	}
	else if (set.in_Tab(src1)) {
		insertCode("sw ", desReg + ", ", to_string(set.reInfo().adr), "($gp)");		//全局变量
	}
}

void MipsCode::insertCode(string op, string des, string src1, string src2){
	code.push_back(op + des + src1 + src2);
//	cout << op + des + src1 + src2 << endl;
}

void MipsCode::insertString(string des) {
	stringvalue.push_back(des);
}

//middleCode <op, src1, src2, des>
void MipsCode::genMipsCode() {
	for (auto i = middlecode.begin(); i != middlecode.end(); i++) {
		switch (i->op)
		{
		case Quaternary::funccall: {
			if (set.in_FTab(i->des)) {
				for (pushadr = set.reFTable().ret_para().ret_TableSize() - 4; pushadr >= 0; pushadr -= 4) {
					string para = *(para_name.end() - 1);
					para_name.pop_back();
					LoadInReg("$t1", para);
					if (set.in_FTab(i->des)) {
						int size = set.reFTable().ret_FTableSize();
						insertCode("sw ", "$t1, ", to_string(pushadr - size - 60), "($sp)");
					}
				}
			}
			StoreReg();		//保存寄存器现场
			if(set.in_FTab(i->des))
				insertCode("addi ", "$sp, ", "$sp, ", to_string(set.reFTable().ret_FTableSize() * (-1)));	//开辟栈空间存储函数
			insertCode("jal ", i->des, "", "");
			LoadReg();		//还原寄存器现场
			break;
		}
		case Quaternary::voidfunc: {
			if (infunc == 0) {
				infunc = 1;
				insertCode("j ", "main", "", "");
			}
			set.in_FTab(i->des);
			set.curFTab = set.reFTable();
			insertCode(i->des, ":", "", "");
			break;
		}
		case Quaternary::intfunc: {
			if (infunc == 0) {
				infunc = 1;
				insertCode("j ", "main", "", "");
			}
			set.in_FTab(i->des);
			set.curFTab = set.reFTable();
			insertCode(i->des, ":", "", "");
			break;
		}
		case Quaternary::charfunc: {
			if (infunc == 0) {
				infunc = 1;
				insertCode("j ", "main", "", "");
			}
			set.in_FTab(i->des);
			set.curFTab = set.reFTable();
			insertCode(i->des, ":", "", "");
			break;
		}
		case Quaternary::mainfunc: {
			infunc = 1;
			set.in_FTab("main");
			set.curFTab = set.reFTable();
			insertCode("main:", "", "", "");
			insertCode("addi ", "$sp, ", "$sp, ", to_string(-1 * set.curFTab.ret_FTableSize()));
			break;
		}
		case Quaternary::funcend: {			//函数结束
			insertCode("addi ", "$sp, ", "$sp, ", to_string(set.curFTab.ret_FTableSize()));
			insertCode("jr ", "$ra", "", "");
//			LoadReg();	//还原寄存器现场
			break;
		}
		case Quaternary::assign: {
			LoadInReg("$t0", i->src1);
			StoreToRam("$t0", i->des);
			break;
		}
		case Quaternary::arrassign: {
			if (set.in_ATab(i->src1)) {					//adr全局数组元素 = 4*下标 + 全局常变量表大小 + 在数组表中的地址
				if (isNum(i->src2)) {
					insertCode("lw ", "$t0, ", to_string(set.getTab().ret_TableSize() + set.reInfo().adr + 4 * atoi((i->src2).c_str())), "($gp)");
					StoreToRam("$t0", i->des);
				}
				else {
					int size = set.getTab().ret_TableSize() + set.reInfo().adr;
					LoadInReg("$t0", i->src2);
					insertCode("sll ", "$t0, ", "$t0, ", "2");
					insertCode("addi ", "$t0, ", "$t0, ", to_string(size));
					insertCode("add ", "$t0, ", "$t0, ", "$gp");
					insertCode("lw ", "$t0, ", "0", "($t0)");
					StoreToRam("$t0", i->des);
				}
			}
			else if (set.curFTab.in_arr(i->src1)) {		//adr函数数组元素 = 4*下标 + 函数参数表大小 + 函数变量表大小 + 在数组表中的地址
				if (isNum(i->src2)) {
					insertCode("lw ", "$t0, ", to_string(set.curFTab.ret_ft().ret_TableSize() + set.curFTab.ret_para().ret_TableSize() + set.curFTab.ret_ifo().adr + 4 * atoi((i->src2).c_str())), "($sp)");
					StoreToRam("$t0", i->des);
				}
				else {
					int size = set.curFTab.ret_ft().ret_TableSize() + set.curFTab.ret_para().ret_TableSize() + set.curFTab.ret_ifo().adr;
					LoadInReg("$t0", i->src2);
					insertCode("sll ", "$t0, ", "$t0, ", "2");
					insertCode("addi ", "$t0, ", "$t0, ", to_string(size));
					insertCode("add ", "$t0, ", "$t0, ", "$sp");
					insertCode("lw ", "$t0, ", "0", "($t0)");
					StoreToRam("$t0", i->des);
				}
			}
			break;
		}
		case Quaternary::assigntoarr: {
			if (set.in_ATab(i->des)) {					//adr全局数组元素 = 4*下标 + 全局常变量表大小 + 在数组表中的地址
				int size;
				if (isNum(i->src1)) {
					size = set.getTab().ret_TableSize() + set.reInfo().adr + 4 * atoi((i->src1).c_str());
					LoadInReg("$t0", i->src2);
					insertCode("sw ", "$t0, ", to_string(size), "($gp)");
				}
				else {
					size = set.getTab().ret_TableSize() + set.reInfo().adr;
					LoadInReg("$t0", i->src1);
					insertCode("sll ", "$t0, ", "$t0, ", "2");
					insertCode("addi ", "$t0, ", "$t0, ", to_string(size));
					insertCode("add ", "$t0, ", "$t0, ", "$gp");
					LoadInReg("$t1", i->src2);
					insertCode("sw ", "$t1, ", "0", "($t0)");
				}
			}
			else if (set.curFTab.in_arr(i->des)) {		//adr函数数组元素 = 4*下标 + 函数参数表大小 + 函数变量表大小 + 在数组表中的地址
				int size;
				if (isNum(i->src1)) {
					size = set.curFTab.ret_ft().ret_TableSize() + set.curFTab.ret_para().ret_TableSize() + set.curFTab.ret_ifo().adr + 4 * atoi((i->src1).c_str());
					LoadInReg("$t0", i->src2);
					insertCode("sw ", "$t0, ", to_string(size), "($sp)");
				}
				else{
					size = set.curFTab.ret_ft().ret_TableSize() + set.curFTab.ret_para().ret_TableSize() + set.curFTab.ret_ifo().adr;
					LoadInReg("$t0", i->src1);
					insertCode("sll ", "$t0, ", "$t0, ", "2");
					insertCode("addi ", "$t0, ", "$t0, ", to_string(size));
					insertCode("add ", "$t0, ", "$t0, ", "$sp");
					LoadInReg("$t1", i->src2);
					insertCode("sw ", "$t1, ", "0", "($t0)");
				}
			}
			break;
		}
		case Quaternary::assignbyfunc: {
			StoreToRam("$v0", i->des);
			break;
		}
		case Quaternary::add: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("add ", "$t0,", "$t1,", "$t2");
			StoreToRam("$t0", i->des);
			break;
		}
		case Quaternary::sub: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("sub ", "$t0,", "$t1,", "$t2");
			StoreToRam("$t0", i->des);
			break;
		}
		case Quaternary::div: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("div ", "$t1, ", "$t2", "");
			insertCode("mflo ", "$t0", "", "");
			StoreToRam("$t0", i->des);
			break;
		}
		case Quaternary::mult: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("mult ", "$t1, ", "$t2", "");
			insertCode("mflo ", "$t0", "", "");
			StoreToRam("$t0", i->des);
			break;
		}
		case Quaternary::neg: {
			LoadInReg("$t1", i->src1);
			insertCode("sub ", "$t0,", "$0,", "$t1");
			StoreToRam("$t0", i->des);
			break;
		}
		case Quaternary::beq: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("beq ", "$t1, ", "$t2, ", i->des);
			break;
		}
		case Quaternary::bne: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("bne ", "$t1, ", "$t2, ", i->des);
			break;
		}
		case Quaternary::bls: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("blt ", "$t1, ", "$t2, ", i->des);
			break;
		}
		case Quaternary::ble: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("ble ", "$t1, ", "$t2, ", i->des);
			break;
		}
		case Quaternary::bgt: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("bgt ", "$t1, ", "$t2, ", i->des);
			break;
		}
		case Quaternary::bge: {
			LoadInReg("$t1", i->src1);
			LoadInReg("$t2", i->src2);
			insertCode("bge ", "$t1, ", "$t2, ", i->des);
			break;
		}
		case Quaternary::label: {
			insertCode(i->des, ":", "", "");
			break;
		}
		case Quaternary::push: {
			para_name.push_back(i->des);
			break;
		}
		case Quaternary::para: {
			break;
		}
		case Quaternary::ret: {
			LoadInReg("$v0", i->des);
			insertCode("addi ", "$sp, ", "$sp, ", to_string(set.curFTab.ret_FTableSize()));
			insertCode("jr ", "$ra", "", "");
//			LoadReg();	//还原寄存器现场
			break;
		}
		case Quaternary::go: {
			insertCode("j ", i->des, "", "");
			break;
		}
		case Quaternary::read: {
			if (set.curFTab.in_ft(i->des) || set.curFTab.in_para(i->des)) {
				if (set.curFTab.ret_ifo().typ == info::ints) {
					insertCode("li ", "$v0, ", "5", "");
					insertCode("syscall", "", "", "");
					StoreToRam("$v0", i->des);
				}
				else {
					insertCode("li ", "$v0, ", "12", "");
					insertCode("syscall", "", "", "");
					StoreToRam("$v0", i->des);
				}
			}
			else if (set.in_Tab(i->des)) {
				if (set.reInfo().typ == info::ints) {
					insertCode("li ", "$v0, ", "5", "");
					insertCode("syscall", "", "", "");
					StoreToRam("$v0", i->des);
				}
				else {
					insertCode("li ", "$v0, ", "12", "");
					insertCode("syscall", "", "", "");
					StoreToRam("$v0", i->des);
				}
			}
			break;
		}
		case Quaternary::write: {
			if (set.in_Tab(i->des)) {
				if (set.reInfo().typ == info::chars) {
					insertCode("li ", "$v0, ", "11", "");
					LoadInReg("$a0", i->des);
					insertCode("syscall", "", "", "");
				}
				else {
					insertCode("li ", "$v0, ", "1", "");
					LoadInReg("$a0", i->des);
					insertCode("syscall", "", "", "");
				}
			}
			else if (set.curFTab.in_ft(i->des) || set.curFTab.in_para(i->des)) {
				if (set.curFTab.ret_ifo().typ == info::chars) {
					insertCode("li ", "$v0, ", "11", "");
					LoadInReg("$a0", i->des);
					insertCode("syscall", "", "", "");
				}
				else {
					insertCode("li ", "$v0, ", "1", "");
					LoadInReg("$a0", i->des);
					insertCode("syscall", "", "", "");
				}
			}
			else if (isNum(i->des)) {
				if (i->src1 == "char") {
					insertCode("li ", "$v0, ", "11", "");
					LoadInReg("$a0", i->des);
					insertCode("syscall", "", "", "");
				}
				else {
					insertCode("li ", "$v0, ", "1", "");
					LoadInReg("$a0", i->des);
					insertCode("syscall", "", "", "");
				}
			}
			break;
		}
		case Quaternary::writestring: {
			insertString("str" + to_string(stringnum) + ":.asciiz \"" + i->des + "\"");
			insertCode("la ", "$a0, ", "str", to_string(stringnum));
			stringnum++;
			insertCode("li ", "$v0, ", "4", "");
			insertCode("syscall", "", "", "");
			break;
		}
		case Quaternary::constdef: {
			if (infunc == 0) {				//全局常量定义
				set.in_Tab(i->des);
				if (set.reInfo().typ == info::ints)
					insertCode("li ", "$t0, ", to_string(set.reInfo().value), "");
				else
					insertCode("li ", "$t0, ", to_string(int(set.reInfo().ch)), "");
				StoreToRam("$t0", i->des);
			}
			else {							//函数内常量定义
				set.curFTab.in_ft(i->des);
				if (set.curFTab.ret_ifo().typ == info::ints)
					insertCode("li ", "$t0, ", to_string(set.curFTab.ret_ifo().value), "");
				else
					insertCode("li ", "$t0, ", to_string(int(set.curFTab.ret_ifo().ch)), "");
				StoreToRam("$t0", i->des);
			}
			break;
		}
		case Quaternary::vardef: {
			break;
		}
		default:
			break;
		}
	}
}

void MipsCode::PrintCode() {
	for (auto i = code.begin(); i != code.end(); i++) {
		cout << *i << endl;
	}
}

void MipsCode::PrintString() {
	for (auto i = stringvalue.begin(); i != stringvalue.end(); i++) {
		cout << *i << endl;
	}
}

void MipsCode::PrintMiddleCodeText() {
	for (auto i = middlecode.begin(); i != middlecode.end(); i++) {
		switch (i->op)
		{
		case Quaternary::funccall: {
			cout << "call " << i->des << endl;
			break;
		}
		case Quaternary::voidfunc: {
			cout << "void " << i->des << "()" << endl;
			break;
		}
		case Quaternary::intfunc: {
			cout << "int " << i->des << "()" << endl;
			break;
		}
		case Quaternary::charfunc: {
			cout << "char " << i->des << "()" << endl;
			break;
		}
		case Quaternary::mainfunc: {
			cout << "void " << "main" << "()" << endl;
			break;
		}
		case Quaternary::funcend: {
			break;
		}
		case Quaternary::assign: {
			cout << i->des << " = " << i->src1 << endl;;
			break;
		}
		case Quaternary::arrassign: {
			cout << i->des << " = " << i->src1 << "[" + i->src2 + "]" << endl;
			break;
		}
		case Quaternary::assigntoarr: {
			cout << i->des << "[" + i->src1 + "]"  << " = " << i->src2 << endl;
			break;
		}
		case Quaternary::assignbyfunc: {			//arrassign =[], assigntoarr []=
			cout << i->des << " = RET" << endl;
			break;
		}
		case Quaternary::add: {
			cout << i->des << " = " << i->src1 << " + " << i->src2 << endl;
			break;
		}
		case Quaternary::sub: {
			cout << i->des << " = " << i->src1 << " - " << i->src2 << endl;
			break;
		}
		case Quaternary::div: {
			cout << i->des << " = " << i->src1 << " / " << i->src2 << endl;
			break;
		}
		case Quaternary::mult: {
			cout << i->des << " = " << i->src1 << " * " << i->src2 << endl;
			break;
		}
		case Quaternary::neg: {
			cout << i->des << " = " << i->src1 << " * " << "-1" << endl;
			break;
		}
		case Quaternary::beq: {
			cout << "if " << i->src1 << "==" << i->src2 << " then go to " << i->des << endl;
			break;
		}
		case Quaternary::bne: {
			cout << "if " << i->src1 << "!=" << i->src2 << " then go to " << i->des << endl;
			break;
		}
		case Quaternary::bls: {
			cout << "if " << i->src1 << "<" << i->src2 << " then go to " << i->des << endl;
			break;
		}
		case Quaternary::ble: {
			cout << "if " << i->src1 << "<=" << i->src2 << " then go to " << i->des << endl;
			break;
		}
		case Quaternary::bgt: {
			cout << "if " << i->src1 << ">" << i->src2 << " then go to " << i->des << endl;
			break;
		}
		case Quaternary::bge: {
			cout << "if " << i->src1 << ">=" << i->src2 << " then go to " << i->des << endl;
			break;
		}
		case Quaternary::label: {
			cout << i->des << ":" << endl;
			break;
		}
		case Quaternary::push: {
			cout << "push " << i->des << endl;
			break;
		}
		case Quaternary::para: {
			if (set.curFTab.in_arr(i->src1)) {
				if(set.curFTab.ret_ifo().typ == info::ints)
					cout << "para" << " int " << i->des << endl;
				else
					cout << "para" << " char " << i->des << endl;
			}
			break;
		}
		case Quaternary::ret: {
			cout << "ret " << i->des << endl;
			break;
		}
		case Quaternary::go: {
			cout << "goto " << i->des << endl;
			break;
		}
		case Quaternary::read: {
			cout << "read " << i->des << endl;
			break;
		}
		case Quaternary::write: {
			cout << "write " << i->des << endl;
			break;
		}
		case Quaternary::writestring: {
			cout << "write string " << i->des << endl;
			break;
		}
		case Quaternary::constdef: {
			if (set.in_Tab(i->des)) {
				if (set.reInfo().typ == info::ints) {
					cout << "const int " << i->des << " = " << to_string(set.reInfo().value) << endl;
				}
				else {
					cout << "const char " << i->des << " = " << set.reInfo().ch << endl;
				}
			}
			else if (set.curFTab.in_ft(i->des)) {
				if (set.curFTab.ret_ifo().typ == info::ints) {
					cout << "const int " << i->des << " = " << to_string(set.curFTab.ret_ifo().value) << endl;
				}
				else {
					cout << "const char " << i->des << " = " << set.reInfo().ch << endl;
				}
			}
			break;
		}
		case Quaternary::vardef: {
			if (set.in_Tab(i->des)) {
				if (set.reInfo().typ == info::ints) {
					cout << "var int " << i->des << endl;
				}
				else {
					cout << "var char " << i->des << endl;
				}
			}
			else if (set.curFTab.in_ft(i->des)) {
				if (set.curFTab.ret_ifo().typ == info::ints) {
					cout << "var int " << i->des << endl;
				}
				else {
					cout << "var char " << i->des << endl;
				}
			}
			break;
		}
		default:
			break;
		}
	}
}