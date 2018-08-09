OBJS_LIB = $(shell ls src/*.cxx | sed 's/\.cxx/.o/')
PROGS = $(shell ls src/*.C | sed 's/\.C//' | sed 's/src\///')
CC = mpicxx
CXXFLAGS = `root-config --cflags` -fPIC -g -DLINUX
MPDIR = $(MPLIBS) #******path to MPICH LIBRARIES******#
MPINCLUDEPATH = $(MPIINCLUDES) #********PATH TO MPICH HEADERS*******#
INCLUDES = -I./include -I$(MPINCLUDEPATH) -I$(shell root-config --incdir)
MPLIB = -lmpich -lmpi -lmpicxx -lmpl -lopa #*********MPICH LIBRARIES********#
ROOTLIBS = $(shell root-config --libs) -lEG -dynamic
COPTS = -fPIC -DLINUX -O0 -g $(shell root-config --cflags) #********m64 or m32 bit(?)*********#
INCLUDE = $(shell ls include/TMPIFile.h) 
INCLUDE += $(shell ls include/TClientInfo.h)
MPINCLUDES = $(shell ls $(MPINCLUDEPATH)/*.h)
all: lib programs

lib: libTMPI.so

libTMPI.so: MPIDict.o $(OBJS_LIB)
	if [ ! -d lib ]; then mkdir -p lib; fi

	$(CC) -shared -m64 -o lib/$@ $^


programs: $(PROGS)
	echo making $(PROGS)


$(PROGS): % : src/%.o MPIDict.o  $(OBJS_LIB) libTMPI.so
	echo obj_progs $(OBJS_LIB)
	if [ ! -d bin ]; then mkdir -p bin; fi
	$(CC) -Wall -m64 -o bin/$@ $< $(ROOTLIBS) -L$(MPDIR) $(MPLIB) -L$(CURDIR)/lib -lTMPI

%.o: %.cxx
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<

%.o: %.C
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<

MPIDict.cxx: $(INCLUDE) include/Linkdef.h
	@echo "Generating MPI Dictionary..."
	@rootcint -f $@ -c -p -I$(MPINCLUDEPATH) $^

clean: 
	-rm src/*.o;
	-rm src/*~
	-rm include/*~
	-rm *~
	rm -rf lib;
	rm -rf bin;
	-rm *.cxx;
	-rm *.pcm;
	-rm *.o
