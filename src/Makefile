CFLAGS=-Wall -g -ggdb
OBJS = networking.o args.o bitmap.o fido.o

fido: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJS)

.PHONY: fido clean
