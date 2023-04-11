#ifndef REPEATASM_HH
#define REPEATASM_HH 1

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <assert.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "align.hh"
#include "util.hh"
#include "Mer.hh"
#include "Ref.hh"
#include "ReadInfo.hh"
#include "ReadStart.hh"
#include "Transcript.hh"
#include "Edge.hh"
#include "ContigLink.hh"
#include "Node.hh"
#include "Path.hh"
#include "Graph.hh"

using namespace std;
using namespace HASHMAP;

#define bvalue(value) ((value ? "true" : "false"))

// constants
//////////////////////////////////////////////////////////////////////////

const char Graph_t::CODE_MAPPED = 'M';
const char Graph_t::CODE_BASTARD = 'B';

const string Graph_t::COLOR_ALL    = "darkorange3"; //"black";
const string Graph_t::COLOR_LOW    = "deepskyblue4"; //"grey85";
const string Graph_t::COLOR_NOVO   = "red";
const string Graph_t::COLOR_SOURCE = "green\" style=\"filled";
const string Graph_t::COLOR_SINK   = "yellow\" style=\"filled";
const string Graph_t::COLOR_TOUCH  = "pink";

class RepeatAsm {

public:
	
	int BUFFER_SIZE;
	
	// Configuration
	//////////////////////////////////////////////////////////////////////////

	bool VERBOSE;
	bool PRINT_DOT_READS;
	bool PRINT_ALL;

	int MIN_QUAL;
	int MIN_MAP_QUAL;

	string KMER_FA;

	int minK;
	int MAX_TIP_LEN;
	int COV_THRESHOLD;
	int TIP_COV_THRESHOLD;
	int NODE_STRLEN;
	int DFS_LIMIT;
	int PATH_LIMIT;
	int MAX_INDEL_LEN;
	int MAX_MISMATCH;
	
	// data structures
	//////////////////////////////////////////////////////////////////////////
	
	int graphCnt;
	
	RepeatAsm() { 
		graphCnt = 0;
		
		BUFFER_SIZE = 10*1024;

		VERBOSE         = false;
		PRINT_DOT_READS = true;
		PRINT_ALL       = false;

		minK = 10;
		MAX_TIP_LEN = minK;
		COV_THRESHOLD = 5;
		TIP_COV_THRESHOLD = 1;

		NODE_STRLEN = 1000;

		DFS_LIMIT = 1000;
		PATH_LIMIT = 0;
		MAX_INDEL_LEN = 250;
		MAX_MISMATCH = 1;
	}
		
	~RepeatAsm() { }
	
	void printConfiguration(ostream & out);	
	void processGraph(Graph_t & g, const string & refname, const string & prefix, int minK);
	void fastqAsm(Graph_t & g, const string & prefix);
    void greedyAssemble(Graph_t & g, const string & prefix);
	int run(int argc, char** argv);
};

#endif
