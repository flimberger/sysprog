#ifndef _STRTAB_H_
#define _STRTAB_H_

#include <stddef.h>

enum {
	STRTAB_SIZE = (4 * 1024)
};

struct strtab_elem {
	struct strtab_elem *next;
	char *data;
};

typedef struct {
	struct strtab_elem *list;
	char *fptr;	/* pointer at free space */
	size_t fsiz;	/* size of free space */
} Strtab;

Strtab *strtab_new(void);
void strtab_free(Strtab *tab);
const char *strtab_insert(Strtab *restrict tab, const char *const restrict str);

#endif /* _STRTAB_H_ */
