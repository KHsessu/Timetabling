#ifndef __OPERATOR__
#include "operator.h"
#endif


TOperator::TOperator() 
{
}

TOperator::~TOperator()
{
}

void TOperator::TransToIndi( TIndi& indi ) 
{
  int v, p;

  assert( indi.fNumOfEvent == fNumOfEvent );
  assert( indi.fNumOfTime == fNumOfTime );
  assert( indi.fNumOfRoom == fNumOfRoom );     
  // indi.fNumOfEvent = fNumOfEvent;     
  indi.fNumOfEjectEvent = fNumOfEjectEvent;     
  indi.fPenalty_S = fPenalty_S;
  indi.fPenalty_S1 = fPenalty_S1;
  indi.fPenalty_S2 = fPenalty_S2;
  indi.fPenalty_S3 = fPenalty_S3;
  indi.fPenalty_S4 = fPenalty_S4; // ken hachikubo add 12.29
  indi.fPenalty_S5 = fPenalty_S5;
  indi.fPenalty_S6 = fPenalty_S6;
  indi.fEvaluationValue = (double)fPenalty_S;  // depend on the strategy

  for( int e = 0; e < fNumOfEvent; ++e ){
    indi.fTimeRoom_Event[ e ][ 0 ] = fTimeRoom_Event[ e ][ 0 ];
    indi.fTimeRoom_Event[ e ][ 1 ] = fTimeRoom_Event[ e ][ 1 ];
  }

  for( int t = 0; t < fNumOfTime; ++t ){
    for( int r = 0; r < fNumOfRoom; ++r ){
      //      printf("%d %d\n",t,r);
      indi.fEvent_TimeRoom[ t ][ r ] = fEvent_TimeRoom[ t ][ r ];
      //      printf("更新");
    }
  }
}

void TOperator::TransFromIndi( TIndi& indi ) 
{
  int time , room;

  this->ResetSol();

  for( int e = 0; e < fNumOfEvent; ++e ){
    time = indi.fTimeRoom_Event[ e ][ 0 ];
    room = indi.fTimeRoom_Event[ e ][ 1 ];
    if( time != -1 )
      this->Insert( e, time, room, 1 );
  }
}


void TOperator::Set( TEvaluator* eval ) 
{
  // Fixed variables (from eval)
  fNumOfEvent = eval->fNumOfEvent;                 
  fNumOfRoom = eval->fNumOfRoom;
  fNumOfStudent = eval->fNumOfStudent;
  fNumOfProf = eval->fNumOfProf;
  fNumOfTime = eval->fNumOfTime;
  fNumOfDay = eval->fNumOfDay;
  fNumOfTimeInDay = eval->fNumOfTimeInDay;
  fNumOfRequiredEvent = eval->fNumOfRequiredEvent;
  fAvail_EventRoom = eval->fAvail_EventRoom;
  fConf_EventEvent = eval->fConf_EventEvent;
  fListStudent_Event = eval->fListStudent_Event;
  fNumOfStudent_Event = eval->fNumOfStudent_Event;
  fListProf_Event = eval->fListProf_Event;
  fNumOfProf_Event = eval->fNumOfProf_Event;
  fListConfEvent_Event = eval->fListConfEvent_Event;
  fListRequiredEvent = eval->fListRequiredEvent;
  fNumOfConfEvent_Event = eval->fNumOfConfEvent_Event;
  fProfCantDo = eval->fProfCantDo;
  fDiffPenalty_S2_Insert = eval->fDiffPenalty_S2_Insert;
  fEvent_Required = eval->fEvent_Required;              // ken hachikubo add 12.15
  fEvent_TimeRequest = eval->fEvent_TimeRequest;           // ken hachikubo add 12.15

  // Varialbes for a solutsion (primary)
  fTimeRoom_Event = new int* [ fNumOfEvent ];
  for( int e = 0; e < fNumOfEvent; ++e )
    fTimeRoom_Event[ e ] = new int [ 2 ];

  fEvent_TimeRoom = new int* [ fNumOfTime ];
  for( int t = 0; t < fNumOfTime; ++t )
    fEvent_TimeRoom[ t ] = new int [ fNumOfRoom ];

  fListEjectEvent = new int [ fNumOfEvent ];  
  fInvEjectEvent = new int [ fNumOfEvent ];  

  // Varialbes for a solutsion (secondary)
  fEvent_StudentTime = new int* [ fNumOfStudent ];
  for( int s = 0; s < fNumOfStudent; ++s )
    fEvent_StudentTime[ s ] = new int [ fNumOfTime ];

  fEvent_ProfTime = new int* [ fNumOfProf ];
  for( int p = 0; p < fNumOfProf; ++p)
    fEvent_ProfTime[ p ] = new int [ fNumOfTime ];
  
  fNumOfEvent_StudentDay = new int* [ fNumOfStudent ];
  for( int s = 0; s < fNumOfStudent; ++s )
    fNumOfEvent_StudentDay[ s ] = new int [ fNumOfDay ];

  fNumOfEvent_ProfDay = new int* [ fNumOfProf ];
  for( int p = 0; p < fNumOfProf; ++p )
    fNumOfEvent_ProfDay[ p ] = new int [ fNumOfDay ];

  fConf_EventTime = new int* [ fNumOfEvent ];
  for( int e = 0; e < fNumOfEvent; ++e )
    fConf_EventTime[ e ] = new int [ fNumOfTime ];
}

void TOperator::SetParameter()
{
  fFlagCheckValid = 0;
}


