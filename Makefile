OBJS = fido.o bit-array.o

fido: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

benchmark: benchmark.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) benchmark.o

.PHONY: fido benchmark clean
