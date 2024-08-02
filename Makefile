CC=		egcc -Wno-discarded-qualifiers -Wno-incompatible-pointer-types-discards-qualifiers -Wno-unknown-warning-option
CFLAGS+=	-I/usr/local/include
LDFLAGS+=	-L/usr/local/lib -lssh2 -pthread


all: ssh-brute-forcer

ssh-brute-forcer: ssh-brute-forcer.o
	$(CC) ssh-brute-forcer.o $(LDFLAGS) -o ssh-brute-forcer

ssh-brute-forcer.o: ssh-brute-forcer.c

clean:
	rm *.o ssh-brute-forcer
