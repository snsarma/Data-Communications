#
# Created by gmakemake (Ubuntu Sep  7 2011) on Sun May  5 22:42:46 2013
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s .S
.c.o:
		$(COMPILE.c) $<
.C.o:
		$(COMPILE.cc) $<
.cpp.o:
		$(COMPILE.cc) $<
.S.s:
		$(CPP) -o $*.s $<
.s.o:
		$(COMPILE.cc) $<
.c.a:
		$(COMPILE.c) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

CC =		gcc
CXX =		g++

RM = rm -f
AR = ar
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
CPP = $(CPP) $(CPPFLAGS)
########## Flags from header.mak

##############################
#### header.mak for LINUX ####
##############################
CCFLAGS =	
CFLAGS =	-g
CCLIBFLAGS =	-Xlinker -zmuldefs -lpthread

########## End of flags from header.mak


CPP_FILES =	Timer.C udp-client.C udp-server.C
C_FILES =	
PS_FILES =	
S_FILES =	
H_FILES =	Timer.h datagram.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	Timer.o 

#
# Main targets
#

all:	udp-client udp-server 

udp-client:	udp-client.o $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o udp-client udp-client.o $(OBJFILES) $(CCLIBFLAGS)

udp-server:	udp-server.o $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o udp-server udp-server.o $(OBJFILES) $(CCLIBFLAGS)

#
# Dependencies
#

Timer.o:	Timer.h
udp-client.o:	Timer.h datagram.h
udp-server.o:	

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm $(OBJFILES) udp-client.o udp-server.o core 2> /dev/null

realclean:        clean
	-/bin/rm -rf udp-client udp-server 
