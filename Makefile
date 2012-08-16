CC    = g++
CWARN = -W -Wall -Wshadow -Wreturn-type -Wformat -Wparentheses -Wpointer-arith -Wuninitialized -O -g

TGT      = ircbot
TGTFLAGS = -rdynamic -ldl

OBJS  = obj/extension_manager.o \
	obj/irc/command.o \
        obj/irc/server.o \
	obj/main.o


all: $(TGT)

$(TGT): $(OBJS)
	$(CC) $(CWARN) -o $@ $(OBJS) $(TGTFLAGS)

obj/%.o: src/%.cpp
	$(CC) $(CWARN) -c -o $@ $? -fPIC
	
clean:
	rm -rf $(TGT) $(OBJS)
