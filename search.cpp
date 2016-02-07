#ifndef __SEARCH__
#include "search.h"
#endif


TSearch::TSearch() : TOperator()
{

}

TSearch::~TSearch()
{

}

void TSearch::Set( TEvaluator* eval ) 
{
  TOperator::Set( eval );
  tIndi_LS.Define( fNumOfEvent );

  fMoved_EventTime = new int* [ fNumOfEvent ];
  for( int e = 0; e < fNumOfEvent; ++e )
    fMoved_EventTime[ e ] = new int [ fNumOfTime ];


  fMoved_EventTimeRoom = new int** [ fNumOfEvent ];
  for( int e = 0; e < fNumOfEvent; ++e ){
    fMoved_EventTimeRoom[ e ] = new int* [ fNumOfTime ];
    for( int t = 0; t < fNumOfTime; ++t ){
      fMoved_EventTimeRoom[ e ][ t ] = new int [ fNumOfRoom ];
    }
  }

}

void TSearch::SetParameter( int* para )
{
  TOperator::SetParameter();
  // fMaxNumOfIterLS = 1000;
  //  fTabuTenure = 150;
  fTabuTenure = para[ 0 ];
  fRadioActiveEvent = para[ 1 ];
  fMaxNumOfIterLS = para[ 2 ];
}


// create an random complete solution allowing infeasible solution
void TSearch::MakeInitRandSol()
{
  int event, time, room;
  int permu[ fNumOfEvent ];
  int find;
  
  this->ResetSol();

  tRand->Permutation( permu, fNumOfEvent, fNumOfEvent );
  for( int i = 0; i < fNumOfEvent; ++i )
  {
    event = permu[ i ];

    find = 0;
    time = tRand->Integer( 0, fNumOfTime-1 );
    for( int h1 = 0; h1 < fNumOfTime; ++h1 ){
      room = tRand->Integer( 0, fNumOfRoom-1 );
      for( int h2 = 0; h2 < fNumOfRoom; ++h2 ){
	if( fAvail_EventRoom[ event ][ room ] == 1 && fEvent_TimeRoom[ time ][ room ] == -1 ){
	  find = 1;
	  goto EEE;
	}
      ++room;
      if( room == fNumOfRoom ) 
	room = 0;
      }
      ++time;
      if( time == fNumOfTime ) 
	time = 0;
    }

  EEE:;
    if( find == 1 ){
      this->Insert( event, time, room, 1 ); // error may happen (illegal inseartion will be tested)
      this->CheckValid();
    }
  }
}


// Minimize fNumOfEjectEvent in the partial feasible solution space
void TSearch::LS_MakeFeasible()
{
  int eventIn, timeIn, roomIn;
  int event, time, room;
  int diff, diffMin;
  int numOfCandi;
  int candi[ 3000 ];
  int rr;
  int tmp;
  int numOfEjectOut;
  int flagRoom[ fNumOfRoom ]; // 1: H1に違反する 0: 違反しない
  int numOfFlagRoom;
  int diffPenalty_S;
  int professor;
  int cantdoevent;
  int eventcount;
  if( fNumOfEjectEvent == 0 )
    return;

  fNumOfIterLS = 0;
  this->TransToIndi( tIndi_LS );

  for( int i = 0; i < fNumOfEvent; ++i )
    for( int t = 0; t < fNumOfTime; ++t )
      fMoved_EventTime[ i ][ t ] = fNumOfIterLS; // Event Timeのみ(room は考慮していない)
  printf("LS_MakeFeasible()\n");
  while( 1 )
  {
    ++fNumOfIterLS;
  
    // if( fNumOfIterLS % 1 == 0 )
    // printf( "iter = %d: %d \n", fNumOfIterLS, fNumOfEjectEvent ); fflush( stdout );

    diffMin = 99999999;   
    numOfCandi = 0;
    for( int h = 0; h < fNumOfEjectEvent; ++h ){
      eventIn = fListEjectEvent[ h ];                         // イジェクトリストから授業を排出
      //      printf("%d request %d timeslot\n",eventIn, fEvent_TimeRequest[ eventIn ]);


      for( int t = 0; t < fNumOfTime; ++t ){

        if( (t % fNumOfTimeInDay) + fEvent_TimeRequest[ eventIn ] -1  < fNumOfTimeInDay ){  // H6 check
	  cantdoevent = 0;
	}else{
	  //	  printf("no time for event in eventIn = %d  time = %d tr = %d\n", eventIn, t, fEvent_TimeRequest[ eventIn ]);
	  continue;
	}
	
	for( int p = 0; p < fNumOfProf_Event[ eventIn ]; ++p ){  // H5 check
	  professor = fListProf_Event[ eventIn ][ p ];
	  for( int tr = 0; tr < fEvent_TimeRequest[ eventIn ]; ++tr ){
	    if( fProfCantDo[ professor ][ t + tr ] == 1){
	      cantdoevent = 1;
	      break;
	    }
    }
	}
	if( cantdoevent == 1 ){
	  //	  printf("professor can not do event on time in eventIn = %d  time = %d is skiped\n", eventIn, t);
	  continue;
	}

	diffPenalty_S = this->DiffPenalty_S_Insert( eventIn, t );   /////// test
	numOfFlagRoom = 0;	
	for( int r = 0; r < fNumOfRoom; ++r ){
	  for( int tr = 0; tr < fEvent_TimeRequest[ eventIn ]; ++tr ){
	    event = fEvent_TimeRoom[ t + tr ][ r ];                        // 授業の競合チェック
	    if( event != -1 && fConf_EventEvent[ eventIn ][ event ] != 0 ){ // その時間に授業が入ってる かつ 授業が競合
	      flagRoom[ r ] = 1;  // この部屋には競合する授業がある
	      ++numOfFlagRoom;    // その時間に入れると競合する授業が入っている部屋の数
        break;
	    }else{
        flagRoom[ r ] = 0;
      }
    }
	}
	//	printf("%d   ",numOfFlagRoom); //check
	
	for( int r = 0; r < fNumOfRoom; ++r ){	  
	  if( fAvail_EventRoom[ eventIn ][ r ] == 1 ){             // その部屋が使えて
	    eventcount = 0;
	    for( int tr = 0; tr < fEvent_TimeRequest[ eventIn ]; ++tr ){
	      if( fEvent_TimeRoom[ t + tr ][ r ] != -1 ){
          eventcount++;
          break;
        }
	    }
	    if( eventcount == 0 )                  // その部屋に授業がない
	      numOfEjectOut = numOfFlagRoom;                       // 0が入る
	    else if( flagRoom[ r ] == 1 )                          // その部屋に授業あるしその授業と競合する
	      numOfEjectOut = numOfFlagRoom;                       // 競合する部屋数が代入
	    else 
	      numOfEjectOut = numOfFlagRoom + 1;                   // その部屋に授業はあるけどその授業とは競合しない 
	  }
	  else 
	    numOfEjectOut = 99999999 + 10;
	  
	  if( numOfEjectOut == 99999999 + 10 )
	    diff = 99999999 + 10;
	  else{
	    --numOfEjectOut;     // EventInを考慮
	    // diff = 100 * numOfEjectOut + diffPenalty_S;  // evaluation function
	    diff = numOfEjectOut;
	  }
	 	  
    //	  	  printf("%d\n",numOfEjectOut);
	  
	  if( diff < diffMin ){   //競合する部屋が他よりも少ない
	    if( fNumOfIterLS > fMoved_EventTime[ eventIn ][ t ] || fNumOfEjectEvent + numOfEjectOut < tIndi_LS.fNumOfEjectEvent ){
	      diffMin = diff;
	      candi[ 0 ] = eventIn;
	      candi[ 1 ] = t;
	      candi[ 2 ] = r;
	      numOfCandi = 1;
	    }
	  }
	  else if( diff == diffMin && numOfCandi < 1000 ){
	    if( fNumOfIterLS > fMoved_EventTime[ eventIn ][ t ] || fNumOfEjectEvent + numOfEjectOut < tIndi_LS.fNumOfEjectEvent ){
	      candi[ 3*numOfCandi ] = eventIn;
	      candi[ 3*numOfCandi+1 ] = t;
	      candi[ 3*numOfCandi+2 ] = r;
	      ++numOfCandi;
	    }
	  }
	}	  
      }
    }
    
    //    printf("%d\n",numOfCandi);
    //    assert( numOfCandi != 0 );   
    
    if( numOfCandi == 0){//初期解生成が詰んだとき
      printf("numOfCandi = %d\n",numOfCandi);
      printf("eventIn=%d\n",eventIn);
      printf("fNumOfEjectEvent = %d\n",fNumOfEjectEvent);
      printf("fListEjectEvent[ 0 ] = %3d \n",fListEjectEvent[ 0 ]);
      printf("r\\t ");
      for(int t = 0; t < fNumOfTime; ++t)
        printf("%3d ",t);
      printf("\n");
      for( int r = 0; r < fNumOfRoom; ++r ){
        printf("%3d ",r);
        for( int t = 0; t < fNumOfTime; ++t ){
          printf("%3d ",fEvent_TimeRoom[ t ][ r ]);
        }
        printf("\n");
      }
      fflush(stdout);   
      return;
    }
    
    rr = rand() % numOfCandi;
    eventIn = candi[ 3*rr ];
    timeIn = candi[ 3*rr+1 ];
    roomIn = candi[ 3*rr+2 ];

    fMoved_EventTime[ eventIn ][ timeIn ] = fNumOfIterLS + 100; // parameter


    //    tmp = fNumOfEjectEvent;                      // for check
    for( int tr = 0; tr < fEvent_TimeRequest[ eventIn ]; ++tr ){
      event = fEvent_TimeRoom[ timeIn + tr ][ roomIn ];
      if( event != -1 ){
        //        printf("eject1\n");
        this->Eject( event, 1 );
      }
      
      for( int r = 0; r < fNumOfRoom; ++r ){
        event = fEvent_TimeRoom[ timeIn + tr ][ r ];
        if( r != roomIn && event != -1 && fConf_EventEvent[ eventIn ][ event ] != 0 ){
          //          printf("eject2\n");
          this->Eject( event, 1 );
        }
      }
    }
    //    printf("Insert( %d %d %d)\n", eventIn, timeIn, roomIn);
    this->Insert( eventIn, timeIn, roomIn, 1 );    
    //    assert( tmp + diffMin == fNumOfEjectEvent ); // for check
    //}

    if( fNumOfEjectEvent < tIndi_LS.fNumOfEjectEvent )    
      this->TransToIndi( tIndi_LS );

    if( fNumOfEjectEvent == 0 )
    break;
    //if( fNumOfIterLS > fMaxNumOfIterLS )
    // break;
    if(fNumOfIterLS > 100000 ){
      printf("fail MakeFeasible()\n");
      printf("r\\t ");
      for(int t = 0; t < fNumOfTime; ++t)
        printf("%3d ",t);
      printf("\n");
      for( int r = 0; r < fNumOfRoom; ++r ){
        printf("%3d ",r);
        for( int t = 0; t < fNumOfTime; ++t ){
          printf("%3d ",fEvent_TimeRoom[ t ][ r ]);
        }
        printf("\n");
      }
      fflush(stdout);    
      return;
    }
    this->CheckValid(); 
  }
  
  printf("make sol\n");
  printf("r\\t ");
  for(int t = 0; t < fNumOfTime; ++t)
    printf("%3d ",t);
  printf("\n");
  for( int r = 0; r < fNumOfRoom; ++r ){
    printf("%3d ",r);
    for( int t = 0; t < fNumOfTime; ++t ){
      printf("%3d ",fEvent_TimeRoom[ t ][ r ]);
    }
	printf("\n");
  }
  fflush(stdout);
  
  this->TransToIndi( tIndi_LS );  
}

