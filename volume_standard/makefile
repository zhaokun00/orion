objects = main.o auto_loudness_control.o

main : $(objects)
	gcc -o $@ $^ -lm

%.o : %.c
	gcc -c -o $@ $<
	
clean :
	rm *.o main