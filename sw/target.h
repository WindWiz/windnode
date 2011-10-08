#ifndef _TARGET_H_
#define _TARGET_H_

#ifdef __AVR
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned short)(&((type *)0)->member)))
#else
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
#endif

#endif				/* _TARGET_H_ */
