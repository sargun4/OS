#Create lib_simpleloader.so from loader.c
all:
	gcc -m32 -r -o loader.o loader.c
	gcc -shared -m32 -o ../bin/lib_simpleloader.so loader.o

#Provide the command for cleanup
clean:
	-@rm -f loader.o ../bin/lib_simpleloader.so