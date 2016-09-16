EXENAME = libyuv.a

CC = arm-linux-gcc
AR= arm-linux-ar rc
CFLAGS =
INCLUDE = -I/home/qte/jpeg-6b-arm 
LIBS = 
DEP_LIBS = 
#libjpeg.a 
HEADER =
OBJS = yuv2rgb.o jpeglib.o jpegutils.o

all: $(EXENAME)
	cp $(EXENAME)  ../bin

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

$(EXENAME): $(OBJS)
#	$(CC) -o $(EXENAME) $(OBJS) $(LIBS) $(DEP_LIBS)
	$(AR) $(EXENAME) $(OBJS) $(LIBS) $(DEP_LIBS) 

clean:
	rm -f *.o a.out $(EXENAME) core *~