void TSearch::Eject_NoStudentEvent()
{
  int numOfNoStudentEvent = 0;
  for( int e = 0; e < fNumOfEvent; ++e ){ 
    if( fNumOfStudent_Event[ e ] == 0 ){
      this->Eject( e, 1 );
      ++numOfNoStudentEvent;
    }
  }
  printf( "num of no-student envents = %d \n", numOfNoStudentEvent );
}


void TSearch::Insert_NoStudentEvent()
{
  int success;

  for( int e = 0; e < fNumOfEvent; ++e ){ 
    if( fNumOfStudent_Event[ e ] == 0 ){
      success = 0;
      for( int t = 0; t < fNumOfTime; ++t ){
	for( int r = 0; r < fNumOfRoom; ++r ){
	  if( fEvent_TimeRoom[ t ][ r ] == -1 && fAvail_EventRoom[ e ][ r ] == 1 ){
	    this->Insert( e, t, r, 1 );
	    success = 1;
	    goto EEE0;
	  }
	}
      }
    EEE0:;
      if( success != 1 )
	printf( "no-student event cannot be inserted \n" );
    }
  }
}


// Minimize fPenalty_S in the feasible partial (and complete) solution space (Relocation)
void TSearch::LS_Relocation()
{
  int eventEj, timeEj, roomEj;
  int event, time, room;
  int penalty_S_before, penalty_S_after;
  int penalty_S3_before, penalty_S3_after;
  int diff, diffMin;
  int numOfCandi;
  int candi[ 3000 ];
  int rr;

  fNumOfIterLS = 0;
  this->TransToIndi( tIndi_LS );

   for( int i = 0; i < fNumOfEvent; ++i ){
    for( int t = 0; t < fNumOfTime; ++t ){
      fMoved_EventTime[ i ][ t ] = fNumOfIterLS; // Event Timeのみ(room は考慮していない)
      for( int r = 0; r < fNumOfRoom; ++r ){
	fMoved_EventTimeRoom[ i ][ t ][ r ] = fNumOfIterLS; 
      }
    }
  }

  while( 1 )
  {
    ++fNumOfIterLS;
    // printf( "iter = %d -- %d: %d = %d %d %d\n", fNumOfIterLS, tIndi_LS.fPenalty_S, fPenalty_S, fPenalty_S1, fPenalty_S2, fPenalty_S3 ); fflush( stdout );

    penalty_S_before = fPenalty_S; 
    diffMin = 99999999;   
    numOfCandi = 0;

    for( int e = 0; e < fNumOfEvent; ++e ){
      eventEj = e;
      timeEj = fTimeRoom_Event[ eventEj ][ 0 ];
      roomEj = fTimeRoom_Event[ eventEj ][ 1 ];
      if( timeEj == -1 )
	goto EEE0;

      this->Eject( eventEj, 0 );
      
      for( int t = 0; t < fNumOfTime; ++t ){
	
	// if( fConf_EventTime[ eventEj ][ t ] != 0 || t == timeEj ){ // R0
	if( fConf_EventTime[ eventEj ][ t ] != 0 && t != timeEj ){    // equevalent to ver1 (R1)
	  goto EEE1;
	}

	for( int r = 0; r < fNumOfRoom; ++r ){
	  if( fAvail_EventRoom[ eventEj ][ r ] == 0 || fEvent_TimeRoom[ t ][ r ] != -1 )
	    goto EEE2;  // この(time,room)にはeventEjを挿入できない
	  if( t == timeEj && r == roomEj )
	    goto EEE2;
	  
	  diff = this->DiffPenalty_S_Insert( eventEj, t ) + fPenalty_S - penalty_S_before; 

	  if( diff < diffMin ){
	    if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	      diffMin = diff;
	      candi[ 0 ] = eventEj;
	      candi[ 1 ] = t;
	      candi[ 2 ] = r;
	      numOfCandi = 1;

	    }
	  }
	  else if( diff == diffMin && numOfCandi < 1000 ){
	    if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	      candi[ 3*numOfCandi ] = eventEj;
	      candi[ 3*numOfCandi+1 ] = t;
	      candi[ 3*numOfCandi+2 ] = r;
	      ++numOfCandi;
	    }
	  }
	EEE2:;
	}
      EEE1:;
      }

      this->Insert( eventEj, timeEj, roomEj, 0 );
    EEE0:;
    }

    if( numOfCandi != 0 )
    {   
      rr = rand() % numOfCandi;
      event = candi[ 3*rr ];
      time = candi[ 3*rr+1 ];
      room = candi[ 3*rr+2 ];

      fMoved_EventTime[ event ][ fTimeRoom_Event[ event ][ 0 ] ] = fNumOfIterLS + fTabuTenure;
      this->Eject( event, 1 );
      this->Insert( event, time, room, 1 );
      assert( penalty_S_before + diffMin == fPenalty_S );  // for check
    }

    if( fPenalty_S < tIndi_LS.fPenalty_S )    
      fNumOfIterFindBest = fNumOfIterLS;
    if( fPenalty_S < tIndi_LS.fPenalty_S ){        // "<" should be used in normal cases
      this->TransToIndi( tIndi_LS );
    }

    if( fPenalty_S == 0 ) 
      break;
    if( fNumOfIterLS > fMaxNumOfIterLS )
      break;

    this->CheckValid(); 
  }

  this->TransFromIndi( tIndi_LS );
}


