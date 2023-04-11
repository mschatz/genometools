#include "repeatasm.hh"
#include <cstdlib>
#include <unistd.h>

// printConfiguration
/////////////////////////////////////////////////////////////////////////

void RepeatAsm::printConfiguration(ostream & out)
{
  out << "VERBOSE: "           << VERBOSE << endl;
  out << "kmer_fa: "           << KMER_FA  << endl;
  out << "kmer_len: "          << minK << endl;
  out << "MAX_TIP_LEN: "       << MAX_TIP_LEN << endl;
  out << "COV_THRESHOLD: "     << COV_THRESHOLD << endl;
  out << "TIP_COV_THRESHOLD: " << TIP_COV_THRESHOLD << endl;
  out << "DFS_LIMIT: "         << DFS_LIMIT << endl;
  out << "PATH_LIMIT: "        << PATH_LIMIT << endl;
  out << "MAX_INDEL_LEN: "     << MAX_INDEL_LEN << endl;
  out << "MAX_MISMATCH: "      << MAX_MISMATCH << endl;
  out << "PRINT_ALL: "         << bvalue(PRINT_ALL) << endl;
  out << "NODE_STRLEN: "       << NODE_STRLEN << endl;
}


// fastqAsm
//////////////////////////////////////////////////////////////////////////

void RepeatAsm::fastqAsm(Graph_t & g, const string & prefix)
{
  string out_prefix = prefix + ".ra";

  cerr << endl << endl;
  cerr << "fastqAsm: " << out_prefix << endl;
  cerr << "=====================================================" << endl;
  
  g.printStats();
  	
  if (PRINT_ALL) 
  { 
    g.printDot(out_prefix + ".0.dot"); 
    g.printFasta(out_prefix + ".0.fa"); 
    cerr << endl;
  }
  
  // Initial compression
  g.compress(); 
  g.printStats();
  
  if (PRINT_ALL) 
  {
    g.printDot(out_prefix + ".1c.dot");
    g.printFasta(out_prefix + ".1c.fa");
  }
  
  // Remove low coverage
  g.removeLowCov();
  if (PRINT_ALL) 
  { 
    g.printDot(out_prefix + ".2l.dot"); 
    g.printFasta(out_prefix + ".2l.fa"); 
  }
  
  // Remove tips
  g.removeTips();
  
  g.printDot(out_prefix + ".final.dot");
  g.printFasta(out_prefix + ".final.fa");
  g.printPairs(out_prefix + ".pairs.fa");
  g.clear(true);
  
  cerr << endl;
}

// greedyAssemble
//////////////////////////////////////////////////////////////////////////

void RepeatAsm::greedyAssemble(Graph_t & g, const string & prefix)
{
  cerr << "Greedy assemble" << endl;
  cerr << "=====================================================" << endl;
  
  string refname = "fastq";
  string out_prefix = prefix + ".greedy";
  
  g.printStats();
  
  // Initial compression
  g.compress();
  g.printStats();
  //g.printFasta(out_prefix + ".raw.fa");
  
  // remove tips
  g.removeTips();
  g.printStats();
  
  // greedy trim
  g.greedyTrim();
  g.printStats();
  
  // print final results
  g.printFasta(out_prefix + ".fa");
  g.printDot(out_prefix + ".final.dot");
}

