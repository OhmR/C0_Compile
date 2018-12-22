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
		cout << "Illegal word '" << message1 << "' ";
		break;
	case 1:
		cout << "Member '" + message1 + "' is illegal, ";
		break;
	case 2:
		cout << "Illegal char " + message1 + " after '!'" << endl;;		//不等号不完整
		break;
	case 3:
		cout << "char '" << message1 << "' out of size, ";
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
		break;
	}
}