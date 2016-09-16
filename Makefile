EXENAME = /tftpboot/sound70

CC = /usr/local/arm/2.95.3/bin/arm-linux-gcc
CFLAGS =
INCLUDE = 
LIBS = -lpthread
DEP_LIBS = ./liba/libyuv.a ./liba/libjpeg.a ./liba/libg711.a ./liba/libimage.a
HEADER =
OBJS = sound70.o sndtools.o udp.o comm.o ratecontrol.o fmpeg4_avcodec.o interface.o gpio.o talk.o setup.o arp.o flashcp.o

all: $(EXENAME)

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

$(EXENAME): $(OBJS)
	$(CC) -o $(EXENAME) $(OBJS) $(LIBS) $(DEP_LIBS)

clean:
	rm -f *.o a.out $(EXENAME) core *~
