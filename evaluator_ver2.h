#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef __EVALUATOR__
#define __EVALUATOR__

#ifndef __INDI__
#include "indi.h"
#endif

#ifndef __RAND__
#include "rand.h"
#endif

class TEvaluator {
 public:
  TEvaluator();
  ~TEvaluator();
  void SetInstance( char* filename );    
  void Preprocessing();       

  int fNumOfEvent;   // Number of events
  int fNumOfRoom;    // Number of rooms
  int fNumOfStudent; // Number of students
  int fNumOfTime;    // Number of timeslot
  int fNumOfDay;     //  Number of days
  int fNumOfTimeInDay;      // Number of timeslots in a day
  int **fAvail_EventRoom;    // [event][room] -> 1: event can be assigned to room, 0: otherwise 
  int **fConf_EventEvent;   // [event1][event2] -> number of students in both event1 and event2
  int **fListStudent_Event; // [event][] -> student list in event
  int *fNumOfStudent_Event; // [event] -> number of studnts in event
  int **fListConfEvent_Event; // [event] -> conflict event list
  int *fNumOfConfEvent_Event; // [event] -> number of conflict event
  
  int **fDiffPenalty_S2_Insert;
};
  
#endif

