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
	T_NONE,
	T_BOOL,
	T_INT,
	T_INTARR
} Datatype;

typedef enum {
	S_NONE,
	S_ELSE,
	S_END,
	S_ERROR,
	S_IDENT,
	S_IF,
	S_INT,   /* keyword "int" */
	S_INTCONST,
	S_PRINT,
	S_READ,
	S_WHILE,
	S_PLUS,  /* + */
	S_MINUS, /* - */
	S_DIV,	 /* / */
	S_MULT,	 /* * */
	S_LESS,	 /* < */
	S_GRTR,	 /* > */
	S_EQUAL, /* = */
	S_UNEQL, /* <!> */
	S_NOT,	 /* ! */
	S_AND,	 /* & */
	S_TERM,	 /* ; */
	S_PAROP, /* ( */
	S_PARCL, /* ) */
	S_CBOP,	 /* { */
	S_CBCL,	 /* } */
	S_BROP,	 /* [ */
	S_BRCL	 /* ] */
} Symboltype;

#endif /* _DEFS_H_ */
