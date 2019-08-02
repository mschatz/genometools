#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

#undef BIGMER
//#define BIGMER 1

#ifdef BIGMER
// BIGMER is a bitvector of arbitrary length
typedef vector<bool> Mer_t;
typedef map<Mer_t, unsigned int> MerTable_t;
#else
// Store shorter mers in a binary within a 64 bit long long
typedef long long unsigned Mer_t;
typedef unordered_multimap<Mer_t, unsigned int, hash<unsigned long> > MerTable_t;
#endif

Mer_t Forward_Mask;
int KmerLen = 22;
int BAD_CHAR = 0;
int KMER_BUFFER_LEN = 128;

unsigned CharToBinary (char ch);
void ForwardAddCh (Mer_t & mer, char ch);
void ReverseAddCh (Mer_t & mer, char ch);
void MerToAscii(Mer_t mer, string & s);
void CountMers (const string & s, MerTable_t & mer_table);
void PrintMers(const MerTable_t & mer_table, int min_count);
Mer_t AsciiToMer(char * mer, int maskpos);

int main (int argc, char * argv [])
{
  if (argc < 3)
  {
    cerr << "Usage: sibling-kmers k kmer_counts.txt [max_kmers] [-v]" << endl 
         << endl
         << "Scans the kmer frequencies in kmer_counts.txt to find sibling pairs that " << endl
         << "differ by at most 1 nucleotide. Reports the counts for such pairs as" << endl
         << "lowcnt\thighcnt, also considering multiple pairs for a kmer. To save RAM" << endl
         << "only the first max_kmers are loaded into RAM. Adding -v make" << endl
         << "the program more verbose" << endl 
         << endl
         << " k:               length of kmer" << endl
         << " kmer_counts.txt: file of kmer+kmer_count values" << endl
         << " max_kmers:       limit the number of kmers to store" << endl
         << " -v:              Be more verbose" << endl << endl;

    #ifndef BIGMER
      cerr << "  Max Kmer length allowed: " << sizeof(Mer_t)*8/2 << endl;
    #else 
      cerr << "  Max Kmer length allowed: " << KMER_BUFFER_LEN << endl;
    #endif
    return 0;
  }

  // Subtract 1 since we are always masking a base
  KmerLen = atoi(argv[1]) - 1; 
  string kmerfile = argv[2];

  int maxKmersToLoad = INT_MAX;
  int DEBUG = 0;

  if (argc > 3) { maxKmersToLoad = atoi(argv[3]); }
  if (argc > 4) { DEBUG = 1; }

  #ifndef BIGMER
  if (KmerLen > 32 || KmerLen < 1)
  {
    cerr << "Kmer length must be <= 32" << endl;
    return 1;
  }
  Forward_Mask = ((long long unsigned) 1 << (2 * KmerLen - 2)) - 1;
  #else
  if (KmerLen > 128 || KmerLen < 1)
  {
    cerr << "Kmer length must be <= 128" << endl;
    return 1;
  }
  #endif

  vector<MerTable_t> mer_table;
  mer_table.resize(KmerLen+1); // masking each possible position

  cerr << "Processing kmer counts in " << kmerfile << " storing at most " << maxKmersToLoad << " kmers ... " << endl;

  FILE * fp = fopen(kmerfile.c_str(), "r");

  if (!fp)
  {
    cerr << "Couldn't open " << kmerfile << endl;
    exit(1);
  }

  long long kmers_read = 0;
  long long hash_inserts = 0;
  long long pairs_found = 0;

  char kmerstr[KMER_BUFFER_LEN];
  unsigned int count;
  bool insert_mers = true;

  while (fscanf(fp, "%s %d", kmerstr, &count) == 2)
  {
    kmers_read++;
    if (kmers_read > maxKmersToLoad) { insert_mers = false; }

    if (kmers_read % 1000000 == 0) {
      cerr << "... kmers_read: " << kmers_read << " kmer: " << kmerstr << " " << count << endl;
    }

    for (int maskpos = 0; maskpos <= KmerLen; maskpos++)
    {
      Mer_t mer = AsciiToMer(kmerstr, maskpos);

      pair<MerTable_t::const_iterator,MerTable_t::const_iterator> range = mer_table[maskpos].equal_range(mer);
      for (MerTable_t::const_iterator it = range.first; it != range.second; ++it) 
      {
        pairs_found++;
        unsigned int paircnt = it->second;
        unsigned int smallcnt = count;
        unsigned int bigcnt   = paircnt;

        if (bigcnt < smallcnt)
        {
          bigcnt = count;
          smallcnt = paircnt;
        }

        if (DEBUG) { cerr << "pair: " << kmerstr << " " << count << " " << paircnt << endl; }
        printf("%d\t%d\n", smallcnt, bigcnt);
      }

      if (insert_mers)
      {
        hash_inserts++;
        mer_table[maskpos].insert(make_pair(mer, count));
      }
    }
  }

  cerr << "Processed: " << kmers_read << " hash_inserts: " << hash_inserts << " pairs_found: " << pairs_found << endl;

  if (DEBUG)
  {
    for (int maskpos = 0; maskpos <= KmerLen; maskpos++)
    {
      cerr << "entries for maskpos " << maskpos << ": " << mer_table[maskpos].size() << endl;
      PrintMers(mer_table[maskpos], 0);
    }
  }
}

