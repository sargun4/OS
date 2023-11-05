#Compile the launch.c by linking it with the lib_simpleloader.so
all:
	gcc -m32 -c -r -o launch.o launch.c
	gcc -m32 -o ../bin/launch launch.o ../bin/lib_simpleloader.so
	
#Provide the command for cleanup
clean:
	-@rm -f ../bin/launch launch.o