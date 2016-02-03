#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>


int make_tim(void);
int read_file(char* filename);
int write_file(char* filename);


int fNumOfDay;
int fNumOfStudent;
int fNumOfProf;
int fNumOfDepEvent; //ひと学科の授業数
int fNumOfOtherEvent; //一般教養の授業数
int fNumOfEvent;
int fNumOfTime;
int fNumOfTimeInDay;
int fNumOfRoom;
int fNumOfFeature;
int fNumOfDepStudent;
int fNumOfDepProf;
int fNumOfDepartment;
int fNumOfGrade;
int fNumOfStudent_G;

int *size_room;
int **studentEvent;
int **profEvent;
int **fProfCantDo;
int **roomFeature;
int **eventFeature;
int *fEvent_Required;
int *fEvent_TimeRequest;
int *fEvent_Department;
int *fEvent_Grade;
int *fStudent_Department;
int *fStudent_Grade;

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
  int dumy;
  fp = fopen(filename, "r");
  fscanf(fp, "%d %d %d %d %d %d %d", fNumOfDepEvent, fNumOfOtherEvent, fNumOfRoom, fNumOfDepStudent, fNumOfDepProf, fNumOfDepartment, fNumOfGrade);
  
  fNumOfTime = 25;
  fNumOfDay = 5;
  fNumOfTimeInDay = 5;
  fNumOfFeature = fNumOfStudent * 2;
  fNumOfEvent = (fNumOfDepEvent * fNumOfDepartment) + fNumOfOtherEvent;
  fNumOfStudent = fNumOfDepStudent * fNumOfDepartment;
  fNumOfProf = fNumOfDepProf * fNumOfDepartment;
  fNumOfStudent_G = fNumOfDepStudent / fNumOfGrade;
  if( fNumOfDepStudent % fNumOfGrade != 0){
    printf("Error: illigal student number");
    exit(1);
  }
  
  size_room = new int [ fNumOfRoom ];

  /* [ stuent ][ event ] -> 1:student attends event 0:otherwise */
  studentEvent = new int* [ fNumOfStudent ];
  for( int s = 0; s < fNumOfStudent; ++s )
    studentEvent[ s ] = new int [ fNumOfEvent ];
  /* [ professor ][ event ] -> 1:professor do event 0:otherwise */
  profEvent = new int* [ fNumOfProf ];
  for( int p = 0; p < fNumOfProf; ++p )
    profEvent[ p ] = new int [ fNumOfEvent ];
  /* [ prof ] [ time ] -> 1:prof can't do this event in time */
  fProfCantDo = new int* [ fNumOfProf ];
  for( int p = 0; p < fNumOfProf; ++p)
    fProfCantDo[ p ] = new int [ fNumOfTime ];
  /* [ room ][ feature ] -> room has feature */
  roomFeature = new int* [ fNumOfRoom ];
  for( int r = 0; r < fNumOfRoom; ++r )
    roomFeature[ r ] = new int [ fNumOfFeature ];
  /* [ event ][ feature ] -> event requires feature */
  eventFeature = new int* [ fNumOfEvent ];
  for( int i = 0; i < fNumOfEvent; ++i )
    eventFeature[ i ] = new int [ fNumOfFeature ];
  /* [ event ] -> 1:this event is required subject */
  fEvent_Required = new int [ fNumOfEvent ];
  /* [ event ] -> timeslot nessesary to the event */
  fEvent_TimeRequest = new int [ fNumOfEvent ];

  fEvent_Department = new int [ fNumOfEvent ];

  fEvent_Grade = new int [ fNumOfEvent ];
  
  fStudent_Department = new int [ fNumOfStudent ];

  fStudent_Grade = new int [ fNumOfStudent ];

  fEvent_Grade = new int [ fNumOfEvent ];

  fEvent_Department = new int [ fNumOfDepEvent ];
  fEvent_Grade = new int [ fNumOfDepEvent ];
  
  ////////////////////////////////////////
  /* scanf                              */

  for( int e = 0; e < fNumOfDepEvent; ++e )
    fscanf( fp, "%d", fEvent_Grade[ e ] );
  
  for( int e = 0; e < fNumOfDepEvent; ++e )
    fscanf( fp, "%d", fEvent_TimeRequest[ e ]);
  
  for( int e = 0; e < fNumOfDepEvent; ++e )
    if( fscanf( fp, "%d", fEvent_Required[ e ]) == EOF ){
      printf("Read File Error");
      exit(1);
    }
  
  if( fscanf( fp, "%d",dumy) != EOF){
    printf("Read File Error");
    exit(1);
  }
  fclose( fp );
  printf("file read complete\n");

  ///////////////////////////////////////
  
  int event = 0;
  for( int d = 0; d < fNumOfDepartment; ++d ){
    for( int ed = 0; ed < fNumOfDepEvent; ++ed ){
      event = (d * fNumOfDepEvent) + ed;
      printf("dep=%d\n",event);
      fEvent_Grade[ event ] = fEvent_Grade[ ed ];
      fEvent_Department[ event ] = fEvent_Grade[ ed ] * (d + 1);
      fEvent_TimeRequest[ event ] = fEvent_TimeRequest[ ed ];
      fEvent_Required[ event ] = fEvent_Required[ ed ] * (d + 1);
    }
  }
  for( int e = 0; e < fNumOfOtherEvent; ++e ){
    event++;
    fEvent_Grade[ event ] = 1;
    fEvent_Department[ event ] = 0;
    fEvent_TimeRequest[ event ] = 1;
    fEvent_Required[ event ] = 0;
  }

  
  int student = 0;
  for( int d = 0; d < fNumOfDepartment; ++d ){
    for( int g = 0; g < fNumOfGrade; ++g ){
      for( int s = 0; s < fNumOfStudent_G; ++s){
        student = s + (g + fNumOfStudent_G) + ( d * fNumOfDepStudent );
        printf("student=%d\n",student); //check
        fStudent_Grade[ student ] = g;
        fStudent_Department[ student ] = d;
      }
    }
  }
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

  for( int p = 0; p < fNumOfProf; ++p )
    for( int i = 0; i < fNumOfEvent; ++i )
      fprintf( fp, "%d", &profEvent[ p ][ i ] );
  
  
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

  for( int i = 0; i < fNumOfEvent; ++i )
    fprintf( fp, "%d", &fEvent_Required [ i ] );
  
  for( int i = 0; i < fNumOfEvent; ++i )
    fprintf( fp, "%d", &fEvent_TimeRequest [ i ] );

}


int make_tim(void){
  
  
  
}
