CC    = g++
CWARN = -W -Wall -Wshadow -Wreturn-type -Wformat -Wparentheses -Wpointer-arith -Wuninitialized -O -g

TGT   = bot

OBJS  = obj/irc/command.o \
        obj/irc/server.o \
	obj/main.o


all: $(TGT)

$(TGT): $(OBJS)
	$(CC) $(CWARN) -o $@ $(OBJS)

obj/%.o: src/%.cpp
	$(CC) $(CWARN) -c -o $@ $?
	
clean:
	rm -rf $(TGT) $(OBJS)
