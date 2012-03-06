CFLAGS=-Wall -g -ggdb
OBJS = fido.o bit-array.o

fido: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJS)

.PHONY: fido clean
