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

  void Set( TEvaluator* evaluator ); // 必要な情報をセット
  void SetParameter();               // パラメータ値をセット

  void TransToIndi( TIndi& indi );   // 状態をindiに変換
  void TransFromIndi( TIndi& indi ); // indiを状態に変換

  void CheckValid();                 // バグ検出用
  void CalEvaluation();                 // ナイーブにペナルティを計算
  int DiffPenalty_S_Eject( int event ); // eventをejectした際のペナルティの差分計算
  int DiffPenalty_S_Insert( int event, int time ); // eventをinsertした際のペナルティの差分計算
  void ResetSol();                      // 状態をempty scheduleに初期化
  void Eject( int event, int flag );    // enentをeject (flagの説明保留)
  void Insert( int event, int time, int room, int flag ); // enentをinsert (flagの説明保留)

  // Fixed variables (from eval)
  int fNumOfEvent;                 
  int fNumOfRoom;         
  int fNumOfStudent;
  int fNumOfProf;
  int fNumOfTime;         
  int fNumOfDay;
  int fNumOfRequiredEvent;
  int fNumOfTimeInDay;
  int **fAvail_EventRoom; 
  int **fConf_EventEvent; 
  int **fListStudent_Event;
  int **fListProf_Event;
  int *fNumOfStudent_Event;
  int *fNumOfProf_Event;
  int ** fListConfEvent_Event;
  int * fNumOfConfEvent_Event;
  int *fEvent_Required;              // ken hachikubo add 12.15
  int *fEvent_TimeRequest;           // ken hachikubo add 12.15
  int *fListRequiredEvent;
  int **fProfCantDo;
  int **fDiffPenalty_S2_Insert;

  // Variables for a solutsion (primary)
  // ken hachikubo add S4 S5 12.29
  int fPenalty_S;
  int fPenalty_S1;
  int fPenalty_S2;
  int fPenalty_S3;
  int fPenalty_S4;
  int fPenalty_S5;
  int fPenalty_S6;

  int **fTimeRoom_Event; // [event][] -> time, room 
  int **fEvent_TimeRoom; // [time][room] -> event
  int *fListEjectEvent;  // list of the ejected events
  int *fInvEjectEvent;   // inverse of the list 
  int fNumOfEjectEvent;  // the length of the list
  
  // Variables for a solutsion (secondary)
  int **fEvent_StudentTime;     // [student][time] -> event
  int **fEvent_ProfTime;	// [professor][time] -> event  ken hachikubo add 1.12
  int **fNumOfEvent_StudentDay; // [student][day] -> number of event
  int **fNumOfEvent_ProfDay;
  int **fConf_EventTime;        // [event][time] -> eventをtimeに割付時のコンフリクト学生数

  // Variables for the methods
  int fDiffPenalty_S, fDiffPenalty_S1, fDiffPenalty_S2, fDiffPenalty_S3, fDiffPenalty_S4, fDiffPenalty_S5, fDiffPenalty_S6;    // ken hachikubo change 12.29 


  // Parameters
  int fFlagCheckValid; // 0: CheckValid 無視, 1: CheckValid 有効
};

#endif

