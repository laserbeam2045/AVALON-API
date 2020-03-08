#include "evaluation_functions.h"

// プライベート関数
static int getClearingPoint(ComboData*, ClearingSettings*); 
static double getEvaluationBy(const char leader, ComboData*, SearchSettings*, ClearingSettings*, int depth);
static double evaluateAnubis(ComboData*);
static double evaluateMetatron(ComboData*, ClearingSettings*);
static double evaluateKomasan(ComboData*);
static double evaluateAmen(ComboData*);
static double evaluateHylen(ComboData*);
static double evaluateCoco(ComboData*);
static double evaluateVeroah(ComboData*);
static double evaluateYashamaru(ComboData*, int depth);


// 共通の評価関数
double evaluate(ComboData *cdp, SearchConditions *scp, int depth)
{
  LeaderSettings *lsp = SearchConditions_getLeaderSettings(scp);
  SearchSettings *ssp = SearchConditions_getSearchSettings(scp);
  ClearingSettings *csp = SearchConditions_getClearingSettings(scp);
  const char leader1 = LeaderSettings_getLeader1(lsp);
  const char leader2 = LeaderSettings_getLeader2(lsp);
  const char explosionCount = ComboData_getExplosionCount(cdp);
  double point = -(depth * 0.001 + (explosionCount << 16));

  if (explosionCount == 1 || explosionCount == 2) {
    point -= 9999999;
  }

  // リーダースキルのコンボ加算を適用する
  ComboData_addCombo(cdp, LeaderSkill_getAdditionalCombo(leader1, cdp));
  ComboData_addCombo(cdp, LeaderSkill_getAdditionalCombo(leader2, cdp));

  // リーダースキルの倍率を適用する
  ComboData_multiplyMagnification(cdp, LeaderSkill_getMagnification(leader1, cdp));
  ComboData_multiplyMagnification(cdp, LeaderSkill_getMagnification(leader2, cdp));

  // リーダー別の評価値を加算する
  ComboData_addEvaluation(cdp, getEvaluationBy(leader1, cdp, ssp, csp, depth));
  ComboData_addEvaluation(cdp, getEvaluationBy(leader2, cdp, ssp, csp, depth));

  // コンボ数が指定と異なっていたらペナルティ
  char comboLimit = SearchSettings_getComboLimit(ssp);
  char comboCount = ComboData_getCombo(cdp);
  if (comboLimit && comboLimit != comboCount) {
    point -= abs(comboLimit - comboCount) << 1;
    ComboData_setFulfillConditions(cdp, false);
  }

  // 消し方に関する設定をどれだけ満たしているか
  if (ClearingSettings_isActive(csp)) {
    point += getClearingPoint(cdp, csp);
  }

  // 評価値に最終的なpointを加算する
  ComboData_addEvaluation(cdp, point);

  return ComboData_getEvaluation(cdp);
}


