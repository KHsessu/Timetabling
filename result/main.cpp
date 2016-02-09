#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
using namespace std;

int main( int argc, char* argv[])
{

  int timetable[100][50];
  int fTimeRoom_Event[400][2];
  int fNumOfEvent;
  int dumy;
  int numoftime = 0;
  int numofroom = 0;
  
  if(argc != 2)
    return -1;
  
  char* DstFile = argv[1];

  FILE *fp;
  char filename[80];
  sprintf(filename,"./%s_Sol", DstFile);

  if((fp = fopen(filename,"r")) == NULL){
    printf("error1\n");
    return -1;
  }
  
  fscanf(fp,"%d",&fNumOfEvent);
  //  printf("%d\n",fNumOfEvent);
  for(int e = 0; e < fNumOfEvent; ++e){
    if((fscanf(fp,"%d %d",&fTimeRoom_Event[ e ][ 0 ], &fTimeRoom_Event[ e ][ 1 ])) == EOF)
    printf("%d %d\n",fTimeRoom_Event[e][0],fTimeRoom_Event[e][1]);
  }

  if((fscanf(fp,"%d", &dumy))!=EOF){
    printf("error\n");
    return -1;
  }
  
  fclose(fp);
  
  for(int e = 0; e < fNumOfEvent; ++e){
    if(numoftime < fTimeRoom_Event[ e ][ 0 ])
      numoftime = fTimeRoom_Event[e][0];
    if(numofroom < fTimeRoom_Event[ e ][ 1 ])
      numofroom = fTimeRoom_Event[e][1];
    
    timetable[ fTimeRoom_Event[e][0] ][ fTimeRoom_Event[e][1] ] = e+1;
    
  }
  
  //  printf("check %d %d\n",numoftime,numofroom);
  //  /*
  for( int r = 0; r < numofroom+1; ++r){
    for(int t = 0; t < numoftime+1; ++t){
      printf("%3d ", timetable[ t ][ r ]);
    }
    printf("\n");
  }
  //  */
}
