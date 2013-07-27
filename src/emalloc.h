/*
 * emalloc.h
 *
 *  Created on: Jul 27, 2013
 *      Author: cody
 */

#ifndef EMALLOC_H
#define EMALLOC_H

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

struct emallocBufferInfo
{
	char* start;
	uint32_t size;
};

void setupEmallocBufferInfo(struct emallocBufferInfo* bufptr, char* buffer, uint32_t size);
void* emalloc(struct emallocBufferInfo buf, size_t size);
void efree(struct emallocBufferInfo buf, void* ptr);
void* ecalloc(struct emallocBufferInfo buf, size_t nmemb, size_t size);
void* erealloc(struct emallocBufferInfo buf, void* ptr, size_t size);
void edfrag(struct emallocBufferInfo buf);

#endif /* EMALLOC_H */
