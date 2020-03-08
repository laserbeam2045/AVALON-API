#ifndef _COUNT_COMBO_H_
#define _COUNT_COMBO_H_

#include <stdbool.h>
#include "../Adjacent.h"
#include "../SearchNode.h"
#include "../search_conditions/SearchConditions.h"
#include "check_how_clear.h"

// 初期化関数
extern void CountCombo_init(BoardSettings *bsp);

// コンボ数などを数える関数
extern void countCombo_5x6(SearchNode *node, SearchConditions *searchConditions, bool dropFallFlag);
extern void countCombo_6x7(SearchNode *node, SearchConditions *searchConditions, bool dropFallFlag);

// クラス変数
extern int CountCombo_canNotClearDrops;   // 消せないドロップ（ビットフラグ)
extern int CountCombo_activeDrops;        // 落ちうるドロップ（ビットフラグ）

#endif  // _COUNT_COMBO_H_