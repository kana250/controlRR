# controlRR

PS4のコントローラで，RRを動作させるプログラム.
-> 12/17(土)現在，まだPS4のコントローラ制御プログラムは書いていない

足回りには，Differential Swerveを使っている．

## 1. MCP2551のピン配置 ([データシート](https://jp.rs-online.com/web/p/can-interface-ics/0402920))

<img width="384" alt="スクリーンショット 2022-12-17 12 11 43" src="https://user-images.githubusercontent.com/43850587/208221322-1a95e337-437a-429b-8f23-7956d601a770.png">

| Pin Number | Pin Name | Function |
| :---: | :---: | :---: |
| 1 | TXD | 送信データ"入力" |
| 2 | VSS | グランド |
| 3 | VDD | 電源 |
| 4 | RXD | 受信データ"出力" |
| 5 | VREF | 基準電源(定義：VDD/2) |
| 6 | CANL | CAN低レベル電圧I/O |
| 7 | CANH | CAN高レベル電圧I/O |
| 8 | RS | スロープ抵抗入力 |

## 2. ESP32のピン配置([データシート](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf))

![image](https://user-images.githubusercontent.com/43850587/208221920-fbe68b1a-98ed-491c-98bb-495f8f95568a.png)

## 3. MCP2551とESP32の接続

| MCP2551 | ESP32 |
| :-------------: | :---: |
| VDD | 3V3 |
| VSS | GND |
| TXD | GPIO_5 |
| RXD | GPIO_4 |

`TXD`, `RXD`のピンは，`CAN.setPins(rx, tx)`で変更できる．
`GPIO_5`, `GPIO_4`を選んでいるのは，デフォルトでそうなっていたため！

> Arduino CANライブラリのデフォルトは，[TI SN65HVD230](https://www.ti.com/product/SN65HVD230)を使っている．