void TOperator::CheckValid()
{
  if( fFlagCheckValid == 0 )
    return;

  int event, time, room, student, day;   
  int numOfEjectEvent;
  int numOfEvent_StudentDay[ fNumOfStudent ][ fNumOfDay ];
  int numOfConf;
  int timeflag;

  // fListEjectEvent, fInvEjectEvent, fNumOfEjectEvent  
  for( int h = 0; h < fNumOfEjectEvent; ++h ){
    event = fListEjectEvent[ h ];
    assert( fInvEjectEvent[ event ] == h );
    assert( fInvEjectEvent[ event ] != -1 );
  }
  
  numOfEjectEvent = 0;
  for( int e = 0; e < fNumOfEvent; ++e ){
    if( fInvEjectEvent[ e ] != -1 ){
      assert( fListEjectEvent[ fInvEjectEvent[ e ] ] == e );
      ++numOfEjectEvent;
    }
  }
  assert( numOfEjectEvent == fNumOfEjectEvent );
  
  // fTimeRoom_Event, fEvent_TimeRoom
  numOfEjectEvent = fNumOfEvent;
  for( int e = 0; e < fNumOfEvent; ++e ){
    time = fTimeRoom_Event[ e ][ 0 ];
    room = fTimeRoom_Event[ e ][ 1 ];
    if( time != -1 ){
      assert( fEvent_TimeRoom[ time ][ room ] == e );
      --numOfEjectEvent;
    }
  }
  assert( numOfEjectEvent == fNumOfEjectEvent );

  for( int t = 0; t < fNumOfTime; ++t ){
    for( int r = 0; r < fNumOfRoom; ++r ){
      event = fEvent_TimeRoom[ t ][ r ];
      if( event != -1 ){
        for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
          if( t == fTimeRoom_Event[ event ][ 0 ] + tr ){
            timeflag = 0;
            break;
          }else{ timeflag = 1; }
        }
        if(timeflag != 0){
          printf("error : timeslot is illegal");
          assert( 1 == 2 );
          }
        assert( fTimeRoom_Event[ event ][ 1 ] == r );
      }
      else{
        assert( fEvent_TimeRoom[ t ][ r ] == -1 );
      } 
    }
  }
  
  // fEvent_StudentTime, fNumOfEvent_StudentDay
  for( int s = 0; s < fNumOfStudent; ++s )
    for( int d = 0; d < fNumOfDay; ++d )
      numOfEvent_StudentDay[ s ][ d ] = 0;

  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int t = 0; t < fNumOfTime; ++t ){
      event = fEvent_StudentTime[ s ][ t ]; 
      if( event != -1 ){
        time = fTimeRoom_Event[ event ][ 0 ];
        day = t / fNumOfTimeInDay; 
        ++numOfEvent_StudentDay[ s ][ day ];
        assert( abs(t - time) < fEvent_TimeRequest[ event ] );
      }
    }
  }
  for( int s = 0; s < fNumOfStudent; ++s )
    for( int d = 0; d < fNumOfDay; ++d )
      assert( fNumOfEvent_StudentDay[ s ][ d ] == numOfEvent_StudentDay[ s ][ d ] );
  
  for( int e = 0; e < fNumOfEvent; ++e ){ 
    for( int t = 0; t < fNumOfTime; ++t ){
      numOfConf = 0;
      for( int r = 0; r < fNumOfRoom; ++r ){
        event = fEvent_TimeRoom[ t ][ r ];
        if( event != -1 ){
          numOfConf += fConf_EventEvent[ e ][ event ];
        }
      }
      assert( fConf_EventTime[ e ][ t ] == numOfConf );
    }
  }
  
  int penalty_S1 = fPenalty_S1;
  int penalty_S2 = fPenalty_S2;
  int penalty_S3 = fPenalty_S3;
  int penalty_S4 = fPenalty_S4;   // ken hachikubo add 12.29
  int penalty_S5 = fPenalty_S5;
  int penalty_S6 = fPenalty_S6;
  //  printf("S1=%d S2=%d S3=%d S4=%d S5=%d\n",penalty_S1,penalty_S2,penalty_S3,penalty_S4,penalty_S5);
  this->CalEvaluation();
  assert( penalty_S1 == fPenalty_S1 );
  assert( penalty_S2 == fPenalty_S2 );
  assert( penalty_S3 == fPenalty_S3 );
  assert( penalty_S4 == fPenalty_S4 );
  assert( penalty_S5 == fPenalty_S5 );
  assert( penalty_S1 + penalty_S2 + penalty_S3 + penalty_S4 + penalty_S5 + penalty_S6 == fPenalty_S );
}

