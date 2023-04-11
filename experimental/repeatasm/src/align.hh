#ifndef ALIGN_HH
#define ALIGN_HH 1

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <sstream>

void global_align(const std::string & S, const std::string & T,
                  std::string & S_aln, std::string & T_aln,
                  int endfree, int verbose);

void global_align_aff(const std::string & S, const std::string & T,
                      std::string & S_aln, std::string & T_aln,
                      int endfree, int verbose);

//void global_cov_align_aff(const std::string & S, const std::string & T, const std::vector<float> & C,
//	                  std::string & S_aln, std::string & T_aln, std::string & C_aln,
//				      int endfree, int verbose);

#endif
