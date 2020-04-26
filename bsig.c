// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "psig.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	/*
	Qsig = makePageSig(Query)
	Pages = AllOneBits
	for each i in 0..pm-1 {
		if (Qsig bit[i] is 1) {
			Slice = get i'th bit slice from bsigFile
			zero bits in Pages which are zero in Slice
		}
	}
	*/
	Reln r = q->rel;
	RelnParams *rp = &(r->params);

	Bits qsig = makePageSig(r, q->qstring);
	setAllBits(q->pages);
	PageID prev_pid;
	for (int i = 0; i < rp->pm; ++i)
	{

		Bits bsig = newBits(bsigBits(r));
		if ( bitIsSet(qsig, i) )
		{
			PageID bsigPid = i / maxBsigsPP(r);
			Page p = getPage(bsigFile(r), bsigPid);
			if ( bsigPid != prev_pid){
				q->nsigpages++;
			}
			prev_pid = bsigPid;
			getBits(p, i % maxBsigsPP(r), bsig);
			for (int j = 0; j < nPages(r); ++j)
			{
				if( !bitIsSet(bsig, j)){
					unsetBit(q->pages, j);
				}
			}
			q->nsigs++;
		}
	}

}