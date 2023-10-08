all: bufferOverflowAttack 

OverflowAttack: bufferOverflowAttack.o
	cc -g -o $@ $^

%.o: %.c
	cc -g  -Wall -o $@ -c $<

clean:
	rm bufferOverflowAttack bufferOverflowAttack.o
	