// 消し方に関する設定と照らし合わせ、得点を求める関数
static int getClearingPoint(ComboData *cdp, ClearingSettings *csp)
{
  char comboCount;
  int point = 0;

  for (char color = 1; color <= DROP_TYPE_MAX; color++) {
    // 消すことが必要である色について、消せていない場合は減点
    if (ClearingSettings_isActiveOf(csp, (CS_TYPE)REQUIRED, color)) {
      comboCount = ComboData_getComboOf(cdp, 0, color);
      if (!comboCount) {
        point -= 10;
        ComboData_setFulfillConditions(cdp, false);
      }
    }
    // 全て消すことが必要である色について、盤面に残っている場合は減点
    if (ClearingSettings_isActiveOf(csp, (CS_TYPE)CLEAR_ALL, color)) {
      char leftoversCount = ComboData_getLeftovers(cdp, color);
      if (leftoversCount) {
        point -= (leftoversCount << 3);
        ComboData_setFulfillConditions(cdp, false);
      }
    }
    // 消してはいけない色について、消している場合は減点
    if (ClearingSettings_isActiveOf(csp, (CS_TYPE)NOT_CLEAR, color)) {
      comboCount = ComboData_getComboOf(cdp, 0, color);
      if (comboCount) {
        point -= (comboCount << 16);
        ComboData_setFulfillConditions(cdp, false);
      }
    }
    if ((DROP_TYPE)HEART < color) {
      continue;
    }
    // 二体攻撃が必要である色について、二体攻撃の数に応じて加点
    if (ClearingSettings_isActiveOf(csp, (CS_TYPE)TWO_WAY, color)) {
      comboCount = ComboData_getClearStyle(cdp, (CS_TYPE)TWO_WAY, color);
      if (comboCount)
        point += (comboCount * 50);
      else
        ComboData_setFulfillConditions(cdp, false);
    }
    // 無効貫通が必要である色について、無効貫通の数に応じて加点
    if (ClearingSettings_isActiveOf(csp, (CS_TYPE)BREAK_THROUGH, color)) {
      comboCount = ComboData_getClearStyle(cdp, (CS_TYPE)BREAK_THROUGH, color);
      if (comboCount)
        point += (comboCount * 1000);
      else
        ComboData_setFulfillConditions(cdp, false);
    }
    // 列消しが必要である色について、列消しの数に応じて加点
    if (ClearingSettings_isActiveOf(csp, (CS_TYPE)LINE, color)) {
      comboCount = ComboData_getClearStyle(cdp, (CS_TYPE)LINE, color);
      if (comboCount)
        point += (comboCount * 200);
      else
        ComboData_setFulfillConditions(cdp, false);
    }
    // 十字消しが必要である色について、十字消しの数に応じて加点
    if (ClearingSettings_isActiveOf(csp, (CS_TYPE)CROSS, color)) {
      comboCount = ComboData_getClearStyle(cdp, (CS_TYPE)CROSS, color);
      if (comboCount)
        point += (comboCount << 3);
      else
        ComboData_setFulfillConditions(cdp, false);
    }
    // L字消しが必要である色について、L字消しの数に応じて加点
    if (ClearingSettings_isActiveOf(csp, (CS_TYPE)L, color)) {
      comboCount = ComboData_getClearStyle(cdp, (CS_TYPE)L, color);
      if (comboCount)
        point += (comboCount * 400);
      else
        ComboData_setFulfillConditions(cdp, false);
    }
  }
  // 回復の縦一列消しが必要である場合、できていれば加点
  if (ClearingSettings_isActiveOf(csp, (CS_TYPE)LINE, 7)) {
    comboCount = ComboData_getClearStyle(cdp, (CS_TYPE)LINE, 7);
    if (comboCount)
      point += 3000000;
    else
      ComboData_setFulfillConditions(cdp, false);
  }
  return point;
}


// リーダー別の評価値を求める関数
static double getEvaluationBy(const char leader, ComboData *cdp,
                              SearchSettings *ssp, ClearingSettings *csp, int depth)
{
  switch (leader) {
    case (LEADER)ANUBIS   : return evaluateAnubis(cdp);
    case (LEADER)METATRON : return evaluateMetatron(cdp, csp);
    case (LEADER)KOMASAN_S: return evaluateKomasan(cdp);
    case (LEADER)AMEN     : return evaluateAmen(cdp);
    case (LEADER)HYLEN    : return evaluateHylen(cdp);
    case (LEADER)COCO     : return evaluateCoco(cdp);
    case (LEADER)VEROAH   : return evaluateVeroah(cdp);
    case (LEADER)YASHAMARU: return evaluateYashamaru(cdp, depth);
  }
}


// 評価関数（アヌビス）
static double evaluateAnubis(ComboData *comboData)
{
  return ComboData_getCombo(comboData);
}


// 極醒メタトロン：
// HP80%以下で攻撃力が3倍。神タイプのHPと攻撃力が2倍。
// 7コンボ以上でダメージを軽減、攻撃力が2.5倍。
static double evaluateMetatron(ComboData *comboData, ClearingSettings *csp)
{
  char darkFlag = ClearingSettings_isActiveOf(csp, (CS_TYPE)BREAK_THROUGH, (DROP_TYPE)DARK);
  char lightFlag = ClearingSettings_isActiveOf(csp, (CS_TYPE)BREAK_THROUGH, (DROP_TYPE)LIGHT);
  char comboNum = ComboData_getCombo(comboData);
  char step = ComboData_getStep(comboData);
  double evaluationValue;

  if (darkFlag || lightFlag) {
    int diff, penalty = 0;

    if (darkFlag) {
      diff = getNearestClearedNum(comboData, 9, (DROP_TYPE)DARK);
      penalty += diff << 1;
    }
    if (lightFlag) {
      diff = getNearestClearedNum(comboData, 9, (DROP_TYPE)LIGHT);
      penalty += diff << 1;
    }
    evaluationValue = (comboNum << 2) - step - penalty;
  } else {
    evaluationValue = (comboNum << 2) - step;
  }
  return evaluationValue;
}


