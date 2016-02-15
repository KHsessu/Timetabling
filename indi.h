#ifndef __INDI__
#define __INDI__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

class TIndi {

public:

  TIndi();
  TIndi( int n , int t , int r );
  ~TIndi();
  TIndi& operator = ( const TIndi& src );  
  void Define( int n , int t , int r);      // member変数の定義
  void PrintOn() const;      // 情報書出し   
  bool ReadFrom( FILE *fp ); // fileから読込
  bool WriteTo( FILE *fp ) const; // fileへ書出し

  int fNumOfEvent;         // イベント数
  int fNumOfTime;
  int fNumOfRoom;
  int fNumOfEjectEvent;    // 未割り付けイベント数
  int **fTimeRoom_Event;   // [timeslot][room] -> event
  int **fEvent_TimeRoom;   
  int fPenalty_S;          // total penalty
  int fPenalty_S1;         // penalty S1
  int fPenalty_S2;         // penalty S2
  int fPenalty_S3;         // penalty S3
  int fPenalty_S4;         // ken hachikubo add S4 S5 S6 12.29
  int fPenalty_S5;
  int fPenalty_S6;
  double fEvaluationValue; // 評価値 = total penalty
};

#endif

