#Create lib_simpleloader.so from loader.c
all:
	gcc -m32 -r -D_POSIX_C_SOURCE=199309L -o loader.o loader.c
	gcc -shared -m32 -D_POSIX_C_SOURCE=199309L -o ../bin/lib_simpleloader.so loader.o

#Provide the command for cleanup
clean:
	-@rm -f loader.o ../bin/lib_simpleloader.so