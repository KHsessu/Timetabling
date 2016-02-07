#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<assert.h>

int make_tim(void);
int read_file(char* filename);
int write_file(char* filename);
int check_tim(void);


int fNumOfDay;
int fNumOfStudent;
int fNumOfProf;
int fNumOfDepEvent; //ひと学科の授業数
int fNumOfOtherEvent; //一般教養の授業数
int fNumOfEvent;
int fNumOfTime;
int fNumOfTimeInDay;
int fNumOfKRoom;
int fNumOf4Room;
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
int *fEvent_ProfRequest;

int make_random(void){

  /* 乱数系列の変更*/
  srand( (unsigned)time(NULL) );
  for( int i = 0; i < 10; ++i)
    rand();
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
  printf("read_file\n");
  read_file( readfile );
  printf("make_tim\n");
  make_tim();           //
  printf("write_file\n");
  write_file( writefile );
  printf("check tim\n");
  check_tim();
  return 0;
}

int read_file(char* filename){
  FILE* fp;
  int dumy;
  char fname[100];
  int room;
  int event;
  //  sprintf(fname, "./%s.txt",filename);
  //  printf("%s\n",fname);
  if( (fp = fopen(filename, "r")) == NULL){
    printf("error: can not open file");
    exit(1);
  }
  fscanf(fp, "%d %d %d %d %d %d %d", &fNumOfDepEvent, &fNumOfOtherEvent, &fNumOfKRoom, &fNumOfDepStudent, &fNumOfDepProf, &fNumOfDepartment, &fNumOfGrade);
  
  fNumOfTime = 25;
  fNumOf4Room = 4;
  fNumOfRoom = fNumOfKRoom + (fNumOfDepartment * 2) + fNumOf4Room; 
  fNumOfDay = 5;
  fNumOfTimeInDay = 5;
  fNumOfFeature = (fNumOfDepartment * 2) + 2;
  fNumOfEvent = (fNumOfDepEvent * fNumOfDepartment) + fNumOfOtherEvent;
  fNumOfStudent = fNumOfDepStudent * fNumOfDepartment;
  fNumOfProf = fNumOfDepProf * fNumOfDepartment;
  fNumOfStudent_G = fNumOfDepStudent / fNumOfGrade;
  if( fNumOfDepStudent % fNumOfGrade != 0){
    printf("Error: illigal student number");
    exit(1);
  }
  printf("fNumOfTime       =%3d\n",fNumOfTime);
  printf("fNumOfKRoom      =%3d\n",fNumOfKRoom);
  printf("fNumOfRoom       =%3d\n",fNumOfRoom);
  printf("fNumOfDay        =%3d\n",fNumOfDay);
  printf("fNumOfTimeInDay  =%3d\n",fNumOfTimeInDay);
  printf("fNumOfFeature    =%3d\n",fNumOfFeature);
  printf("fNumOfDepartment =%3d\n",fNumOfDepartment);
  printf("fNumOfDepEvent   =%3d\n",fNumOfDepEvent);
  printf("fNumOfOtherEvent =%3d\n",fNumOfOtherEvent);
  printf("fNumOfEvent      =%3d\n",fNumOfEvent);
  printf("fNumOfDepStudent =%3d\n",fNumOfDepStudent);
  printf("fNumOfStudent    =%3d\n",fNumOfStudent);
  printf("fNumOfDepProf    =%3d\n",fNumOfDepProf);
  printf("fNumOfProf       =%3d\n",fNumOfProf);
  printf("fNumOfStudent_G  =%3d\n",fNumOfStudent_G);
  
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

  fEvent_ProfRequest = new int[ fNumOfDepEvent ];

  printf("set instance\n");
  ////////////////////////////////////////
  /* scanf                              */
  for( int r = 0; r < fNumOfKRoom; ++r )
    fscanf( fp, "%d", &size_room[ r ]);
  /*
    for( int r = 0; r < fNumOfKRoom; ++r )
    fscanf( fp, "%d", &roomFeature[ r ]);
  */  
  for(int e = 0; e < fNumOfDepEvent; ++e )
    fscanf( fp, "%d", &eventFeature[ e ][ 0 ]);
  
  for( int e = 0; e < fNumOfDepEvent; ++e )
    fscanf( fp, "%d", &fEvent_ProfRequest[ e ]);

  for( int e = 0; e < fNumOfDepEvent; ++e )
    fscanf( fp, "%d", &fEvent_Grade[ e ] );

  for( int e = 0; e < fNumOfDepEvent; ++e )
    fscanf( fp, "%d", &fEvent_TimeRequest[ e ]);

  for( int e = 0; e < fNumOfDepEvent; ++e )
    if( fscanf( fp, "%d", &fEvent_Required[ e ]) == EOF ){
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
  for( int e = 0; e < fNumOfEvent; ++e ){
    printf("event%d is grade %d , department %d\n",e,fEvent_Grade[e],fEvent_Department[e]);
  }
  
  
  for(int e = 0; e < fNumOfDepEvent; ++e )
    if( eventFeature[ e ][ 0 ] == 0){
      eventFeature[ e ][ 0 ] = 1;
    }else{
      eventFeature[ e ][ eventFeature[e][0] ] = 1;
      eventFeature[ e ][ 0 ] = 0;
    }
  event = 0;
  for( int d = 0; d < fNumOfDepartment; ++d ){
    for( int ed = 0; ed < fNumOfDepEvent; ++ed ){
      event = (d * fNumOfDepEvent) + ed;
      printf("dep=%d ed=%d\n",event,ed);
      fEvent_Grade[ event ] = fEvent_Grade[ ed ];
      fEvent_Department[ event ] = (d + 1);
      fEvent_TimeRequest[ event ] = fEvent_TimeRequest[ ed ];
      fEvent_Required[ event ] = fEvent_Required[ ed ] * (d + 1);
      eventFeature[ event ][ 0 ] = eventFeature[ ed ][ 0 ];
      eventFeature[ event ][ 1 ] = eventFeature[ ed ][ 1 ];
      eventFeature[ event ][ 2*(d+1) ] = eventFeature[ ed ][ 2 ];
      eventFeature[ event ][ 2*(d+1)+1 ] = eventFeature[ ed ][ 3 ];   
    }
  }
  for( int e = 0; e < fNumOfOtherEvent; ++e ){
    event++;
    printf("dep=%d\n",event);
    fEvent_Grade[ event ] = 1;
    fEvent_Department[ event ] = 0;
    fEvent_TimeRequest[ event ] = 1;
    fEvent_Required[ event ] = 0;
    eventFeature[ event ][ 0 ] = 1;
  }
  //check
  for( int e = 0; e < fNumOfEvent; ++e ){
    printf("event%d is grade %d , department %d\n",e,fEvent_Grade[e],fEvent_Department[e]);
  }
  

  
  int student = 0;
  for( int d = 0; d < fNumOfDepartment; ++d ){
    for( int g = 0; g < fNumOfGrade; ++g ){
      for( int s = 0; s < fNumOfStudent_G; ++s){
        student = s + (g * fNumOfStudent_G) + ( d * fNumOfDepStudent );
        printf("student=%2d, d=%d, g=%d, s=%d\n",student,d,g,s); //check
        fStudent_Grade[ student ] = g;
        fStudent_Department[ student ] = d + 1; //0は般教
      }
    }
  }
}


int write_file(char* filename){
  FILE* fp;
  
  fp = fopen(filename, "a");
  
  fprintf( fp,"%d %d %d %d %d\n", fNumOfEvent, fNumOfRoom, fNumOfFeature, fNumOfStudent, fNumOfProf);

  
  /////////* write file *////////
  for( int r = 0; r < fNumOfRoom; ++r )
    fprintf( fp, "%d\n", size_room[ r ] );
  
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int i = 0; i < fNumOfEvent; ++i ){
      fprintf( fp, "%d\n", studentEvent[ s ][ i ] );
    }
  }

  for( int p = 0; p < fNumOfProf; ++p )
    for( int i = 0; i < fNumOfEvent; ++i )
      fprintf( fp, "%d\n", profEvent[ p ][ i ] );
  
  
  for( int p = 0; p < fNumOfProf; ++p)
    for (int t = 0; t < fNumOfTime; ++t)
      fprintf( fp, "%d\n", fProfCantDo[ p ][ t ]);
  
  for( int r = 0; r < fNumOfRoom; ++r )
    for( int f = 0; f < fNumOfFeature; ++f )
      fprintf( fp, "%d\n", roomFeature[ r ][ f ] );
  
  for( int i = 0; i < fNumOfEvent; ++i ){
    for( int f = 0; f < fNumOfFeature; ++f ){
      fprintf( fp, "%d\n", eventFeature[ i ][ f ] );
    }
  }

  for( int i = 0; i < fNumOfEvent; ++i )
    fprintf( fp, "%d\n", fEvent_Required [ i ] );
  
  for( int i = 0; i < fNumOfEvent; ++i )
    fprintf( fp, "%d\n", fEvent_TimeRequest [ i ] );

}


