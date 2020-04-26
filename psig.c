// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "hash.h"

Bits codewordPsig(char* tv, int m, int k){
  Bits cword = newBits(m);
  srandom(hash_any(tv, strlen(tv)));

  int nbits = 0;
  while(nbits < k){
    int i = random() % m;
    if(!bitIsSet(cword, i)){
      setBit(cword, i);
      nbits++;
    }
  }
  return cword;
}

Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	//TODO
	Bits psig = newBits(psigBits(r));
	char** tvs = tupleVals(r, t);
	for(int i = 0; i < nAttrs(r); ++i){
		char *tv = tvs[i];
    if(strcmp(tv, "?") != 0){
      Bits cw = codewordPsig(tv, psigBits(r), codeBits(r));
      orBits(psig, cw);
    }
	}
	return psig; // remove this
}

void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	Bits qsig = makePageSig(q->rel, q->qstring);
  unsetAllBits(q->pages);
  File psigs = psigFile(q->rel);
  for(int i = 0; i < nPsigPages(q->rel); ++i){
    Page p = getPage(psigs, i);
    q->nsigpages++;

    for(int j = 0; j < pageNitems(p); ++j){
      Bits psig = newBits(psigBits(q->rel));
			getBits(p, j, psig);
			if (isSubset(qsig, psig)) {
				int psid = ((i * maxPsigsPP(q->rel)) + j);
				setBit(q->pages, psid);
			}
			q->nsigs++;
    }
  }
  // printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}

