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
  // indi.fNumOfEvent = fNumOfEvent;     
  indi.fNumOfEjectEvent = fNumOfEjectEvent;     
  indi.fPenalty_S = fPenalty_S;
  indi.fPenalty_S1 = fPenalty_S1;
  indi.fPenalty_S2 = fPenalty_S2;
  indi.fPenalty_S3 = fPenalty_S3;
  indi.fEvaluationValue = (double)fPenalty_S;  // depend on the strategy

  for( int e = 0; e < fNumOfEvent; ++e ){
    indi.fTimeRoom_Event[ e ][ 0 ] = fTimeRoom_Event[ e ][ 0 ];
    indi.fTimeRoom_Event[ e ][ 1 ] = fTimeRoom_Event[ e ][ 1 ];
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
  fNumOfTime = eval->fNumOfTime;
  fNumOfDay = eval->fNumOfDay;
  fNumOfTimeInDay = eval->fNumOfTimeInDay;
  fAvail_EventRoom = eval->fAvail_EventRoom;
  fConf_EventEvent = eval->fConf_EventEvent;
  fListStudent_Event = eval->fListStudent_Event;
  fNumOfStudent_Event = eval->fNumOfStudent_Event;
  fListConfEvent_Event = eval->fListConfEvent_Event;
  fNumOfConfEvent_Event = eval->fNumOfConfEvent_Event;
  fDiffPenalty_S2_Insert = eval->fDiffPenalty_S2_Insert;

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

  fNumOfEvent_StudentDay = new int* [ fNumOfStudent ];
  for( int s = 0; s < fNumOfStudent; ++s )
    fNumOfEvent_StudentDay[ s ] = new int [ fNumOfDay ];

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
	assert( fTimeRoom_Event[ event ][ 0 ] == t );
	assert( fTimeRoom_Event[ event ][ 1 ] ==  r );
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
	assert( t == time );
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

  this->CalEvaluation();
  assert( penalty_S1 == fPenalty_S1 );
  assert( penalty_S2 == fPenalty_S2 );
  assert( penalty_S3 == fPenalty_S3 );
  assert( penalty_S1 + penalty_S2 + penalty_S3 == fPenalty_S );
}

void TOperator::CalEvaluation()
{
  int penalty_H1, penalty_S1, penalty_S2, penalty_S3;
  int time;
  int count;


  // fPenalty_S1
  penalty_S1 = 0;
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int d = 0; d < fNumOfDay; ++d ){
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

  fPenalty_S = penalty_S1 + penalty_S2 + penalty_S3;
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
  int count_p, count_n;

  day = time / fNumOfTimeInDay; 
  slot =  time % fNumOfTimeInDay; 

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

  
  fDiffPenalty_S = fDiffPenalty_S1 + fDiffPenalty_S2 + fDiffPenalty_S3;

  return fDiffPenalty_S;
}


void TOperator::ResetSol()
{
  fPenalty_S = 0;
  fPenalty_S1 = 0;
  fPenalty_S2 = 0;
  fPenalty_S3 = 0;

  for( int e = 0; e < fNumOfEvent; ++e ){
    fTimeRoom_Event[ e ][ 0 ] = -1;
    fTimeRoom_Event[ e ][ 1 ] = -1;
  }

  for( int t = 0; t < fNumOfTime; ++t )
    for( int r = 0; r < fNumOfRoom; ++r )
      fEvent_TimeRoom[ t ][ r ] = -1;

  tRand->Permutation( fListEjectEvent, fNumOfEvent, fNumOfEvent ); 
  for( int i = 0; i < fNumOfEvent; ++i )
    fInvEjectEvent[ fListEjectEvent[ i ] ] = i;

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

  for( int e = 0; e < fNumOfEvent; ++e )
    for( int t = 0; t < fNumOfTime; ++t )
      fConf_EventTime[ e ][ t ] = 0;
}

void TOperator::Eject( int event, int flag )
{
  int student, time, room, day;
  int slot, time_s, time_e;
  int count_p, count_n;
  int event1;

  assert( 0 <= event < fNumOfEvent );
  assert( fTimeRoom_Event[ event ][ 0 ] != -1 );

  time = fTimeRoom_Event[ event ][ 0 ];
  room = fTimeRoom_Event[ event ][ 1 ];
  fEvent_TimeRoom[ time ][ room ] = -1;

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
      fConf_EventTime[ event1 ][ time ] -= fConf_EventEvent[ event1 ][ event ];      
    }
  }
  
  day = time / fNumOfTimeInDay;  
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    // fNumOfEvent_StudentDay;
    student = fListStudent_Event[ event ][ k ];
    assert( fEvent_StudentTime[ student ][ time ] != -1 );
    fEvent_StudentTime[ student ][ time ] = -1;
    --fNumOfEvent_StudentDay[ student ][ day ];
    
    // fPenalty_S3;
    if( fNumOfEvent_StudentDay[ student ][ day ] == 1 )
      ++fPenalty_S3;
    else if( fNumOfEvent_StudentDay[ student ][ day ] == 0 )
      --fPenalty_S3;

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

  fPenalty_S = fPenalty_S1 + fPenalty_S2 + fPenalty_S3;
}



void TOperator::Insert( int event, int time, int room, int flag )
{
  int student, day;
  int slot, time_s, time_e;
  int count_p, count_n;
  int event1;

  assert( 0 <= event < fNumOfEvent );
  assert( 0 <= time < fNumOfTime );
  assert( 0 <= room < fNumOfRoom );
  assert( fAvail_EventRoom[ event ][ room ] == 1 );
  assert( fEvent_TimeRoom[ time ][ room ] == -1 );
  assert( fTimeRoom_Event[ event ][ 0 ] == -1 ); 

  for( int r = 0; r < fNumOfRoom; ++r ){ // partial feasibility
    event1 = fEvent_TimeRoom[ time ][ r ];
    if( event1 != -1 && fConf_EventEvent[ event ][ event1 ] != 0 )
      assert( 1 == 2 );
  }

  fTimeRoom_Event[ event ][ 0 ] = time;
  fTimeRoom_Event[ event ][ 1 ] = room;
  fEvent_TimeRoom[ time ][ room ] = event;

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
      fConf_EventTime[ event1 ][ time ] += fConf_EventEvent[ event1 ][ event ];      
    }
  }

  day = time / fNumOfTimeInDay;  
  for( int k = 0; k < fNumOfStudent_Event[ event ]; ++k ){
    // fNumOfEvent_StudentDay;
    student = fListStudent_Event[ event ][ k ];
    assert( fEvent_StudentTime[ student ][ time ] == -1 );
    fEvent_StudentTime[ student ][ time ] = event;
    ++fNumOfEvent_StudentDay[ student ][ day ];

    // fPenalty_S3;
    if( fNumOfEvent_StudentDay[ student ][ day ] == 1 )
      ++fPenalty_S3;
    else if( fNumOfEvent_StudentDay[ student ][ day ] == 2 )
      --fPenalty_S3;

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

  fPenalty_S = fPenalty_S1 + fPenalty_S2 + fPenalty_S3;


}

