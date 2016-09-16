EXENAME = libg711.a

CC = arm-linux-gcc
AR= arm-linux-ar rc
CFLAGS =
INCLUDE = 
LIBS = 
DEP_LIBS = 
HEADER =
OBJS = g711core.o

all: $(EXENAME)
	cp $(EXENAME)  ../bin
.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

$(EXENAME): $(OBJS)
#	$(CC) -o $(EXENAME) $(OBJS) $(LIBS) $(DEP_LIBS)
	$(AR) $(EXENAME) $(OBJS) $(LIBS) $(DEP_LIBS) 
clean:
	rm -f *.o a.out $(EXENAME) core *~
