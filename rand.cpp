#ifndef __RAND__
#include "rand.h"
#endif

TRandom* tRand = NULL;

void InitRandom( void )
{
  int seed;
  unsigned short seed16v[3];
  // printf("input rand seed = ");
  // scanf("%d",&seed);
  seed = 1111;
  printf( "seed = 1111\n" ); fflush(stdout);

  seed16v[0] = 100;
  seed16v[1] = 200;
  seed16v[2] = seed;

  seed48( seed16v );
  tRand = new TRandom();
  srand( seed );

  srand((unsigned int)time(NULL));   // comment out this line when using a fixed value
}

TRandom::TRandom()
{
}

TRandom::~TRandom()
{
}

int TRandom::Integer( int minNumber, int maxNumber )
{
  return minNumber + (int)(drand48() * (double)(maxNumber - minNumber + 1));
}

double TRandom::Double( double minNumber, double maxNumber )
{
  return minNumber + drand48() * (maxNumber - minNumber);
}

void TRandom::Permutation( int *array, int numOfElement, int numOfSample )
{
  int i,j,k,r;
  int *b;

  b= new int[numOfElement];

  for(j=0;j<numOfElement;j++) b[j]=0;
  for(i=0;i<numOfSample;i++)
  {  
    r=rand()%(numOfElement-i);
    k=0;
    for(j=0;j<=r;j++)
    {
      while(b[k]==1)
      {
	k++;
      }
      k++;
    }
    array[i]=k-1;
    b[k-1]=1;
  }
  delete [] b;
}

void TRandom::Permutation( int *array, int numOfElement )
{
  int s, e;
  int stock;

  for( int i = 0; i < numOfElement; ++i )
    array[ i ] = i;

  for( int i = numOfElement; i > 1; --i )
  {
    e = i - 1;
    s = rand() % i;
    stock = array[ e ];
    array[ e ] = array[ s ];
    array[ s ] = stock;
  }
}

double TRandom::NormalDistribution( double mu, double sigma )
{
  double U1,U2,X;
  double PI = 3.1415926;
  
  while( 1 ){
    U1 = this->Double( 0.0, 1.0 );
    if( U1 != 0.0 ) break;
  }
  U2 = this->Double( 0.0, 1.0 );

  X = sqrt(-2.0*log(U1)) * cos(2*PI*U2);
  return( mu + sigma*X );
}




