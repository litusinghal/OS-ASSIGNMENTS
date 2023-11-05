all:
	gcc -m32 -no-pie -nostdlib -o fib fib.c
	gcc -m32 -no-pie -nostdlib -o sum sum.c
	gcc -m32 -o SimpleSmartLoader SimpleSmartLoader.c
	gcc -m32 -o SimpleSmartLoaderbonus SimpleSmartLoaderbonus.c

	./SimpleSmartLoader fib
	./SimpleSmartLoaderbonus fib
	./SimpleSmartLoader sum
	./SimpleSmartLoaderbonus sum
	
clean:
	-@rm -f fib loader