void TOperator::CalEvaluation()
{
  int penalty_H1, penalty_S1, penalty_S2, penalty_S3, penalty_S4, penalty_S5, penalty_S6;
  int time;
  int count;
  int event;
  int event1, event2;
  int professor;
  int slot;
  double e_ave, e_day, e_diff;

  /* original penalty */
  /*
  // fPenalty_S1
  penalty_S1 = 0;
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int d = 0; d < fNumOfDay; ++d ){
      time = d * (fNumOfTimeInDay) +1;
      if( fEvent_StudentTime[ s ][ time ] != -1 )
	++penalty_S1;
      time = (d+1) * (fNumOfTimeInDay) -1;
      if( fEvent_StudentTime[ s ][ time ] != -1 )
	++penalty_S1;
    }
  }
  fPenalty_S1 = penalty_S1;

  // fPenalty_S2
  penalty_S2 = 0;
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int d = 0; d < fNumOfDay; ++d ){
      time = d * (fNumOfTimeInDay);
      count = 0;
      for( int h = 0; h < fNumOfTimeInDay; ++h ){
	if( fEvent_StudentTime[ s ][ time + h ] != -1 )
	  ++count;
	else
	  count = 0;
	if( count >= 3 )
	  ++penalty_S2 ;
      }
    }
  }
  fPenalty_S2 = penalty_S2;

  // fPenalty_S3
  penalty_S3 = 0;
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int d = 0; d < fNumOfDay; ++d ){
      time = d * (fNumOfTimeInDay);
      count = 0;
      for( int h = 0; h < fNumOfTimeInDay; ++h ){
	if( fEvent_StudentTime[ s ][ time + h ] != -1 )
	  ++count;
      }
      if( count == 1 )
	++penalty_S3 ;
    }
  }
  fPenalty_S3 = penalty_S3;
  */


  
  /*  new calevaluation S1 S2 S3 S4 S5*/
  // fPenalty_S1
  /* 日の最初か最後に授業があるとペナルティ*/
  penalty_S1 = 0;
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int d = 0; d < fNumOfDay; ++d ){
      time = (d * (fNumOfTimeInDay));
      if( fEvent_StudentTime[ s ][ time ] != -1 )
        ++penalty_S1;
      time = (d+1) * (fNumOfTimeInDay) -1;
      if( fEvent_StudentTime[ s ][ time ] != -1 )
        ++penalty_S1;
    }
  }
  //  printf("S1=%d ",penalty_S1);
  fPenalty_S1 = penalty_S1;
  
  // fPenalty_S2 生徒の同日における授業数の偏り
  penalty_S2 = 0;
  for( int s = 0; s < fNumOfStudent; ++s ){
    count = 0;
    e_ave = 0;
    e_day = 0;
    for( int t = 0; t < fNumOfTime; ++t ){
      if( fEvent_StudentTime[ s ][ t ] != -1)
        e_ave++;
    }
    e_ave /= fNumOfDay;
    for ( int d = 0; d < fNumOfDay; ++d){
      e_day = 0;
      time = d * (fNumOfTimeInDay);
      for(int h = 0; h < fNumOfTimeInDay; ++h ){
        if( fEvent_StudentTime[ s ][ time + h ] != -1 )
          ++e_day;
      }
      //      printf("student=%d e_ave=%f e_day=%f\n",s,e_ave,e_day);
      e_diff = e_ave - e_day;
      //      if( (e_diff * e_diff) > 2 )
      penalty_S2 += abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }
  //  printf("S2=%d ",penalty_S2);
  fPenalty_S2 = penalty_S2;
  
  // fPenalty_S3 教授の曜日における授業数の偏り
  penalty_S3 = 0;
 
  for( int p = 0; p < fNumOfProf; ++p ){
    e_ave = 0;
    e_day = 0;
    for( int t = 0; t < fNumOfTime; ++t ){
      if( fEvent_ProfTime[ p ][ t ] != -1)
        e_ave++;
    }
    e_ave /= fNumOfDay;
    for ( int d = 0; d < fNumOfDay; ++d){
      e_day = 0;
      time = d * (fNumOfTimeInDay);
      count = 0;
      for(int h = 0; h < fNumOfTimeInDay; ++h ){
        if( fEvent_ProfTime[ p ][ time + h ] != -1 )
          ++e_day;
      }
      e_diff = e_ave - e_day;
      //      if( (e_diff * e_diff) > 2 )
      penalty_S3 += abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }
  //  printf("S3=%d ",penalty_S3);
  fPenalty_S3 = penalty_S3;
  
  // fPenalty_S4 連続授業が昼休みをまたいでいるかどうかでペナルティ
  penalty_S4 = 0;
  for(int e = 0; e < fNumOfEvent; e++){
    slot = fTimeRoom_Event[ e ][ 0 ] % fNumOfTimeInDay;
    if( slot != -1 && slot <= 1 && ( slot + fEvent_TimeRequest[ e ]) > 2 ){
        printf("S4 penaty!! e=%d\n",e);
      penalty_S4 += 1000;
    }
  }
  //  printf("S4=%d ",penalty_S4);
  fPenalty_S4 = penalty_S4;
  
  // fPenalty_S5 必修が被っている時限数がペナルティ
  penalty_S5 = 0;
  
  for(int t = 0; t < fNumOfTime; ++t){
    for( int r = 0; r < fNumOfRoom -1; ++r){
      event1 = fEvent_TimeRoom[ t ][ r ];
      for( int rr = r + 1; rr < fNumOfRoom; ++rr){
        event2 = fEvent_TimeRoom[ t ][ rr ];
        if(event1 != -1 && event2 != -1)
          if( fEvent_Required[ event1 ] !=0 && fEvent_Required[ event1 ] == fEvent_Required[ event2 ]){
	    //int event1a, event2a;
	    //event1a = event1 % 44;
	    //event2a = event2 % 44;
	    printf("S5 penalty!! e1=%d & e2=%d e1amari44=%d e2amari44=%d\n",event1,event2,event1%44,event2%44);
	    penalty_S5 += 500;
	  }
      }
    }
  }
  
  //  printf("S5=%d\n",penalty_S5);
  fPenalty_S5 = penalty_S5;

  // fPenalty_S6教授の都合の悪い時間帯に行わないこと
  penalty_S6 = 0;
  for( int t = 0; t < fNumOfTime; ++t){  
    for( int r = 0; r < fNumOfRoom; ++r ){
      event = fEvent_TimeRoom[ t ][ r ];
      for( int p = 0; p < fNumOfProf_Event[ event ]; ++p){
	professor = fListProf_Event[ event ][ p ];
	if( fProfCantDo[ professor ][ t ] == 1)
	  penalty_S6+=100;
      }
      
    }
  }
  fPenalty_S6 = penalty_S6;
  
  fflush(stdout);
  fPenalty_S = penalty_S1 + penalty_S2 + penalty_S3 + penalty_S4 + penalty_S5 + penalty_S6;;
}


int TOperator::DiffPenalty_S_Eject( int event )
{
  int student, day, slot, time, time_s, time_e;
  int count_p, count_n;

  time = fTimeRoom_Event[ event ][ 0 ];
  assert( time != -1 );
  day = time / fNumOfTimeInDay; 
  slot =  time % fNumOfTimeInDay; 

  fDiffPenalty_S1 = 0;
  if( slot == fNumOfTimeInDay - 1 )
    fDiffPenalty_S1 = - fNumOfStudent_Event[ event ];

  fDiffPenalty_S2 = 0;
  fDiffPenalty_S3 = 0;
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    student = fListStudent_Event[ event ][ k ];
    if( fNumOfEvent_StudentDay[ student ][ day ] == 1 )
      --fDiffPenalty_S3;
    else if( fNumOfEvent_StudentDay[ student ][ day ] == 2 )
      ++fDiffPenalty_S3;

    time_s = day * fNumOfTimeInDay;        // start time of the day
    time_e = time_s + fNumOfTimeInDay - 1; // end time of the day

    count_p = 0;  // the number of successive events preceding to the event
    for( int t = time-1; t >= time_s; --t ){
      if( fEvent_StudentTime[ student ][ t ] != -1 )
	++count_p;
      else 
	break;
    }  

    count_n = 0;  // the number of successive events next to the event
    for( int t = time+1; t <= time_e; ++t ){
      if( fEvent_StudentTime[ student ][ t ] != -1 )
	++count_n;
      else 
	break;
    }
    fDiffPenalty_S2 -= fDiffPenalty_S2_Insert[ count_p ][ count_n ];  
  }

  
  fDiffPenalty_S = fDiffPenalty_S1 + fDiffPenalty_S2 + fDiffPenalty_S3;

  return fDiffPenalty_S;
}


