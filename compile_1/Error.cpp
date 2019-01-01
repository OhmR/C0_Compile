#include "pch.h"
#include "Error.h"
#include <iostream>

Error::Error()
{
	err_flag = 0;
}

Error::~Error()
{
}

void Error::ErrorMessage(int errortype, int line) {
	err_flag = 1;
	cout << "错误发生在第" << line << "行" << endl;
	switch (errortype)
	{
	case 8:
		cout << "need a ';' in the end" << endl;
		break;
	case 9:
		cout << "illegal factor" << endl;
		break;
	case 10:
		cout << "In condition, two ident type are unmap" << endl;
		break;
	case 13:
		cout << "illegal return statement in a int or char func" << endl;
		break;
	case 14:
		cout << "illegal return statement in a void func" << endl;
		break;
	default:
		break;
	}
}

void Error::ErrorMessage(int errortype, int line, string message1) {
	err_flag = 1;
	cout << "错误发生在第" << line << "行" << endl;
	switch (errortype)
	{
	case 0:
		cout << "Illegal word '" << message1 << "' "<<endl;
		break;
	case 1:
		cout << "Member '" + message1 + "' is illegal, "<<endl;
		break;
	case 2:
		cout << "Illegal char " + message1 + " after '!'" << endl;;		//不等号不完整
		break;
	case 3:
		cout << "char '" << message1 << "' out of size, " << endl;
		break;
	case 4:
		cout << "Illegal char '" + message1 + "' in isChar" << endl;
		break;
	case 6:
		cout << "hope " + message1 +" here but I get another word" << endl;
		break;
	case 11:
		cout<< "Array " + message1 + " out of bound" <<endl;
		break;
	case 12:
		cout << "Illegal assign to const " + message1 << endl;
		break;
	case 15:
		cout << "In condition, " + message1 + " is illegal type char" << endl;
		break;
	case 16:
		cout << "return a char message in not char func " + message1 << endl;
		break;
	case 17:
		cout << "return a int message in not int func " + message1 << endl;
		break;
	case 18:
		cout << "Unknown function name " + message1 << endl;
		break;
	case 19:
		cout << "Multiple def identity" + message1 << endl;
		break;
	case 20:
		cout << "functioncall " + message1 + "'s para type is unmap" << endl;
		break;
	default:
		break;
	}
}

void Error::ErrorMessage(int errortype, int line, string message1, string message2) {
	err_flag = 1;
	cout << "错误发生在第" << line << "行" << endl;
	switch (errortype)
	{
	case 5:
		cout << "期望获取类型的" + message1 + "sy," + "错误获取" + message2 + "类型的sy" << endl;
		break;
	case 7:
		cout << "hope " + message1 + " after " + message2 + ",but we get another word" << endl;
		break;
	default:
		cout << "3" << endl;
		break;
	}
}