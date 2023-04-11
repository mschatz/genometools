#include "ContigLink.hh"

float ContigLinkList_t::mean()
{
	float sum = 0;
	float cnt = 0;

	for (unsigned int i = 0; i < linklist_m.size(); i++)
	{
		sum += linklist_m[i].linkdist_m;
		cnt++;
	}

	if (cnt == 0) { return 0.0; }
	return sum / cnt;
}

float ContigLinkList_t::stdev(float mean)
{
	float diffsq = 0;
	float cnt = 0;

	for (unsigned int i = 0; i < linklist_m.size(); i++)
	{
		float diff = linklist_m[i].linkdist_m - mean;
		diffsq += diff*diff;
		cnt++;
	}

	if (cnt == 0) { return 0.0; }
	return sqrtf(diffsq / cnt);
}
