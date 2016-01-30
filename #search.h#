#ifndef __SEARCH__
#define __SEARCH__

#ifndef __RAND__
#include "rand.h"
#endif

#ifndef __Sort__
#include "sort.h"
#endif

#ifndef __EVALUATOR__
#include "evaluator.h"
#endif

#ifndef __OPERATPR__
#include "operator.h"
#endif

#ifndef __INDI__
#include "indi.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

class TSearch : public TOperator{

public:
  TSearch();
  ~TSearch();
  void Set( TEvaluator* evaluator );  // メンバ変数などの定義
  void SetParameter( int* para );     // パラメータ値の設定
  
  void MakeInitRandSol();         // ランダム解を生成
  void LS_MakeFeasible();         // 実行可能解を生成
  void Eject_NoStudentEvent();    // 空イベントの削除（あれば）
  void Insert_NoStudentEvent();   // 空イベントの挿入（あれば）
  void LS_Relocation();           // relocation近傍のみでLS
  void LS_Relocation_Swap();      // relocationとswap近傍でLS
  void LS_Relocation_Swap_Extend();  // swapでroomが変ってもOK
  void DispFeatureSol();          // 解の特徴量を表示
  int Distance( TIndi& indi, int flag ); // 説明保留（使わない）
  int Distance_D( TIndi& indi1, TIndi& indi2 ); // 説明保留（使わない）
  int Distance_D( TIndi& indi1 ); // 説明保留（使わない）
  void QuasiGES();                // 説明保留（使わない）
  void Perturbation();            // 説明保留（使わない）

  TIndi tIndi_LS;

  int fNumOfIterLS;              // Iteration回数
  int fMaxNumOfIterLS;           // １試行のIteration回数の上限
  int fNumOfIterFindBest;        // ベスト解を発見時のIteration回数
  int fTabuTenure;               // tabu tenure
  char *fDstFile;                // 出力ファイル名

  int **fMoved_EventTime;        // tabu list
  int ***fMoved_EventTimeRoom;   // tabu list

  int fRadioActiveEvent; // 論文中のparameter ratio (Test3)
};

#endif