// コマさんS：
// 火と木属性のHPが1.5倍。6コンボ以上で攻撃力が5倍。
// ドロップを5個以上つなげて消すとダメージを軽減、攻撃力が3倍。
static double evaluateKomasan(ComboData *comboData)
{
  double magnification = ComboData_getMagnification(comboData);
  char comboNum = ComboData_getCombo(comboData);
  char step = ComboData_getStep(comboData);
  double evaluationValue;

  evaluationValue = (magnification * 4) + (comboNum << 1) - step;

  return evaluationValue;
}


// アメン：
//【落ちコンなし】 7コンボちょうどで攻撃力が10倍。
// パズル後の残りドロップ数が3個以下で攻撃力が10倍。
static double evaluateAmen(ComboData *comboData)
{
  double magnification = ComboData_getMagnification(comboData);
  char comboNum = ComboData_getCombo(comboData);
  char leftovers = ComboData_getLeftovers(comboData, 0);
  char penalty = ComboData_getStep(comboData);

  for (char color = 1; color <= DROP_TYPE_MAX; color++) {
    char leftovers = ComboData_getLeftovers(comboData, color);
    if (0 < leftovers && leftovers < 3) {
      penalty++;
    }
  }
  double evaluationValue = magnification - abs(7 - comboNum) - (leftovers << 3) - penalty;

  return evaluationValue;
}


// ハイレン：
// 水属性のHPが2倍。10コンボ以上で攻撃力が30倍。
// 火と水を同時に7個以上つなげて消すと、3コンボ加算。
static double evaluateHylen(ComboData *comboData)
{
  double magnification = ComboData_getMagnification(comboData);
  char comboNum = ComboData_getCombo(comboData);
  char step = ComboData_getStep(comboData);

  double evaluationValue = magnification + comboNum - step;

  return evaluationValue;
}


// ココ・フェルケナ：
// 水属性の攻撃力が4倍。6コンボ以上でダメージを半減。
// 水を6個以上つなげて消すと攻撃力が4倍、固定5万ダメージ。
static double evaluateCoco(ComboData *comboData)
{
  double magnification = ComboData_getMagnification(comboData);
  char comboNum = ComboData_getCombo(comboData);
  char step = ComboData_getStep(comboData);

  double evaluationValue = magnification + comboNum - step;

  return evaluationValue;
}


// 水着ヴェロア：
// 水属性のHPが2倍、攻撃力は6倍。ドロップ操作を2秒延長。
// 水を9個以上つなげて消すと攻撃力が3倍、3コンボ加算。
static double evaluateVeroah(ComboData *comboData)
{
  double magnification = ComboData_getMagnification(comboData);
  char comboNum = ComboData_getCombo(comboData);
  char step = ComboData_getStep(comboData);

  double evaluationValue = magnification + comboNum - step;

  return evaluationValue;
}


// リーダー「鞍馬夜叉丸」の評価関数
// 【落ちコンなし】HPと回復力が2倍。6コンボ以上で攻撃力が3倍。
// パズル後の残りドロップ数が15個以下で攻撃力が上昇、最大9倍。
static double evaluateYashamaru(ComboData *comboData, int depth)
{
  double magnification = ComboData_getMagnification(comboData);
  char comboNum = ComboData_getCombo(comboData);
  int penalty = ComboData_getStep(comboData);

  for (char color = 1; color <= DROP_TYPE_MAX; color++) {
    char leftovers = ComboData_getLeftovers(comboData, color);
    if (0 < leftovers && leftovers < 3) {
      penalty++;
    }
  }
  double evaluationValue = magnification + (comboNum << 2) - penalty;

  return evaluationValue;
}