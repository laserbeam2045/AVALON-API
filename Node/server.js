const bodyParser = require("body-parser")
const express = require("express")
const app = express()

const DROP_TYPE_MAX = 10
const ANUBIS = 0
const METATRON = 1
const KOMASAN_S = 2
const AMEN = 3
const HYLEN = 4
const COCO = 5
const VEROAH = 6
const YASHAMARU = 7

app.use(bodyParser.urlencoded({extended: false}))
app.use(bodyParser.json())
app.use(function(req, res, next){
  res.header("Access-Control-Allow-Origin", "*")
  next()
})


// 盤面で可能な最大コンボ数と、最大倍率を返すAPI
app.get('/api/maxData/:leader1-:leader2-:board', function(req, res){
  const leader1 = Number(req.params.leader1),
        leader2 = Number(req.params.leader2),
        board = JSON.parse(req.params.board)

  const boardData = getBoardData(board)

  addCombo(boardData, leader1, true)
  addCombo(boardData, leader2, false)
  multiplyMagnification(boardData, leader1)
  multiplyMagnification(boardData, leader2)

  res.send(JSON.stringify(boardData))
}).listen(1200)

console.log("Node server listening...")


// コンボ加算系リーダースキルを適用する関数
function addCombo(boardData, leader, firstTime) {
  switch (leader) {
  case HYLEN:
    if (boardData['dropCountArray'][1] >= 7 &&
        boardData['dropCountArray'][2] >= 7)
    {
      boardData.maxComboCount += 3
      if (firstTime) {
        boardData.maxComboCount -= 2
      }
    }
    break
  case VEROAH:
    if (boardData['dropCountArray'][2] >= 9) {
      boardData.maxComboCount += 3
      if (firstTime) {
        boardData.maxComboCount -= 2
      }
    }
  }
}


// リーダースキルの倍率を適用する関数
function multiplyMagnification(boardData, leader) {
  const unclearableDrops = boardData['dropCountArray'].filter(num => num < 3)
  const unclearableDropSum = unclearableDrops.length ?
                              unclearableDrops.reduce((a,b) => a+b) : 0
  switch (leader) {
  // 転生アヌビス：
  // 8コンボ以上で攻撃力が上昇、最大10倍。
  // スキル使用時、攻撃力が3倍、回復力は1.5倍。
  case ANUBIS:
    if (boardData['maxComboCount'] <= 7) {
      break
    }
    switch (boardData['maxComboCount']) {
      case 8: boardData['maxMagnification'] *= 5; break
      case 9: boardData['maxMagnification'] *= 7.5; break
      default: boardData['maxMagnification'] *= 10; break
    }
    break
  // 極醒メタトロン：
  // HP80%以下で攻撃力が3倍。神タイプのHPと攻撃力が2倍。
  // 7コンボ以上でダメージを軽減、攻撃力が2.5倍。
  case METATRON:
    boardData['maxMagnification'] *= 2
    if (boardData['maxComboCount'] >= 7) {
      boardData['maxMagnification'] *= 2.5
    }
    break
  // コマさんS：
  // 火と木属性のHPが1.5倍。6コンボ以上で攻撃力が5倍。
  // ドロップを5個以上つなげて消すとダメージを軽減、攻撃力が3倍。
  case KOMASAN_S:
    if (boardData['maxComboCount'] >= 6) {
      boardData['maxMagnification'] *= 5
    }
    if (boardData['mostDropCount'] >= 5) {
      boardData['maxMagnification'] *= 3
    }
    break
  // アメン：
  // 【落ちコンなし】 7コンボちょうどで攻撃力が10倍。
  // パズル後の残りドロップ数が3個以下で攻撃力が10倍。
  case AMEN:
    if (boardData['maxComboCount'] >= 7) {
      boardData['maxMagnification'] *= 10
    }
    if (unclearableDropSum <= 3) {
      boardData['maxMagnification'] *= 10
    }
    break
  // ハイレン：
  // 水属性のHPが2倍。10コンボ以上で攻撃力が30倍。
  // 火と水を同時に7個以上つなげて消すと、3コンボ加算。
  case HYLEN:
    if (boardData['maxComboCount'] >= 10) {
      boardData['maxMagnification'] *= 30
    }
    break
  // ココ・フェルケナ：
  // 水属性の攻撃力が4倍。6コンボ以上でダメージを半減。
  // 水を6個以上つなげて消すと攻撃力が4倍、固定5万ダメージ。
  case COCO:
    boardData['maxMagnification'] *= 4
    if (boardData['dropCountArray'][2] >= 6) {
      boardData['maxMagnification'] *= 4
    }
    break
  // 水着ヴェロア：
  // 水属性のHPが2倍、攻撃力は6倍。ドロップ操作を2秒延長。
  // 水を9個以上つなげて消すと攻撃力が3倍、3コンボ加算。
  case VEROAH:
    boardData['maxMagnification'] *= 6
    if (boardData['dropCountArray'][2] >= 9) {
      boardData['maxMagnification'] *= 3
    }
    break
  // 鞍馬夜叉丸：
  // 【落ちコンなし】HPと回復力が2倍。6コンボ以上で攻撃力が3倍。
  // パズル後の残りドロップ数が15個以下で攻撃力が上昇、最大9倍。
  case YASHAMARU:
    if (boardData['maxComboCount'] >= 6) {
      boardData['maxMagnification'] *= 3
    }
    boardData['maxMagnification'] *= Math.max(9 - 0.5 * unclearableDropSum, 1)
    break
  }
}


