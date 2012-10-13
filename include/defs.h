/* convenient type definitions */

#ifndef _DEFS_H_
#define _DEFS_H_

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
typedef long long      vlong;
typedef unsigned long long uvlong;

typedef enum {
	ERROR,
	IDENTIFIER,
	INTEGER,
	PRINT,
	READ,
	SIGN
} Symboltype;

typedef struct {
	const char *lexem;
	Symboltype type;
} Lexerinfo;

#endif /* _DEFS_H_ */
