// アナログ入力とタイマ#2を用いた逆数式周波数測定
// 音程の認識への応用
// MIDI 音名-ノート番号-周波数の対応:
//  C - #48 - 130.8
//  C - #60 - 262.6
//  A - #69 - 440.0
//  C - #72 - 523.3
//  C - #84 - 1046.5
//  C - #96 - 2093.0
//
// テストに利用できる正弦波発生サイト：
// https://tomari.org/main/java/audioapi/audio_osc2.html
// https://www.pg-fl.jp/music/hzmaker.htm

#include <FlexiTimer2.h>

const int F_RES = 10000;        // タイマ割り込みの生起周波数（Hz）
const double T_RES = 1.0/F_RES; // タイマ割り込みの生起周期（秒）
const int N_ACC = 10;           // 何周期分累積して計測するか
const int THRESHOLD = 92;      // このしきい値以下は無音とみなす  

const int N_LOW = 262;  // MIDIノート60番
const int N_HIGH = 523; // MIDIノート72番
const int MERGIN = 128;  
#define LOUD_ENOUGH (abs(a)>=THRESHOLD)

volatile int freq; // 最後に計算された周波数（Hz）
volatile int a;    // 最後に計測したA0の値（-512〜511）


void count_period() {
  static int n = 0;        // （リセット以降に）count_period()が呼ばれた回数
  static int n_period = 0; // （リセット以降に）観測した周期の開始の回数
  static int last_a = analogRead(A0)-512; // 前回count_period()が呼ばれたときのA0の計測値（-512〜511）

  n++;
  a = analogRead(A0)-512; 
  if (last_a<0 && a>0) n_period++; // 周期の開始（RISING）を観測

  // 所定の累積回数に達していれば周波数を計算
  if (n_period==N_ACC) {
    freq = ((long)F_RES*N_ACC)/n; 
    n = n_period = 0;
  }
  last_a = a;
}


void setup() {
  ADCSRA &= 0xf8; // 下位3ビットを一旦クリア（もともとは128分周なので下3ビットは111のはず）
  ADCSRA |= 0x03; // 下位3ビットに011を書き込んでクロックを8分周
  Serial.begin(115200);
  // 周期T_RES秒でタイマ割り込みを発生
  FlexiTimer2::set(1, T_RES, count_period);
  FlexiTimer2::start();
  a = analogRead(A0)-512;
}


void loop() {

  if (LOUD_ENOUGH) {
    //#if 0
    if (N_LOW-MERGIN <= freq && freq < N_LOW+MERGIN) {
      Serial.print("0: ");
      Serial.println(freq);
    }
    if (N_HIGH-MERGIN <= freq && freq < N_HIGH+MERGIN) {
      Serial.print("1: ");  
      Serial.println(freq);
    }
    //#endif
    //Serial.println(freq);
  }
   
  delay(10);
}
