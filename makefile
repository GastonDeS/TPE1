all: slave solve view

slave:
	gcc -g -Wall slave.c -o slave

solve:
	gcc -Wall -g -std=gnu99 solve.c -o solve -lrt -pthread

view: 
	gcc -g -Wall -lrt view.c -o view -lrt -pthread

clean:
	rm slave solve view result.txt