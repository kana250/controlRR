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
#define TOP_MOTOR_ID 0x201 // 上のギアを回すモータのID
#define BOTTOM_MOTOR_ID 0x202 // 下のギアを回すモータのID

// MOTOR: ANGLE
uint16_t TOP_MOTOR_ANGLE = 0; // 上のギアを回すモータの角度[0, 8191] 0 ~ 360°
uint16_t BOTTOM_MOTOR_ANGLE = 0; // 下のギアを回すモータの角度[0, 8191] 0 ~ 360°

// MOTOR: PWM
uint16_t TOP_MOTOR_PWM = 150; // 上のギアを回すモータのPWM値[0, 255] 0 ~ 100%
uint16_t BOTTOM_MOTOR_PWM = 150; // 下のギアを回すモータのPWM値[0, 255] 0 ~ 100%

// MOTOR: TORQUE
uint16_t TOP_MOTOR_TORQUE = 0; // 上のギアを回すモータのトルク[0, ?]
uint16_t BOTTOM_MOTOR_TORQUE = 0; // 下のギアを回すモータのトルク[0, ?]


void canSender(int motor_id, uint16_t angle_value, uint16_t pwm_value, uint16_t torque_value){
    Serial.println("> CANデータを送信しています...");

    int length = 8; // データ長 8byte
    uint8_t buffer[length];// 8byte分のバッファを用意 1byte(8bitの):16進数表記で0x00〜0xFF
    // 例: uint8_t buffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; <- このように，8byte分のデータを格納してる

    // 任意の角度，PWM値，トルク値をバッファに格納
    buffer[0] = (uint8_t)(angle_value >> 8); // 角度の上位8bit
    buffer[1] = (uint8_t)(angle_value & 0xFF); // 角度の下位8bit
    buffer[2] = (uint8_t)(pwm_value >> 8); // PWM値の上位8bit
    buffer[3] = (uint8_t)(pwm_value & 0xFF); // PWM値の下位8bit
    buffer[4] = (uint8_t)(torque_value >> 8); // トルク値の上位8bit
    buffer[5] = (uint8_t)(torque_value & 0xFF); // トルク値の下位8bit
    // 6, 7byteはnull

    CAN.beginPacket(motor_id); // ID:11bitか29bitで指定する．どちらも,8byteまでのデータを送信できる．
    CAN.write(buffer, length);
    CAN.endPacket();

    Serial.println("> CANデータを送信しました");
}

void setup() {
  Serial.begin(115200);
  while(!Serial);

  if (!CAN.begin(1000E3)) { // 1Mbps
    Serial.println("> CAN通信の接続に失敗しました");
    while (1);
  }

  CAN.setPins(rx, tx); // CRX, CTX
}

void loop() {
  // 上のギアを回すモータのCANデータを送信
  canSender(TOP_MOTOR_ID, 0, TOP_MOTOR_PWM, 0);

  // 下のギアを回すモータのCANデータを送信
  canSender(BOTTOM_MOTOR_ID, 0, BOTTOM_MOTOR_PWM, 0);

  // 1秒待つ
  delay(1000); 
}