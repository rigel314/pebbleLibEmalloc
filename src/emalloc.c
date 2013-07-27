/*
 * emalloc.c
 *
 *  Created on: Jul 27, 2013
 *      Author: cody
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "emalloc.h"

void setupEmallocBufferInfo(struct emallocBufferInfo* bufptr, char* buffer, uint32_t size)
{
	bufptr->start = buffer;
	bufptr->size = size;
	((uint32_t*) buffer)[0] = size & 0x7FFFFFFF;
	return;
}

void* emalloc(struct emallocBufferInfo buf, size_t size)
{
	uint32_t i;
	char* buffer = buf.start;
	char* ptr;

	for(ptr = buffer, i = *((uint32_t*) ptr); ptr < buffer+buf.size-1; ptr += i+4, i = *((uint32_t*) ptr))
	{
		if (i & 0x80000000)
			continue;

		if (i >= size+4)
		{
			*((uint32_t*) ptr) = size & 0x80000000;
			ptr += 4;
			*((uint32_t*) ptr+4+size) = (i - (size + 4)) & 0x7FFFFFFF;
			return ptr;
		}
		if (i >= size)
		{
			*((uint32_t*) ptr) = i & 0x80000000;
			return ptr+4;
		}
	}
	return NULL;
}

void combineFree(struct emallocBufferInfo buf)
{
	uint32_t i;
	char* buffer = buf.start;
	char* ptr;
	char* lastptr = NULL;
	int lasti;

	for(ptr = buffer, i = *((uint32_t*) ptr); ptr < buffer+buf.size-1; ptr += i+4, i = *((uint32_t*) ptr))
	{
		if (lastptr && !(i & 0x80000000) && !(lasti & 0x80000000))
		{ // !(i & 0x80000000 || lasti & 0x80000000)
			*((uint32_t*) lastptr) = (i + lasti + 4) & 0x7FFFFFFF;
			lasti = (i + lasti + 4) & 0x7FFFFFFF;
			lastptr = lastptr;
			continue;
		}

		lasti = i;
		lastptr = ptr;
	}
}

void efree(struct emallocBufferInfo buf, void* point)
{
	uint32_t i;
	char* buffer = buf.start;
	char* ptr;

	for(ptr = buffer, i = *((uint32_t*) ptr); ptr < buffer+buf.size-1; ptr += i+4, i = *((uint32_t*) ptr))
	{
		if (ptr == point-4)
		{
			*((uint32_t*) ptr) = i & 0x7FFFFFFF;
			return;
		}
	}

	combineFree(buf);

	return;
}

void* ecalloc(struct emallocBufferInfo buf, size_t nmemb, size_t size)
{
	char* ptr = emalloc(buf, nmemb * size);
	if (!ptr)
		return NULL;
	memset(ptr, 0, nmemb * size);
	return ptr;
}

void* erealloc(struct emallocBufferInfo buf, void* point, size_t size)
{
	if (size == 0)
	{
		efree(buf, point);
		return NULL;
	}
	uint32_t oldsize = *((uint32_t*) point-4);
	char* ptr = emalloc(buf, size);
	if (!ptr)
		return NULL;

	memcpy(ptr, point, (size < oldsize) ? size : oldsize);

	efree(buf, point);
	return ptr;
}
/*
void edfrag(struct emallocBufferInfo buf)
{
	combineFree(buf);
	return;
}
*/
