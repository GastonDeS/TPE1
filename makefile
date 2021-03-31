all: slave solve view

slave:
	gcc -g -Wall slave.c -o slave

solve:
	gcc -g -Wall solve.c -o solve

view: 


clean:
	rm slave solve