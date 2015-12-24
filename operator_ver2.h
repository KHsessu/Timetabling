#ifndef __OPERATOR__
#define __OPERATOR__

#ifndef __RAND__
#include "rand.h"
#endif

#ifndef __Sort__
#include "sort.h"
#endif

#ifndef __INDI__
#include "indi.h"
#endif

#ifndef __EVALUATOR__
#include "evaluator.h"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

class TOperator {

 public:
  TOperator();
  ~TOperator();

  void Set( TEvaluator* evaluator ); 
  void SetParameter();

  void TransToIndi( TIndi& indi );
  void TransFromIndi( TIndi& indi );

  void CheckValid();
  void CalEvaluation();
  int DiffPenalty_S_Eject( int event );
  int DiffPenalty_S_Insert( int event, int time );
  void ResetSol();
  void Eject( int event, int flag );
  void Insert( int event, int time, int room, int flag ); 

  // Fixed variables (from eval)
  int fNumOfEvent;                 
  int fNumOfRoom;         
  int fNumOfStudent;      
  int fNumOfTime;         
  int fNumOfDay;       
  int fNumOfTimeInDay; 
  int **fAvail_EventRoom; 
  int **fConf_EventEvent; 
  int **fListStudent_Event;
  int *fNumOfStudent_Event;
  int ** fListConfEvent_Event;
  int * fNumOfConfEvent_Event;
  int **fDiffPenalty_S2_Insert;

  // Variables for a solutsion (primary)
  int fPenalty_S;
  int fPenalty_S1;
  int fPenalty_S2;
  int fPenalty_S3;

  int **fTimeRoom_Event;
  int **fEvent_TimeRoom;
  int *fListEjectEvent;  
  int *fInvEjectEvent;  
  int fNumOfEjectEvent;  

  // Variables for a solutsion (secondary)
  int **fEvent_StudentTime;  
  int **fNumOfEvent_StudentDay;
  int **fConf_EventTime;

  // Variables for the methods
  int fDiffPenalty_S, fDiffPenalty_S1, fDiffPenalty_S2, fDiffPenalty_S3; 


  // Parameters
  int fFlagCheckValid;
};

#endif