int TOperator::DiffPenalty_S_Insert( int event, int time )
{
  int student, day, slot, time_s, time_e;
  int prof;
  int count_p, count_n;
  double e_ave_b,e_ave_a,e_day_b,e_day_a;
  double e_diff_b, e_diff_a;
  
  day = time / fNumOfTimeInDay; 
  slot =  time % fNumOfTimeInDay; 
  
  fDiffPenalty_S1 = 0;
  for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
    if( (slot + tr) == fNumOfTimeInDay - 1 || (slot + tr) == 0)
      fDiffPenalty_S1 += fNumOfStudent_Event[ event ];
  }

  fDiffPenalty_S2 = 0;
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    student = fListStudent_Event[ event ][ k ];
    e_ave_b = 0;
    e_ave_a = 0;
    e_day_b = 0;
    e_day_a = 0;
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave_b += fNumOfEvent_StudentDay[ student ][ d ];
    e_ave_a = e_ave_b + fEvent_TimeRequest[ event ];
    e_ave_b /= fNumOfDay;
    e_ave_a /= fNumOfDay;
    
    for( int d = 0; d < fNumOfDay; ++d ){
      e_day_b = fNumOfEvent_StudentDay[ student ][ d ];
      if( day == d ){
        e_day_a = e_day_b + fEvent_TimeRequest[ event ];
      }else{
        e_day_a = e_day_b;
      }
      e_diff_b = e_ave_b - e_day_b;
      e_diff_a = e_ave_a - e_day_a;
      //      printf("insert student %d after e_ave=%f e_day=%f\n",fListStudent_Event[event][k], e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fDiffPenalty_S2 -=abs( (int)e_diff_b);	// 他のペナルティとのバランスを考える
      fDiffPenalty_S2 +=abs( (int)e_diff_a);
    }
  }

  fDiffPenalty_S3 = 0;
  for( int k = 0; k < fNumOfProf_Event[ event ]; ++k ){
    prof = fListProf_Event[ event ][ k ];
    e_ave_b = 0;
    e_ave_a = 0;
    e_day_b = 0;
    e_day_a = 0;
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave_b += fNumOfEvent_ProfDay[ prof ][ d ];
    e_ave_a = e_ave_b + fEvent_TimeRequest[ event ];
    e_ave_b /= fNumOfDay;
    e_ave_a /= fNumOfDay;
    
    for( int d = 0; d < fNumOfDay; ++d ){
      e_day_b = fNumOfEvent_ProfDay[ prof ][ d ];
      if( day == d ){
        e_day_a = e_day_b + fEvent_TimeRequest[ event ];
      }else{
        e_day_a = e_day_b;
      }
      e_diff_b = e_ave_b - e_day_b;
      e_diff_a = e_ave_a - e_day_a;
      //      printf("insert student %d after e_ave=%f e_day=%f\n",fListStudent_Event[event][k], e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fDiffPenalty_S3 -=abs( (int)e_diff_b);	// 他のペナルティとのバランスを考える
      fDiffPenalty_S3 +=abs( (int)e_diff_a);
    }
  }

  fDiffPenalty_S4 = 0;
  if( slot <= 1 && (slot + fEvent_TimeRequest[ event ]) > 2 )
    fDiffPenalty_S4 += 1000;
  
  fDiffPenalty_S5 = 0;
  for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
    for( int r = 0; r < fNumOfRoom; ++r){
      if( fEvent_Required[ event ] != 0 &&  fEvent_Required[ event ] ==  fEvent_Required[ fEvent_TimeRoom[time+tr][r]] )
        fDiffPenalty_S5 += 500;
    }
  }
  
  fDiffPenalty_S6 = 0;
  for( int p = 0; p < fNumOfProf_Event[ event ]; ++p ){
    prof = fListProf_Event[ event ][ p ];
    for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
      if( fProfCantDo[ prof ][ time + tr ] == 1)
	fDiffPenalty_S6+=100;
    }
  }
  



  
  /*
  fDiffPenalty_S1 = 0;
  if( slot == fNumOfTimeInDay - 1 )
    fDiffPenalty_S1 = fNumOfStudent_Event[ event ];

  fDiffPenalty_S2 = 0;
  fDiffPenalty_S3 = 0;
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    student = fListStudent_Event[ event ][ k ];
    if( fNumOfEvent_StudentDay[ student ][ day ] == 0 )
      ++fDiffPenalty_S3;
    else if( fNumOfEvent_StudentDay[ student ][ day ] == 1 )
      --fDiffPenalty_S3;

    time_s = day * fNumOfTimeInDay;        // start time of the day
    time_e = time_s + fNumOfTimeInDay - 1; // end time of the day

    count_p = 0;  // the number of successive events preceding to the event
    for( int t = time-1; t >= time_s; --t ){
      if( fEvent_StudentTime[ student ][ t ] != -1 )
	++count_p;
      else 
	break;
    }  

    count_n = 0;  // the number of successive events next to the event
    for( int t = time+1; t <= time_e; ++t ){
      if( fEvent_StudentTime[ student ][ t ] != -1 )
	++count_n;
      else 
	break;
    }
    fDiffPenalty_S2 += fDiffPenalty_S2_Insert[ count_p ][ count_n ];  
  }
  */
  
    fDiffPenalty_S = fDiffPenalty_S1 + fDiffPenalty_S2 + fDiffPenalty_S3 + fDiffPenalty_S4 + fDiffPenalty_S5 + fDiffPenalty_S6;

  return fDiffPenalty_S;
}


