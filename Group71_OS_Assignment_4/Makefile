all:
	gcc -m32 -no-pie -nostdlib -o fib fib.c
	gcc -m32 -no-pie -nostdlib -o sum sum.c
	gcc -m32 -o SimpleSmartLoader SimpleSmartLoader.c
	./SimpleSmartLoader fib
	./SimpleSmartLoader sum
	
clean:
	-@rm -f fib loader
