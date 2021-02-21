CC     = gcc
INC    = -I.
CFLAGS = $(INC) -Wall -Wextra -Werror
CFILES = hw1.c
OBJS   = $(patsubst %.c, %.o,$(CFILES))
BIN    = hw1

%.o:%.c
	$(info Compiling $<)
	@$(CC) $(CFLAGS) -o $@ -c $<

$(BIN):$(OBJS)
	$(CC) -o $@ $^

exec:$(BIN)
	./hw1

clean:
	@rm -f $(BIN) $(OBJS) *~