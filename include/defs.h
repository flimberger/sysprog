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
	NONE,
	ELSE,
	END,
	ERROR,
	IDENTIFIER,
	IF,
	INT,
	INTEGER,
	PRINT,
	READ,
	WHILE,
	SIGN_PLUS,	/* + */
	SIGN_MINUS,	/* - */
	SIGN_DIV,	/* / */
	SIGN_MULT,	/* * */
	SIGN_LESS,	/* < */
	SIGN_GRTR,	/* > */
	SIGN_EQUAL,	/* = */
	SIGN_UNEQL,	/* <!> */
	SIGN_NOT,	/* ! */
	SIGN_AND,	/* & */
	SIGN_TERM,	/* ; */
	SIGN_PAROP,	/* ( */
	SIGN_PARCL,	/* ) */
	SIGN_CBOP,	/* { */
	SIGN_CBCL,	/* } */
	SIGN_BROP,	/* [ */
	SIGN_BRCL	/* ] */
} Symboltype;

#endif /* _DEFS_H_ */
