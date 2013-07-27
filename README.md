pebbleLibEmalloc
================

My attempt at making a simple malloc() command for pebble.

It will operate on a staticly allocated buffer.  Inside the buffer it will keep track of used space and return pointers to unused space in the buffer.

Refer to [malloc()](http://linux.die.net/man/3/malloc) for more details.

struct emallocBufferInfo
{
	char* start;
	uint32_t size;
};

void setupEmallocBufferInfo(struct emallocBufferInfo* bufptr, char* buffer, uint32_t size);
	Initializes the buffer and the info struct to be used with this malloc suite.

void* emalloc(struct emallocBufferInfo buf, size_t size);
	Acts like malloc().  *buf* must have been initialized with setupEmallocBufferInfo().

void efree(struct emallocBufferInfo buf, void* ptr);
	Acts like free().  *buf* must have been initialized with setupEmallocBufferInfo().

void* ecalloc(struct emallocBufferInfo buf, size_t nmemb, size_t size);
	Acts like calloc().  *buf* must have been initialized with setupEmallocBufferInfo().

void* erealloc(struct emallocBufferInfo buf, void* ptr, size_t size);
	Acts like realloc().  *buf* must have been initialized with setupEmallocBufferInfo().

At present, my implementation is as follows:
	* Fragments, the word I'm using to think about the chunks of memory emalloc() allocates.
		* 4 byte length - most significant bit is set when a fragment is allocated.
		* *length* bytes of data
	* When setupEmallocBufferInfo() is called.
		* The buffer in initialized with an unused fragment that is as long as the buffer - 4 bytes.
	* When malloc() is called.
		* It finds the first fragment that is both big enough to fit *size* and marked unused.
		* If there is enough space to create another fragment that is at least 0 bytes in size, then it makes a new unallocated fragment.
		* Else, it allocates the entire fragment.
	* When efree() is called.
		* It looks at each fragment for *ptr*.
		* If it finds *ptr*, it marks it unallocated.  It then combines adjcent unallocated fragments.
		* Else it returns.
	* When ecalloc() is called.
		* It calls emalloc().
		* Then it calls memset() to set the new space to 0s.
	* When erealloc() is called.
		* It calls emalloc().
		* Then it calls memcpy() to copy as much of the old data as will fit in the new.
		* Then it calls efree() on *ptr*;
