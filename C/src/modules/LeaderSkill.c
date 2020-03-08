#include "LeaderSkill.h"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

// プライベート関数
static double LeaderSkill_getMagnificationAnubis(ComboData *comboData);
static double LeaderSkill_getMagnificationMetatron(ComboData *comboData);
static double LeaderSkill_getMagnificationKomasan(ComboData *comboData);
static double LeaderSkill_getMagnificationAmen(ComboData *comboData);
static double LeaderSkill_getMagnificationHylen(ComboData *comboData);
static double LeaderSkill_getMagnificationCoco(ComboData *comboData);
static double LeaderSkill_getMagnificationVeroah(ComboData *comboData);
static double LeaderSkill_getMagnificationYashamaru(ComboData *comboData);


// リーダースキルで加算されるコンボ数を取得する関数
char LeaderSkill_getAdditionalCombo(char leader, ComboData *comboData)
{
  char flag1 = 0, flag2 = 0;

  switch (leader) {
  case (LEADER)HYLEN:
    for (int i = 0; i < COUNT_COMBO_MAX; i++) {
      if (7 <= ComboData_getComboOf(comboData, (DROP_TYPE)FIRE, i))
        flag1 = 1;
      if (7 <= ComboData_getComboOf(comboData, (DROP_TYPE)WATER, i))
        flag2 = 1;
    }
    if (flag1 && flag2) {
      return 3;
    }
    break;
  case (LEADER)VEROAH:
    for (int i = 0; i < COUNT_COMBO_MAX; i++) {
      if (9 <= ComboData_getComboOf(comboData, (DROP_TYPE)WATER, i)) {
        return 3;
      }
    }
    break;
  }
  return 0;
}

// リーダースキルで適用される倍率を取得する関数
double LeaderSkill_getMagnification(char leader, ComboData *comboData)
{
  double magnification;

  switch (leader) {
    case (LEADER)ANUBIS   : magnification = LeaderSkill_getMagnificationAnubis(comboData); break;
    case (LEADER)METATRON : magnification = LeaderSkill_getMagnificationMetatron(comboData); break;
    case (LEADER)KOMASAN_S: magnification = LeaderSkill_getMagnificationKomasan(comboData); break;
    case (LEADER)AMEN     : magnification = LeaderSkill_getMagnificationAmen(comboData); break;
    case (LEADER)HYLEN    : magnification = LeaderSkill_getMagnificationHylen(comboData); break;
    case (LEADER)COCO     : magnification = LeaderSkill_getMagnificationCoco(comboData); break;
    case (LEADER)VEROAH   : magnification = LeaderSkill_getMagnificationVeroah(comboData); break;
    case (LEADER)YASHAMARU: magnification = LeaderSkill_getMagnificationYashamaru(comboData); break;
  }
  return magnification;
}

// リーダー「転生アヌビス」の倍率を取得する関数
// 8コンボ以上で攻撃力が上昇、最大10倍。
// スキル使用時、攻撃力が3倍、回復力は1.5倍。
static double LeaderSkill_getMagnificationAnubis(ComboData *comboData)
{
  char comboNum = ComboData_getCombo(comboData);

  if (7 >= comboNum)      return 1;
  else if (8 == comboNum) return 5;
  else if (9 == comboNum) return 7.5;
  else                    return 10;
}

// リーダー「極醒メタトロン」の倍率を取得する関数
// HP80%以下で攻撃力が3倍。神タイプのHPと攻撃力が2倍。
// 7コンボ以上でダメージを軽減、攻撃力が2.5倍。
static double LeaderSkill_getMagnificationMetatron(ComboData *comboData)
{
  char comboNum = ComboData_getCombo(comboData);
  double magnification = 2;

  if (7 <= comboNum) {
    magnification *= 2.5;
  }
  return magnification;
}

// リーダー「コマさんS」の倍率を取得する関数
// 火と木属性のHPが1.5倍。6コンボ以上で攻撃力が5倍。
// ドロップを5個以上つなげて消すとダメージを軽減、攻撃力が3倍。
static double LeaderSkill_getMagnificationKomasan(ComboData *comboData)
{
  char comboNum = ComboData_getCombo(comboData);
  char maxConnection = ComboData_getMaxConnection(comboData);
  double magnification = 1;

  if (6 <= comboNum)
    magnification *= 5;
  if (5 <= maxConnection)
    magnification *= 3;

  return magnification;
}

// リーダー「アメン」の倍率を取得する関数
//【落ちコンなし】 7コンボちょうどで攻撃力が10倍。
// パズル後の残りドロップ数が3個以下で攻撃力が10倍。
static double LeaderSkill_getMagnificationAmen(ComboData *comboData)
{
  char comboNum = ComboData_getCombo(comboData);
  char leftovers = ComboData_getLeftovers(comboData, 0);
  double magnification = 1;

  if (7 == comboNum)
    magnification *= 10;
  if (3 >= leftovers)
    magnification *= 10;

  return magnification;
}

// リーダー「ハイレン」の倍率を取得する関数
// 水属性のHPが2倍。10コンボ以上で攻撃力が30倍。
// 火と水を同時に7個以上つなげて消すと、3コンボ加算。
static double LeaderSkill_getMagnificationHylen(ComboData *comboData)
{
  char comboNum = ComboData_getCombo(comboData);

  if (10 <= comboNum) {
    return 30;
  } else {
    return 1;
  }
}

// リーダー「ココ・フェルケナ」の倍率を取得する関数
// 水属性の攻撃力が4倍。6コンボ以上でダメージを半減。
// 水を6個以上つなげて消すと攻撃力が4倍、固定5万ダメージ。
static double LeaderSkill_getMagnificationCoco(ComboData *comboData)
{
  double magnification = 4;

  for (char i = 0; i <= COUNT_COMBO_MAX; i++) {
    char clearedNum = ComboData_getComboOf(comboData, (DROP_TYPE)WATER, i);
    if (6 <= clearedNum) {
      magnification *= 4;
      break;
    }
  }
  return magnification;
}

// リーダー「水着ヴェロア」の倍率を取得する関数
// 水属性のHPが2倍、攻撃力は6倍。ドロップ操作を2秒延長。
// 水を9個以上つなげて消すと攻撃力が3倍、3コンボ加算。
static double LeaderSkill_getMagnificationVeroah(ComboData *comboData)
{
  double magnification = 6;

  for (char i = 0; i <= COUNT_COMBO_MAX; i++) {
    char clearedNum = ComboData_getComboOf(comboData, (DROP_TYPE)WATER, i);
    if (9 <= clearedNum) {
      magnification *= 3;
      break;
    }
  }
  return magnification;
}

// リーダー「鞍馬夜叉丸」の倍率を取得する関数
// 【落ちコンなし】HPと回復力が2倍。6コンボ以上で攻撃力が3倍。
// パズル後の残りドロップ数が15個以下で攻撃力が上昇、最大9倍。
static double LeaderSkill_getMagnificationYashamaru(ComboData *comboData)
{
  char comboNum = ComboData_getCombo(comboData);
  char leftovers = ComboData_getLeftovers(comboData, 0);
  double magnification = 1;

  if (6 <= comboNum)
    magnification *= 3;

  if (15 >= leftovers)
    magnification *= 9 - 0.5 * leftovers;

  return magnification;
}