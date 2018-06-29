#ifndef _NOCOPYABLE_H_
#define _NOCOPYABLE_H_

#include <iostream>
using namespace std;

/**
禁止对象被拷贝构造或被赋值。
编译期间发现错误。
**/  

class NoCopyable
{
public:
	NoCopyable() 
	{
		//cout << "NoCopyable() " << endl;
	}
  
	virtual ~NoCopyable()
	{
		//cout << "NoCopyable() " << endl;
	}

private:
	NoCopyable(const NoCopyable &r);

	NoCopyable &operator=(const NoCopyable &r);
};

#endif