#pragma once
#include "main.h"

class ByteStream
{
public:
	ByteStream();
	template <class R>
	void Read(R& data);
	void Read(char* data, int len);
	void Read(byte* data, int len);
	template <class T>
	void Write(T data);
	void Write(byte* data, int len);
	void Write(BigInt num);
	void Write(const char* text, int len = -1);
	void Clear();
	unsigned int GetLength();
	byte* GetData(unsigned int OUT * len);
	void ResetWritePointer();
	void ResetReadPointer();
private:
	byte* m_Buffer;
	int m_iReadPointer;
	int m_iWritePointer;
	int m_iLength; // allocated buffer length
};

template<class R>
inline void ByteStream::Read(R& data)
{
	for (int end = m_iReadPointer + sizeof(R), k = 0; m_iReadPointer < end; m_iReadPointer++, k++) {
		*(unsigned char*)((size_t)(void*)&data + k) = m_Buffer[m_iReadPointer];
	}
}

template<class T>
inline void ByteStream::Write(T data)
{
	if (!m_Buffer) {
		m_Buffer = (byte*)malloc(sizeof(T));
		m_iLength = sizeof(T);
	}
	else {
		unsigned int free_space = m_iLength - m_iWritePointer;
		if (free_space < sizeof(T)) {
			m_Buffer = (byte*)realloc(m_Buffer, m_iLength + (sizeof(T) - free_space));
		}
		m_iLength = m_iLength + (sizeof(T) - free_space);
	}
	for (int end = m_iWritePointer + sizeof(T), k = 0; m_iWritePointer < end; m_iWritePointer++) {
		m_Buffer[m_iWritePointer] = ((unsigned char*)&data)[k++];
	}
}
