#ifndef _SYSPROG_H_
#define _SYSPROG_H_

#include "defs.h"

typedef struct {
	union {
		long long val;
		Symbol	*sym;
	}
	uint	row;
	uint	col;
	Symboltype	type;
} Token;

#endif /* _SYSPROG_H_ */
