#ifndef __ENVIRONMENT__
#include "env.h"
#endif

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char* argv[] )
{
  int para[ 10 ];

  if ( argc != 7 ){
    fprintf( stderr,"usage: %s numOfTrial OutputName InstanceName para1 para2 \n", argv[0] );
    return 1;
  }
  int numOfTrial;
  sscanf( argv[1], "%d", &numOfTrial );
  char* dstFile = argv[2];

  InitRandom();  
  InitSort();

  TEnvironment* tEnv = NULL;
  tEnv = new TEnvironment();

  tEnv->fInstanceName = argv[3];

  sscanf( argv[4], "%d", &para[ 0 ] );
  sscanf( argv[5], "%d", &para[ 1 ] );
  sscanf( argv[6], "%d", &para[ 2 ] ); 

  tEnv->fDstFile = dstFile;
  tEnv->Define();
  tEnv->SetParameter( para );
  
  for( int n = 1; n <= numOfTrial; ++n )
  { 
    tEnv->DoIt();
    tEnv->Result( n, dstFile );
  }

  return 0;
}