// 盤面を分析してオブジェクトにする関数
function getBoardData(board) {

  // 盤面に存在するドロップの数(種類別)
  const dropCountArray = (new Array(DROP_TYPE_MAX + 1)).fill(0)

  // 盤面を見て、その色に対応するインデックスの値をインクリメント
  board.forEach(color => dropCountArray[color]++)

  // 存在するドロップだけ残した配列
  const existDropCountArray = dropCountArray.filter(num => num)

  // 盤面に存在するドロップの種類数
  const dropTypeCount = existDropCountArray.length

  // 最も多いドロップの数
  const mostDropCount = Math.max.apply(null, existDropCountArray)

  // 最も少ないドロップの数
  const fewestDropCount = Math.min.apply(null, existDropCountArray)

  // ドロップの種類別の、可能な最大コンボ数（３で割るだけ）
  const maxComboCountArray = dropCountArray.map(num => Math.floor(num / 3))

  // ドロップの種類数に応じて可能な最大コンボ数の和を求める
  let maxComboCount = null
  switch (maxComboCount) {
  case 1:
    maxComboCount = 1
    break
  case 2:
    maxComboCount = getMaxComboOfTwoColorBoard(board.length, fewestDropCount)
    break
  default:
    maxComboCount = maxComboCountArray.reduce((a, b) => a + b)
  }
  const maxMagnification = 1

  return {
    dropCountArray,
    existDropCountArray,
    dropTypeCount,
    mostDropCount,
    fewestDropCount,
    maxComboCountArray,
    maxComboCount,
    maxMagnification,
  }
}


// ２色盤面での最大コンボ数を返す関数
function getMaxComboOfTwoColorBoard(boardLength, fewDropCount) {
  if (boardLength === (5 * 6)) {
    switch (fewDropCount) {
      case 1  :
      case 2  : return 1
      case 3  : return 2
      case 4  : return 3
      case 5  : return 4
      case 6  : return 5
      case 7  : return 6
      case 8  : return 7
      case 9  :
      case 10 : return 8
      case 11 :
      case 12 :
      case 13 :
      case 14 : return 9
      case 15 : return 10
      default : return 0
    }
  } else if (boardLength === (6 * 7)) {
    switch (fewDropCount) {
      case 1  :
      case 2  : return 1
      case 3  : return 2
      case 4  :
      case 5  : return 3
      case 6  : return 5
      case 14 :
      case 15 :
      case 16 :
      case 17 : return 13
      case 18 : return 14
      case 19 :
      case 20 : return 13
      case 21 : return 14
      default : return 12 // 7~13はデータがないからとりあえず12
    }
  } else {
    return 0
  }
}


// 盤面で可能な十字消しの個数を求める関数（不動明王、緋村剣心用）
// TODO:不完全なので手入れが必要
function getMaxCrossCount(boardSize, board, leader) {
  let maxCrossCount = null

  // ２色盤面の場合
  if (boardSize === "5x6" && dropCountSum === (5 * 6) ||
      boardSize === "6x7" && dropCountSum === (6 * 7)) {
    const fewDropCount = Math.min(color1Count, color2Count)

    switch (boardSize) {
    case "5x6":
      if (fewDropCount <= 4)       maxCrossCount = 0
      else if (fewDropCount <= 6)  maxCrossCount = 1
      else if (fewDropCount <= 11) maxCrossCount = 2
      else                         maxCrossCount = 3
      break
    case "6x7":
      if (fewDropCount <= 4)       maxCrossCount = 0
      else if (fewDropCount <= 6)  maxCrossCount = 1
      else if (fewDropCount <= 11) maxCrossCount = 2
      else if (fewDropCount <= 13) maxCrossCount = 3
      else if (fewDropCount <= 15) maxCrossCount = 4
      else if (fewDropCount <= 17) maxCrossCount = 5
      else                         maxCrossCount = 6
      break
    }
  // ２色盤面以外の場合
  } else {
    maxCrossCount = Math.floor(color1Count / 5)
                  + Math.floor(color2Count / 5)
    switch (boardSize) {
      case "5x6": maxCrossCount = Math.min(3, maxCrossCount); break
      case "6x7": maxCrossCount = Math.min(6, maxCrossCount); break
    }
  }
  return maxCrossCount
}