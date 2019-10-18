CC      = gcc
C_FLAGS = -Wall -g 
L_FLAGS = -lz -lcrypt

O_FILES = socket.o io.o strings.o utils.o interpret.o help.o list.o \
	  action_safe.o save.o event.o event-handler.o stack.o \
	  telopt.c msdp.o mth.o color.o

all: $(O_FILES)
	rm -f SocketMud
	$(CC) -o SocketMud $(O_FILES) $(L_FLAGS)

.c.o: all
	@echo [`date +%T`] Compiling $< ...
	@$(CC) -c $(C_FLAGS) $<

clean:
	@echo Cleaning code $< ...
	@rm -f *.o
	@rm -f SocketMud
	@rm -f *.*~
