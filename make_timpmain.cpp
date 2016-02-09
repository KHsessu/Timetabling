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
int **fProfCantDo;
int **roomFeature;
int **eventFeature;
int *fEvent_Required;
int *fEvent_TimeRequest;


int make_random(void){

  /* 乱数系列の変更*/
  srand( (unsigned)time(NULL) );
}


int main(int argc, char* argv[]){
    
  char* readfile = argv[ 1 ];
  char* writefile = argv[ 2 ];

  
  if ( argc != 3){
    printf( "Error: argc = %d\n",argc);
    return -1;
  }

  //  printf("point1\n");
  //引数を移す
  //  printf("point2\n");

  //////////////////////////////
  read_file( readfile );
  //  printf("point3\n");
  make_tim();          


  write_file( writefile );

  return 0;
}

int read_file(char* readfilename){
  FILE* fp;
  char filename[80];
  
  int dumy;

  sprintf( filename, "%s", readfilename);
  printf("%s\n",filename);
  if( (fp = fopen(filename, "r")) == NULL){
    printf("can not open fp\n");
    exit( 1 );
  }else
    printf("open file\n");

  
  fscanf(fp, "%d %d %d %d %d %d %d", &fNumOfEvent, &fNumOfRoom, &fNumOfFeature, &fNumOfStudent, &fNumOfProf, &fNumOfTime, &fNumOfTimeInDay);

  printf( "%d %d %d %d %d %d %d\n", fNumOfEvent, fNumOfRoom, fNumOfFeature, fNumOfStudent, fNumOfProf, fNumOfTime, fNumOfTimeInDay );
  printf( " read file complete !!\n" );
  
  fEvent_Feature = new int [ fNumOfEvent ]; // イベントのの学年、学科、必修を数字で表す。

  size_room = new int [ fNumOfRoom ];
    
  studentEvent = new int *[ fNumOfStudent ];
  for( int s = 0; s < fNumOfStudent; ++s)
    studentEvent[ s ] = new int [ fNumOfEvent ]; 
  
  profEvent = new int *[ fNumOfProf ];
  for( int p = 0; p < fNumOfProf; ++p )
    profEvent[ p ] = new int [ fNumOfEvent ];

  fProfCantDo = new int *[ fNumOfProf ];
  for( int p = 0; p < fNumOfProf; ++p)
    fProfCantDo[ p ] = new int [ fNumOfTime ];
  
  roomFeature = new int *[ fNumOfRoom ];
  for( int r = 0; r < fNumOfRoom; ++r )
    roomFeature[ r ] = new int [ fNumOfFeature ];

  eventFeature = new int *[ fNumOfEvent ];
  for( int e = 0; e < fNumOfEvent; ++e )
    eventFeature[ e ] = new int [ fNumOfFeature ];
  
  fEvent_Required = new int [ fNumOfEvent ];
  fEvent_TimeRequest = new int [ fNumOfEvent ];

  //  for( int i; i < fNumOfEvent; ++i )
  //  fscanf( fp, "%d", fEvent_Feature[ i ] );
  
  if( fscanf(fp, "%d", &dumy ) != EOF){
    printf("error dumy can read\n");
    exit( 1 );
  }
  
  fclose( fp );
}


int write_file(char* writefilename){
  FILE* fp;
  char filename[80];

  sprintf(filename, "%s.tim",writefilename);
  fp = fopen(filename, "a");
  
  fprintf( fp,"%d %d %d %d %d", fNumOfEvent, fNumOfRoom, fNumOfFeature, fNumOfStudent, fNumOfProf);

  
  /////////* write file *////////
  
  for( int r = 0; r < fNumOfRoom; ++r )
    fprintf( fp, "%d", &size_room[ r ] );
  
  for( int s = 0; s < fNumOfStudent; ++s )
    for( int i = 0; i < fNumOfEvent; ++i )
      fprintf( fp, "%d", &studentEvent[ s ][ i ] );
  
  for( int p = 0; p < fNumOfProf; ++p )
    for( int i = 0; i < fNumOfEvent; ++i )
      fprintf( fp, "%d", &profEvent[ p ][ i ] );
  
  for( int p = 0; p < fNumOfProf; ++p)
    for (int t = 0; t < fNumOfTime; ++t)
      fprintf( fp, "%d", &fProfCantDo[ p ][ t ] );
  
  for( int r = 0; r < fNumOfRoom; ++r )
    for( int f = 0; f < fNumOfFeature; ++f )
      fprintf( fp, "%d", &roomFeature[ r ][ f ] );

  for( int i = 0; i < fNumOfEvent; ++i )
    for( int f = 0; f < fNumOfFeature; ++f )
      fprintf( fp, "%d", &eventFeature[ i ][ f ] );

  for( int i = 0; i < fNumOfEvent; ++i )
    fprintf( fp, "%d", &fEvent_Required [ i ] );
  
  for( int i = 0; i < fNumOfEvent; ++i )
    fprintf( fp, "%d", &fEvent_TimeRequest [ i ] );

}


int make_tim(void){
  int s = 0;
  int g = 0;
  int d = 1;
  int flag = 0;
  int numofdepartment = 8;	// 各学科 0:共通講義
  int grade = 4;
  int DivideStu = ( fNumOfStudent / numofdepartment ) / grade;

  printf( "%d %d %d %d \n", fNumOfStudent, numofdepartment, grade, DivideStu );
  while(1){
    if(flag == 0){		// DivideStuで学年学科を振り分けていく
      if( s % DivideStu == 0 ){
	++g;
	if( g == grade ){
	  g = 0;
	  ++d;
	  if( d == numofdepartment ){
	    flag = 1;
	    g = 0;
	    d = 1;
	  }
	}	
      }
    }else{			// 余った生徒を振り分ける
      ++g;
      if( g == grade ){
	g = 0;
	++d;
	if( d == numofdepartment ){
	  g = 0;
	  d = 0;
	}
      }
    }
    
    /* 処理開始 */
    for( int i = 0; i < fNumOfEvent; ++i ){
      if ( g == ( fEvent_Feature[ i ] / 1000 ) ){ // 学生と授業の学科が一致
	if( d == ( fEvent_Feature[ i ] / 100 ) ){ // 学生と授業の学年が一致
	  if( ( fEvent_Feature[ i ] / 10 ) == 1 ){// 授業が必修
	    studentEvent[ s ][ i ] = 1;
	  }else{		// 授業が選択
	     
	  }
	}else{			// 学年が不一致
	  
	}
      }else if( fEvent_Feature[ i ] / 1000 == 0 ){			// 共通授業
	
      }else{			// 学科が不一致
	studentEvent[ s ][ i ] = 0;
      }
    }
    /* 処理終了 */
    
    ++s;
    printf( "%d %d %d\n", g, d, s);
    if( s == fNumOfStudent ) break;
    
    
    for( int e = 0; e < fNumOfEvent; ++e ){
      
    }
  } // end of while
  
  
}
