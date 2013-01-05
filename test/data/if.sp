int foo;
int bar;

foo = 1;
bar = 1;

if (foo = bar)
	foo = 0
else
	bar = 0;

if (foo = bar) {
	foo = 0;
} else
	bar = 0;

if (foo = bar) {
	foo = 0;
	bar = 1;
} else
	bar = 0;

if (foo = bar)
	foo = 0
else {
	bar = 0;
};

if (foo = bar)
	foo = 0
else {
	foo = 1;
	bar = 0;
};

if (foo = bar) {
	foo = 0;
	bar = 1;
} else {
	foo = 1;
	bar = 0;
};
