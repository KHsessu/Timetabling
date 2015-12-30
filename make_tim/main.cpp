#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>


int make_tim(void);
int read_file(char* filename);
int write_file(char* filename);



int fNumOfStudent;
int fNumOfProf;
int fNumOfEvent;
int fNumOfTime;
int fNumOfTimeInDay;
int fNumOfRoom;
int fNumOfFeature;

int *fEvent_Feature;
int *size_room;
int **studentEvent;
int **profEvent;
int *fProfCantDo;
int **roomFeature;
int **eventFeature;
int *fEvent_Required;
int *fEvent_TimeRequest;

int make_random(void){

  /* 乱数系列の変更*/
  srand( (unsigned)time(NULL) );
}


int main(int argc, char* argv[]){
  
  
  char* readfile;
  char* writefile;

  

  if ( argc != 3){
    printf( "Error: argc = %d\n",argc);
    return -1;
  }
  
  //引数を移す
  readfile = argv[ 1 ];
  writefile = argv[ 2 ];


  //////////////////////////////
  read_file( readfile );
  make_tim();           //


  write_file( writefile );

  return 0;
}

int read_file(char* filename){
  FILE* fp;
  
  fp = fopen(filename, "r");
  fscanf(fp, "%d %d %d %d %d %d %d", fNumOfEvent, fNumOfRoom, fNumOfFeature, fNumOfStudent, fNumOfProf, fNumOfTime, fNumOfTimeInDay);

  fEvent_Feature = new int [fNumOfEvent];
  for( int e = 0; e < fNumOfEvent; ++e)
    fscanf(fp, "%d", fEvent_Feature[ e ]);
  
  fclose( fp );
}


int write_file(char* filename){
  FILE* fp;
  
  fp = fopen(filename, "a");
  
  fprintf( fp,"%d %d %d %d %d", fNumOfEvent, fNumOfRoom, fNumOfFeature, fNumOfStudent, fNumOfProf);

  
  /////////* write file *////////
  for( int r = 0; r < fNumOfRoom; ++r )
    fprintf( fp, "%d", &size_room[ r ] );
  
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int i = 0; i < fNumOfEvent; ++i ){
      fprintf( fp, "%d", &studentEvent[ s ][ i ] );
    }
  }
  /* ken hachikubo add 12.12. */
  for( int p = 0; p < fNumOfProf; ++p )
    for( int i = 0; i < fNumOfEvent; ++i )
      fprintf( fp, "%d", &profEvent[ p ][ i ] );
  
  /* ken hachikubo add 12.30 */ 
  for( int p = 0; p < fNumOfProf; ++p)
    for (int t = 0; t < fNumOfTime; ++t)
      fprintf( fp, "%d", &fProfCantDo);
  
  for( int r = 0; r < fNumOfRoom; ++r )
    for( int f = 0; f < fNumOfFeature; ++f )
      fprintf( fp, "%d", &roomFeature[ r ][ f ] );

  for( int i = 0; i < fNumOfEvent; ++i ){
    for( int f = 0; f < fNumOfFeature; ++f ){
      fprintf( fp, "%d", &eventFeature[ i ][ f ] );
    }
  }

  /* ken hachikubo add 12.14 */
  for( int i = 0; i < fNumOfEvent; ++i )
    fprintf( fp, "%d", &fEvent_Required [ i ] );
  
  for( int i = 0; i < fNumOfEvent; ++i )
    fprintf( fp, "%d", &fEvent_TimeRequest [ i ] );

}


int make_tim(void){

}
