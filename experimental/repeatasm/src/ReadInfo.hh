#ifndef READINFO_HH
#define READINFO_HH 1

#include <string>

#include "Mer.hh"

using namespace std;

// ReadId_t
//////////////////////////////////////////////////////////////////////////

typedef int ReadId_t;

// ReadInfo_t
//////////////////////////////////////////////////////////////////////////

class ReadInfo_t
{
public:
	ReadInfo_t(const string & set, const string & readname, const string & seq, char code)
		: set_m(set), readname_m(readname), seq_m(seq), code_m(code), mateid_m(-1), trm5(0), trm3(0), isjunk(false)
		{ }

	string       set_m;
	string       readname_m;
	string       seq_m;
	char         code_m;
	ReadId_t     mateid_m;
	Mer_t        contigid_m;
	unsigned int readstartidx_m;
	int trm5;
	int trm3;
	bool isjunk;
};

typedef vector<ReadInfo_t> ReadInfoList_t;

#endif
