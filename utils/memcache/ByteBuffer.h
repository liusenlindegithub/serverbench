#ifndef _BYTEBUFFER_H_
#define _BYTEBUFFER_H_

#include <exception>

using namespace std;

class ByteBuffer
{
public:
	ByteBuffer();
	~ByteBuffer();
	
	void ExpandBuff(int expandSize) throw(exception);

	// encode
	template <class Type>
	void WriteType(Type t) throw(exception);
	void WriteChar(char c) throw(exception);
	void WriteShort(short int si) throw(exception);
	void WriteInt(int i) throw(exception);
	void WriteBytes(char *bytes, int offset, int len) throw(exception);

	// ≤Èø¥ª∫¥Ê
	void PrintBuff();
	char *GetBuff(char *&out, int &len);

	// decode
	void wrap(char *bytes, int offset, int len);
	template <class Type>
	void ReadType(Type &t) throw(exception);
	void ReadChar(char &c) throw(exception);
	void ReadShort(short &si) throw(exception);
	void ReadInt(int &i) throw(exception);
	void ReadBytes(char *bytes, int len) throw(exception);
	
private:
	char *buff;
	int capacity;
	int size;
	char *cursor; // ∂¡÷∏’Î
};

#endif