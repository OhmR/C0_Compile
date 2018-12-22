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
	void program();				//程序
	SymbolSet return_symbolset() { return set; }
	vector<Quaternary> return_middleCode(){ return Code.getcode(); }
	int err_flag;

private:
	string operande;			//辅助表达式语句的处理，跨函数存储变量名称
	int flag_AddSub;			//存储运算属于加1或减2
	int flag_MultDiv;			//存储运算属于乘1或除2
	int temp_count = 0;
	int label = 0;				//用于循环跳转使用（函数不使用这个标记）
	string op1, op2;
	int relatesign;

	MiddleCode Code;
	info ifo;
	SymbolSet set;
	string name;				//记录变量、常量或函数名
	string filename;
	lexical *lex;
	Error err;
	string WriteConst;
	int sign;
	int num;					//存储整数数值
	char charvalue;				//存储char数值
	void reset_temp() { temp_count = 0; }
	void JumpToChar(char ch);

	int addOperator();			//加法运算符
	int multOperator();			//乘法运算符
	int relateOperator();		//关系运算符
//	void isNum();				//数字
	void isChar();				//字符
	void constDec();			//常量说明
	void constDef();			//常量定义
	int unsignInt();			//无符号整数
	void Int();					//整数
	int identity();				//标识符
	void statementHead();		//声明头部
	void varDec();				//变量说明
	void varDef();				//变量定义
	int Type();					//类型标识符
	void reFuncDef();			//有返回函数定义
	void noFuncDef();			//无返回函数定义
	void compoundStatement();	//复合语句
	void parametersTable();		//参数表
	void mainFunc();			//主函数
	void expr();				//表达式
	void term();				//项
	void factor();				//因子
	void statement();			//语句
	void assignStatement();		//赋值语句
	void ifStatement();			//条件语句
	string condition();			//条件
	void cycleStatement();		//循环语句
	void stepSize();			//步长
	void reFuncCall();			//有返回值函数调用语句
	void noFuncCall();			//无返回值函数调用语句
	void valueParTable(string funcname);		//值参数表
	void statementColumn();		//语句列
	void write();				//写语句
	void read();				//读语句
	void Return();				//返回语句

	int isNum(string name);		//判断name是否为数字
	int transNum(string name);	//将string转化为int
};

#endif