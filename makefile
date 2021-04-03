all: slave solve 

slave:
	gcc -g -Wall slave.c -o slave

solve:
	gcc -g -Wall -lrt solve.c -o solve

view: 
	gcc -g -Wall view.c -o view

clean:
	rm slave solve