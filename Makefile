OBJS = sock.o bit-array.o

sock: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJS)

.PHONY: sock clean
