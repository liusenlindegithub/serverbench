#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <stdlib.h>

using namespace std;

/**
智能指针是进行内存管理的重要工具。我们就以智能指针对类实例指针进行封装，已达到释放堆内存的作用。
https://blog.csdn.net/lanchunhui/article/details/50878505
**/
template <class T>
class Singleton
{
public:
	static shared_ptr<T> getInstance()
	{
		if(instance == NULL)
		{
			instance = shared_ptr<T>(new T());
		}

		return instance;
	}

private:
	static shared_ptr<T> instance;
};

template <class T>
shared_ptr<T> Singleton<T>::instance = NULL;
#endif