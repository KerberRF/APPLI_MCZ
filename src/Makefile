all : Appli_Emit 

Appli_Emit : Appli_Emit.o manchester.o utils.o
	gcc -o $@ $^ -lwiringPi -lmxml

Appli_Emit.o : Appli_Emit.c
	gcc -c $< -o $@

%.o : %.c %.h
	gcc -c -std=c99 $< -o $@

clean:
	rm -f Appli_Emit *.o

install: Appli_Emit
	cp -f Appli_Emit /usr/local/bin/.
