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

	return;
}

void* ecalloc(struct emallocBufferInfo buf, size_t nmemb, size_t size)
{
	return NULL;
}

void* erealloc(struct emallocBufferInfo buf, void* point, size_t size)
{
	return NULL;
}

void edfrag(struct emallocBufferInfo buf)
{
	return;
}
