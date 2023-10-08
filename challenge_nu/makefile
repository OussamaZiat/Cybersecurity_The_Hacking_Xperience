all: attacking

try: attacking.o
	cc -g -o $@ $^
	
%.o: %.c
	cc -g -z execstack -fno-stack-protector -Wall -o $@ -c $<

clean:
	rm attacking attacking.o
