#ifndef __INDI__
#include "indi.h"
#endif

TIndi::TIndi()
{
}

TIndi::TIndi( int n , int t , int r)
{               
  fNumOfEvent = n;
  fTimeRoom_Event = new int* [ fNumOfEvent ];
  for( int e = 0; e < fNumOfEvent; ++e ) 
    fTimeRoom_Event[ e ] = new int [ 2 ];
  fNumOfTime = t;
  fNumOfRoom = r;
  fEvent_TimeRoom = new int* [ fNumOfTime ];
  for( int t = 0; t < fNumOfTime; ++t)
    fEvent_TimeRoom[ t ] = new int[ fNumOfRoom ];
}

TIndi::~TIndi()
{
  for( int e = 0; e < fNumOfEvent; ++e ) 
    delete [] fTimeRoom_Event[ e ]; 
  delete [] fTimeRoom_Event;
}

TIndi& TIndi::operator = ( const TIndi& src )
{
  assert ( fNumOfEvent == src.fNumOfEvent );

  fNumOfEvent = src.fNumOfEvent;
  fNumOfEjectEvent = src.fNumOfEjectEvent;
  fPenalty_S = src.fPenalty_S;
  fEvaluationValue = src.fEvaluationValue;  

  for( int e = 0; e < fNumOfEvent; ++e ){
    fTimeRoom_Event[ e ][ 0 ] = src.fTimeRoom_Event[ e ][ 0 ];
    fTimeRoom_Event[ e ][ 1 ] = src.fTimeRoom_Event[ e ][ 1 ];
  }

  return *this;
}

void TIndi::Define( int n , int t , int r)
{
  fNumOfEvent = n;
  fTimeRoom_Event = new int* [ fNumOfEvent ];
  for( int e = 0; e < fNumOfEvent; ++e ) 
    fTimeRoom_Event[ e ] = new int [ 2 ];
  fNumOfTime = t;
  fNumOfRoom = r;
  fEvent_TimeRoom = new int* [ fNumOfTime ];
  for( int t = 0; t < fNumOfTime; ++t)
    fEvent_TimeRoom[ t ] = new int[ fNumOfRoom ];
  
} 

void TIndi::PrintOn() const
{


  
}       

bool TIndi::ReadFrom( FILE* fp ) 
{
  int n;  

  if( feof( fp ) != 0 )
    return false;

  if( fscanf( fp, "%d", &n ) == EOF ) 
    return false;
  assert( fNumOfEvent == n );
  for( int e = 0; e < fNumOfEvent; ++e ){
    if( fscanf( fp, "%d %d", &fTimeRoom_Event[ e ][ 0 ], &fTimeRoom_Event[ e ][ 1 ] ) == EOF )
      return false;
  }

  return true;
}


bool TIndi::WriteTo( FILE* fp ) const
{
  fprintf( fp, "%d\n", fNumOfEvent );

  for( int e = 0; e < fNumOfEvent; ++e ){
    fprintf( fp, "%d %d\n", fTimeRoom_Event[ e ][ 0 ], fTimeRoom_Event[ e ][ 1 ] );
  }
  fprintf( fp, "\n" );

  return true;
}
