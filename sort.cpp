#ifndef __Sort__
#include "sort.h"
#endif

TSort* tSort = NULL;

void InitSort( void )
{
  tSort = new TSort();
}

TSort::TSort()
{
  fMax = 10000;
  fChecked = new int [ fMax ];
}

TSort::~TSort()
{
}

void TSort::Index( double* Arg, int numOfArg, int* indexOrderd, int numOfOrd )
{
  int indexBest;
  double valueBest;

  assert( Arg[0] < 99999999999.9 );
  assert( numOfArg < fMax );

  for( int i = 0 ; i < numOfArg ; ++i ) 
    fChecked[ i ] = 0;
  
  for( int i = 0; i < numOfOrd; ++i )
  {
    valueBest = 99999999999.9;
    for( int j = 0; j < numOfArg; ++j )
    {
      if( ( Arg[j] < valueBest ) && fChecked[j]==0){
	valueBest = Arg[j];
	indexBest = j;
      }
    }
    indexOrderd[ i ]=indexBest;
    fChecked[ indexBest ]=1;
  }
}


void TSort::Index_B( double* Arg, int numOfArg, int* indexOrderd, int numOfOrd )
{
  int indexBest;
  double valueBest;

  assert( Arg[0] > -99999999999.9 );
  assert( numOfArg < fMax );

  for( int i = 0 ; i < numOfArg ; ++i ) 
    fChecked[ i ] = 0;
  
  for( int i = 0; i < numOfOrd; ++i )
  {
    valueBest = -99999999999.9;
    for( int j = 0; j < numOfArg; ++j )
    {
      if( ( Arg[j] > valueBest ) && fChecked[j]==0){
	valueBest = Arg[j];
	indexBest = j;
      }
    }
    indexOrderd[ i ]=indexBest;
    fChecked[ indexBest ]=1;
  }
}


void TSort::Index( int* Arg, int numOfArg, int* indexOrderd, int numOfOrd )
{
  int indexBest;
  int valueBest;

  assert( Arg[0] <= 999999999 );
  assert( numOfArg < fMax );

  for( int i = 0 ; i < numOfArg ; ++i ) 
    fChecked[ i ] = 0;
  
  for( int i = 0; i < numOfOrd; ++i )
  {
    valueBest = 99999999;
    for( int j = 0; j < numOfArg; ++j )
    {
      if( ( Arg[j] < valueBest ) && fChecked[j]==0){
	valueBest = Arg[j];
	indexBest = j;
      }
    }
    indexOrderd[ i ]=indexBest;
    fChecked[ indexBest ]=1;
  }
}

void TSort::Index_B( int* Arg, int numOfArg, int* indexOrderd, int numOfOrd )
{
  int indexBest;
  int valueBest;

  assert( Arg[0] > -999999999 );
  assert( numOfArg < fMax );

  for( int i = 0 ; i < numOfArg ; ++i ) 
    fChecked[ i ] = 0;
  
  for( int i = 0; i < numOfOrd; ++i )
  {
    valueBest = -999999999;
    for( int j = 0; j < numOfArg; ++j )
    {
      if( ( Arg[j] > valueBest ) && fChecked[j]==0){
	valueBest = Arg[j];
	indexBest = j;
      }
    }
    indexOrderd[ i ]=indexBest;
    fChecked[ indexBest ]=1;
  }
}


void TSort::Sort( int* Arg, int numOfArg )
{
  int indexBest;
  int valueBest;
  int stock;

  assert( Arg[0] < 99999999 );
  assert( numOfArg < fMax );

  for( int i = 0; i < numOfArg; ++i )
  {
    valueBest = 99999999;
    for( int j = i; j < numOfArg; ++j )
    {
      if( ( Arg[j] < valueBest ) ){
	valueBest = Arg[j];
	indexBest = j;
      }
    }
    stock = Arg[ i ];
    Arg[ i ] = Arg[ indexBest ];
    Arg[ indexBest ] = stock;
  }
}


void TSort::Sort( double* Arg, int numOfArg )
{
  int indexBest;
  double valueBest;
  double stock;

  assert( Arg[0] < 99999999.9 );
  assert( numOfArg < fMax );

  for( int i = 0; i < numOfArg; ++i )
  {
    valueBest = 99999999.9;
    for( int j = i; j < numOfArg; ++j )
    {
      if( ( Arg[j] < valueBest ) ){
	valueBest = Arg[j];
	indexBest = j;
      }
    }
    stock = Arg[ i ];
    Arg[ i ] = Arg[ indexBest ];
    Arg[ indexBest ] = stock;
  }
}


void TSort::Sort_B( int* Arg, int numOfArg )
{
  int indexBest;
  int valueBest;
  int stock;

  assert( Arg[0] > -99999999 );
  assert( numOfArg < fMax );

  for( int i = 0; i < numOfArg; ++i )
  {
    valueBest = -99999999;
    for( int j = i; j < numOfArg; ++j )
    {
      if( ( Arg[j] > valueBest ) ){
	valueBest = Arg[j];
	indexBest = j;
      }
    }
    stock = Arg[ i ];
    Arg[ i ] = Arg[ indexBest ];
    Arg[ indexBest ] = stock;
  }
}

void TSort::QuickIndex( double* Arg, int* indexOrderd, int begin, int end )
{
  int i, j, m;
  double pivot;
  int stock; 
  int flag; 

  flag = 0;

  for( m = (begin + end)/2; m < end; ++m ){
    if( Arg[ indexOrderd[m] ] != Arg[ indexOrderd[m+1] ] ){
      if( Arg[ indexOrderd[m] ] > Arg[ indexOrderd[m+1] ] )
	pivot = Arg[ indexOrderd[m] ];
      else
	pivot = Arg[ indexOrderd[m+1] ];
      flag = 1;
      break;
    }
  }
  if( flag == 0 ){
    for( m = (begin + end)/2; m > begin; --m ){
      if( Arg[ indexOrderd[m] ] != Arg[ indexOrderd[m-1] ] ){
	if( Arg[ indexOrderd[m] ] > Arg[ indexOrderd[m-1] ] )
	  pivot = Arg[ indexOrderd[m] ];
	else
	  pivot = Arg[ indexOrderd[m-1] ];
	flag = 1;
	break;
      }
    }
  }

  if( flag == 0 )
    return;


  i = begin;
  j = end;

  while(1)
  {

    while(1){
      if( Arg[ indexOrderd[i] ] >= pivot )
	break;
      ++i;
    }
    while(1){
      if( Arg[ indexOrderd[j] ] < pivot )
	break;
      --j;
    }

    if( i >= j ) 
      break;

    stock = indexOrderd[i];
    indexOrderd[i] = indexOrderd[j];
    indexOrderd[j] = stock;
  }

  if( begin < i-1 )
    this->QuickIndex( Arg, indexOrderd, begin, i-1 );
  if( j+1 < end )
    this->QuickIndex( Arg, indexOrderd, j+1, end );
}


