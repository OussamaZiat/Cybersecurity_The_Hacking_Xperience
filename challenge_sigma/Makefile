all: attack multi_thread_attack

attack: attack.o
	cc -g -o attack attack.o

multi_thread_attack: multi_thread_attack.o attack.o
	cc -g -o multi_thread_attack multi_thread_attack.o -pthread



clean:
	rm -f multi_thread_attack multi_thread_attack.o
	rm -f attack attack.o

%.o: %.c
	cc -g3 -Wall -o $@ -c $<