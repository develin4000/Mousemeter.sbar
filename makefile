#->====================================================<-
#->= Mousemeter.sbar - © Copyright 2013-2020 OnyxSoft =<-
#->====================================================<-
#->= Version  : 1.2                                   =<-
#->= File     : Makefile                              =<-
#->= Author   : Stefan Blixth                         =<-
#->= Compiled : 2020-11-18                            =<-
#->====================================================<-

CC = ppc-morphos-gcc-9
STRIP = ppc-morphos-strip

SRC_LOCALE = Mousemeter.sbar_locale.c
OBJ_LOCALE = Mousemeter.sbar_locale.o
OBJS = Mousemeter.sbar.o
CFLAGS = -D__AMIGADATE__=\"$(shell date "+%d.%m.%y")\" -noixemul 

all: Mousemeter.sbar_locale Mousemeter.sbar

Mousemeter.sbar_locale: $(OBJ_LOCALE)
	@echo "Making $@..."
	@$(CC) $(CFLAGS) $(SRC_LOCALE) -c -o $(OBJ_LOCALE)

Mousemeter.sbar: $(OBJS) $(OBJ_LOCALE)
	@echo "Making $@..."
	@$(CC) $(CFLAGS) $(OBJ_LOCALE) -nostdlib $(OBJS) -o $@.db -lm -labox -laboxstubs -lc
	@$(STRIP) --strip-unneeded --remove-section=.comment $@.db -o $@
	@chmod a+x $@

clean:
	rm -f *.o *.db Mousemeter.sbar
