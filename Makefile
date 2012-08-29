CC    = g++
CWARN = -g -W -Wall -Wshadow -Wreturn-type -Wformat -Wparentheses -Wpointer-arith -Wuninitialized

TGT      = ircbot
TGTFLAGS = -rdynamic -ldl -lsqlite3

OBJS  = obj/extension_manager.o \
        obj/irc/user.o \
	obj/irc/command.o \
        obj/irc/server.o \
        obj/funcs.o \
        obj/database.o \
	obj/main.o

DIRS  = obj \
        obj/irc \
        lib \
	extensions


all: $(DIRS) $(TGT)

$(TGT): $(OBJS)
	$(CC) $(CWARN) -o $@ $(OBJS) $(TGTFLAGS)
	# If it's your first time compiling this,
	# compile the actual bot with command:
	# ./manage.sh build bot
	# Same works for building other extensions too.

obj/%.o: src/%.cpp
	$(CC) -fPIC $(CWARN) -c -o $@ $?

$(DIRS):
	mkdir -p $(DIRS)
	
clean:
	rm -rf $(TGT) $(OBJS) $(DIRS)