int RepeatAsm::run(int argc, char** argv)
{
  string USAGE = " Usage: repeatasm [options] -k <kmerlen> -j <kmers.fa>\n";
  
  if (argc == 1)
  {
  	cerr << USAGE;
  	exit(0);
  }
  
  cerr.setf(ios::fixed,ios::floatfield);
  cerr.precision(1);
  
  stringstream helptext;
  helptext << USAGE <<
  	"\n"
  	"   -j <kmers.fa> : file of kmers to assemble, using kmer frequency as sequence id\n"
  	"   -k <kmersize> : kmersize (default: " << minK << ")\n"
      "\n"
  	"   -l <tiplen>   : max tip length (default: " << MAX_TIP_LEN << ")\n"
  	"   -c <cov>      : coverage threshold (default: " << COV_THRESHOLD << ")\n"
  	"   -d <tipcov>   : tip coverage threshold (default: " << TIP_COV_THRESHOLD << ")\n"
  	"   -F <dfs>      : limit dfs search space (default: " << DFS_LIMIT << ")\n"
  	"   -P <maxp>     : limit on number of paths to report (default: " << PATH_LIMIT << ")\n"
  	"   -T <maxindel> : limit on size of detectable indel (default: " << MAX_INDEL_LEN << ")\n"
  	"   -M <max-mismatch> : max number of mismatches for near-perfect repeats (default: " << MAX_MISMATCH << ")\n"
  	"\n"
  	"   -A            : print graph after every stage\n"
  	"   -L <len>      : length of sequence to display at graph node (default: " << NODE_STRLEN << ")\n"
  	"\n"
  	"   -v            : be verbose\n"
  	"\n";
  
  bool errflg = false;
  int ch;
  
  optarg = NULL;
  
  while (!errflg && ((ch = getopt (argc, argv, "j:k:;:c:d:L:F:P:T:M:hvAI")) != EOF))
  {
  	switch (ch)
  	{
  		case 'j': KMER_FA          = optarg;       break; 
  
  		case 'k': minK             = atoi(optarg); break;
  		case 'l': MAX_TIP_LEN      = atoi(optarg); break;
  		case 'c': COV_THRESHOLD    = atoi(optarg); break;
  		case 'd': TIP_COV_THRESHOLD= atoi(optarg); break;
  
  		case 'v': VERBOSE          = 1;            break;
  		case 'A': PRINT_ALL        = 1;            break;
  		case 'L': NODE_STRLEN      = atoi(optarg); break;
  		case 'F': DFS_LIMIT        = atoi(optarg); break;
  		case 'P': PATH_LIMIT       = atoi(optarg); break;
  		case 'T': MAX_INDEL_LEN    = atoi(optarg); break;
  		case 'M': MAX_MISMATCH     = atoi(optarg); break;
  
  		case 'h': errflg = 1;                      break;
  
  		case '?':
  		fprintf (stderr, "Unrecognized option -%c\n", optopt);
  
  		default:
  		errflg = true;
  	}
  
  	if (errflg)
  	{
  		cout << helptext.str();
  		exit (EXIT_FAILURE);
  	}
  }
  
  if (KMER_FA == "") { cerr << "ERROR: Must provide a kmer fasta file (-j)" << endl; errflg++; }
  if (errflg) { exit(EXIT_FAILURE); }
  
  if (VERBOSE) { printConfiguration(cerr); }
  
  //set configuration parameters
  Graph_t g;
  g.setK(minK);
  g.setVerbose(VERBOSE);
  g.setBufferSize(BUFFER_SIZE);
  g.setDFSLimit(DFS_LIMIT);
  g.setPathLimit(PATH_LIMIT);
  g.setCovThreshold(COV_THRESHOLD);
  g.setTipCovThreshold(TIP_COV_THRESHOLD);
  g.setPrintDotReads(PRINT_DOT_READS);
  g.setNodeStrlen(NODE_STRLEN);
  g.setMaxTipLength(MAX_TIP_LEN);
  g.setMaxIndelLen(MAX_INDEL_LEN);
  g.setMaxMismatch(MAX_MISMATCH);
  
  char readname [BUFFER_SIZE];
  char seq [BUFFER_SIZE];
  int readcount = 0;
  
  // Process the reads
  FILE * fp = NULL;
  cerr << "loading raw kmers... " << KMER_FA << endl;
  fp = xfopen(KMER_FA.c_str(), "r");
  
  while (fscanf(fp, "%s", readname) == 1) // Assume readname is: ">kmercov"
  {
    readcount++;
    fscanf(fp, "%s", seq); // now get the kmer sequence
    g.addSeqCov(readcount, seq, atoi(readname+1));
  }
  
  cerr << readcount << " reads loaded" << endl;
  fastqAsm(g, KMER_FA);

  return 0;
}

// main
//////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv)
{
	try {
		RepeatAsm* assembler = new RepeatAsm();
		assembler->run(argc, argv);
	}
	catch (int e) {
		cout << "An exception occurred. Exception Nr. " << e << endl;
	} 
	//catch(std::out_of_range& e) {
   	//	cerr << e.what( ) << endl;
 	//}
	//catch (...) { cout << "default exception"; }
}
