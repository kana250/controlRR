#include <Arduino.h>
#include <unity.h> // ユニットテスト用ライブラリ

// ユニットテストとは，プログラムの一部の機能をテストすること
// ホストコンピュータ上で実行される(マイコンは必要ない)

void setup() {
  Serial.begin(115200);
  Serial.println("STARTING");
}

void loop() {
  Serial.println("Hello World!");
  delay(1000);
}