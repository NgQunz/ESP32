/*
 * ============================================================
 *  Nguyen Minh Quan - B22DCDT248
 *  Node 1: ESP32 + BH1750 (I2C SDA=21, SCL=22) + LED D2 (GPIO2)
 *
 *  Ket noi UART:
 *   - TX=GPIO17 -> STM32 PA3 (RX)
 *   - RX=GPIO16 <- STM32 PA2 (TX)
 *
 *  Logic:
 *  1. Doc BH1750
 *  2. Gui ban tin sang STM32 (Node 2):
 *     "Nguyen Minh Quan - B22DCDT248 - Lux:<lux>"
 *  3. Cho phan hoi tu STM32: nhiet do DHT22
 *     Format: "Phan hoi tu Node 2 ve Node 1: T:<temp>C H:<hum>%"
 *  4. Nhiet do > 30C -> bat LED D2
 *     Nhiet do <= 30C -> tat LED D2
 * ============================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <string.h>

// ============================================================
//  CHAN PHAN CUNG
// ============================================================
#define LED_PIN 2
#define UART_RX_PIN 16
#define UART_TX_PIN 17
#define BH1750_ADDR 0x23

// ============================================================
//  GIAO THUC DONG GOI
// ============================================================
#define START_BYTE 0xAA
#define END_BYTE 0x55
#define CMD_TEXT 0x01
#define CMD_ACK 0x02
#define MAX_DATA 64

#define TEMP_THRESHOLD 30.0f

typedef struct
{
  uint8_t start;
  uint8_t cmd;
  uint8_t len;
  uint8_t data[MAX_DATA + 1];
  uint8_t checksum;
  uint8_t end;
} Message_t;

// ============================================================
//  BH1750
// ============================================================
void BH1750_Init()
{
  Wire.begin();
  Wire.beginTransmission(BH1750_ADDR);
  Wire.write(0x01); // Power On
  Wire.endTransmission();
  Wire.beginTransmission(BH1750_ADDR);
  Wire.write(0x10); // Continuous High Resolution Mode
  Wire.endTransmission();
  delay(180);
  Serial.println("[BH1750] Ready!");
}

uint16_t BH1750_ReadLux()
{
  uint16_t lux = 0;
  Wire.requestFrom(BH1750_ADDR, 2);
  if (Wire.available() >= 2)
  {
    lux = (uint16_t)Wire.read() << 8;
    lux |= Wire.read();
    lux = (uint16_t)(lux / 1.2f);
  }
  return lux;
}

// ============================================================
//  GIAO THUC
// ============================================================
uint8_t MSG_Checksum(Message_t *msg)
{
  uint8_t cs = msg->cmd ^ msg->len;
  for (int i = 0; i < msg->len; i++)
    cs ^= msg->data[i];
  return cs;
}

void MSG_Build(Message_t *msg, uint8_t cmd, const char *data)
{
  msg->start = START_BYTE;
  msg->cmd = cmd;
  msg->len = (uint8_t)strlen(data);
  memcpy(msg->data, data, msg->len);
  msg->checksum = MSG_Checksum(msg);
  msg->end = END_BYTE;
}

void MSG_Send_STM32(Message_t *msg)
{
  Serial2.write(msg->start);
  Serial2.write(msg->cmd);
  Serial2.write(msg->len);
  for (int i = 0; i < msg->len; i++)
    Serial2.write(msg->data[i]);
  Serial2.write(msg->checksum);
  Serial2.write(msg->end);
}

// ============================================================
//  NHAN BAN TIN TU STM32
// ============================================================
static uint8_t rx_buf[128];
static uint8_t rx_idx = 0;

bool MSG_Receive_STM32(Message_t *out)
{
  while (Serial2.available())
  {
    uint8_t b = (uint8_t)Serial2.read();

    if (rx_idx == 0 && b != START_BYTE)
      continue;
    rx_buf[rx_idx++] = b;

    if (rx_idx >= 3)
    {
      uint8_t total = 3 + rx_buf[2] + 2;
      if (rx_idx == total)
      {
        out->start = rx_buf[0];
        out->cmd = rx_buf[1];
        out->len = rx_buf[2];
        memcpy(out->data, &rx_buf[3], out->len);
        out->data[out->len] = 0;
        out->checksum = rx_buf[3 + out->len];
        out->end = rx_buf[3 + out->len + 1];
        rx_idx = 0;

        if (out->end == END_BYTE && out->checksum == MSG_Checksum(out))
          return true;
      }
    }
    if (rx_idx >= 128)
      rx_idx = 0;
  }
  return false;
}

// ---- Parse nhiet do tu "...: T:<temp>C H:<hum>%" ----
float parse_temperature(const char *str)
{
  const char *p = strstr(str, "T:");
  if (!p)
    return -999;
  p += 2;
  return atof(p);
}

// ============================================================
//  SETUP
// ============================================================
void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  BH1750_Init();

  Serial.println("=== ESP32 Node 1 READY ===");
  Serial.println("Nguyen Minh Quan - B22DCDT248");
}

// ============================================================
//  LOOP
// ============================================================
void loop()
{
  Message_t tx_msg, rx_msg;

  // =========================================================
  // BUOC 1: Doc BH1750
  // =========================================================
  uint16_t lux = BH1750_ReadLux();
  Serial.print("[BH1750] Lux = ");
  Serial.println(lux);

  // =========================================================
  // BUOC 2: Gui ban tin sang STM32
  // "Nguyen Minh Quan - B22DCDT248 - Lux:<lux>"
  // =========================================================
  char payload[80];
  snprintf(payload, sizeof(payload),
           "Nguyen Minh Quan - B22DCDT248 - Lux:%d", lux);

  MSG_Build(&tx_msg, CMD_TEXT, payload);
  MSG_Send_STM32(&tx_msg);
  // Serial.print("[TX->STM32] ");
  Serial.println(payload);

  // =========================================================
  // BUOC 3: Cho phan hoi tu STM32 (toi da 500ms)
  // =========================================================
  uint32_t t_wait = millis();
  while ((millis() - t_wait) < 500)
  {
    if (MSG_Receive_STM32(&rx_msg))
    {
      // Serial.print("Ban tin nhan tu STM32: ");
      Serial.println((char *)rx_msg.data);

      // =================================================
      // BUOC 4: Parse nhiet do -> dieu khien LED D2
      //         Nhiet do > 30C -> bat LED D2
      // =================================================
      if (rx_msg.cmd == CMD_TEXT)
      {
        float temp = parse_temperature((char *)rx_msg.data);
        if (temp > TEMP_THRESHOLD)
        {
          digitalWrite(LED_PIN, HIGH);
          Serial.println(" Nhiet do > 30C -> BAT LED2");
        }
        else
        {
          digitalWrite(LED_PIN, LOW);
          Serial.println(" Nhiet do <= 30C -> TAT LED2");
        }
      }
      Serial.println("");
      break;
    }
  }

  delay(2000);
}