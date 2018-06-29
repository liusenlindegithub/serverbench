#ifndef _SLOT_H_
#define _SLOT_H_

#include <memory>
#include <vector>
using namespace std;

#include "../utils/log/log.h"

#define MAX_SLOT_ELEMENT_SIZE 100

/**
loyangliu 2016/03/31
功能：资源分配器
1. 支持资源快速申请和释放
2. 支持快速检索到资源
SlotElement<T> *CSlot::allocNewSlot();
void CSlot::freeOldSlot(SlotElement<T> *p);
SlotElement<T> & CSlot::getSlotElement(unsigned int seq);
**/

template <class T> class SlotBlock;
template <class T> class CSlot;
template <class T>
class SlotElement
{
public:
	friend class SlotBlock<T>;
	friend class CSlot<T>;
	SlotElement<T>():seq(-1),object(NULL),freenext(NULL) {}
	SlotElement<T>(unsigned int index):seq(index),object(NULL),freenext(NULL) {}
	virtual ~SlotElement<T>() { }

	shared_ptr<T> getElement()
	{
		return object;
	}
	void setElement(shared_ptr<T> elem)
	{
		object = elem;
	}

	unsigned int getSeq()
	{
		return seq;
	}
private:
	unsigned int seq;
	shared_ptr<T> object;
	SlotElement<T> *freenext;
};

template <class T> class CSlot;
template <class T>
class SlotBlock
{
public:
	friend class CSlot<T>;
	SlotBlock<T>():table(NULL),free(NULL),blockindex(-1)
	{
	}

	virtual ~SlotBlock()
	{
		fnit();
	}

	int init()
	{
		table = new SlotElement<T>[MAX_SLOT_ELEMENT_SIZE];
		if(!table)
		{
			log_debug("calloc failed.");
			return -1;
		}

		for(int i=0; i<MAX_SLOT_ELEMENT_SIZE - 1; i++)
		{
			table[i].freenext = &table[i + 1];
		}
		table[MAX_SLOT_ELEMENT_SIZE - 1].freenext = NULL;
		free = &table[0];

		return 0;
	}

	void fnit()
	{
		if(table)
		{
			delete[] table;
		}
	}

	void setBlockIndex(unsigned short int index)
	{
		blockindex = index;
		if(table)
		{
			for(unsigned short int i=0; i<MAX_SLOT_ELEMENT_SIZE; i++)
			{
				unsigned int seq = 0;
				seq |= index;
				seq <<= 16;
				seq |= i;
				table[i].seq = seq;
			}
		}
	}

	SlotElement<T> **getLastFree()
	{
		return &table[MAX_SLOT_ELEMENT_SIZE - 1].freenext;
	}

	SlotElement<T> & operator[](int i)
	{
		return table[i];
	}
private:
	SlotElement<T> *table;
	SlotElement<T> *free;
	unsigned int blockindex;
};

template <class T>
class CSlot
{
public:
	CSlot<T>():free(NULL) 
	{

	}
	virtual ~CSlot<T>()
	{
		for(int i=0; i<btable.size(); i++)
		{
			SlotBlock<T> * &p = btable[i];
			if(p)
			{
				delete p;
			}
		}
	}

	SlotElement<T> *allocNewSlot()
	{
		SlotElement<T> *result = free;
		if(result == NULL)
		{
			int ret = expandBlock();
			if(ret != 0)
			{
				return NULL;
			}
		}
		
		result = free;
		free = free->freenext;
		return result;
	}

	void freeOldSlot(SlotElement<T> *p)
	{
		if(p)
		{
			p->freenext = free;
			p->object = NULL; //及时释放指针内存
			free = p;
		}
	}

	void freeOldSlot(unsigned int seq)
	{
		SlotElement<T> *p = getSlotElement(seq);
		freeOldSlot(p);
	}

	shared_ptr<T> getSlotObject(unsigned int seq)
	{
		unsigned int pos1 = seq >> 16;
		unsigned int pos2 = seq & 0x0000FFFF;
		
		if(pos1 >= btable.size())
		{
			log_debug("pos1 error. pos1=%d", pos1);
			return NULL;
		}

		if(pos2 >= MAX_SLOT_ELEMENT_SIZE)
		{
			log_debug("pos2 error. pos2=%d", pos2);
			return NULL;
		}

		SlotBlock<T> *block = btable.at(pos1);
		return block->table[pos2].object;
	}

	SlotElement<T> * getSlotElement(unsigned int seq)
	{
		unsigned int pos1 = seq >> 16;
		unsigned int pos2 = seq & 0x0000FFFF;
		
		if(pos1 >= btable.size())
		{
			log_debug("pos1 error. pos1=%d", pos1);
			return NULL;
		}

		if(pos2 >= MAX_SLOT_ELEMENT_SIZE)
		{
			log_debug("pos2 error. pos2=%d", pos2);
			return NULL;
		}

		SlotBlock<T> *block = btable.at(pos1);
		return &block->table[pos2];
	}

private:
	vector<SlotBlock<T> *> btable;
	SlotElement<T> *free;

	int expandBlock()
	{
		SlotBlock<T> *newblock = new SlotBlock<T>();
		if(newblock->init() == 0)
		{
			unsigned int insertindex = btable.size();
			if(insertindex > (unsigned int)0xFFFF)
			{
				log_debug("max limit btable size. insertindex=%u", insertindex);
				delete newblock;
				return -1;
			}

			newblock->setBlockIndex(insertindex);
			btable.push_back(newblock);
		}
		else
		{
			return -1;
		}

		SlotElement<T> **lastfree = newblock->getLastFree();
		*lastfree = free;
		free = newblock->free;

		return 0;
	}
};

#endif