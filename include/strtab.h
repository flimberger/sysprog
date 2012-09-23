#ifndef _STRTAB_H_
#define _STRTAB_H_

#include <stddef.h>

#define STRTAB_SIZE (4 * 1024)

typedef struct strtab_elem strtab_elem;
struct strtab_elem {
	strtab_elem *nextelem;
	char *nextstr;
	char *data;
	size_t size;
};

strtab_elem *strtab_new(size_t size);
void strtab_free(strtab_elem *tab);
const char *strtab_insert(strtab_elem *restrict tab, const char *const restrict str);

#endif /* _STRTAB_H_ */