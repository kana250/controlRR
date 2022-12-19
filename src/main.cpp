#include <Arduino.h>
#include <CAN.h>

// CANで使うESP32のGPIOピン番号
#define rx 4
#define tx 5

/*
    * 以下のギアを回すモータのIDを定義
    * 0x201: 上のギアを回すモータ
    * 0x202: 下のギアを回すモータ
    * 
    * 上記の識別子の与え方は，C610のデータシート(p.8)を参照
    * ID: 0x200 + モータの番号（1〜4）
    *     0x1FF + モータの番号（5〜8）
    * 
    * 最大，8個のモータを制御できる
*/

// 上下のギアを回すモータのID
#define TOP_MOTOR_ID 1 // 上のギアを回すモータのESC_ID
#define BOTTOM_MOTOR_ID 2 // 下のギアを回すモータのESC_ID

// MOTOR: ANGLE
uint16_t TOP_MOTOR_ANGLE = 0; // 上のギアを回すモータの角度[0, 8191] 0 ~ 360°
uint16_t BOTTOM_MOTOR_ANGLE = 0; // 下のギアを回すモータの角度[0, 8191] 0 ~ 360°

// MOTOR: PWM
uint16_t TOP_MOTOR_PWM = 150; // 上のギアを回すモータのPWM値[0, 255] 0 ~ 100%
uint16_t BOTTOM_MOTOR_PWM = 150; // 下のギアを回すモータのPWM値[0, 255] 0 ~ 100%

// MOTOR: TORQUE
uint16_t TOP_MOTOR_TORQUE = 0; // 上のギアを回すモータのトルク[0, ?]
uint16_t BOTTOM_MOTOR_TORQUE = 0; // 下のギアを回すモータのトルク[0, ?]

// MOTOR: CURRENT (-10000mA ~ 10000mA) -> -10A ~ 10A
int16_t CURRENT_CW = 10E3; // 正転するモータの電流値[-10000, 10000]
int16_t CURRENT_CCW = -10E3; // 反転するモータの電流値[-10000, 10000]

// モータのID, 制御電流値
void canSender(uint16_t id, uint16_t current_value) {
  // CANデータを送信
  // モータID: 1〜4

  int length = 8; // データ長 8byte
  uint8_t buffer[length];// 8byte分のバッファを用意 1byte(8bitの):16進数表記で0x00〜0xFF
  // 例: uint8_t buffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; <- このように，8byte分のデータを格納してる

  // 任意の電流値をバッファに格納(モータのIDによって，バッファのどの位置に格納するかを変える)
  buffer[0 + (id - 1) * 2] = (current_value >> 8) & 0xFF; // 0byte目に，current_valueの上位8bitを格納
  buffer[1 + (id - 1) * 2] = current_value & 0xFF; // 1byte目に，current_valueの下位8bitを格納 

  /*
    * 上記の「current_value & 0xFF」の説明
    * まず，16進数の0xFFは，2進数で表すと11111111となる．つまり，1が8個並んでる8bitの値である．
    * 
    * そして，current_valueは，16bitの値である．
    * 例えば，current_value = 0x1234とすると，2進数で表すと1001000110100となる．
    * 
    * このとき，「current_value & 0xFF」は，2進数で表すと「1001000110100 & 0000000011111111」となる．
    * つまり，current_valueの下位8bitを取り出すことができる．（＆はAND演算子の意味である） 
  */

  // CAN.beginPacket():11bitか29bitで指定する．
  CAN.beginPacket(0x200); // 0x200
  CAN.write(buffer, length);
  CAN.endPacket();

  // デバッグ用
  Serial.println("> [CAN] ID:" + String(id) + ", " + String(current_value) + "mA");
}

void setup() {
  Serial.begin(115200);
  while(!Serial);

  if (!CAN.begin(1000E3)) { // 1Mbps
    Serial.println("> CAN通信の接続に失敗しました");
    while (1);
  }
  Serial.println("> CAN通信の接続に成功しました");

  CAN.setPins(rx, tx); // CRX, CTX
}

void loop() {
  // 上のギアを回すモータのCANデータを送信
  canSender(TOP_MOTOR_ID, CURRENT_CW); // 正転（10A）

  // 下のギアを回すモータのCANデータを送信
  canSender(BOTTOM_MOTOR_ID, CURRENT_CCW); // 反転（-10A）

  // 1秒待つ
  delay(1000); 
}