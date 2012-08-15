CC    = g++
CWARN = -W -Wall -Wshadow -Wreturn-type -Wformat -Wparentheses -Wpointer-arith -Wuninitialized -O -g

TGT   = ircbot
TGTFLAGS = -rdynamic -ldl

OBJS  = obj/extension_manager.o \
	obj/main.o

LIBIRC  = libirc.so.1.1
IRCOBJS = obj/irc/command.o \
          obj/irc/server.o
IRCFLAGS = -shared -Wl,-soname,libirc.so.1



all: $(LIBIRC) $(TGT)

$(TGT): $(OBJS)
	# Compiling main
	$(CC) $(CWARN) -o $@ $(OBJS) $(TGTFLAGS)

$(LIBIRC): $(IRCOBJS)
	# Compiling libirc.a
	$(CC) $(CWARN) $(IRCFLAGS) -o $@ $(IRCOBJS)
	# Linking the libirc
	mv $(LIBIRC) /opt/lib/
	ln -sf /opt/lib/$(LIBIRC) /opt/lib/libirc.so
	ln -sf /opt/lib/$(LIBIRC) /opt/lib/libirc.so.1

obj/%.o: src/%.cpp
	$(CC) $(CWARN) -c -o $@ $? -fPIC
	
clean:
	rm -rf $(TGT) $(LIBIRC) $(LIBBOT) $(OBJS)
