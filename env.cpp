#ifndef __ENVIRONMENT__
#include "env.h"
#endif

TEnvironment::TEnvironment()
{
}


TEnvironment::~TEnvironment()
{
  delete tEvaluator;
  delete tSearch;
}

void TEnvironment::Define()
{
  tEvaluator = new TEvaluator();
  tEvaluator->SetInstance( fInstanceName );
  tEvaluator->Preprocessing();

  tSearch = new TSearch();
  tSearch->Set( tEvaluator );  
  tSearch->fDstFile = fDstFile;

  tIndi.Define( tEvaluator->fNumOfEvent );
}

void TEnvironment::SetParameter( int *para )
{
  tSearch->SetParameter( para );  
}

void TEnvironment::Result( int numOfTrial, char* dstFile )
{
  FILE *fp;
  char filename[80];

  // assert( tEvaluator->CheckFeasibility( tIndi ) == 1 );

  printf( "n = %d val = %d \n", numOfTrial, (int)tIndi.fEvaluationValue );
  fflush(stdout);

  sprintf( filename, "./result/%s_Result", dstFile );
  fp = fopen( filename, "a" );
  fprintf( fp, "%d %d %lf %d %d %d %d\n", numOfTrial, tSearch->fNumOfIterFindBest  , (double)(fTimeEnd - fTimeStart)/(double)CLOCKS_PER_SEC, (int)tIndi.fEvaluationValue , tIndi.fPenalty_S1 , tIndi.fPenalty_S2 , tIndi.fPenalty_S3 );
  fclose(fp);

  sprintf( filename, "./result/%s%d_Sol", dstFile , numOfTrial );
  fp = fopen( filename, "a" );
  tIndi.WriteTo( fp );
  fclose(fp);
}



void TEnvironment::DoIt()
{
  fTimeStart = clock();

  /*
  tSearch->MakeInitRandSol();
  tSearch->LS_FindFeasible();
  tIndi = tSearch->tIndi_LS;
  tSearch->TransFromIndi( tIndi );
  tSearch->CalEvaluation();
  */

  while( 1 ){
    tSearch->ResetSol();
    tSearch->LS_MakeFeasible();         // 内容見てるなう。
    if( tSearch->fNumOfEjectEvent == 0 )
      break;
  } 
  // tSearch->Eject_NoStudentEvent();   // Option

  // tSearch->LS_Relocation();
  // tSearch->LS_Relocation_Swap();
  tSearch->LS_Relocation_Swap_Extend();
  //  tSearch->QuasiGES();

  // tSearch->Insert_NoStudentEvent();  // Option

  tSearch->CalEvaluation();

  tSearch->TransToIndi( tIndi );



  /*
  tSearch->ResetSol();
  tSearch->LS_MakeFeasible();
  tSearch->fMaxNumOfIterLS = 1000;
  tSearch->LS_Normal();
  tSearch->fMaxNumOfIterLS = 10000;
  tSearch->GES_main();
  tIndi = tSearch->tIndi_LS;
  tSearch->TransFromIndi( tIndi );
  tSearch->CalEvaluation();
  */


  printf( "S1 = %d S2 = %d S3 = %d \n",  tSearch->fPenalty_S1,  tSearch->fPenalty_S2,  tSearch->fPenalty_S3 );


  fTimeEnd = clock();
}




