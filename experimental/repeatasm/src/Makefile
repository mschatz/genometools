CXX := g++
CXXFLAGS := -Wno-deprecated -Wall -g
# -Wl,-rpath,$(BAMTOOLS_LIB_DIR)

all: repeatasm simple-repeat-finder

repeatasm: repeatasm.cc repeatasm.hh align.cc util.cc Mer.hh Ref.hh ReadInfo.hh ReadStart.hh Transcript.hh Edge.cc Edge.hh ContigLink.hh Node.cc Node.hh Path.cc Path.hh ContigLink.cc Graph.cc Graph.hh
	$(CXX) $(CXXFLAGS) $(INCLUDES) repeatasm.cc Edge.cc Node.cc Graph.cc Path.cc ContigLink.cc align.cc util.cc -o repeatasm 

simple-repeat-finder: simple-repeat-finder.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) simple-repeat-finder.cc -o simple-repeat-finder


clean:
	rm -f repeatasm simple-repeat-finder