int make_tim(void){

  int student;
  int prof;
  int event;
  int room;
  int ct;
  int pf;
  int numOfProfEvent[ fNumOfDepProf ];
  int minProfEvent=0;
  int minProfCount=0;
  
  //size_room[]
  printf("size_room\n");
  room = fNumOfKRoom;
  for( int d = 0; d < fNumOfDepartment; ++d ){
    size_room[ room++ ] = fNumOfStudent_G+10;
    size_room[ room++ ] = fNumOfStudent_G+10;
  }
  for( int r = 0; r < fNumOf4Room; ++r)
    size_room[ room++ ] = fNumOfStudent+10;
  if(room != fNumOfRoom)
    printf("size_room error\n");
  
  //studentEvent[][]
  printf("studentEvent\n");
  for( int s = 0; s < fNumOfStudent; ++s ){
    for( int e = 0; e < fNumOfEvent; ++e ){
      studentEvent[ s ][ e ] = 0; //初期化
      if(fEvent_Department[ e ] == 0){ //一般教養の場合
        if( fStudent_Grade[ s ] == 0){ //1年は80%参加
          if(rand() % 10 < 7)
            studentEvent[ s ][ e ] = 1;
        }else if( fStudent_Grade[ s ] == 1 ){ //2年は20%参加
          if(rand() % 10 < 3)
            studentEvent[ s ][ e ] = 1;
        }
      }else if(fStudent_Grade[ s ] == fEvent_Grade[ e ] &&
               fStudent_Department[ s ] == fEvent_Department[ e ]){ //学年学科が一緒
        switch(fStudent_Grade[ s ]){
        case 0:
          if(rand() % 10 < 9)
            studentEvent[ s ][ e ] = 1;
          break;
        case 1:          
          if(rand() % 10 < 8)
            studentEvent[ s ][ e ] = 1;
          break;
        case 2:          
          if(rand() % 10 < 7)
            studentEvent[ s ][ e ] = 1;
          break;
        case 3:          
          if(rand() % 10 < 4)
            studentEvent[ s ][ e ] = 1;
          break;
        default:
          printf("error: illegal grade\n");
          break;
        }
      }
    }
  }
  
  //profEvent[][]
  printf("profEvent\n");
  prof = 0;
  for(int p = 0; p < fNumOfDepProf; ++p)
    numOfProfEvent[ p ] = 0;
  for( int e = 0; e < fNumOfEvent; ++e){
    for(int p = 0; p < fNumOfProf; ++p)
      profEvent[ p ][ e ] = 0; //初期化
  }
  printf("test%d\n",profEvent[35][114]);
  for( int e = 0; e < fNumOfDepEvent; ++e){
    printf("ProfRequest[%d]=%d\n",e,fEvent_ProfRequest[e]);
    if( fEvent_ProfRequest[ e ] == 1){ //必要な教授が一人の時
      profEvent[ prof++ ][ e ] = 1;
      if( prof == fNumOfDepProf ) prof = 0;
    }else if(fEvent_ProfRequest[ e ] >= 2){ //複数の教授が必要な場合
      ct = 0;
      while(ct < fEvent_ProfRequest[ e ]){
        pf = rand() % fNumOfDepProf;
        //        printf("%d\n",pf);
        while(1){
          if( profEvent[ pf ][ e ] == 0 && numOfProfEvent[ pf ] <= minProfEvent){
            profEvent[ pf ][ e ] = 1;
            ct++;
            numOfProfEvent[ pf ]++;
            if(minProfEvent < numOfProfEvent[ pf ])
              minProfEvent = numOfProfEvent[ pf ];
            break;
          }else{
            pf++;
            pf %= fNumOfDepProf;
            //            printf("%d\n",pf);
            //            printf("numOfProfEvent[ %d ]=%d\n",pf,numOfProfEvent[pf]);
            //    printf("profEvent[ %d ][ %d ] = %d\n",pf,e,profEvent[pf][e]);
          }
        }
      }
    }
  }
  
  for( int d = 0; d < fNumOfDepartment-1; ++d){
    for( int e = 0; e < fNumOfDepEvent; ++e){
      for( int p = 0; p < fNumOfDepProf; ++p ){
        profEvent[ (fNumOfDepProf*(d+1))+p ][ (fNumOfDepEvent*(d+1))+e ] = profEvent[ p ][ e ];
      }
    }
  }
  
  //fProfCantDo[][]
  printf("fProfCantDo\n");
  for( int p = 0; p < fNumOfProf; ++p ){
    for( int t = 0; t < fNumOfTime; ++t ){
      fProfCantDo[ p ][ t ] = 0;
      if(rand() % 20 == 0) //5%の確立で授業できない
        fProfCantDo[ p ][ t ] = 1;
    }
  }

  //roomFeature[][]
  printf("roomFeature\n");
  for( int r = 0; r < fNumOfRoom; ++r ){
    for( int f = 0; f < fNumOfFeature; ++f){
      roomFeature[ r ][ f ] = 0;
    }
  }
  for( int r = 0; r < fNumOfKRoom; ++r )
    roomFeature[ r ][ 1 ] = 1;
    
  room = fNumOfKRoom;
  for( int d = 0; d < fNumOfDepartment; ++d ){
    roomFeature[ room++ ][ 2*(d+1) ] = 1;
    roomFeature[ room++ ][ 2*(d+1)+1 ] = 1;
  }
  printf("room=%d\n",room);
  for(int r = 0; r < fNumOf4Room; ++r)
  roomFeature[ room++ ][ 0 ] = 1;
  printf("room=%d\n",room);
  if(room != fNumOfRoom)
    printf("roomFeature error\n");
  
  //eventFeature[][]
  //fEvent_Required[]
  //fEvent_TimeRequest[]
  //fEvent_Grade[]
  //fEvent_Department[]
  //readfileにて
  
}


