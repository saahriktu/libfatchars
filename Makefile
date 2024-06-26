ifeq ($(prefix),)
        prefix=/usr
endif
ifeq ($(shell which $(CC)),)
        CC=gcc
endif
ifeq ($(libdir),)
        libdir=$(prefix)/lib
endif
all:
	$(CC) -c -fPIC $(CFLAGS) -o libfatchars.o fatchars.c
	$(CC) -fPIC -shared $(CFLAGS) -o libfatchars.so.1 libfatchars.o
	ln -s libfatchars.so.1 libfatchars.so
install:
	install -pDm644 libfatchars.so $(DESTDIR)$(libdir)/libfatchars.so
	install -pDm644 libfatchars.so.1 $(DESTDIR)$(libdir)/libfatchars.so.1
	install -pDm644 fatchars.h $(DESTDIR)$(prefix)/include/fatchars/fatchars.h
clean:
	rm libfatchars.o libfatchars.so libfatchars.so.1 example
example:
	$(CC) -o example example.c -lfatchars
