#ifndef __SEARCH__
#define __SEARCH__

#ifndef __RAND__
#include "rand.h"
#endif

#ifndef __Sort__
#include "sort.h"
#endif

#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

#ifndef __OPERATPR__
#include "operator.h"
#endif

#ifndef __INDI__
#include "indi.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

class TSearch : public TOperator{

public:
  TSearch();
  ~TSearch();
  void Set( TEvaluator* evaluator ); 
  void SetParameter( int* para );
  
  void MakeInitRandSol();
  void LS_MakeFeasible(); 
  void Eject_NoStudentEvent();
  void Insert_NoStudentEvent();
  void LS_Relocation(); 
  void LS_Relocation_Swap(); 
  void LS_Relocation_Swap_Extend(); 
  void LS_Relocation_Swap_In_Day( int day );
  void GES_main();
  void GES_Routine( int limitPenalty_S );
  void Randomize( int limitPenalty_S );
  void DispFeatureSol();
  int Distance( TIndi& indi, int flag );
  int Distance_D( TIndi& indi1, TIndi& indi2 );
  int Distance_D( TIndi& indi1 );
  void QuasiGES();
  void Perturbation();

  TIndi tIndi_LS;

  int fNumOfIterLS; 
  int fMaxNumOfIterTotal;
  int fMaxNumOfIterLS;
  int fNumOfIterFindBest;
  int fMaxNumOfIterRand;
  int fTabuTenure;
  char *fDstFile;

  int **fMoved_EventTime;
  int ***fMoved_EventTimeRoom;

  int fRadioActiveEvent; // Test3
};

#endif