void TOperator::ResetSol()
{
  int count;
  int listn;
  int *listevent;
  
  fPenalty_S = 0;
  fPenalty_S1 = 0;
  fPenalty_S2 = 0;
  fPenalty_S3 = 0;
  fPenalty_S4 = 0;
  fPenalty_S5 = 0;
  fPenalty_S6 = 0;

  for( int e = 0; e < fNumOfEvent; ++e ){
    fTimeRoom_Event[ e ][ 0 ] = -1;
    fTimeRoom_Event[ e ][ 1 ] = -1;
  }

  for( int t = 0; t < fNumOfTime; ++t )
    for( int r = 0; r < fNumOfRoom; ++r )
      fEvent_TimeRoom[ t ][ r ] = -1;
  listevent = new int [ fNumOfEvent ];
  
  tRand->Permutation( listevent , fNumOfEvent, fNumOfEvent );
  
  listn = fNumOfEvent-1;
  for( int t = 1; t < fNumOfTimeInDay+1; ++t ){
    for( int i = 0; i < fNumOfEvent; ++i ){
      if( t == fEvent_TimeRequest[ listevent[i] ]){
        fListEjectEvent[ listn ] = listevent[ i ];
        --listn;
	printf("%d\n",listn);
      }
    }
  }
  assert( listn == -1 );
    
  for( int i = 0; i < fNumOfEvent; ++i ){
    fInvEjectEvent[ fListEjectEvent[ i ] ] = i;
    //    printf("%3d\n",fListEjectEvent[ i ]);
    }
  /*
  for( int e = 0; e < fNumOfEvent; ++e ){
    fListEjectEvent[ e ] = e;
    fInvEjectEvent[ e ] = e;
  }
  */

  fNumOfEjectEvent = fNumOfEvent;  

  for( int s = 0; s < fNumOfStudent; ++s )
    for( int t = 0; t < fNumOfTime; ++t )
      fEvent_StudentTime[ s ][ t ] = -1;

  for( int s = 0; s < fNumOfStudent; ++s )
    for( int d = 0; d < fNumOfDay; ++d )
      fNumOfEvent_StudentDay[ s ][ d ] = 0;

  for( int p = 0; p < fNumOfProf; ++p )
    for( int t = 0; t < fNumOfTime; ++t )
      fEvent_ProfTime[ p ][ t ] = -1;

  for( int p = 0; p < fNumOfProf; ++p )
    for( int d = 0; d < fNumOfDay; ++d )
      fNumOfEvent_ProfDay[ p ][ d ] = 0;

  for( int e = 0; e < fNumOfEvent; ++e )
    for( int t = 0; t < fNumOfTime; ++t )
      fConf_EventTime[ e ][ t ] = 0;
}

