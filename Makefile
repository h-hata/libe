T=hsvr
CFLAGS=-Wall -Wconversion -ggdb -DHTTP
OBJ= main.o storage.o $(T).o
$(T): $(OBJ)
	gcc -o $(T) $(OBJ) -lpthread -lssl -lcrypto -levent -levent_openssl -levent_pthreads
.c.o:
	gcc $(CFLAGS)  -c $< -ggdb
#	gcc $(CFLAGS) -DDEBUG -c $< -ggdb
clean:
	rm -fr *.o $(T) 

