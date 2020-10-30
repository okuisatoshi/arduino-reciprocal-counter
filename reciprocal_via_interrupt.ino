//デジタル入力の外部割り込みを用いた逆数式周波数測定
//
// テストに利用できる正弦波発生サイト：
// https://tomari.org/main/java/audioapi/audio_osc2.html
// https://www.pg-fl.jp/music/hzmaker.htm

const int N_ACC = 5;   // 何周期分累積して計測するか（上げすぎるとオーバフローに注意、5程度が適当か？）
volatile int freq = 0; // 最後に計測した周波数(Hz)

void get_duration() {
  static int n_period = 0;       // （リセット以降に）観測した周期の回数
  static long last_t = micros(); // （リセット以降に）最初に観測した（起動時からの）作動時間（u秒）
  
  n_period++;
  if (n_period==N_ACC) {
    long t = micros();
    freq = ((long)1000000*N_ACC)/(t-last_t);
    n_period = 0;
    last_t = t;
  }    
}


void setup() {
  attachInterrupt(digitalPinToInterrupt(2), get_duration, RISING);
  Serial.begin(115200);
}

void loop() {
  if (freq<5000) Serial.println(freq); // プロッタの縦軸のスケールを5000までに抑えるため
  delay(100);
}
