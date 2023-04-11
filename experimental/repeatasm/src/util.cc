#include "util.hh"
#include <iostream>
#include <cstdio>

using namespace std;


// xfopen
//////////////////////////////////////////////////////////////////////////

FILE * xfopen(const string & filename, const string & mode)
{
	if (filename == "-")
	{
		if (mode == "r") { return stdin; }
		else             { return stdout; }
	}

	FILE * fp = fopen(filename.c_str(), mode.c_str());

	if (!fp)
	{
		cerr << "Couldn't open " << filename << " for " << mode << endl;
		exit(1);
	}

	return fp;
}

// xfclose
//////////////////////////////////////////////////////////////////////////

void xfclose(FILE * fp)
{
	if (fp != stdin && fp != stdout)
	{
		fclose(fp);
	}
}


// isDNA
//////////////////////////////////////////////////////////////////////////

bool isDNA(char b)
{
	if (b == 'A' || b == 'a' ||
		b == 'C' || b == 'c' ||
		b == 'G' || b == 'g' ||
		b == 'T' || b == 't')
	{ 
		return true;
	}

	return false;
}

// rc
//////////////////////////////////////////////////////////////////////////

char rrc(char b)
{
	switch(b)
	{
		case 'A' : return 'T'; case 'a' : return 't';
		case 'C' : return 'G'; case 'c' : return 'g';
		case 'G' : return 'C'; case 'g' : return 'c';
		case 'T' : return 'A'; case 't' : return 'a';
	}

	return 0;
}

// rc_str
//////////////////////////////////////////////////////////////////////////
string rc_str(const string & str)
{
	string retval;

	for (int i = str.length()-1; i >= 0; i--)
	{
		retval.push_back(rrc(str[i]));
	}

	return retval;
}


// isRepeat
// returns true if the input sequence contains any repeat of size K
// (multiple occurence of the same K-mer)
//////////////////////////////////////////////////////////////////////////
bool isRepeat(const string & seq, int K)
{
	bool result = false;
	
	set<string> mers;

	int end = seq.length() - K;
	for (int offset = 0; offset < end; offset++)
	{
		string s = seq.substr(offset,K);

	    if (mers.count(s)>0) { // s is in the set
			result = true;
			break;
		}
		else { // not in the set
			mers.insert(s);
		}
	}
	return result;	
}

bool isAlmostRepeat(const string & seq, int K, int max)
{
	bool result = false;
	
	set<string> mers;

	int end = seq.length() - K;
	for (int offset = 0; offset < end; offset++)
	{
		string s = seq.substr(offset,K);

		if(kMismatch(s,seq,offset+1,max)) {
			result = true;
			break;
		}
	}
	return result;	
}

bool kMismatch(const string & p, const string & t, int start, int max) { // p==pattern, t==text
	bool flag;
	int count;
	size_t i=start;
	while(i<(t.size()-p.size()+1)) {
		flag = true;
		count = 0;
		size_t j=0;
		while(j<p.size() && i+j<t.size()) {
			if(t[i+j] != p[j]) {
				count++;
				if(count>max) { flag = false; break; }
			}
			j++;
		}
		if(flag && j==p.size()) { return true; }
		i++;
	}
	return false;
}


// Fasta_Read
//////////////////////////////////////////////////////////////////////////

bool Fasta_Read (FILE * fp, string & s, string & hdr)
{
	int  ch;

	s . erase ();
	hdr . erase ();

	// skip till next '>' if necessary
	while  ((ch = fgetc (fp)) != EOF && ch != '>')
		;

	if  (ch == EOF)
		return  false;

	// skip spaces if any
	while  ((ch = fgetc (fp)) != EOF && ch == ' ')
		;
	if  (ch == EOF)
		return  false;
	ungetc (ch, fp);

	// put rest of line into  hdr
	while  ((ch = fgetc (fp)) != EOF && ch != '\n')
		hdr . push_back (char (ch));

	// put everything up till next '>' into  s
	while  ((ch = fgetc (fp)) != EOF && ch != '>')
	{
		if  (! isspace (ch))
			s . push_back (char (ch));
	}

	if  (ch == '>')
		ungetc (ch, fp);

	return  true;
}

