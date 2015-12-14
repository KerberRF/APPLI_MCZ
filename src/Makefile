all : Appli_Emit Appli_Cmd_Mcz Appli_Cmd_Arg

Appli_Emit : Appli_Emit.o manchester.o
	gcc -o $@ $^ -lwiringPi -lmxml

Appli_Cmd_Mcz : Appli_Cmd_Mcz.o manchester.o
	gcc -o $@ $^ -lwiringPi -lmxml

Appli_Cmd_Arg : Appli_Cmd_Arg.o manchester.o
	gcc -o $@ $^ -lwiringPi

Appli_Emit.o : Appli_Emit.c
	gcc -c $< -o $@

Appli_Cmd_Mcz.o : Appli_Cmd_Mcz.c
	gcc -c $< -o $@

Appli_Cmd_Arg.o : Appli_Cmd_Arg.c
	gcc -c $< -o $@

%.o : %.c %.h
	gcc -c -std=c99 $< -o $@ | gcc -c -std=c99 $< -o $@

clean:
	rm -f Appli_Emit *.o |  rm -f Appli_Cmd_Mcz *.o | rm -f Appli_Cmd_Arg *.o

install: Appli_Emit Appli_Cmd_Mcz
	cp -f Appli_Emit /usr/local/bin/. | cp -f Appli_Cmd_Mcz /usr/local/bin/. | cp -f Appli_Cmd_Arg /usr/local/bin/.
