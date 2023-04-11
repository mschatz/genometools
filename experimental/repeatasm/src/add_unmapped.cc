#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "api/BamReader.h"
#include "api/BamWriter.h"

using namespace std;
using namespace BamTools;


void AddUnmappedReads(string in, string out) {
	int num_10 = 0;
	int num_01 = 0;
	int num_00 = 0;
	// at some point, start our merge operation
	string inputFilenames = in;
	string outputFilename = out;
	// provide some input & output filenames
	// attempt to open our BamMultiReader
	BamReader reader;
	if ( !reader.Open(inputFilenames) ) {
		cerr << "Could not open input BAM files." << endl;
		return;
	}
	// retrieve 'metadata' from BAM files, these are required by BamWriter
	const SamHeader header = reader.GetHeader();
	const RefVector references = reader.GetReferenceData();
	// attempt to open our BamWriter
	BamWriter writer;
	if ( !writer.Open(outputFilename, header, references) ) {
		cerr << "Could not open output BAM file" << endl;
		return;
	}
	// iterate through all alignments
	BamAlignment R1;
	BamAlignment R2;
	while ( reader.GetNextAlignment(R1) ) {
		reader.GetNextAlignment(R2);
		if( R1.IsMapped() && !(R2.IsMapped()) ) {
			// add sequence and quality information of the R2 to R1
			R1.AddTag("R2", "Z", R2.QueryBases);
			R1.AddTag("Q2", "Z", R2.Qualities);
			num_10++;
		}
		else if( !(R1.IsMapped()) && R2.IsMapped() ) {
			// add sequence and quality information of the R1 to R2
			R2.AddTag("R2", "Z", R1.QueryBases);
			R2.AddTag("Q2", "Z", R1.Qualities);
			num_01++;
		}
		else if( !(R1.IsMapped()) && !(R2.IsMateMapped()) ) {
			// add sequence and quality information of the R1 to R2 and viceversa
			R1.AddTag("R2", "Z", R2.QueryBases);
			R1.AddTag("Q2", "Z", R2.Qualities);
			R2.AddTag("R2", "Z", R1.QueryBases);
			R2.AddTag("Q2", "Z", R1.Qualities);
			num_00++;
		}
		// save read alignment to output bam file
		writer.SaveAlignment(R1);
		writer.SaveAlignment(R2);
	}
	// close the reader & writer
	reader.Close();
	writer.Close();
	
	cout << "Number of mapped reads whose mate is unmapped. : " << num_10 << endl;
	cout << "Number of unmapped reads whose mate is mapped. : " << num_01 << endl;	
	cout << "Number of unmapped reads whose mate is also unmapped. : " << num_00 << endl;
}

int main(int argc, char** argv) {

	string USAGE = " Usage: BT-test -m bamfile -o outfile \n";
	
	string INFILE;
	string OUTFILE;
 
	if (argc == 1)
	{
		cerr << USAGE;
		exit(0);
	}
	
	stringstream helptext;
	helptext <<
	USAGE
	<<
	"\n"
	"   -m <bamfile>  : bamfile with alignments to reference\n"
	"   -o <outfile>  : output file\n"
	"\n";

	optarg = NULL;

	bool errflg = false;
	int ch;
	while (!errflg && ((ch = getopt (argc, argv, "m:o:h")) != EOF))
	{
		switch (ch)
		{
			case 'm': INFILE          = optarg;       break; 
			case 'o': OUTFILE         = optarg;       break;
	      	case 'h': errflg 		  = true;         break;

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

	AddUnmappedReads(INFILE, OUTFILE);
}