// Minimize fPenalty_S in the feasible partial (and complete) solution space (Relocation+Swap)
void TSearch::LS_Relocation_Swap() 
{
  int eventEj, timeEj, roomEj;
  int event, time, room;
  int penalty_S_before, penalty_S_after;
  int penalty_S3_before, penalty_S3_after;
  int diff, diffMin;
  int numOfCandi;
  int candi[ 3000 ];
  int rr;
  int event1, time1, room1, event2, time2, room2;

  fNumOfIterLS = 0;
  this->TransToIndi( tIndi_LS );

   for( int i = 0; i < fNumOfEvent; ++i ){
    for( int t = 0; t < fNumOfTime; ++t ){
      fMoved_EventTime[ i ][ t ] = fNumOfIterLS; // Event Timeのみ(room は考慮していない)
      for( int r = 0; r < fNumOfRoom; ++r ){
	fMoved_EventTimeRoom[ i ][ t ][ r ] = fNumOfIterLS; 
      }
    }
  }


  while( 1 )
  {
    ++fNumOfIterLS;
    // printf( "iter = %d -- %d: %d = %d %d %d\n", fNumOfIterLS, tIndi_LS.fPenalty_S, fPenalty_S, fPenalty_S1, fPenalty_S2, fPenalty_S3 ); fflush( stdout );

    penalty_S_before = fPenalty_S; 
    diffMin = 99999999;   
    numOfCandi = 0;

    // Relocation (start)
    for( int e = 0; e < fNumOfEvent; ++e ){ 
      eventEj = e;
      timeEj = fTimeRoom_Event[ eventEj ][ 0 ];
      roomEj = fTimeRoom_Event[ eventEj ][ 1 ];
      if( timeEj == -1 )
	goto EEE0;

      this->Eject( eventEj, 0 );
      
      for( int t = 0; t < fNumOfTime; ++t ){

	// if( fConf_EventTime[ eventEj ][ t ] != 0 || t == timeEj ){  //  R0 
	if( fConf_EventTime[ eventEj ][ t ] != 0 && t != timeEj ){     // equevalent to ver1 (R1)
	  goto EEE1;
	}

	for( int r = 0; r < fNumOfRoom; ++r ){
	  if( fAvail_EventRoom[ eventEj ][ r ] == 0 || fEvent_TimeRoom[ t ][ r ] != -1 )
	    goto EEE2;  // この(time,room)にはeventEjを挿入できない
	  if( t == timeEj && r == roomEj )
	    goto EEE2;
	  
	  diff = this->DiffPenalty_S_Insert( eventEj, t ) + fPenalty_S - penalty_S_before; 

	  if( diff < diffMin ){
	    if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	      diffMin = diff;
	      candi[ 0 ] = eventEj;
	      candi[ 1 ] = t;
	      candi[ 2 ] = r;
	      numOfCandi = 1;

	    }
	  }
	  else if( diff == diffMin && numOfCandi < 1000 ){
	    if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	      candi[ 3*numOfCandi ] = eventEj;
	      candi[ 3*numOfCandi+1 ] = t;
	      candi[ 3*numOfCandi+2 ] = r;
	      ++numOfCandi;
	    }
	  }
	EEE2:;
	}
      EEE1:;
      }

      this->Insert( eventEj, timeEj, roomEj, 0 );
    EEE0:;
    }
    // Relocation (end)
    
    // Swap (start)
    for( int e1 = 0; e1 < fNumOfEvent-1; ++e1 ){ 
      for( int e2 = e1+1; e2 < fNumOfEvent; ++e2 ){
	event1 = e1;
	time1 = fTimeRoom_Event[ event1 ][ 0 ];
	room1 = fTimeRoom_Event[ event1 ][ 1 ];
	event2 = e2;
	time2 = fTimeRoom_Event[ event2 ][ 0 ];
	room2 = fTimeRoom_Event[ event2 ][ 1 ];
	if( time1 == -1 || time2 == -1 )
	  goto FFF0;

	if( fAvail_EventRoom[ event1 ][ room2 ] == 0 || fAvail_EventRoom[ event2 ][ room1 ] == 0 )
	  goto FFF0; 


	if( fConf_EventTime[ event1 ][ time2 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
	  goto FFF0;  // このtimeにはeventEjを挿入できない
	if( fConf_EventTime[ event2 ][ time1 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
	  goto FFF0;  // このtimeにはeventEjを挿入できない
	if( time1 == time2 )  // これはenpty eventが存在している時のみ発生 // not used in ver1
	  goto FFF0;
	
	this->Eject( event1, 0 );
	this->Eject( event2, 0 );
	this->Insert( event1, time2, room2, 0 );
	this->Insert( event2, time1, room1, 0 );
      
	diff = fPenalty_S - penalty_S_before; 

	if( diff < diffMin ){
	  if( fNumOfIterLS > fMoved_EventTime[ event1 ][ time2 ] || fNumOfIterLS > fMoved_EventTime[ event2 ][ time1 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	    diffMin = diff;
	    candi[ 0 ] = event1;
	    candi[ 1 ] = event2;
	    candi[ 2 ] = -1;   // meaning Swap move
	    numOfCandi = 1;
	  }
	}
	else if( diff == diffMin && numOfCandi < 1000 ){
	  if( fNumOfIterLS > fMoved_EventTime[ event1 ][ time2 ] ||  fNumOfIterLS > fMoved_EventTime[ event2 ][ time1 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	    candi[ 3*numOfCandi ] = event1;
	    candi[ 3*numOfCandi+1 ] = event2;
	    candi[ 3*numOfCandi+2 ] = -1;
	    ++numOfCandi;
	  }
	}

	this->Eject( event1, 0 );
	this->Eject( event2, 0 );
	this->Insert( event1, time1, room1, 0 );
	this->Insert( event2, time2, room2, 0 );
	assert( fPenalty_S == penalty_S_before );

      FFF0:;
      }    
    }
    // Swap (end)

    if( numOfCandi != 0 )
    {  
      rr = rand() % numOfCandi;
      room = candi[ 3*rr+2 ];

      if( room != -1 ){ // Relocation
	event = candi[ 3*rr ];
	time = candi[ 3*rr+1 ];
	room = candi[ 3*rr+2 ];

	fMoved_EventTime[ event ][ fTimeRoom_Event[ event ][ 0 ] ] = fNumOfIterLS + fTabuTenure;
	this->Eject( event, 1 );
	this->Insert( event, time, room, 1 );
	assert( penalty_S_before + diffMin == fPenalty_S );  // for check
      }
      else{  // Swap
	event1 = candi[ 3*rr ];
	event2 = candi[ 3*rr+1 ];
	time1 = fTimeRoom_Event[ event1 ][ 0 ];
	room1 = fTimeRoom_Event[ event1 ][ 1 ];
	time2 = fTimeRoom_Event[ event2 ][ 0 ];
	room2 = fTimeRoom_Event[ event2 ][ 1 ];

	fMoved_EventTime[ event1 ][ time1 ] = fNumOfIterLS + fTabuTenure;
	fMoved_EventTime[ event2 ][ time2 ] = fNumOfIterLS + fTabuTenure;
	this->Eject( event1, 1 );
	this->Eject( event2, 1 );
	this->Insert( event1, time2, room2, 1 );
	this->Insert( event2, time1, room1, 1 );
      }
    }
    
    if( fPenalty_S < tIndi_LS.fPenalty_S )    
      fNumOfIterFindBest = fNumOfIterLS;
    if( fPenalty_S < tIndi_LS.fPenalty_S ){        // "<" should be used in normal cases
      this->TransToIndi( tIndi_LS );
    }

    if( fPenalty_S == 0 ) 
      break;
    if( fNumOfIterLS > fMaxNumOfIterLS )
      break;

    this->CheckValid(); 
  }

  this->TransFromIndi( tIndi_LS );
}


// time1 と time2間でのswap (e.g. event1をtime2のどのroomに割当てても良い)
/*
void TSearch::LS_Relocation_Swap_Extend() 
{
  int eventEj, timeEj, roomEj;
  int event, time, room;
  int penalty_S_before, penalty_S_after;
  int penalty_S3_before, penalty_S3_after;
  int diff, diffMin;
  int numOfCandi;
  int candi[ 4000 ];
  int rr;
  int event1, time1, room1, event2, time2, room2;
  int numCandiRoomIn1, numCandiRoomIn2;
  int candiRoomIn1[ fNumOfRoom ], candiRoomIn2[ fNumOfRoom ]; 
  int roomIn1, roomIn2;

  fNumOfIterLS = 0;
  this->TransToIndi( tIndi_LS );

   for( int i = 0; i < fNumOfEvent; ++i ){
    for( int t = 0; t < fNumOfTime; ++t ){
      fMoved_EventTime[ i ][ t ] = fNumOfIterLS; // Event Timeのみ(room は考慮していない)
      for( int r = 0; r < fNumOfRoom; ++r ){
	fMoved_EventTimeRoom[ i ][ t ][ r ] = fNumOfIterLS; 
      }
    }
  }


  while( 1 )
  {
    ++fNumOfIterLS;
    // printf( "iter = %d -- %d: %d = %d %d %d\n", fNumOfIterLS, tIndi_LS.fPenalty_S, fPenalty_S, fPenalty_S1, fPenalty_S2, fPenalty_S3 ); fflush( stdout );

    penalty_S_before = fPenalty_S; 
    diffMin = 99999999;   
    numOfCandi = 0;

    // Relocation (start)
    for( int e = 0; e < fNumOfEvent; ++e ){ 
      eventEj = e;
      timeEj = fTimeRoom_Event[ eventEj ][ 0 ];
      roomEj = fTimeRoom_Event[ eventEj ][ 1 ];
      if( timeEj == -1 )
	goto EEE0;

      this->Eject( eventEj, 0 );
      
      for( int t = 0; t < fNumOfTime; ++t ){

	// if( fConf_EventTime[ eventEj ][ t ] != 0 || t == timeEj ){ // R0
	if( fConf_EventTime[ eventEj ][ t ] != 0 && t != timeEj ){  // equevalent to ver1 (R1)
	  goto EEE1;
	}

	for( int r = 0; r < fNumOfRoom; ++r ){
	  if( fAvail_EventRoom[ eventEj ][ r ] == 0 || fEvent_TimeRoom[ t ][ r ] != -1 )
	    goto EEE2;  // この(time,room)にはeventEjを挿入できない
	  if( t == timeEj && r == roomEj )
	    goto EEE2;
	  
	  diff = this->DiffPenalty_S_Insert( eventEj, t ) + fPenalty_S - penalty_S_before; 

	  if( diff < diffMin ){
	    if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	      diffMin = diff;
	      candi[ 0 ] = eventEj;
	      candi[ 1 ] = t;
	      candi[ 2 ] = r;
	      candi[ 3 ] = -1;  //meaning relocation
	      numOfCandi = 1;

	    }
	  }
	  else if( diff == diffMin && numOfCandi < 1000 ){
	    if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	      candi[ 4*numOfCandi ] = eventEj;
	      candi[ 4*numOfCandi+1 ] = t;
	      candi[ 4*numOfCandi+2 ] = r;
	      candi[ 4*numOfCandi+3 ] = -1;
	      ++numOfCandi;
	    }
	  }
	EEE2:;
	}
      EEE1:;
      }

      this->Insert( eventEj, timeEj, roomEj, 0 );
    EEE0:;
    }
    // Relocation (end)

    // Swap (start)
    for( int e1 = 0; e1 < fNumOfEvent-1; ++e1 ){ 
      for( int e2 = e1+1; e2 < fNumOfEvent; ++e2 ){
	event1 = e1;
	time1 = fTimeRoom_Event[ event1 ][ 0 ];
	room1 = fTimeRoom_Event[ event1 ][ 1 ];
	event2 = e2;
	time2 = fTimeRoom_Event[ event2 ][ 0 ];
	room2 = fTimeRoom_Event[ event2 ][ 1 ];
	if( time1 == -1 || time2 == -1 )
	  goto FFF0;

	if( fConf_EventTime[ event1 ][ time2 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
	  goto FFF0;  // このtimeにはeventEjを挿入できない
	if( fConf_EventTime[ event2 ][ time1 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
	  goto FFF0;  // このtimeにはeventEjを挿入できない
	if( time1 == time2 )  // これはenpty eventが存在している時のみ発生
	  goto FFF0;

	numCandiRoomIn2 = 0;
	for( int r = 0; r < fNumOfRoom; ++r ){
	  event = fEvent_TimeRoom[ time2 ][ r ];
	  if( (event == event2 || event == -1) && fAvail_EventRoom[ event1 ][ r ] == 1 )
	    candiRoomIn2[ numCandiRoomIn2++ ] = r;
	}
	numCandiRoomIn1 = 0;
	for( int r = 0; r < fNumOfRoom; ++r ){
	  event = fEvent_TimeRoom[ time1 ][ r ];
	  if( (event == event1 || event == -1) && fAvail_EventRoom[ event2 ][ r ] == 1 )
	    candiRoomIn1[ numCandiRoomIn1++ ] = r;
	}

	if( numCandiRoomIn1 == 0 || numCandiRoomIn2 == 0 )
	  goto FFF0; 

	roomIn1 = candiRoomIn1[ rand() % numCandiRoomIn1 ];
	roomIn2 = candiRoomIn2[ rand() % numCandiRoomIn2 ];
	
	this->Eject( event1, 0 );
	this->Eject( event2, 0 );

	this->Insert( event1, time2, roomIn2, 0 );
	this->Insert( event2, time1, roomIn1, 0 );
	diff = fPenalty_S - penalty_S_before; 

	if( diff < diffMin ){
	  if( fNumOfIterLS > fMoved_EventTime[ event1 ][ time2 ] || fNumOfIterLS > fMoved_EventTime[ event2 ][ time1 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	    diffMin = diff;
	    candi[ 0 ] = event1;
	    candi[ 1 ] = event2;
	    candi[ 2 ] = roomIn2;  
	    candi[ 3 ] = roomIn1;  
	    numOfCandi = 1;
	  }
	}
	else if( diff == diffMin && numOfCandi < 1000 ){
	  if( fNumOfIterLS > fMoved_EventTime[ event1 ][ time2 ] || fNumOfIterLS > fMoved_EventTime[ event2 ][ time1 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	    candi[ 4*numOfCandi ] = event1;
	    candi[ 4*numOfCandi+1 ] = event2;
	    candi[ 4*numOfCandi+2 ] = roomIn2;
	    candi[ 4*numOfCandi+3 ] = roomIn1;
	    ++numOfCandi;
	  }
	}

	this->Eject( event1, 0 );
	this->Eject( event2, 0 );
	this->Insert( event1, time1, room1, 0 );
	this->Insert( event2, time2, room2, 0 );
	assert( fPenalty_S == penalty_S_before );

      FFF0:;
      }    
    }
    // Swap (end)

    if( numOfCandi != 0 )
    {  
      rr = rand() % numOfCandi;
      room = candi[ 4*rr+3 ];

      if( room == -1 ){ // Relocation
	event = candi[ 4*rr ];
	time = candi[ 4*rr+1 ];
	room = candi[ 4*rr+2 ];

	fMoved_EventTime[ event ][ fTimeRoom_Event[ event ][ 0 ] ] = fNumOfIterLS + fTabuTenure;
	this->Eject( event, 1 );
	this->Insert( event, time, room, 1 );
	assert( penalty_S_before + diffMin == fPenalty_S );  // for check
      }
      else{  // Swap
	event1 = candi[ 4*rr ];
	event2 = candi[ 4*rr+1 ];
	roomIn2 = candi[ 4*rr+2 ];
	roomIn1 = candi[ 4*rr+3 ];
	time1 = fTimeRoom_Event[ event1 ][ 0 ];
	//	room1 = fTimeRoom_Event[ event1 ][ 1 ];
	time2 = fTimeRoom_Event[ event2 ][ 0 ];
	//	room2 = fTimeRoom_Event[ event2 ][ 1 ];

	fMoved_EventTime[ event1 ][ time1 ] = fNumOfIterLS + fTabuTenure;
	fMoved_EventTime[ event2 ][ time2 ] = fNumOfIterLS + fTabuTenure;
	this->Eject( event1, 1);
	this->Eject( event2, 1 );
	this->Insert( event1, time2, roomIn2, 1 );
	this->Insert( event2, time1, roomIn1, 1 );
      }
    }
    
    if( fPenalty_S < tIndi_LS.fPenalty_S )    
      fNumOfIterFindBest = fNumOfIterLS;
    if( fPenalty_S < tIndi_LS.fPenalty_S ){        // "<" should be used in normal cases
      this->TransToIndi( tIndi_LS );
    }

    if( fPenalty_S == 0 ) 
      break;
    if( fNumOfIterLS > fMaxNumOfIterLS )
      break;

    this->CheckValid(); 
  }

  this->TransFromIndi( tIndi_LS );
}
*/

// Test1(タブの属性はeventのみ)
 /*
void TSearch::LS_Relocation_Swap_Extend() 
{
  int eventEj, timeEj, roomEj;
  int event, time, room;
  int penalty_S_before, penalty_S_after;
  int penalty_S3_before, penalty_S3_after;
  int diff, diffMin;
  int numOfCandi;
  int candi[ 4000 ];
  int rr;
  int event1, time1, room1, event2, time2, room2;
  int numCandiRoomIn1, numCandiRoomIn2;
  int candiRoomIn1[ fNumOfRoom ], candiRoomIn2[ fNumOfRoom ]; 
  int roomIn1, roomIn2;
  int moved_event[ fNumOfEvent ];

  fNumOfIterLS = 0;
  this->TransToIndi( tIndi_LS );

  for( int i = 0; i < fNumOfEvent; ++i ){
    moved_event[ i ] = fNumOfIterLS;
    for( int t = 0; t < fNumOfTime; ++t ){
      fMoved_EventTime[ i ][ t ] = fNumOfIterLS; // Event Timeのみ(room は考慮していない)
    }
  }

  while( 1 )
  {
    ++fNumOfIterLS;
    // printf( "iter = %d -- %d: %d = %d %d %d\n", fNumOfIterLS, tIndi_LS.fPenalty_S, fPenalty_S, fPenalty_S1, fPenalty_S2, fPenalty_S3 ); fflush( stdout );

    penalty_S_before = fPenalty_S; 
    diffMin = 99999999;   
    numOfCandi = 0;

    // Relocation (start)
    for( int e = 0; e < fNumOfEvent; ++e ){ 
      eventEj = e;
      timeEj = fTimeRoom_Event[ eventEj ][ 0 ];
      roomEj = fTimeRoom_Event[ eventEj ][ 1 ];
      if( timeEj == -1 )
	goto EEE0;

      this->Eject( eventEj, 0 );
      
      for( int t = 0; t < fNumOfTime; ++t ){

	// if( fConf_EventTime[ eventEj ][ t ] != 0 || t == timeEj ){ // R0
	if( fConf_EventTime[ eventEj ][ t ] != 0 && t != timeEj ){  // equevalent to ver1 (R1)
	  goto EEE1;
	}

	for( int r = 0; r < fNumOfRoom; ++r ){
	  if( fAvail_EventRoom[ eventEj ][ r ] == 0 || fEvent_TimeRoom[ t ][ r ] != -1 )
	    goto EEE2;  // この(time,room)にはeventEjを挿入できない
	  if( t == timeEj && r == roomEj )
	    goto EEE2;
	  
	  diff = this->DiffPenalty_S_Insert( eventEj, t ) + fPenalty_S - penalty_S_before; 

	  if( diff < diffMin ){
	    //if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	    if( fNumOfIterLS > moved_event[ eventEj ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	      diffMin = diff;
	      candi[ 0 ] = eventEj;
	      candi[ 1 ] = t;
	      candi[ 2 ] = r;
	      candi[ 3 ] = -1;  //meaning relocation
	      numOfCandi = 1;

	    }
	  }
	  else if( diff == diffMin && numOfCandi < 1000 ){
	    // if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){	    
	    if( fNumOfIterLS > moved_event[ eventEj ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	      candi[ 4*numOfCandi ] = eventEj;
	      candi[ 4*numOfCandi+1 ] = t;
	      candi[ 4*numOfCandi+2 ] = r;
	      candi[ 4*numOfCandi+3 ] = -1;
	      ++numOfCandi;
	    }
	  }
	EEE2:;
	}
      EEE1:;
      }

      this->Insert( eventEj, timeEj, roomEj, 0 );
    EEE0:;
    }
    // Relocation (end)

    // Swap (start)
    for( int e1 = 0; e1 < fNumOfEvent-1; ++e1 ){ 
      for( int e2 = e1+1; e2 < fNumOfEvent; ++e2 ){
	event1 = e1;
	time1 = fTimeRoom_Event[ event1 ][ 0 ];
	room1 = fTimeRoom_Event[ event1 ][ 1 ];
	event2 = e2;
	time2 = fTimeRoom_Event[ event2 ][ 0 ];
	room2 = fTimeRoom_Event[ event2 ][ 1 ];
	if( time1 == -1 || time2 == -1 )
	  goto FFF0;

	if( fConf_EventTime[ event1 ][ time2 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
	  goto FFF0;  // このtimeにはeventEjを挿入できない
	if( fConf_EventTime[ event2 ][ time1 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
	  goto FFF0;  // このtimeにはeventEjを挿入できない
	if( time1 == time2 )  // これはenpty eventが存在している時のみ発生
	  goto FFF0;

	numCandiRoomIn2 = 0;
	for( int r = 0; r < fNumOfRoom; ++r ){
	  event = fEvent_TimeRoom[ time2 ][ r ];
	  if( (event == event2 || event == -1) && fAvail_EventRoom[ event1 ][ r ] == 1 )
	    candiRoomIn2[ numCandiRoomIn2++ ] = r;
	}
	numCandiRoomIn1 = 0;
	for( int r = 0; r < fNumOfRoom; ++r ){
	  event = fEvent_TimeRoom[ time1 ][ r ];
	  if( (event == event1 || event == -1) && fAvail_EventRoom[ event2 ][ r ] == 1 )
	    candiRoomIn1[ numCandiRoomIn1++ ] = r;
	}

	if( numCandiRoomIn1 == 0 || numCandiRoomIn2 == 0 )
	  goto FFF0; 

	roomIn1 = candiRoomIn1[ rand() % numCandiRoomIn1 ];
	roomIn2 = candiRoomIn2[ rand() % numCandiRoomIn2 ];
	
	this->Eject( event1, 0 );
	this->Eject( event2, 0 );

	this->Insert( event1, time2, roomIn2, 0 );
	this->Insert( event2, time1, roomIn1, 0 );
	diff = fPenalty_S - penalty_S_before; 

	if( diff < diffMin ){
	  // if( fNumOfIterLS > fMoved_EventTime[ event1 ][ time2 ] || fNumOfIterLS > fMoved_EventTime[ event2 ][ time1 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	  if( fNumOfIterLS > moved_event[ event1 ] || fNumOfIterLS > moved_event[ event2 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	    diffMin = diff;
	    candi[ 0 ] = event1;
	    candi[ 1 ] = event2;
	    candi[ 2 ] = roomIn2;  
	    candi[ 3 ] = roomIn1;  
	    numOfCandi = 1;
	  }
	}
	else if( diff == diffMin && numOfCandi < 1000 ){
	  // if( fNumOfIterLS > fMoved_EventTime[ event1 ][ time2 ] || fNumOfIterLS > fMoved_EventTime[ event2 ][ time1 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	  if( fNumOfIterLS > moved_event[ event1 ] || fNumOfIterLS > moved_event[ event2 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
	    candi[ 4*numOfCandi ] = event1;
	    candi[ 4*numOfCandi+1 ] = event2;
	    candi[ 4*numOfCandi+2 ] = roomIn2;
	    candi[ 4*numOfCandi+3 ] = roomIn1;
	    ++numOfCandi;
	  }
	}

	this->Eject( event1, 0 );
	this->Eject( event2, 0 );
	this->Insert( event1, time1, room1, 0 );
	this->Insert( event2, time2, room2, 0 );
	assert( fPenalty_S == penalty_S_before );

      FFF0:;
      }    
    }
    // Swap (end)

    if( numOfCandi != 0 )
    {  
      rr = rand() % numOfCandi;
      room = candi[ 4*rr+3 ];

      if( room == -1 ){ // Relocation
	event = candi[ 4*rr ];
	time = candi[ 4*rr+1 ];
	room = candi[ 4*rr+2 ];

	// fMoved_EventTime[ event ][ fTimeRoom_Event[ event ][ 0 ] ] = fNumOfIterLS + fTabuTenure;
	moved_event[ event ] = fNumOfIterLS + fTabuTenure;
	this->Eject( event, 1 );
	this->Insert( event, time, room, 1 );
	assert( penalty_S_before + diffMin == fPenalty_S );  // for check
      }
      else{  // Swap
	event1 = candi[ 4*rr ];
	event2 = candi[ 4*rr+1 ];
	roomIn2 = candi[ 4*rr+2 ];
	roomIn1 = candi[ 4*rr+3 ];
	time1 = fTimeRoom_Event[ event1 ][ 0 ];
	// room1 = fTimeRoom_Event[ event1 ][ 1 ];
	time2 = fTimeRoom_Event[ event2 ][ 0 ];
	// room2 = fTimeRoom_Event[ event2 ][ 1 ];

	// fMoved_EventTime[ event1 ][ time1 ] = fNumOfIterLS + fTabuTenure;
	// fMoved_EventTime[ event2 ][ time2 ] = fNumOfIterLS + fTabuTenure;
	moved_event[ event1 ] = fNumOfIterLS + fTabuTenure;
	moved_event[ event2 ] = fNumOfIterLS + fTabuTenure;
	this->Eject( event1, 1);
	this->Eject( event2, 1 );
	this->Insert( event1, time2, roomIn2, 1 );
	this->Insert( event2, time1, roomIn1, 1 );
      }
    }
    
    if( fPenalty_S < tIndi_LS.fPenalty_S )    
      fNumOfIterFindBest = fNumOfIterLS;
    if( fPenalty_S < tIndi_LS.fPenalty_S ){        // "<" should be used in normal cases
      this->TransToIndi( tIndi_LS );
    }

    if( fPenalty_S == 0 ) 
      break;
    if( fNumOfIterLS > fMaxNumOfIterLS )
      break;

    this->CheckValid(); 
  }

  this->TransFromIndi( tIndi_LS );
}
*/


// Test3(タブの属性はeventのみ)
void TSearch::LS_Relocation_Swap_Extend() 
{
  int eventEj, timeEj, roomEj;
  int event, time, room;
  int professor;
  int penalty_S_before, penalty_S_after;
  int penalty_S3_before, penalty_S3_after;
  int diff, diffMin;
  int numOfCandi;
  int candi[ 4000 ];
  int rr;
  int event1, time1, room1, event2, time2, room2;
  int numCandiRoomIn1, numCandiRoomIn2, numCandiRoomIn;
  int candiRoomIn1[ fNumOfRoom ], candiRoomIn2[ fNumOfRoom ], candiRoomIn[ fNumOfRoom ];
  int roomin1, roomin2;
  int roomIn1, roomIn2, roomIn;
  int confcount;
  int moved_event[ fNumOfEvent ];
  int listEvent1[ fNumOfEvent ];
  int selectedEvent1[ fNumOfEvent ];
  int numOfActiveEvent, numOfActiveEvent_swap;
  // int count1, count2, countTest;
  int countStagBest;

  //  FILE *fp;
  //  char filename[ 80 ]; 
  //  sprintf( filename, "behavior_%s", fDstFile );
  //  fp = fopen( filename, "w" ); 


  fNumOfIterLS = 0;
  this->TransToIndi( tIndi_LS );

  for( int i = 0; i < fNumOfEvent; ++i ){
    moved_event[ i ] = fNumOfIterLS;
  }

  // count1 = 0; count2 = 0; countTest = 0;
  countStagBest = 0;

  numOfActiveEvent = (int)((double)fNumOfEvent * (double)fRadioActiveEvent/(double)100); 
  numOfActiveEvent_swap = (int)((2.0*(double)fNumOfEvent - 1.0 - sqrt(4.0*(double)fNumOfEvent*((double)fNumOfEvent-1.0)*(1.0-0.01*(double)fRadioActiveEvent)+1.0))*0.5) + 1; 
  if( numOfActiveEvent_swap > fNumOfEvent )
    numOfActiveEvent_swap = fNumOfEvent;
  assert( numOfActiveEvent_swap <= numOfActiveEvent );

  while( 1 )
    {
      ++fNumOfIterLS;
      printf( "iter = %d -- %d: %d = %d %d %d %d %d\n", fNumOfIterLS, tIndi_LS.fPenalty_S, fPenalty_S, fPenalty_S1, fPenalty_S2, fPenalty_S3, fPenalty_S4, fPenalty_S5); fflush( stdout );
      // fprintf( fp, "%d %d %d  \n", fNumOfIterLS, tIndi_LS.fPenalty_S, fPenalty_S );
    

      penalty_S_before = fPenalty_S; 
      diffMin = 99999999;   
      numOfCandi = 0;

      // numOfActiveEvent = tRand->Integer( 1, numOfActiveEvent );   // Test3R (use with Test3)


      // Test3B
      // if( countStagBest >= 100 )
      //  numOfActiveEvent = (int)((double)fNumOfEvent * (double)fRadioActiveEvent/(double)100); 
      //else 
      //  numOfActiveEvent = fNumOfEvent; 


      tRand->Permutation( listEvent1, fNumOfEvent );
      for( int e = 0; e < fNumOfEvent; ++e )
        selectedEvent1[ e ] = 0;
      for( int i = 0; i < numOfActiveEvent_swap; ++i ){ 
        eventEj = listEvent1[ i ];
        selectedEvent1[ eventEj ] = 1;
      }


      // Relocation (start)
      for( int i = 0; i < numOfActiveEvent; ++i ){ 
        eventEj = listEvent1[ i ];
        //        printf("eventEj = %d\n",eventEj);        // check
        timeEj = fTimeRoom_Event[ eventEj ][ 0 ];
        roomEj = fTimeRoom_Event[ eventEj ][ 1 ];
        if( timeEj == -1 )
          goto EEE0;
        //printf("relocation eject1 event=%d\n",eventEj);
        this->Eject( eventEj, 0 );
      
        for( int t = 0; t < fNumOfTime; ++t ){
          if( (t % fNumOfTimeInDay) + fEvent_TimeRequest[ eventEj ] -1 >= fNumOfTimeInDay ) // check H6
            goto EEE1;
	
          // if( fConf_EventTime[ eventEj ][ t ] != 0 || t == timeEj ){ // R0
          for( int tr = 0; tr < fEvent_TimeRequest[ eventEj ]; ++tr){
            if( fConf_EventTime[ eventEj ][ t + tr ] != 0 && t != timeEj ){  // equevalent to ver1 (R1) H1 & H4 check???
              goto EEE1;
            }
          }

          for( int p = 0; p < fNumOfProf_Event[ eventEj ]; ++p ){
            professor = fListProf_Event[ eventEj ][ p ];
            for(int tr = 0; tr < fEvent_TimeRequest[ eventEj ]; ++tr ){
              if( fProfCantDo[ professor ][ t + tr ] == 1)               // check H5 
                goto EEE1;
            }
          }
	
          // diff = 999999999;
          numCandiRoomIn = 0;	                         //入れる部屋を探す
          int roomflag;
          for( int r = 0; r < fNumOfRoom; ++r ){
            roomflag = 0;
            for( int tr = 0; tr < fEvent_TimeRequest[ eventEj ]; ++tr ){
              if( fAvail_EventRoom[ eventEj ][ r ] == 1 && fEvent_TimeRoom[ t + tr ][ r ] == -1 ){ //check H3
                if( t != timeEj || r != roomEj )
                  roomflag = 1;
              }else{
                roomflag = 0;
                break;
              }
            }
            if( roomflag != 0)
              candiRoomIn[ numCandiRoomIn++ ] = r;
          }
          
          if( numCandiRoomIn == 0 ) // なかったらパス
            goto EEE1;

          diff = this->DiffPenalty_S_Insert( eventEj, t ) + fPenalty_S - penalty_S_before; 
          // ++count1;

          roomIn = candiRoomIn[ rand() % numCandiRoomIn ];              // 入れる部屋からランダムに選ぶ
	
          if( diff < diffMin ){
            //if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
            if( fNumOfIterLS > moved_event[ eventEj ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
              diffMin = diff;
              candi[ 0 ] = eventEj;
              candi[ 1 ] = t;
              candi[ 2 ] = roomIn;
              candi[ 3 ] = -1;  //meaning relocation
              numOfCandi = 1;

            }
          }
          else if( diff == diffMin && numOfCandi < 1000 ){
            // if( fNumOfIterLS > fMoved_EventTime[ eventEj ][ t ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){	    
            if( fNumOfIterLS > moved_event[ eventEj ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
              candi[ 4*numOfCandi ] = eventEj;
              candi[ 4*numOfCandi+1 ] = t;
              candi[ 4*numOfCandi+2 ] = roomIn;
              candi[ 4*numOfCandi+3 ] = -1;
              ++numOfCandi;
            }
          }
        EEE1:;
        }
        //printf("relocation insert1 event=%d time=%d room=%d\n",eventEj, timeEj, roomEj);
        this->Insert( eventEj, timeEj, roomEj, 0 );
      EEE0:;
      }
      // Relocation (end)
      //printf("Relocation end\n");

      // Swap (start)
      for( int i = 0; i < numOfActiveEvent_swap; ++i ){ 
        event1 = listEvent1[ i ];
        time1 = fTimeRoom_Event[ event1 ][ 0 ];
        room1 = fTimeRoom_Event[ event1 ][ 1 ];
        for( int e2 = 0; e2 < fNumOfEvent; ++e2 ){
          event2 = e2;
          //   printf("event %d and %d Swap\n", event1, event2);      // check
          time2 = fTimeRoom_Event[ event2 ][ 0 ];
          room2 = fTimeRoom_Event[ event2 ][ 1 ];
          if( time1 == -1 || time2 == -1 )   // enpty eventが存在している時のみ必要
            goto FFF0;
          if( selectedEvent1[ event2 ] == 1 && event1 >= event2 )
            goto FFF0;
          // ++countTest;
          
          
          confcount = 0;
          while(2){
            if( confcount < fEvent_TimeRequest[ event1 ] && confcount < fEvent_TimeRequest[ event2 ]){
              if( fConf_EventTime[ event1 ][ time2 + confcount ] - fConf_EventEvent[ event1 ][ event2 ] != 0 ){
                goto FFF0;
              }
              if( fConf_EventTime[ event2 ][ time1 + confcount ] - fConf_EventEvent[ event1 ][ event2 ] != 0 ){
                goto FFF0;  // このtimeにはeventEjを挿入できない
              }
            }else if( confcount < fEvent_TimeRequest[ event1 ]){
              if( fConf_EventTime[ event1 ][ time2 + confcount ] != 0){
                goto FFF0;
              }
            }else if( confcount < fEvent_TimeRequest[ event2 ]){
              if( fConf_EventTime[ event2 ][ time1 + confcount ] != 0){
                goto FFF0;
              }
            }else{
              break;
            }
            confcount++;
          }
          

          /*
          // check H1 H4
          if( fEvent_TimeRequest[ event1 ] == 1 && fEvent_TimeRequest[ event2 ] == 1){
          if( fConf_EventTime[ event1 ][ time2 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
            goto FFF0;  // このtimeにはeventEjを挿入できない
          if( fConf_EventTime[ event2 ][ time1 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
            goto FFF0;  // このtimeにはeventEjを挿入できない
          }else{
            
            
          }
          */
          if( time1 == time2 )  // これはenpty eventが存在している時のみ発生
            goto FFF0;

          
          for( int p = 0; p < fNumOfProf_Event[ event1 ]; ++p ){
            professor = fListProf_Event[ event1 ][ p ];
            for(int tr = 0; tr < fEvent_TimeRequest[ event1 ]; ++tr ){
              if( fProfCantDo[ professor ][ time2 + tr ] == 1)               // check H5 
                goto FFF0;
            }
          }
          for( int p = 0; p < fNumOfProf_Event[ event2 ]; ++p ){
            professor = fListProf_Event[ event2 ][ p ];
            for(int tr = 0; tr < fEvent_TimeRequest[ event2 ]; ++tr ){
              if( fProfCantDo[ professor ][ time1 + tr ] == 1)               // check H5 
                goto FFF0;
            }
          }
        
          if( (time2 % fNumOfTimeInDay) + fEvent_TimeRequest[ event1 ] -1 >= fNumOfTimeInDay ) // check H6
            goto FFF0;
          if( (time1 % fNumOfTimeInDay) + fEvent_TimeRequest[ event2 ] -1 >= fNumOfTimeInDay ) // check H6
            goto FFF0;

          //ここの条件式を多時限講義に対応させること//////////////////////////////////////////////////////////////////////////////////////////////
          int roomcount;
          numCandiRoomIn2 = 0;
          for( int r = 0; r < fNumOfRoom; ++r ){
            roomcount = 0;
            for( int tr = 0; tr <fEvent_TimeRequest[ event1 ]; ++tr){
              event = fEvent_TimeRoom[ time2 + tr ][ r ];
              if( (event == event2 || event == -1) && fAvail_EventRoom[ event1 ][ r ] == 1 )
                roomcount = 1;
              else{
                roomcount = 0;
                break;
              }
            }
            if( roomcount != 0)
              candiRoomIn2[ numCandiRoomIn2++ ] = r;
          }
          numCandiRoomIn1 = 0;
          for( int r = 0; r < fNumOfRoom; ++r ){
            roomcount = 0;
            for( int tr = 0; tr < fEvent_TimeRequest[ event2 ]; ++tr ){
              event = fEvent_TimeRoom[ time1 + tr ][ r ];
              if( (event == event1 || event == -1) && fAvail_EventRoom[ event2 ][ r ] == 1 )
                roomcount = 1;
              else{
                roomcount = 0;
                break;
              }
            }
            if( roomcount != 0 )
              candiRoomIn1[ numCandiRoomIn1++ ] = r;
          }

          if( numCandiRoomIn1 == 0 || numCandiRoomIn2 == 0 )
            goto FFF0; 

          roomin1 = rand() % numCandiRoomIn1;
          roomin2 = rand() % numCandiRoomIn2;
          roomIn1 = candiRoomIn1[ roomin1 ];
          roomIn2 = candiRoomIn2[ roomin2 ];
          /* 複数授業のときにインサートでエラーを起こさないように */
          int abs_time;
          abs_time = abs(time1 - time2);
          if(abs_time < fEvent_TimeRequest[ event1 ] || abs_time < fEvent_TimeRequest[ event2 ]){ // 2つの多時限授業が互いに影響するほど近い時
            if( roomIn1 == roomIn2 ){                                                             // 部屋が一緒だとインサートでエラーを起こすので部屋を変える
              //  printf("ncr1 %d    ncr2 %d\n",numCandiRoomIn1,numCandiRoomIn2);
              for( int nr; nr < numCandiRoomIn1; ++nr )
                //  printf("candiroomin1 %d ",candiRoomIn1[ nr ]);
              for( int nr; nr < numCandiRoomIn2; ++nr )
                //  printf("candiroomin2 %d\n",candiRoomIn2[ nr ]);
              
              if( numCandiRoomIn1 == 1 && numCandiRoomIn2 == 1){
                goto FFF0;
              }else if( numCandiRoomIn1 > numCandiRoomIn2 ){
                roomIn1 = candiRoomIn1[ (roomin1 + 1) % numCandiRoomIn1 ];
                //  printf("roomIn1 change %d\n",roomIn1);
              }else{
                roomIn2 = candiRoomIn2[ (roomin2 + 1) % numCandiRoomIn2 ];
                //  printf("roomIn2 change %d\n",roomIn2);
              }
            }
          }
          
          //  printf("swap eject1 event%d time%d room%d\n",event1,time1,room1);
          this->Eject( event1, 0 );


          /* for check */
          /*
 printf("r\\t ");
  for(int t = 0; t < fNumOfTime; ++t)
    printf("%3d ",t);
  printf("\n");
  for( int r = 0; r < fNumOfRoom; ++r ){
    printf("%3d ",r);
    for( int t = 0; t < fNumOfTime; ++t ){
      printf("%3d ",fEvent_TimeRoom[ t ][ r ]);
    }
	printf("\n");
  }         */
          
          //   printf("swap eject2 event%d time%d room%d\n",event2,time2,room2);
          this->Eject( event2, 0 );

          /*
           printf("r\\t ");
  for(int t = 0; t < fNumOfTime; ++t)
    printf("%3d ",t);
  printf("\n");
  for( int r = 0; r < fNumOfRoom; ++r ){
    printf("%3d ",r);
    for( int t = 0; t < fNumOfTime; ++t ){
      printf("%3d ",fEvent_TimeRoom[ t ][ r ]);
    }
	printf("\n");
  }*/
          
          //  printf("swap insert1 event%d time%d room%d \n",event1,time2,roomIn2);
          this->Insert( event1, time2, roomIn2, 0 );// 怪しい

          /*
          printf("r\\t ");
  for(int t = 0; t < fNumOfTime; ++t)
    printf("%3d ",t);
  printf("\n");
  for( int r = 0; r < fNumOfRoom; ++r ){
    printf("%3d ",r);
    for( int t = 0; t < fNumOfTime; ++t ){
      printf("%3d ",fEvent_TimeRoom[ t ][ r ]);
    }
	printf("\n");
  }*/
          //   printf("swap insert2 event%d time%d room%d \n",event2,time1,roomIn1);
          this->Insert( event2, time1, roomIn1, 0 );//


          /*
          printf("r\\t ");
  for(int t = 0; t < fNumOfTime; ++t)
    printf("%3d ",t);
  printf("\n");
  for( int r = 0; r < fNumOfRoom; ++r ){
    printf("%3d ",r);
    for( int t = 0; t < fNumOfTime; ++t ){
      printf("%3d ",fEvent_TimeRoom[ t ][ r ]);
    }
	printf("\n");
  }
          */
  /* end check */
  
          //     printf("kari\n");
          diff = fPenalty_S - penalty_S_before; 
          // ++count2;
        
          if( diff < diffMin ){
            // if( fNumOfIterLS > fMoved_EventTime[ event1 ][ time2 ] || fNumOfIterLS > fMoved_EventTime[ event2 ][ time1 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
            if( fNumOfIterLS > moved_event[ event1 ] || fNumOfIterLS > moved_event[ event2 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
              diffMin = diff;
              candi[ 0 ] = event1;
              candi[ 1 ] = event2;
              candi[ 2 ] = roomIn2;  
              candi[ 3 ] = roomIn1;  
              numOfCandi = 1;
            }
          }
          else if( diff == diffMin && numOfCandi < 1000 ){
            // if( fNumOfIterLS > fMoved_EventTime[ event1 ][ time2 ] || fNumOfIterLS > fMoved_EventTime[ event2 ][ time1 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
            if( fNumOfIterLS > moved_event[ event1 ] || fNumOfIterLS > moved_event[ event2 ] || penalty_S_before + diff < tIndi_LS.fPenalty_S ){
              candi[ 4*numOfCandi ] = event1;
              candi[ 4*numOfCandi+1 ] = event2;
              candi[ 4*numOfCandi+2 ] = roomIn2;
              candi[ 4*numOfCandi+3 ] = roomIn1;
              ++numOfCandi;
            }
          }
          //          printf("swap eject3 event=%d\n",event1);
          this->Eject( event1, 0 );
          // printf("swap eject4 event=%d\n",event2);
          this->Eject( event2, 0 );
          //printf("swap insert3 event%d time%d room%d \n",event1,time1,room1);
          this->Insert( event1, time1, room1, 0 );
          //printf("swap insert4 event%d time%d room%d \n",event2,time2,room2);
          this->Insert( event2, time2, room2, 0 );
 
          assert( fPenalty_S == penalty_S_before );
        
        FFF0:;
        }    
      }
      // Swap (end)
      //printf("Swap end\n");
      if( numOfCandi != 0 )
        {  
          rr = rand() % numOfCandi;
          room = candi[ 4*rr+3 ];
        
          if( room == -1 ){ // Relocation
            event = candi[ 4*rr ];
            time = candi[ 4*rr+1 ];
            room = candi[ 4*rr+2 ];
          
            // fMoved_EventTime[ event ][ fTimeRoom_Event[ event ][ 0 ] ] = fNumOfIterLS + fTabuTenure;
            moved_event[ event ] = fNumOfIterLS + fTabuTenure;
            //printf("do relocation eject1 event=%d",event);
            this->Eject( event, 1 );
            //printf("do relocation insert1 event%d time%d room%d \n",event,time,room);
            this->Insert( event, time, room, 1 );
            assert( penalty_S_before + diffMin == fPenalty_S );  // for check
          }
          else{  // Swap
            event1 = candi[ 4*rr ];
            event2 = candi[ 4*rr+1 ];
            roomIn2 = candi[ 4*rr+2 ];
            roomIn1 = candi[ 4*rr+3 ];
            time1 = fTimeRoom_Event[ event1 ][ 0 ];
            // room1 = fTimeRoom_Event[ event1 ][ 1 ];
            time2 = fTimeRoom_Event[ event2 ][ 0 ];
            // room2 = fTimeRoom_Event[ event2 ][ 1 ];
          
            // fMoved_EventTime[ event1 ][ time1 ] = fNumOfIterLS + fTabuTenure;
            // fMoved_EventTime[ event2 ][ time2 ] = fNumOfIterLS + fTabuTenure;
            moved_event[ event1 ] = fNumOfIterLS + fTabuTenure;
            moved_event[ event2 ] = fNumOfIterLS + fTabuTenure;
            // printf("do swap eject1 event=%d\n",event1);
            this->Eject( event1, 1);
            //printf("do swap eject2 event=%d\n",event2);
            this->Eject( event2, 1 );
            //printf("do swap insert1 event%d time%d room%d \n",event1,time2,roomIn2);
            this->Insert( event1, time2, roomIn2, 1 );
            //printf("do swap insert2 event%d time%d room%d \n",event2,time1,roomIn1);
            this->Insert( event2, time1, roomIn1, 1 );
          }
        }
    
      if( fPenalty_S < tIndi_LS.fPenalty_S ){    
        fNumOfIterFindBest = fNumOfIterLS;
        countStagBest = 0;
      }
      else 
        ++countStagBest;
    
      if( fPenalty_S < tIndi_LS.fPenalty_S ){        // "<" should be used in normal cases
        this->TransToIndi( tIndi_LS );
      }
    
      if( fPenalty_S == 0 ) 
        break;
      if( fNumOfIterLS > fMaxNumOfIterLS )
        break;
  
      this->CheckValid(); 


    }
  
  this->TransFromIndi( tIndi_LS );
  printf("r\\t ");
  for(int t = 0; t < fNumOfTime; ++t)
    printf("%3d ",t);
  printf("\n");
  for( int r = 0; r < fNumOfRoom; ++r ){
    printf("%3d ",r);
    for( int t = 0; t < fNumOfTime; ++t ){
      printf("%3d ",fEvent_TimeRoom[ t ][ r ]);
    }
    printf("\n");
  }

  // printf( "count = %d %d %d\n", count1, count2, countTest );
  // fclose( fp );
}



void TSearch::DispFeatureSol()
{
  int time, day;
  int numOfEvent_Day[ fNumOfDay ];


  for( int d = 0; d < fNumOfDay; ++d )
    numOfEvent_Day[ d ] = 0;

  for( int e = 0; e < fNumOfEvent; ++e ){
    time = fTimeRoom_Event[ e ][ 0 ];
    if( time != -1 ){
      day = time / fNumOfTimeInDay;  
      ++numOfEvent_Day[ day ];
    }
  }

  printf( "D = " );
  for( int d = 0; d < fNumOfDay; ++d )
    printf( " %d", numOfEvent_Day[ d ] );
  printf( "\n" );

}


// Return the distance between indi and current solution
int TSearch::Distance( TIndi& indi, int flag )
{
  int count;
  int time, room;

  count = 0;
  for( int e = 0; e < fNumOfEvent; ++e )
  {
    time = fTimeRoom_Event[ e ][ 0 ];
    room = fTimeRoom_Event[ e ][ 1 ];

    if( flag == 0 ){  // both time and room are considered
      if( time == indi.fTimeRoom_Event[ e ][ 0 ] && room == indi.fTimeRoom_Event[ e ][ 1 ] )
	++count;
    } // only time is considered
    else{ 
      if( time == indi.fTimeRoom_Event[ e ][ 0 ] )
	++count;
    }
  }
  
  return fNumOfEvent - count;
}


// distance in terms of the assignment of events to the days
int TSearch::Distance_D( TIndi& indi1, TIndi& indi2 )
{
  int day, day1, day2;
  int k1, k2, fK1, fK2;
  int wei[ fNumOfDay ][ fNumOfDay ];
  int d1[ fNumOfDay ], d2[ fNumOfDay ];

  int minD, k1_s, k2_s, k2_e;
  int linked1[ fNumOfDay ];   // index of the linked node by the maching edges, -1: not linked
  int linked2[ fNumOfDay ];   // index of the linked node by the maching edges, -1: not linked
  int used1[ fNumOfDay ]; // 1: d[] is updated and the corresponding k has not been used as k_s after then, 0: otherwise
  int used2[ fNumOfDay ]; 
  int Mlink1[ fNumOfDay ], Mlink2[ fNumOfDay ];  // index of the linked critical node, -1: start node
  int dd;
  int iter;
  int numM;  // the number of temporal matching edges
  int Alist1[ fNumOfDay ], Alist2[ fNumOfDay ]; // list of the concidered nodes


  for( day1 = 0; day1 < fNumOfDay; ++day1 ){
    for( day2 = 0; day2 < fNumOfDay; ++day2 ){
      wei[ day1 ][ day2 ] = 0;
    }
  }

  for( int e = 0; e < fNumOfEvent; ++e ){
    day = indi1.fTimeRoom_Event[ e ][ 0 ] / fNumOfTimeInDay;  
    ++wei[ day ][ 0 ];
  }
  for( day1 = 0; day1 < fNumOfDay; ++day1 ){
    for( day2 = 1; day2 < fNumOfDay; ++day2 ){
      wei[ day1 ][ day2 ] = wei[ day1 ][ 0 ];
    }
  }

  for( int e = 0; e < fNumOfEvent; ++e ){
    assert( indi1.fTimeRoom_Event[ e ][ 0 ] != -1 );
    assert( indi2.fTimeRoom_Event[ e ][ 0 ] != -1 );

    day1 = indi1.fTimeRoom_Event[ e ][ 0 ] / fNumOfTimeInDay;  
    day2 = indi2.fTimeRoom_Event[ e ][ 0 ] / fNumOfTimeInDay;  
    --wei[ day1 ][ day2 ];
    assert( wei[ day1 ][ day2 ] >= 0 );
  }

  numM = 0;
  for( k1 = 0; k1 < fNumOfDay; ++k1 ) 
    linked1[ k1 ] = -1;
  for( k2 = 0; k2 < fNumOfDay; ++k2 ) 
    linked2[ k2 ] = -1;

  // LLL1 (start)
  while( 1 ){ 

    for( int s1 = 0; s1 < numM; ++s1 ){ 
      k1 = Alist1[ s1 ];
      d1[ k1 ] = 99999999;
      used1[ k1 ] = 0;
    }
    for( int s2 = 0; s2 < numM; ++s2 ){ 
      k2 = Alist2[ s2 ];
      d2[ k2 ] = 99999999;
      used2[ k2 ] = 0;
    }

    for( k1 = 0; k1 < fNumOfDay; ++k1 ){
      if( linked1[ k1 ] == -1 ){ 
	d1[ k1 ] = 0;
	used1[ k1 ] = 0;
	Mlink1[ k1 ] = -1;
	Alist1[ numM ] = k1;
	break;
      }
    }
    for( k2 = 0; k2 < fNumOfDay; ++k2 ){
      if( linked2[ k2 ] == -1 ){ 
	d2[ k2 ] = 99999999;
	used2[ k2 ] = 0;
	Alist2[ numM ] = k2;
	break;
      }
    }

    iter = 0;

    // LLL2 (start)
    while( 1 ){ 

      minD = 99999999+1;      // 現状態d1[], d2[]から最小の値のノードを探す --> k1_s, k2_s, minD
      k1_s = -1; k2_s = -1;
      for( int s1 = 0; s1 <= numM; ++s1 ){
	k1 = Alist1[ s1 ];
	if( d1[ k1 ] < minD && used1[ k1 ] == 0 ){
	  k1_s = k1; 
	  k2_s = -1;
	  minD = d1[ k1 ];
	}
      }
      assert( k1_s != -1 );

      used1[ k1_s ] = 1;

      ++iter;

      for( int s2 = 0; s2 <= numM; ++s2 ){
	k2 = Alist2[ s2 ];
	if( linked1[ k1_s ] != k2 ){
	  assert( linked2[ k2 ] != k1_s );
	  dd = d1[ k1_s ] + wei[ k1_s ][ k2 ]; 
	  if( dd < d2[ k2 ] ){
	    d2[ k2 ] = dd;
	    Mlink2[ k2 ] = k1_s;

	    if( linked2[ k2 ] != -1 ){
	      k1 = linked2[ k2 ];
	      dd = d2[ k2 ] - wei[ k1 ][ k2 ];

	      if( dd < d1[ k1 ] ){
		used1[ k1 ] = 0;              // due to the existence of edges with minus weight
		d1[ k1 ] = dd;
		Mlink1[ k1 ] = k2;
	      }
	    }
	  }
	}
      }

      int LLL2 = 0;
      for( int s1 = 0; s1 <= numM; ++s1 ){
	k1 = Alist1[ s1 ];
	if( used1[ k1 ] == 0 )
	  LLL2 = 1;
      }
      if( LLL2 == 0 )
	break;
    }
    // LLL2 (end)

    k2_e = Alist2[ numM ];
    k2 = k2_e;
    while( 1 ){
      k1 = Mlink2[ k2 ];
      linked2[ k2 ] = k1;
      linked1[ k1 ] = k2;

      if( Mlink1[ k1 ] == -1 )
	break;

      k2 = Mlink1[ k1 ];
    }

    ++numM;
    if( numM == fNumOfDay || numM == fNumOfDay )
      break;
  }
  // LLL1 (start)  

  int distance = 0;
  for( k1 = 0; k1 < fNumOfDay; ++k1 ) 
    distance += wei[ k1 ][ linked1[ k1 ] ];


  // Tmp ここは削除して良い。挙動解析用
  /*
  for( k1 = 0; k1 < fNumOfDay; ++k1 ){
    fWei[ k1 ] = wei[ k1 ][ linked1[ k1 ] ];
    fLinked1[ k1 ] = linked1[ k1 ];
  }
  for( k2 = 0; k2 < fNumOfDay; ++k2 )
    fLinked2[ k2 ] = linked2[ k2 ];
  */
  // Tmp

  return distance;
}

int TSearch::Distance_D( TIndi& indi1 )
{
  int day, day1, day2;
  int k1, k2, fK1, fK2;
  int wei[ fNumOfDay ][ fNumOfDay ];
  int d1[ fNumOfDay ], d2[ fNumOfDay ];

  int minD, k1_s, k2_s, k2_e;
  int linked1[ fNumOfDay ];   // index of the linked node by the maching edges, -1: not linked
  int linked2[ fNumOfDay ];   // index of the linked node by the maching edges, -1: not linked
  int used1[ fNumOfDay ]; // 1: d[] is updated and the corresponding k has not been used as k_s after then, 0: otherwise
  int used2[ fNumOfDay ]; 
  int Mlink1[ fNumOfDay ], Mlink2[ fNumOfDay ];  // index of the linked critical node, -1: start node
  int dd;
  int iter;
  int numM;  // the number of temporal matching edges
  int Alist1[ fNumOfDay ], Alist2[ fNumOfDay ]; // list of the concidered nodes


  for( day1 = 0; day1 < fNumOfDay; ++day1 ){
    for( day2 = 0; day2 < fNumOfDay; ++day2 ){
      wei[ day1 ][ day2 ] = 0;
    }
  }

  for( int e = 0; e < fNumOfEvent; ++e ){
    day = indi1.fTimeRoom_Event[ e ][ 0 ] / fNumOfTimeInDay;  
    ++wei[ day ][ 0 ];
  }
  for( day1 = 0; day1 < fNumOfDay; ++day1 ){
    for( day2 = 1; day2 < fNumOfDay; ++day2 ){
      wei[ day1 ][ day2 ] = wei[ day1 ][ 0 ];
    }
  }

  for( int e = 0; e < fNumOfEvent; ++e ){
    assert( indi1.fTimeRoom_Event[ e ][ 0 ] != -1 );
    assert( fTimeRoom_Event[ e ][ 0 ] != -1 );

    day1 = indi1.fTimeRoom_Event[ e ][ 0 ] / fNumOfTimeInDay;  
    day2 = fTimeRoom_Event[ e ][ 0 ] / fNumOfTimeInDay;  
    --wei[ day1 ][ day2 ];
    assert( wei[ day1 ][ day2 ] >= 0 );
  }

  numM = 0;
  for( k1 = 0; k1 < fNumOfDay; ++k1 ) 
    linked1[ k1 ] = -1;
  for( k2 = 0; k2 < fNumOfDay; ++k2 ) 
    linked2[ k2 ] = -1;

  // LLL1 (start)
  while( 1 ){ 

    for( int s1 = 0; s1 < numM; ++s1 ){ 
      k1 = Alist1[ s1 ];
      d1[ k1 ] = 99999999;
      used1[ k1 ] = 0;
    }
    for( int s2 = 0; s2 < numM; ++s2 ){ 
      k2 = Alist2[ s2 ];
      d2[ k2 ] = 99999999;
      used2[ k2 ] = 0;
    }

    for( k1 = 0; k1 < fNumOfDay; ++k1 ){
      if( linked1[ k1 ] == -1 ){ 
	d1[ k1 ] = 0;
	used1[ k1 ] = 0;
	Mlink1[ k1 ] = -1;
	Alist1[ numM ] = k1;
	break;
      }
    }
    for( k2 = 0; k2 < fNumOfDay; ++k2 ){
      if( linked2[ k2 ] == -1 ){ 
	d2[ k2 ] = 99999999;
	used2[ k2 ] = 0;
	Alist2[ numM ] = k2;
	break;
      }
    }

    iter = 0;

    // LLL2 (start)
    while( 1 ){ 

      minD = 99999999+1;      // 現状態d1[], d2[]から最小の値のノードを探す --> k1_s, k2_s, minD
      k1_s = -1; k2_s = -1;
      for( int s1 = 0; s1 <= numM; ++s1 ){
	k1 = Alist1[ s1 ];
	if( d1[ k1 ] < minD && used1[ k1 ] == 0 ){
	  k1_s = k1; 
	  k2_s = -1;
	  minD = d1[ k1 ];
	}
      }
      assert( k1_s != -1 );

      used1[ k1_s ] = 1;

      ++iter;

      for( int s2 = 0; s2 <= numM; ++s2 ){
	k2 = Alist2[ s2 ];
	if( linked1[ k1_s ] != k2 ){
	  assert( linked2[ k2 ] != k1_s );
	  dd = d1[ k1_s ] + wei[ k1_s ][ k2 ]; 
	  if( dd < d2[ k2 ] ){
	    d2[ k2 ] = dd;
	    Mlink2[ k2 ] = k1_s;

	    if( linked2[ k2 ] != -1 ){
	      k1 = linked2[ k2 ];
	      dd = d2[ k2 ] - wei[ k1 ][ k2 ];

	      if( dd < d1[ k1 ] ){
		used1[ k1 ] = 0;              // due to the existence of edges with minus weight
		d1[ k1 ] = dd;
		Mlink1[ k1 ] = k2;
	      }
	    }
	  }
	}
      }

      int LLL2 = 0;
      for( int s1 = 0; s1 <= numM; ++s1 ){
	k1 = Alist1[ s1 ];
	if( used1[ k1 ] == 0 )
	  LLL2 = 1;
      }
      if( LLL2 == 0 )
	break;
    }
    // LLL2 (end)

    k2_e = Alist2[ numM ];
    k2 = k2_e;
    while( 1 ){
      k1 = Mlink2[ k2 ];
      linked2[ k2 ] = k1;
      linked1[ k1 ] = k2;

      if( Mlink1[ k1 ] == -1 )
	break;

      k2 = Mlink1[ k1 ];
    }

    ++numM;
    if( numM == fNumOfDay || numM == fNumOfDay )
      break;
  }
  // LLL1 (start)  

  int distance = 0;
  for( k1 = 0; k1 < fNumOfDay; ++k1 ) 
    distance += wei[ k1 ][ linked1[ k1 ] ];


  // Tmp ここは削除して良い。挙動解析用
  /*
  for( k1 = 0; k1 < fNumOfDay; ++k1 ){
    fWei[ k1 ] = wei[ k1 ][ linked1[ k1 ] ];
    fLinked1[ k1 ] = linked1[ k1 ];
  }
  for( k2 = 0; k2 < fNumOfDay; ++k2 )
    fLinked2[ k2 ] = linked2[ k2 ];
  */
  // Tmp

  return distance;
}


void TSearch::QuasiGES()
{
  //  this->Ejection();
  int event;

  this->Perturbation();
  //  this->Insertion();
  // this->LS_Relocation_Swap_Extend(); 
}

void TSearch::Perturbation() 
{
  int eventEj, timeEj, roomEj;
  int event, time, room;
  int timeIn;
  int penalty_S_before, penalty_S_after;
  int penalty_S3_before, penalty_S3_after;
  int diff, diffMin;
  int numOfCandi;
  int candi[ 4000 ];
  int rr;
  int event1, time1, room1, event2, time2, room2;
  int numCandiRoomIn1, numCandiRoomIn2;
  int candiRoomIn1[ fNumOfRoom ], candiRoomIn2[ fNumOfRoom ]; 
  int roomIn1, roomIn2;
  int moved_event[ fNumOfEvent ];
  int numOfIter;
  int randEvent, randSwap, randTime;
  int penalty_S_limit;
  int indexSwap, indexSwapMax;
  int eventSwap[ fNumOfEvent*(fNumOfEvent-1)/2 ][ 2 ];
  int fMaxNumOfIterPerturbation = 100;


  indexSwap = 0;
  for( int e1 = 0; e1 < fNumOfEvent-1; ++e1 ){ 
    for( int e2 = e1+1; e2 < fNumOfEvent; ++e2 ){
      eventSwap[ indexSwap ][ 0 ] = e1;
      eventSwap[ indexSwap ][ 1 ] = e2;
      ++indexSwap;
    }
  }
  indexSwapMax = indexSwap;

  numOfIter = 0;
  penalty_S_limit = fPenalty_S;
  this->TransToIndi( tIndi_LS );

  for( int i = 0; i < fNumOfEvent; ++i ){
    moved_event[ i ] = numOfIter;
  }

  while( 1 )
  {
    ++numOfIter;
    //    printf( "iterR = %d -- %d: %d = %d %d %d\n", numOfIter, penalty_S_limit, fPenalty_S, fPenalty_S1, fPenalty_S2, fPenalty_S3 ); fflush( stdout );
    //    printf( "dis = %d %d\n", Distance( tIndi_LS, 0 ), Distance( tIndi_LS, 1 ) );

    penalty_S_before = fPenalty_S; 
    numOfCandi = 0;

    if( numOfIter % 2 == 0 )
      goto RELOCATION;
    else
      goto SWAP;

    // Relocation (start)
  RELOCATION:;    
    randEvent = tRand->Integer( 0, fNumOfEvent - 1 );
    for( int e = 0; e < fNumOfEvent; ++e ){ 
      eventEj = (e + randEvent) % fNumOfEvent;

      timeEj = fTimeRoom_Event[ eventEj ][ 0 ];
      roomEj = fTimeRoom_Event[ eventEj ][ 1 ];
      if( timeEj == -1 )
	goto EEE0;

      this->Eject( eventEj, 0 );
      
      randTime = tRand->Integer( 0, fNumOfTime - 1 );
      for( int t = 0; t < fNumOfTime; ++t ){
	timeIn = (t + randTime) % fNumOfTime;

	if( fConf_EventTime[ eventEj ][ timeIn ] != 0 && timeIn != timeEj )  // different
	  goto EEE1;
	if( timeIn == timeEj )  // different
	  goto EEE1;

	numCandiRoomIn1 = 0;
	for( int r = 0; r < fNumOfRoom; ++r ){
	  event = fEvent_TimeRoom[ timeIn ][ r ];
	  if( event == -1 && fAvail_EventRoom[ eventEj ][ r ] == 1 )
	    candiRoomIn1[ numCandiRoomIn1++ ] = r;
	}

	if( numCandiRoomIn1 == 0 )
	  goto EEE1; 

	penalty_S_after = this->DiffPenalty_S_Insert( eventEj, timeIn ) + fPenalty_S; 

	if( penalty_S_after <= penalty_S_limit ){
	  candi[ 0 ] = eventEj;
	  candi[ 1 ] = timeIn;
	  candi[ 2 ] = candiRoomIn1[ rand() % numCandiRoomIn1 ];
	  candi[ 3 ] = -1;
	  numOfCandi = 1;
	}
      EEE1:;
	if( numOfCandi == 1 )
	  break;
      }
      this->Insert( eventEj, timeEj, roomEj, 0 );
    EEE0:;
      if( numOfCandi == 1 )
	goto Move;
    }
    goto Move;
    // Relocation (end)
    printf("Relocation end\n");

    
    // Swap (start)
  SWAP:;
    
    randSwap = tRand->Integer( 0, indexSwapMax );
    for( int i = 0; i < indexSwap; ++i ){ 
      indexSwap = (i + randSwap) % indexSwapMax;
      
      event1 = eventSwap[ i ][ 0 ];
      event2 = eventSwap[ i ][ 1 ];

      //event1 = e1;
	time1 = fTimeRoom_Event[ event1 ][ 0 ];
	room1 = fTimeRoom_Event[ event1 ][ 1 ];
	//	event2 = e2;
	time2 = fTimeRoom_Event[ event2 ][ 0 ];
	room2 = fTimeRoom_Event[ event2 ][ 1 ];
	if( time1 == -1 || time2 == -1 )
	  goto FFF0;

	if( fConf_EventTime[ event1 ][ time2 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
	  goto FFF0;  // このtimeにはeventEjを挿入できない
	if( fConf_EventTime[ event2 ][ time1 ] - fConf_EventEvent[ event1 ][ event2 ] != 0 )
	  goto FFF0;  // このtimeにはeventEjを挿入できない
	if( time1 == time2 )  // これはenpty eventが存在している時のみ発生
	  goto FFF0;

	numCandiRoomIn2 = 0;
	for( int r = 0; r < fNumOfRoom; ++r ){
	  event = fEvent_TimeRoom[ time2 ][ r ];
	  if( (event == event2 || event == -1) && fAvail_EventRoom[ event1 ][ r ] == 1 )
	    candiRoomIn2[ numCandiRoomIn2++ ] = r;
	}
	numCandiRoomIn1 = 0;
	for( int r = 0; r < fNumOfRoom; ++r ){
	  event = fEvent_TimeRoom[ time1 ][ r ];
	  if( (event == event1 || event == -1) && fAvail_EventRoom[ event2 ][ r ] == 1 )
	    candiRoomIn1[ numCandiRoomIn1++ ] = r;
	}

	if( numCandiRoomIn1 == 0 || numCandiRoomIn2 == 0 )
	  goto FFF0; 

	roomIn1 = candiRoomIn1[ rand() % numCandiRoomIn1 ];
	roomIn2 = candiRoomIn2[ rand() % numCandiRoomIn2 ];
	
	this->Eject( event1, 0 );
	this->Eject( event2, 0 );

	this->Insert( event1, time2, roomIn2, 0 );
	this->Insert( event2, time1, roomIn1, 0 );
	penalty_S_after = fPenalty_S;

	if( penalty_S_after <= penalty_S_limit ){
	  candi[ 0 ] = event1;
	  candi[ 1 ] = event2;
	  candi[ 2 ] = roomIn2;
	  candi[ 3 ] = roomIn1;
	  numOfCandi = 1;
	}

	this->Eject( event1, 0 );
	this->Eject( event2, 0 );
	this->Insert( event1, time1, room1, 0 );
	this->Insert( event2, time2, room2, 0 );
	assert( fPenalty_S == penalty_S_before );

      FFF0:;
	if( numOfCandi != 0 )
	  goto Move;
      }    

    // Swap (end)


  Move:;
    if( numOfCandi == 1 )
    {  
      rr = rand() % numOfCandi;
      room = candi[ 4*rr+3 ];

      if( room == -1 ){ // Relocation
	// printf( "Rel\n" );
	event = candi[ 4*rr ];
	time = candi[ 4*rr+1 ];
	room = candi[ 4*rr+2 ];

	moved_event[ event ] = numOfIter + fTabuTenure;
	this->Eject( event, 1 );
	this->Insert( event, time, room, 1 );
	assert( fPenalty_S <= penalty_S_limit );  // for check
      }
      else{  // Swap
	// printf( "Swa\n" );
	event1 = candi[ 4*rr ];
	event2 = candi[ 4*rr+1 ];
	roomIn2 = candi[ 4*rr+2 ];
	roomIn1 = candi[ 4*rr+3 ];
	time1 = fTimeRoom_Event[ event1 ][ 0 ];
	time2 = fTimeRoom_Event[ event2 ][ 0 ];

	moved_event[ event1 ] = numOfIter + fTabuTenure;
	moved_event[ event2 ] = numOfIter + fTabuTenure;
	this->Eject( event1, 1);
	this->Eject( event2, 1 );
	this->Insert( event1, time2, roomIn2, 1 );
	this->Insert( event2, time1, roomIn1, 1 );
      }
    }

    if( numOfIter > fMaxNumOfIterPerturbation )
      break;

    this->CheckValid(); 
  }
}