void TOperator::Eject( int event, int flag )
{
  int student, professor, time, room, day;
  int slot, time_s, time_e;
  int count_p, count_n;
  int event1;
  double e_ave, e_day, e_diff;
  
  assert( 0 <= event < fNumOfEvent );
  assert( fTimeRoom_Event[ event ][ 0 ] != -1 );
  assert( fTimeRoom_Event[ event ][ 1 ] != -1 );
  time = fTimeRoom_Event[ event ][ 0 ];
  room = fTimeRoom_Event[ event ][ 1 ];

  for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){//
    assert(fEvent_TimeRoom[ time + tr ][ room ] == event);//
    fEvent_TimeRoom[ time + tr ][ room ] = -1;//
    assert(fEvent_TimeRoom[ time + tr ][ room ] == -1);
  }
  fTimeRoom_Event[ event ][ 0 ] = -1;
  fTimeRoom_Event[ event ][ 1 ] = -1;

  fListEjectEvent[ fNumOfEjectEvent ] = event;
  fInvEjectEvent[ event ] = fNumOfEjectEvent;
  ++fNumOfEjectEvent;

  if( flag == 1 ){
    // for( int e = 0; e < fNumOfEvent; ++e )
    // fConf_EventTime[ e ][ time ] -= fConf_EventEvent[ e ][ event ];
    for( int k = 0; k < fNumOfConfEvent_Event[ event ]; ++k ){
      event1 = fListConfEvent_Event[ event ][ k ];
      for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
        fConf_EventTime[ event1 ][ time + tr ] -= fConf_EventEvent[ event1 ][ event ];
      }
    }
  }
  
  day = time / fNumOfTimeInDay;


  /* penalty_S2 _S3をあらかじめ引いておく */
  
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    e_ave = 0;
    e_day = 0;
    student = fListStudent_Event[ event ][ k ];
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave += fNumOfEvent_StudentDay[ student ][ d ];
    e_ave /= fNumOfDay;
    for( int d = 0; d < fNumOfDay; ++d ){
      e_day = fNumOfEvent_StudentDay[ student ][ d ];
      e_diff = e_ave - e_day;
      //     printf("eject student %d before e_ave=%f e_day=%d\n",k , e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fPenalty_S2 -= abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }
  for( int j = 0; j < fNumOfProf_Event[ event ]; ++j ){
    e_ave = 0;
    e_day = 0;
    professor = fListProf_Event[ event ][ j ];
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave += fNumOfEvent_ProfDay[ professor ][ d ];
    e_ave /= fNumOfDay;
    for( int d = 0; d < fNumOfDay; ++d ){
      e_day = fNumOfEvent_ProfDay[ professor ][ d ];
      e_diff = e_ave - e_day;
      //     printf("eject student %d before e_ave=%f e_day=%d\n",k , e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fPenalty_S3 -= abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }
  
  
  
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    // fNumOfEvent_StudentDay;
    student = fListStudent_Event[ event ][ k ];
    for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){//
      assert( fEvent_StudentTime[ student ][ time + tr ] != -1 );//
      fEvent_StudentTime[ student ][ time + tr ] = -1;//
      --fNumOfEvent_StudentDay[ student ][ day ];
    }
  }
  
  for( int j = 0; j < fNumOfProf_Event[ event ]; ++j ){
    //fNumOfEvent_ProfDay;
    professor = fListProf_Event[ event ][ j ];
    for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
      assert( fEvent_ProfTime[ professor ][ time + tr ] != -1 );
      fEvent_ProfTime[ professor ][ time + tr ] = -1;
      --fNumOfEvent_ProfDay[ professor ][ day ];
    }
  }

  
  slot =  time % fNumOfTimeInDay;
  //  fPenalty_S4
  if( slot < 2 && (slot + fEvent_TimeRequest[ event ]) > 2)
  fPenalty_S4 -= 1000;
  // fPenalty_S1
  for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
    if( slot == fNumOfTimeInDay - 1 || slot == 0)
      fPenalty_S1 -= fNumOfStudent_Event[ event ];
    ++slot;
  }

    // fPenalty_S2 生徒の同日における授業数の偏り
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    e_ave = 0;
    e_day = 0;
    student = fListStudent_Event[ event ][ k ];
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave += fNumOfEvent_StudentDay[ student ][ d ];
    e_ave /= fNumOfDay;

    for( int d = 0; d < fNumOfDay; ++d ){
      e_day = fNumOfEvent_StudentDay[ student ][ d ];
      e_diff = e_ave - e_day;
      //     printf("eject student %d after e_ave=%f e_day=%d\n",k,e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fPenalty_S2 += abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }
  
  // fPenalty_S3
  for( int j = 0; j < fNumOfProf_Event[ event ]; ++j ){
    e_ave = 0;
    e_day = 0;
    professor = fListProf_Event[ event ][ j ];
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave += fNumOfEvent_ProfDay[ professor ][ d ];
    e_ave /= fNumOfDay;
    
    for( int d = 0; d < fNumOfDay; ++d ){
      e_day = fNumOfEvent_ProfDay[ professor ][ d ];
      e_diff = e_ave - e_day;
      //     printf("eject student %d after e_ave=%f e_day=%d\n",k,e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fPenalty_S3 += abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }

  //fPenalty_S4 は上
  
  // fPenalty_S5
  for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
    for( int r = 0; r < fNumOfRoom; ++r){
      if( r != room && fEvent_Required[ event ] != 0 && fEvent_Required[ event ] ==  fEvent_Required[ fEvent_TimeRoom[time+tr][r]] )
        fPenalty_S5 -= 500;
    }
  }
  
  //  fPenalty_S6
  for( int p = 0; p < fNumOfProf_Event[ event ]; ++p ){
    professor = fListProf_Event[ event ][ p ];
    for(int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
      if( fProfCantDo[ professor ][ time + tr ] == 1){
        fPenalty_S6-=100;
      }
    }
  }
  //  printf("fPenalty_S6 = %d\n",fPenalty_S6);
  
    /*
    // fPenalty_S3;
    
    if( fNumOfEvent_StudentDay[ student ][ day ] == 1 )
      ++fPenalty_S3;
    else --fPenalty_S3;
    
    // fPenalty_S2;
    time_s = day * fNumOfTimeInDay;        // start time of the day
    time_e = time_s + fNumOfTimeInDay - 1; // end time of the day

    count_p = 0;  // the number of successive events preceding to the event
    for( int t = time-1; t >= time_s; --t ){
      if( fEvent_StudentTime[ student ][ t ] != -1 )
	++count_p;
      else 
	break;
    }  

    count_n = 0;  // the number of successive events next to the event
    for( int t = time+1; t <= time_e; ++t ){
      if( fEvent_StudentTime[ student ][ t ] != -1 )
	++count_n;
      else 
	break;
    }
    fPenalty_S2 -= fDiffPenalty_S2_Insert[ count_p ][ count_n ];  

  }

  // fPenalty_S1
  slot =  time % fNumOfTimeInDay; 
  if( slot == fNumOfTimeInDay - 1 )
    fPenalty_S1 -= fNumOfStudent_Event[ event ];
  */

  //  printf("eject event%d S1=%d S2=%d S3=%d S4=%d S5=%d\n", event, fPenalty_S1, fPenalty_S2, fPenalty_S3, fPenalty_S4, fPenalty_S5);
  fPenalty_S = fPenalty_S1 + fPenalty_S2 + fPenalty_S3 + fPenalty_S4 + fPenalty_S5 + fPenalty_S6;
}



void TOperator::Insert( int event, int time, int room, int flag )
{
  int student, day;
  int professor;
  int slot, time_s, time_e;
  int count_p, count_n;
  int event1;
  double e_ave, e_day, e_diff;

  assert( 0 <= event < fNumOfEvent );
  assert( 0 <= time < fNumOfTime );
  assert( 0 <= room < fNumOfRoom );
  assert( fAvail_EventRoom[ event ][ room ] == 1 );
  assert( 0 <= ( time % fNumOfTimeInDay ) + fEvent_TimeRequest[ event ] < fNumOfTimeInDay );
  for( int tr = 0; tr < fEvent_TimeRequest[ event ]; tr++ ){//
    if(fEvent_TimeRoom[ time + tr ][ room ] != -1){
      printf("event=%d time=%d tr=%d room=%d flag=%d\n", event, time,tr, room, flag);
      printf("fEvent_TimeRoom[ time + tr ][ room ] = %d\n",fEvent_TimeRoom[ time + tr ][ room ]);
    }
    assert( fEvent_TimeRoom[ time + tr ][ room ] == -1 );//   
  }//
  //  printf("event=%d time=%d room=%d flag=%d\n", event, time, room, flag);
  assert( fTimeRoom_Event[ event ][ 0 ] == -1 ); 
  for( int r = 0; r < fNumOfRoom; ++r ){ // partial feasibility
    event1 = fEvent_TimeRoom[ time ][ r ];
    if( event1 != -1 && fConf_EventEvent[ event ][ event1 ] != 0 ){//check H1 H4
      printf("event=%d time=%d room=%d flag=%d\n", event, time, room, flag);
      printf("error event%d & event%d is conflict\n",event, event1);
      assert( 1 == 2 );
    }
  }
  
  
  fTimeRoom_Event[ event ][ 0 ] = time;
  fTimeRoom_Event[ event ][ 1 ] = room;
  for(int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr){//
    fEvent_TimeRoom[ time + tr ][ room ] = event;//
    assert(fEvent_TimeRoom[ time + tr ][ room ] == event);
  } //

  
  fListEjectEvent[ fInvEjectEvent[ event ] ] = fListEjectEvent[ fNumOfEjectEvent-1 ]; 
  fInvEjectEvent[ fListEjectEvent[ fNumOfEjectEvent-1 ] ] = fInvEjectEvent[ event ];
  fInvEjectEvent[ event ] = -1;
  --fNumOfEjectEvent;

  if( flag == 1 ){
    // for( int e = 0; e < fNumOfEvent; ++e )
    // fConf_EventTime[ e ][ time ] += fConf_EventEvent[ e ][ event ];

    // If this is not effective, the following should be replaced with the above two lines and fListConfEvent_Event[][] can be removed;
    for( int k = 0; k < fNumOfConfEvent_Event[ event ]; ++k ){ 
      event1 = fListConfEvent_Event[ event ][ k ];
      for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
      fConf_EventTime[ event1 ][ time + tr ] += fConf_EventEvent[ event1 ][ event ];
      }
    }
  }

  /* penalty_S2 _S3をあらかじめ引いておく */

  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    student = fListStudent_Event[ event ][ k ];
    e_ave = 0;
    e_day = 0;
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave += fNumOfEvent_StudentDay[ student ][ d ];
    e_ave /= fNumOfDay;
    for( int d = 0; d < fNumOfDay; ++d ){
      e_day = fNumOfEvent_StudentDay[ student ][ d ];
      e_diff = e_ave - e_day;
      //   printf("insert student %d before e_ave=%f e_day=%d\n",k , e_ave,e_day);
      
      //     if( (e_diff * e_diff) > 2 )
      fPenalty_S2 -= abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }
  for( int j = 0; j < fNumOfProf_Event[ event ]; ++j ){
    e_ave = 0;
    e_day = 0;
    professor = fListProf_Event[ event ][ j ];
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave += fNumOfEvent_ProfDay[ professor ][ d ];
    e_ave /= fNumOfDay;
    for( int d = 0; d < fNumOfDay; ++d ){
      e_day = fNumOfEvent_ProfDay[ professor ][ d ];
      e_diff = e_ave - e_day;
      //     printf("eject student %d before e_ave=%f e_day=%d\n",k , e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fPenalty_S3 -= abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }
  
  
  day = time / fNumOfTimeInDay;
  
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    // fNumOfEvent_StudentDay;
    student = fListStudent_Event[ event ][ k ];
    for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){//
      assert( fEvent_StudentTime[ student ][ time + tr ] == -1 );//
      fEvent_StudentTime[ student ][ time + tr ] = event;//
      ++fNumOfEvent_StudentDay[ student ][ day ];
    }
  }
  for( int j = 0; j < fNumOfProf_Event[ event ]; ++j ){
    //fNumOfEvent_ProfDay;
    professor = fListProf_Event[ event ][ j ];
    for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
      assert( fEvent_ProfTime[ professor ][ time + tr ] == -1 );
      fEvent_ProfTime[ professor ][ time + tr ] = event;
      ++fNumOfEvent_ProfDay[ professor ][ day ];
    }
  }
  
    /*      
    // fPenalty_S3;
    
    if( fNumOfEvent_StudentDay[ student ][ day ] == 1 )
      ++fPenalty_S3;
    else --fPenalty_S3;
    
    // fPenalty_S2;
    time_s = day * fNumOfTimeInDay;        // start time of the day
    time_e = time_s + fNumOfTimeInDay - 1; // end time of the day

    count_p = 0;  // the number of successive events preceding to the event
    for( int t = time-1; t >= time_s; --t ){
      if( fEvent_StudentTime[ student ][ t ] != -1 )
	++count_p;
      else 
	break;
    }  

    count_n = 0;  // the number of successive events next to the event
    for( int t = time+1; t <= time_e; ++t ){
      if( fEvent_StudentTime[ student ][ t ] != -1 )
	++count_n;
      else 
	break;
    }
    fPenalty_S2 += fDiffPenalty_S2_Insert[ count_p ][ count_n ];  

  }

  // fPenalty_S1
  slot =  time % fNumOfTimeInDay; 
  if( slot == fNumOfTimeInDay - 1 )
    fPenalty_S1 += fNumOfStudent_Event[ event ];
  */


  
  slot =  time % fNumOfTimeInDay;
  // fPenalty_S4
  if( slot < 2 && (slot + fEvent_TimeRequest[ event ]) > 2){
    fPenalty_S4 += 1000;
    //   printf("event %d in slot is %d\n",event,slot);
  }
  // fPenalty_S1
  for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
    if( slot == fNumOfTimeInDay - 1 || slot == 0)
      fPenalty_S1 += fNumOfStudent_Event[ event ];
    slot++;
    
  }


  // fPenalty_S2 生徒の同日における授業数の偏り
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    student = fListStudent_Event[ event ][ k ];
    e_ave = 0;
    e_day = 0;
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave += fNumOfEvent_StudentDay[ student ][ d ];
    e_ave /= fNumOfDay;

    for( int d = 0; d < fNumOfDay; ++d ){
      e_day = fNumOfEvent_StudentDay[ student ][ d ];
      e_diff = e_ave - e_day;
      //      printf("insert student %d after e_ave=%f e_day=%f\n",fListStudent_Event[event][k], e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fPenalty_S2 +=abs( (int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }

   // fPenalty_S3
  for( int j = 0; j < fNumOfProf_Event[ event ]; ++j ){
    e_ave = 0;
    e_day = 0;
    professor = fListProf_Event[ event ][ j ];
    for( int d = 0; d < fNumOfDay; ++d)
      e_ave += fNumOfEvent_ProfDay[ professor ][ d ];
    e_ave /= fNumOfDay;
    
    for( int d = 0; d < fNumOfDay; ++d ){
      e_day = fNumOfEvent_ProfDay[ professor ][ d ];
      e_diff = e_ave - e_day;
      //     printf("eject student %d after e_ave=%f e_day=%d\n",k,e_ave,e_day);
      //      if( (e_diff * e_diff) > 2 )
      fPenalty_S3 += abs((int)e_diff);	// 他のペナルティとのバランスを考える
    }
  }
  
  //fPenalty_S4 はS1の直前
  
  // fPenalty_S5
  for( int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
    for( int r = 0; r < fNumOfRoom; ++r){
      if( r != room && fEvent_Required[ event ] != 0 && fEvent_Required[ event ] ==  fEvent_Required[ fEvent_TimeRoom[time+tr][r]] )
        fPenalty_S5 += 500;
    }
  }

  
  //  fPenalty_S6
  for( int p = 0; p < fNumOfProf_Event[ event ]; ++p ){
    professor = fListProf_Event[ event ][ p ];
    for(int tr = 0; tr < fEvent_TimeRequest[ event ]; ++tr ){
      if( fProfCantDo[ professor ][ time + tr ] == 1){
        fPenalty_S6+=100;
      }
    }
  }
  //  printf("fPenalty_S6 = %d\n",fPenalty_S6);
  
  //  printf("insert event%d S1=%d S2=%d S3=%d S4=%d S5=%d\n", event, fPenalty_S1, fPenalty_S2, fPenalty_S3, fPenalty_S4, fPenalty_S5);
  fPenalty_S = fPenalty_S1 + fPenalty_S2 + fPenalty_S3 + fPenalty_S4 + fPenalty_S5 + fPenalty_S6;


}

