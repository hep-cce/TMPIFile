OBJS_LIB = $(shell ls src/*.cxx | sed 's/\.cxx/.o/')
PROGS = $(shell ls test/*.C | sed 's/\.C//' | sed 's/test\///')
CC=CC
CXXFLAGS = `root-config --cflags` -fPIC -g -DLINUX
MPDIR = #-L $(MPLIBS) #******path to MPICH LIBRARIES******#
MPINCLUDEPATH = $(MPICH_DIR)/include #********PATH TO MPICH HEADERS*******#
INCLUDES = -I./src -I$(MPINCLUDEPATH) -I$(shell root-config --incdir)
MPLIB = #-lmpich -lmpi -lmpicxx -lmpl -lopa #*********MPICH LIBRARIES********#
ROOTLIBS = $(shell root-config --libs) -lEG -dynamic
COPTS = -fPIC -DLINUX -O0 -g $(shell root-config --cflags) #********m64 or m32 bit(?)*********#
INCLUDE = $(shell ls src/TMPIFile.h) 
INCLUDE += $(shell ls src/TClientInfo.h)
INCLUDE += $(shell ls src/JetEvent.h)
MPINCLUDES = $(shell ls $(MPINCLUDEPATH)/*.h)
all: lib programs

lib: libTMPI.so

libTMPI.so: MPIDict.o $(OBJS_LIB)
	if [ ! -d lib ]; then mkdir -p lib; fi

	$(CC) -shared -m64 -o lib/$@ $^


programs: $(PROGS)
	echo making $(PROGS)


$(PROGS): % : test/%.o MPIDict.o  $(OBJS_LIB) libTMPI.so
	@echo Generating $@
	echo obj_progs $(OBJS_LIB)
	if [ ! -d bin ]; then mkdir -p bin; fi
	$(CC) -Wall -m64 -o bin/$@ $< $(ROOTLIBS) $(MPDIR) $(MPLIB) -L$(CURDIR)/lib -lTMPI

%.o: %.cxx
	@echo Generating $@
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<

%.o: %.C
	@echo Generating $@
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<

MPIDict.cxx: $(INCLUDE) src/Linkdef.h
	@echo "Generating MPI Dictionary..."
	@rootcint -f $@ -c -p -I$(MPINCLUDEPATH) $^

clean: 
	-rm src/*.o;
	-rm src/*~
	#-rm include/*~
	-rm *~
	rm -rf lib;
	rm -rf bin;
	-rm *.cxx;
	-rm *.pcm;
	-rm *.o
