procedural language that allows you to define functions, use conditional, math, etc.

support the if/then/else, a for loop, user defined operators, JIT compiliation with a simple command line interface.

just use int64_t

def fib(x):
    if x < 3:
       return 1
    else:
	return fib(x-1) + fib(x-2)

