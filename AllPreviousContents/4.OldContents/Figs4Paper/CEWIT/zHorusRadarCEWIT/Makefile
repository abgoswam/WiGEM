LIBSRCS  = scenario location accesspoint bssid training observation stat mlhood bayes markov pvector topology
INCLUDES = eval.h
TARGETS  = localize postprocess
CFLAGS   = -O2

LIBOBJS  = $(addprefix librwl/, $(addsuffix .o, $(LIBSRCS)))

default: all

%.o: %.cc $(addprefix include/, $(INCLUDES))
	g++ $(CFLAGS) -Iinclude $< -c -o $@

librwl/librwl.a: $(LIBOBJS)
	ar cru $@ $+

postprocess: src/postprocess.o librwl/librwl.a
	g++ $< -Llibrwl/ -lm -lrwl -o $@

localize: src/localize.o librwl/librwl.a
	g++ $< -Llibrwl/ -lm -lrwl -o $@

all: $(TARGETS)

clean::
	rm -f $(LIBOBJS) $(addsuffix .o, $(TARGETS)) librwl/librwl.a $(TARGETS) frontend/*.class src/*.o

