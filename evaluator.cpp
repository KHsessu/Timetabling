#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

TEvaluator::TEvaluator()
{
}

TEvaluator::~TEvaluator()
{

}


void TEvaluator::SetInstance( char* filename )  
{
  FILE* fp;

  int dumy;
  int *size_room;
  int **studentEvent;
  int **profEvent;     // ken hachikuo add 12.12
  int **roomFeature;
  int **eventFeature;
  int numOfFeature;

  fNumOfTime = 45;
  fNumOfDay = 5;
  fNumOfTimeInDay = 9; 
  /////////////////////////////////////////////////
  fp = fopen( filename, "r" );
  /* ken hachikubo modify 12.12. */
  fscanf(fp, "%d %d %d %d %d", &fNumOfEvent, &fNumOfRoom, &numOfFeature, &fNumOfStudent, &fNumOfProf);

  /* fAvail_EventRoom[ event ][ room ] -> 1:event can do in room 0:otherwise */
  fAvail_EventRoom = new int* [ fNumOfEvent ];
  for( int i = 0; i < fNumOfEvent; ++i )
    fAvail_EventRoom[ i ] = new int [ fNumOfRoom ];

  /* [ event1 ][ event2 ] -> 1:event 1 & 2 is conflict  0:otherwise */
  fConf_EventEvent = new int* [ fNumOfEvent ];
  for( int i = 0; i < fNumOfEvent; ++i )
    fConf_EventEvent[ i ] = new int [ fNumOfEvent ];

  /* [ event ][  ] -> list of student who attends the event */
  fListStudent_Event = new int* [ fNumOfEvent ];
  for( int i = 0; i < fNumOfEvent; ++i )
    fListStudent_Event[ i ] = new int [ fNumOfStudent ];

  fListProf_Event = new int* [ fNumOfEvent ];
  for( int i = 0; i < fNumOfEvent; ++i )
    fListProf_Event[ i ] = new int [ fNumOfProf ];
  
  /* [ event ] -> number of student who attends the event */
  fNumOfStudent_Event = new int [ fNumOfEvent ];

  /* [ event ] -> number of prof who attends the event */
  fNumOfProf_Event = new int [ fNumOfEvent ];

  /* [ event1 ][  ] -> list of event that conflicts the event1 */
  fListConfEvent_Event = new int* [ fNumOfEvent ];
  for( int e = 0; e < fNumOfEvent; ++e )
    fListConfEvent_Event[ e ] = new int [ fNumOfEvent ];

  /* ken hachikubo add 12.14 */
  /* [ event ] -> 1:this event is required subject */
  fEvent_Required = new int [ fNumOfEvent ];
  /* [ event ] -> timeslot nessesary to the event */
  fEvent_TimeRequest = new int [ fNumOfEvent ];
  /* [ prof ] [ time ] -> 1:prof can't do this event in time */
  fProfCantDo = new int* [ fNumOfProf ];
  for( int p = 0; p < fNumOfProf; ++p)
    fProfCantDo[ p ] = new int [ fNumOfTime ];

  
  fNumOfConfEvent_Event = new int [ fNumOfEvent ];

  /* S2は変更しているので要検証 */
  fDiffPenalty_S2_Insert = new int* [ fNumOfTimeInDay ];
  for( int h = 0; h < fNumOfTimeInDay; ++h )
    fDiffPenalty_S2_Insert[ h ] = new int [ fNumOfTimeInDay ];

  size_room = new int [ fNumOfRoom ];

  /* [ stuent ][ event ] -> 1:student attends event 0:otherwise */
  studentEvent = new int* [ fNumOfStudent ];
  for( int s = 0; s < fNumOfStudent; ++s )
    studentEvent[ s ] = new int [ fNumOfEvent ];

  /* ken hachikubo add 12.12. */
  /* [ professor ][ event ] -> 1:professor do event 0:otherwise */
  profEvent = new int* [ fNumOfProf ];
  for( int p = 0; p < fNumOfProf; ++p )
    profEvent[ p ] = new int [ fNumOfEvent ];

  /* [ room ][ feature ] -> room has feature */
  roomFeature = new int* [ fNumOfRoom ];
  for( int r = 0; r < fNumOfRoom; ++r )
    roomFeature[ r ] = new int [ numOfFeature ];

  /* [ event ][ feature ] -> event requires feature */
  eventFeature = new int* [ fNumOfEvent ];
  for( int i = 0; i < fNumOfEvent; ++i )
    eventFeature[ i ] = new int [ numOfFeature ];

  /* List of  RequiredEvent */
  fListRequiredEvent = new int [ fNumOfEvent ];
  
  
  /////////* read file *////////
  for( int r = 0; r < fNumOfRoom; ++r )
    fscanf( fp, "%d", &size_room[ r ] );
  
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int i = 0; i < fNumOfEvent; ++i ){
      fscanf( fp, "%d", &studentEvent[ s ][ i ] );
    }
  }
  /* ken hachikubo add 12.12. */
  for( int p = 0; p < fNumOfProf; ++p )
    for( int i = 0; i < fNumOfEvent; ++i )
      fscanf( fp, "%d", &profEvent[ p ][ i ] );

  for( int p = 0; p < fNumOfProf; ++p)
    for (int t = 0; t < fNumOfTime; ++t)
      fscanf( fp, "%d", &fProfCantDo[ p ][ t ]);
  
  for( int r = 0; r < fNumOfRoom; ++r )
    for( int f = 0; f < numOfFeature; ++f )
      fscanf( fp, "%d", &roomFeature[ r ][ f ] );

  for( int i = 0; i < fNumOfEvent; ++i ){
    for( int f = 0; f < numOfFeature; ++f ){
      if( fscanf( fp, "%d", &eventFeature[ i ][ f ] ) == EOF ){
	printf( "Error1!\n" );
	exit( 1 ); 
      }
    }
  }

  /* ken hachikubo add 12.14 */
  for( int i = 0; i < fNumOfEvent; ++i )
    if( fscanf( fp, "%d", &fEvent_Required [ i ] ) == EOF ){
	printf("Error1!\n");
	exit( 1 );
    }

  
  for( int i = 0; i < fNumOfEvent; ++i )
    if( fscanf( fp, "%d", &fEvent_TimeRequest [ i ] ) == EOF ){
	printf("Error1!\n");
	exit( 1 );
    }


  if( fscanf( fp, "%d", &dumy ) != EOF ){
    printf( "Error2!\n" );
    exit( 1 ); 
  }

  fclose( fp );
  //////////////////////////////

  for( int i = 0; i < fNumOfEvent; ++i ){
    fNumOfStudent_Event[ i ] = 0;
    for( int s = 0; s < fNumOfStudent; ++s ){
      
      if(  studentEvent[ s ][ i ] == 1 ){
	fListStudent_Event[ i ][ fNumOfStudent_Event[ i ] ] = s;
	++fNumOfStudent_Event[ i ];
      }
    }
  }
  
  for( int i = 0; i < fNumOfEvent; ++i ){
    fNumOfProf_Event[ i ] = 0;
    for( int p = 0; p < fNumOfProf; ++p ){
      
      if(  profEvent[ p ][ i ] == 1 ){
        fListProf_Event[ i ][ fNumOfProf_Event[ i ] ] = p;
        ++fNumOfProf_Event[ i ];
      }
    }
  }


  for( int i = 0; i < fNumOfEvent; ++i ){
    for( int r = 0; r < fNumOfRoom; ++r ){
      fAvail_EventRoom[ i ][ r ] = 0; 
      if( fNumOfStudent_Event[ i ] > size_room[ r ] )
	goto EEE;

      for( int f = 0; f < numOfFeature; ++f ){
	if( eventFeature[ i ][ f ] == 1 && roomFeature[ r ][ f ] == 0 ) 
	  goto EEE;
      }
      fAvail_EventRoom[ i ][ r ] = 1; 
	
    EEE:;
    }
  }

  /*
  // test
  for( int i = 0; i < fNumOfEvent; ++i ){
    for( int r = 0; r < fNumOfRoom; ++r ){
      if ( tRand->Double(0.0, 1.0) < 0.29 ) 
	fAvail_EventRoom[ i ][ r ] = 0;
    }
  } 
  */

  int count = 0;
  for( int i = 0; i < fNumOfEvent; ++i ){
    int check = 0;
    for( int r = 0; r < fNumOfRoom; ++r ){
      check += fAvail_EventRoom[ i ][ r ];
    }
    //    if( check == 0 ){
    //      fAvail_EventRoom[ i ][ tRand->Integer(0, 4 ) ] = 1;
    //      check = 1;
    //    }
    assert( check != 0 );
    count += check; 
  } 
  printf( "Count1 = %d \n", count );

  for( int i = 0; i < fNumOfEvent; ++i )
    for( int j = 0; j < fNumOfEvent; ++j )
      fConf_EventEvent[ i ][ j ] = 0;

  int listEvent[ fNumOfEvent ];
  int numOfEvent;
  int e1, e2;

  count = 0;
  for (int s = 0; s < fNumOfStudent; ++s) {
    numOfEvent = 0;
    for (int i = 0; i < fNumOfEvent; ++i) {
      if (studentEvent[s][i] == 1) {
        listEvent[numOfEvent++] = i;
        ++count;
      }
    }
    for (int h1 = 0; h1 < numOfEvent; ++h1) {
      for (int h2 = 0; h2 < numOfEvent; ++h2) {
        e1 = listEvent[h1];
        e2 = listEvent[h2];
        ++fConf_EventEvent[e1][e2];
      }
    }
  }
  
  printf("Count2Student = %d \n", count);
  
  /* ken hachikubo add 12.25 */

  for (int p = 0; p < fNumOfProf; ++p) {
    numOfEvent = 0;
    for (int i = 0; i < fNumOfEvent; ++i) {
      if (profEvent[p][i] == 1) {
        listEvent[numOfEvent++] = i;
        ++count;
      }
    }
    for (int h1 = 0; h1 < numOfEvent; ++h1) {
      for (int h2 = 0; h2 < numOfEvent; ++h2) {
        e1 = listEvent[h1];
        e2 = listEvent[h2];
        ++fConf_EventEvent[e1][e2];
      }
    }
  }
  
  for(int i = 0; i < fNumOfEvent; ++i){
    for(int j = 0; j < fNumOfEvent; ++j ){
      if(fConf_EventEvent[ i ][ j ] == 1)
        printf("event1=%d event2=%d\n",i,j);
    }
  }
  
  printf("Count2Prof + Count2Student = %d \n", count);

  count = 0;
  for( int i = 0; i < fNumOfEvent; ++i ){
    fNumOfConfEvent_Event[ i ] = 0;
    for( int j = 0; j < fNumOfEvent; ++j ){
      if( fConf_EventEvent[ i ][ j ] != 0 ){
	fListConfEvent_Event[ i ][ fNumOfConfEvent_Event[ i ]++ ] = j; 
	++count;
      }
    }
  }
  printf( "Count3 = %d \n", count );

  fNumOfRequiredEvent = 0;
  for( int i = 0; i < fNumOfEvent; ++i ){
    if( fEvent_Required[ i ] == 1 )
      fListRequiredEvent[ fNumOfRequiredEvent++ ] = i;
  }
  
  //////////////////////////////////
  delete [] size_room;

  for( int s = 0; s < fNumOfStudent; ++s )
    delete [] studentEvent[ s ];
  delete [] studentEvent;

  for( int r = 0; r < fNumOfRoom; ++r )
    delete [] roomFeature[ r ];
  delete [] roomFeature;

  for( int i = 0; i < fNumOfEvent; ++i )
    delete [] eventFeature[ i ];
  delete [] eventFeature;  

  // for( int a = 0; a < fNumOfEvent; ++a )
  // printf( "%d ", fNumOfStudent_Event[ a ] );
}


void TEvaluator::Preprocessing()
{
  int after, before;

  for( int n1 = 0; n1 < fNumOfTimeInDay; ++n1 ){
    for( int n2 = 0; n2 < fNumOfTimeInDay; ++n2 ){
      if( n1 + n2 - 1 > 0 )
	after = n1 + n2 - 1;
      else 
	after = 0;

      before = 0;
      if( n1 - 2 > 0 )
	before += (n1 - 2);
      if( n2 - 2 > 0 )
	before += (n2 - 2);

      fDiffPenalty_S2_Insert[ n1 ][ n2 ] =  after - before;
      // printf( "%2d ",       fDiffPenalty_S2_Insert[ n1 ][ n2 ] );
    }
    // printf( "\n" );
  }

  


  /*
  for( int e1 = 0; e1 < fNumOfEvent; ++e1 ) {
    int count = 0;
    for( int e2 = 0; e2 < fNumOfEvent; ++e2 ) {
      if( fConf_EventEvent[ e1 ][ e2 ] != 0 )
	++count;
    }
    printf( "%d ", count );
  }
  */
}



