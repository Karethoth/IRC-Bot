CC    = g++
CWARN = -W -Wall -Wshadow -Wreturn-type -Wformat -Wparentheses -Wpointer-arith -Wuninitialized -O -g

TGT   = bot
TGTFLAGS = -rdynamic -ldl

LIBIRC  = libirc.so.1.0
IRCOBJS = obj/irc/command.o \
          obj/irc/server.o
IRCFLAGS = -shared -Wl,-soname,libirc.so.1

OBJS  = obj/main.o


all: $(LIBIRC) $(TGT)

$(TGT): $(OBJS)
	# Compiling main
	$(CC) $(CWARN) -o $@ $(OBJS) $(TGTFLAGS)

$(LIBIRC): $(IRCOBJS)
	# Compiling libirc.a
	$(CC) $(CWARN) $(IRCFLAGS) -o $@ $(IRCOBJS)
	# Linking the lib 
	mv $(LIBIRC) /opt/lib/
	ln -sf /opt/lib/$(LIBIRC) /opt/lib/libirc.so
	ln -sf /opt/lib/$(LIBIRC) /opt/lib/libirc.so.1

obj/%.o: src/%.cpp
	$(CC) $(CWARN) -c -o $@ $? -fPIC
	
clean:
	rm -rf $(TGT) $(LIBIRC) $(OBJS)
