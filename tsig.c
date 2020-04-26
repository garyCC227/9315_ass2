// tsig.c ... functions on Tuple Signatures (tsig's)
// part of SIMC signature files
// Written by John Shepherd, March 2020

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

// make a tuple signature
Bits codewordTsig(char* tv, int m, int k){
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

Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	//TODO
	Bits tsig = newBits(tsigBits(r));
	char **tvs = tupleVals(r, t);
	for (int i = 0; i < nAttrs(r); ++i) {
		char *tv = tvs[i];
		if (strcmp(tv, "?") != 0) {
			Bits cw = codewordTsig(tv, tsigBits(r), codeBits(r));
			orBits(tsig, cw);
		}
	}
	return tsig;
}

void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);
	//TODO
	PageID pid;
	Bits qsig = makeTupleSig(q->rel, q->qstring);
	unsetAllBits(q->pages); 

	for (int i = 0; i < nTsigPages(q->rel); i++){
		Page p = getPage(tsigFile(q->rel), i);
		q->nsigpages++;
		for (int j = 0; j < pageNitems(p); j++){
			Bits tsig = newBits(tsigBits(q->rel));
			getBits(p, j, tsig);
			
			if ( isSubset(qsig, tsig) ){
				pid = q->nsigs / maxTupsPP(q->rel);
				setBit(q->pages, pid);
			}
			q->nsigs++;
		}

	}
	// The printf below is primarily for debugging
	// Remove it before submitting this function
	// printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}