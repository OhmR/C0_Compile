#ifndef _ERROR_H_
#define _ERROR_H_

#include <string>

using namespace std;

class Error
{
public:
	Error();
	~Error();
	void ErrorMessage(int errortype, int line);
	void ErrorMessage(int errortype, int line, string message1);
	void ErrorMessage(int errortype, int line, string message1, string message2);
	int err_flag;
};


#endif // !_ERROR_H_