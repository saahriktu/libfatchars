all:
	gcc -c -fPIC -Wall -O2 -o fatchars.o fatchars.c
	gcc -fPIC -Wall -shared -O2 -o libfatchars.so fatchars.o
install:
	mkdir /usr/include/fatchars
	cp libfatchars.so /usr/lib/
	cp fatchars.h /usr/include/fatchars
clean:
	rm fatchars.o libfatchars.so example
uninstall:
	rm -r /usr/include/fatchars /usr/lib/libfatchars.so
example:
	gcc -o example example.c -lfatchars