int check_tim(){
  int rfct;
  int efct;
  int stct;
  
  for( int r = 0; r < fNumOfRoom; ++r )
    assert( size_room[ r ] > 0 );

  for( int i = 0; i < fNumOfEvent; ++i ){
    stct = 0;
    for( int s = 0; s < fNumOfStudent; ++s ){
      if(studentEvent[ s ][ i ] == 1){
        printf("student%d attend\n",s);
        stct++;
      }
      assert( studentEvent[ s ][ i ] == 1 || studentEvent[ s ][ i ] == 0);
    }
    if(fEvent_Department[ i ] == 0){
      printf("event%d stct = %d\n",i,stct);
      assert( stct < fNumOfStudent + 1 );
    }else{
      printf("stct = %d\n",stct);
      assert( stct < fNumOfStudent_G + 1);
    }
  }
  
  for( int p = 0; p < fNumOfProf; ++p ){
    for( int i = 0; i < fNumOfEvent; ++i){
      printf("profEvent[%d][%d]=%d\n",p,i,profEvent[p][i]);
      assert( profEvent[ p ][ i ] == 1 || profEvent[ p ][ i ] == 0);
    }
  }
  for( int p = 0; p < fNumOfProf; ++p )
    for( int t = 0; t < fNumOfTime; ++t)
      assert( fProfCantDo[ p ][ t ] == 1 || fProfCantDo[ p ][ t ] == 0);

  for( int r = 0; r < fNumOfRoom; ++r ){
    rfct = 0;
    for( int f = 0; f < fNumOfFeature; ++f ){
      printf("roomFeature[ %d ][ %d ] = %d\n",r,f,roomFeature[r][f]);
      if( roomFeature[ r ][ f ] == 1)
        rfct++;
    }
    assert(rfct == 1);
  }

  for( int i = 0; i < fNumOfEvent; ++i){
    efct = 0;
    for( int f = 0; f < fNumOfFeature; ++f ){
      printf("eventFeature[ %d ][ %d ] = %d\n",i,f,eventFeature[i][f]);
      if( eventFeature[ i ][ f ] == 1)
        efct++;
    }
    assert(efct == 1);
  }
  
  for( int i = 0; i < fNumOfEvent; ++i){
    assert( fEvent_Required[ i ] > -1);
  }
  for( int i = 0; i < fNumOfEvent; ++i){
    assert( fEvent_TimeRequest[ i ] > 0 );
  }
}
