#include "ringbuffer.h"

namespace agh {

RingBuffer::RingBuffer(long size, int packetSize)
{
	writeCursor = 0;
	readCursor = 0;
	readyCount = 0;
	bufferSize = size;
	this->sampleSize = packetSize;
	buffer = new char[size*packetSize];
}

RingBuffer::~RingBuffer()
{
	delete buffer;
}
	
bool RingBuffer::putData(char *data, long size)
{
	if( bufferSize - getReadyCount() < size )
		return false; 
	
	char *ptr1 = buffer + writeCursor*sampleSize;
	char *ptr2 = data;
	
	long toEnd = bufferSize - writeCursor;
	
	if(toEnd >= size) {
		for(long i=0;i<size*sampleSize;i++) *ptr1++ = *ptr2++;
	} else {
		for(long i=0;i<toEnd*sampleSize;i++) *ptr1++ = *ptr2++;
		ptr1 = buffer;
		for(long i=0;i<(size-toEnd)*sampleSize;i++) *ptr1++ = *ptr2++;	
	}
	
	writeCursor += size;
	if( writeCursor >= bufferSize )
		writeCursor -= bufferSize;
				
	readyCount += size;
	
	return true;
}

bool RingBuffer::putSilence(long size)
{
	if( bufferSize - getReadyCount() < size )
		return false; 
	
	char *ptr1 = buffer + writeCursor*sampleSize;
	
	long toEnd = bufferSize - writeCursor;
	
	if(toEnd >= size) {
		for(long i=0;i<size*sampleSize;i++) *ptr1++ = 0;
	} else {
		for(long i=0;i<toEnd*sampleSize;i++) *ptr1++ = 0;
		ptr1 = buffer;
		for(long i=0;i<(size-toEnd)*sampleSize;i++) *ptr1++ = 0;	
	}
	
	writeCursor += size;
	if( writeCursor >= bufferSize )
		writeCursor -= bufferSize;
				
	readyCount += size;
	
	return true;
}

bool RingBuffer::getData(char *data, long size)
{
	if( getReadyCount() < size )
		return false; 
	
	char *ptr1 = buffer + readCursor*sampleSize;
	char *ptr2 = data;
	
	long toEnd = bufferSize - readCursor;
	
	if(toEnd >= size) {
		for(long i=0;i<size*sampleSize;i++) *ptr2++ = *ptr1++;
	} else {
		for(long i=0;i<toEnd*sampleSize;i++) *ptr2++ = *ptr1++;
		ptr1 = buffer;
		for(long i=0;i<(size-toEnd)*sampleSize;i++) *ptr2++ = *ptr1++;	
	}
	
	readCursor += size;
	if( readCursor >= bufferSize )
		readCursor -= bufferSize;
				
	readyCount -= size;
	
	return true;
}

bool RingBuffer::peekData(char *data, long size)
{
	if( getReadyCount() < size )
		return false; 

	char *ptr1 = buffer + readCursor*sampleSize;
	char *ptr2 = data;
	
	long toEnd = bufferSize - readCursor;
	
	if(toEnd >= size) {
		for(long i=0;i<size*sampleSize;i++) *ptr2++ = *ptr1++;
	} else {
		for(long i=0;i<toEnd*sampleSize;i++) *ptr2++ = *ptr1++;
		ptr1 = buffer;
		for(long i=0;i<(size-toEnd)*sampleSize;i++) *ptr2++ = *ptr1++;	
	}
	
	return true;
}

bool RingBuffer::skipData(long size)
{
	if( getReadyCount() < size )
		return false; 

	readCursor += size;
	if( readCursor >= bufferSize )
		readCursor -= bufferSize;
				
	readyCount -= size;
	
	return true;
}

long RingBuffer::getReadyCount()
{
	return readyCount;
}

long RingBuffer::getFreeCount()
{
	return bufferSize - readyCount;
}

bool RingBuffer::moveData(RingBuffer *dest, long size)
{
	if( (getReadyCount() < size) || (dest->getFreeCount() < size) )
		return false;
	
	char *ptr1 = buffer + readCursor*sampleSize;

	long toEnd = bufferSize - readCursor;
	
	if(toEnd >= size) {
		dest->putData(ptr1, size);
	} else {
		dest->putData(ptr1, toEnd);
		ptr1 = buffer;
		dest->putData(ptr1, size-toEnd);
	}
	
	readCursor += size;
	if( readCursor >= bufferSize )
		readCursor -= bufferSize;
				
	readyCount -= size;
	
	return true;
}

} /* namespace agh */