const char * bintoascii = "ACGT";

//  Return the binary equivalent of  ch .
unsigned CharToBinary (char ch)
{
  switch  (tolower (ch))
  {
    case  'a' : return  0;
    case  'c' : return  1;
    case  'g' : return  2;
    case  't' : return  3;
    default: BAD_CHAR++; return 0;
  };

  return  0;
}


#ifdef BIGMER

// BIGMER is a bit vector, initialize as all zeros
void InitMer(Mer_t & mer)
{
  mer.clear();
  mer.resize(KmerLen*2);
}

// Returns an ascii string of nucleotides
void MerToAscii(Mer_t mer, string & s)
{
  s.erase();
  s.resize(KmerLen);

  for (int i = 0; i < KmerLen; i++)
  {
    char m = 0;
    m |= mer[i*2+1] << 1;
    m |= mer[i*2];

    s[KmerLen-i-1] = bintoascii[m];
  }
}

// Add  ch  to  mer  on the right, sliding one character
// off the left end of  mer .
void ForwardAddCh(Mer_t & mer, char ch)
{
  // shift left
  for (int i = KmerLen * 2-1; i > 1; i--)
  {
    mer[i] = mer[i-2];
  }

  // append to end
  ch = CharToBinary(ch);
  mer[1] = ch & 2;
  mer[0] = ch & 1;
}


// Add the Watson-Crick complement of ch to mer on the left,
// sliding one character off the right end of mer .
void ReverseAddCh(Mer_t & mer, char ch)
{
  // shift right
  for (int i = 0; i < KmerLen*2-2; i++)
  {
    mer[i] = mer[i+2];
  }

  // append to front
  ch = 3^CharToBinary(ch);
  mer[KmerLen*2-1] = ch&2;
  mer[KmerLen*2-2] = ch&1;
}


#else

// Initialze the mer (effective all A's)
void InitMer(Mer_t & mer)
{
  mer = 0;
}

// converts a Mer_t to an ascii string of nucleotides
void MerToAscii(Mer_t mer, string & s)
{
  s.erase();
  s.resize(KmerLen);

  for (int i = 0; i < KmerLen; i++)
  {
    s[KmerLen-i-1] = bintoascii[mer & 0x3];
    mer >>= 2;
  }
}

// Add ch to mer on the right, sliding one character
// off the left end of mer if needed
void ForwardAddCh(Mer_t & mer, char ch)
{
  mer &= Forward_Mask;
  mer <<= 2;
  mer |= CharToBinary(ch);
}


// Add the Watson-Crick complement of ch to mer on the left,
// sliding one character off the right end of mer if needed
void ReverseAddCh(Mer_t & mer, char ch)
{
  mer >>= 2;
  mer |= ((long long unsigned) (3 ^ CharToBinary(ch)) << (2 * KmerLen - 2));
}

#endif


// Print out a hash table of (masked) mers and their counts
void PrintMers(const MerTable_t & mer_table, int min_count)
{
  cerr << mer_table.size() << " total distinct mers" << endl;
  string mer;
  int printed = 0;
  int skip = 0;

  MerTable_t::const_iterator fi;
  for (fi = mer_table.begin(); fi != mer_table.end(); fi++)
  {
    if (fi->second >= min_count)
    {
      MerToAscii(fi->first, mer);
      printf("%s\t%d\n", mer.c_str(), fi->second);
    }
    else
    {
      skip++;
    }
  }

  cerr << printed << " mers occur at least " << min_count << " times" << endl;
  cerr << "Skipped " << skip << endl;
}

// Converts a ascii nucleotide string to a canonically Mer_t by considering the fwd and rev mers
Mer_t AsciiToMer(char * mer, int maskpos)
{
  Mer_t fwd_mer;
  Mer_t rev_mer;

  InitMer(fwd_mer);
  InitMer(rev_mer);

  int l = strlen(mer);
  for (int i = 0; i < l; i++)
  {
    if (i != maskpos)
    {
      ForwardAddCh(fwd_mer, mer[i]);
      ReverseAddCh(rev_mer, mer[i]);
    }
  }

  return (fwd_mer < rev_mer) ? fwd_mer : rev_mer;
}
