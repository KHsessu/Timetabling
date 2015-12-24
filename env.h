#ifndef __ENVIRONMENT__
#define __ENVIRONMENT__

#ifndef __RAND__
#include "rand.h"
#endif

#ifndef __SORT__
#include "sort.h"
#endif

#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

#ifndef __Search__
#include "search.h"
#endif

#ifndef __INDI__
#include "indi.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

class TEnvironment {
public:
  TEnvironment(); 
  ~TEnvironment();
  void SetParameter( int *para );
  void Define();
  void Result( int numOfTrial, char* dstFile );
  void DoIt();

  TEvaluator* tEvaluator; 
  TSearch* tSearch;
  TIndi tIndi;

  char* fInstanceName;
  //   int fPara1, fPara2;
  //   int fNumOfTrial;
  char *fDstFile;

  // time
  clock_t fTimeStart, fTimeEnd;
};


#endif
