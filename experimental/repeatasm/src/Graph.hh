#ifndef GRAPH_HH
#define GRAPH_HH 1

#include <string>
#include <deque>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <exception>
#include <stdexcept> // out_of_range exception

#include "align.hh"
#include "util.hh"
#include "Edge.hh"
#include "Node.hh"
#include "Mer.hh"
#include "Ref.hh"
#include "ContigLink.hh"
#include "Path.hh"
#include "Transcript.hh"

#define WHITE 1
#define GREY 2
#define BLACK 3

using namespace std;

// Graph_t
//////////////////////////////////////////////////////////////////////////

typedef map<Mer_t, Node_t *> MerTable_t;

class Graph_t
{
public:

	static const char CODE_MAPPED;
	static const char CODE_BASTARD;

	static const string COLOR_ALL;
	static const string COLOR_LOW;
	static const string COLOR_NOVO;
	static const string COLOR_SOURCE;
	static const string COLOR_SINK;
	static const string COLOR_TOUCH;

	bool VERBOSE;
	bool PRINT_DOT_READS;

	int MIN_QUAL;
	int INCLUDE_BASTARDS;
	int BUFFER_SIZE;

	int DFS_LIMIT;
	int PATH_LIMIT;
	int NODE_STRLEN;

	int K;	
	int MAX_TIP_LEN;
	int MAX_INDEL_LEN;
	int MIN_THREAD_READS;
	int COV_THRESHOLD;
	int TIP_COV_THRESHOLD;
	bool SCAFFOLD_CONTIGS;
	int  INSERT_SIZE;
	int  INSERT_STDEV;
	int  MAX_MISMATCH;

	MerTable_t nodes_m;
	int totalreadbp_m;

	Node_t * source_m;
	Node_t * sink_m;

	Ref_t * ref_m;
	bool is_ref_added;

	ReadInfoList_t readid2info;
	int readCycles;

	Graph_t() : ref_m(NULL), is_ref_added(0), readCycles(0) {
		clear(true); 
	}

	void setK(int k) { K = k; }
	void setVerbose(bool v) { VERBOSE = v; }
	void setMinQual(int mq) { MIN_QUAL = mq; }
	void setIncludeBastards(int ib) { INCLUDE_BASTARDS = ib; }
	void setBufferSize(int bs) { BUFFER_SIZE = bs; }
	void setDFSLimit(int l) { DFS_LIMIT = l; }
	void setPathLimit(int l) { PATH_LIMIT = l; }
	void setCovThreshold(int ct) { COV_THRESHOLD = ct; }
	void setTipCovThreshold(int ct) { TIP_COV_THRESHOLD = ct; }
	void setPrintDotReads(bool pdr) { PRINT_DOT_READS = pdr; }
	void setNodeStrlen(int ns) { NODE_STRLEN = ns; }
	void setMaxTipLength(int mtl) { MAX_TIP_LEN = mtl; }
	void setMaxIndelLen(int mil) { MAX_INDEL_LEN = mil; }
	void setMinThreadReads(int mtr) { MIN_THREAD_READS = mtr; }
	void setScaffoldContigs(bool sc) { SCAFFOLD_CONTIGS = sc; }
	void setInsertSize(int is) { INSERT_SIZE = is; }
	void setInsertStdev(int is) { INSERT_STDEV = is; }
	void setMaxMismatch(int mm) { MAX_MISMATCH = mm; }
	bool hasRepeatsInGraphPaths() { return findRepeatsInGraphPaths(source_m, sink_m, F); }

	void clear(bool flag);
	void loadSequence(int readid, const string & seq, bool isRef, int trim5);
	void trimAndLoad(int readid, const string & seq, const string & qv, bool isRef);
	void trim(int readid, const string & seq, const string & qv, bool isRef);
	void buildgraph();

	int countBastardReads();

	int countMappedReads();

	ReadId_t addRead(const string & set, const string & readname, const string & seq, char code);

	void addMates(ReadId_t r1, ReadId_t r2);

	void printReads();

	void addPair(const string & set, const string & readname,
		const string & seq1, const string & qv1,
		const string & seq2, const string & qv2,
		char code);

	void addUnpaired(const string & set,
		const string & readname,
		const string & seq,
		const string & qv,
		char code);

    void addSeqCov(int readid, const string & seq, int cov);

	void addpaired(const string & set,
		const string & readname,
		const string & seq,
		const string & qv,
		const int mate_id,
		char code);

	void loadReadsSFA(const string & filename);
	void dfs(Node_t * source, Node_t * sink, Ori_t dir, Ref_t * ref, FILE * fp, bool printPathsToFile);
	bool findRepeatsInGraphPaths(Node_t * source, Node_t * sink, Ori_t dir);
	bool hasCycle();
	void hasCycleRec(Node_t * node, Ori_t dir, bool* ans);
	string nodeColor(Node_t * cur, string & who);
	string edgeColor(Node_t * cur, Edge_t & e);
	void printDot(const string & filename);
	void printFasta(const string & filename);
	void printPairs(const string & filename);
	void markRefEnds(Ref_t * refinfo);
	void markRefNodes();
	void denovoNodes(const string & filename, const string & refname);
	void alignRefNodes();
	void countRefPath(const string & filename, const string & refname, bool printPathsToFile);
	Node_t * getNode(Mer_t nodeid);
	Node_t * getNode(CanonicalMer_t mer);
	Node_t * getNode(Edge_t & edge);
	void compressNode(Node_t * node, Ori_t dir);
	void compress();
	void cleanDead();
	void removeNode(Node_t * node);
	void removeLowCov();
	void removeTips();
    void greedyTrim();
	void threadReads();
	void checkReadStarts();
	void updateContigReadStarts();
	void bundleMates();
	void scaffoldContigs();
	void printGraph();
	void printStats();
};

#endif
