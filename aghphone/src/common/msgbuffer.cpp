#include "msgbuffer.h"
#include <string.h>

namespace agh {

MsgBuffer::MsgBuffer(int maxMsgSize, long tableSize)
{
	this->tableSize = tableSize;
	this->maxMsgSize = maxMsgSize;
	readyCount = 0;
	writeCursor = 0;
	readCursor = 0;
	
	msgTable = new msg_struct[tableSize];
	
	for(long i=0;i<tableSize;i++)
		msgTable[i].data = new char[maxMsgSize];
}

MsgBuffer::~MsgBuffer()
{
	for(long i=0;i<tableSize;i++)
		delete msgTable[i].data;
	
	delete msgTable;
}

bool MsgBuffer::putMessage(char* data, int size)
{
	if(getFreeCount() > 0) {
		msg_struct* msgptr = &msgTable[writeCursor];
		msgptr->size = size;
		memcpy(msgptr->data, data, size);
		
		writeCursor++;
		if(writeCursor == tableSize)
			writeCursor = 0;
		readyCount++;
		
		return true;
	} else {
		return false;
	}
}

int MsgBuffer::getMessage(char* dest)
{
	if(getReadyCount() > 0) {
		msg_struct* msgptr = &msgTable[readCursor];
		
		memcpy(dest, msgptr->data, msgptr->size);
		
		readCursor++;
		if(readCursor == tableSize)
			readCursor = 0;
		readyCount--;
		
		return msgptr->size;
	} else {
		return -1;
	}
}
		
long MsgBuffer::getReadyCount()
{
	return readyCount;
}

long MsgBuffer::getFreeCount()
{
	return tableSize - readyCount;
}

int MsgBuffer::peekMessage(char **dataPtr)
{
	msg_struct* msgptr = &msgTable[readCursor];
	*dataPtr = msgptr->data;
	return msgptr->size;
}

void MsgBuffer::skipMessage()
{
	if(readyCount > 0) {
		readCursor++;
		if(readCursor >= tableSize)
			readCursor -= tableSize;
		readyCount--;
	}
}

void MsgBuffer::debug()
{
//	printf("sendBuffer| readycount: %ld, writeCursor: %ld, readCursor: %ld\n", readyCount, writeCursor, readCursor); 
}

} /* namespace agh */
