#ifndef _ERROR_H_
#define _ERROR_H_

void setpname(char *name);
char *getpname(void);
void warn(const char *fmt, ...);
void die(int status, const char *fmt, ...);
void panic(const char *fmt, ...);

#endif /* _ERROR_H_ */
