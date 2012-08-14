CC    = g++
CWARN = -W -Wall -Wshadow -Wreturn-type -Wformat -Wparentheses -Wpointer-arith -Wuninitialized -O -g

TGT   = ircbot
TGTFLAGS = -rdynamic -ldl

OBJS  = obj/main.o

LIBIRC  = libirc.so.1.0
IRCOBJS = obj/irc/command.o \
          obj/irc/server.o
IRCFLAGS = -shared -Wl,-soname,libirc.so.1

LIBBOT  = libbot.so.1.0
BOTOBJS = obj/bot/handlers.o
BOTFLAGS = -shared -Wl,-soname,libbot.so.1



all: $(LIBIRC) $(LIBBOT) $(TGT)

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

$(LIBBOT): $(BOTOBJS)
	# Compiling libbot.a
	$(CC) $(CWARN) $(BOTFLAGS) -o $@ $(BOTOBJS)
	# Linking the libbot
	mv $(LIBBOT) /opt/lib/
	ln -sf /opt/lib/$(LIBBOT) /opt/lib/libbot.so
	ln -sf /opt/lib/$(LIBBOT) /opt/lib/libbot.so.1

obj/%.o: src/%.cpp
	$(CC) $(CWARN) -c -o $@ $? -fPIC
	
clean:
	rm -rf $(TGT) $(LIBIRC) $(LIBBOT) $(OBJS)
