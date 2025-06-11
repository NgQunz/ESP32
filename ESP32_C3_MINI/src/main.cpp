// Note:
// 1. In the Tools menu of the Arduino IDE -> USB CDC On Boot -> Select:
// + Enabled: If you want to read logs from the Serial Monitor
// + Disabled: If you want to control Leanbot
// 2. After selecting, click the Upload button to upload the code to the board and update the functionality.

#include <Bluepad32.h>
#include <esp_system.h>

#define BP32_MAX_GAMEPADS 1
#define LED_PIN 8
#define TOUCH_SENSOR_PIN 2

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
bool checkConnected = false;
void DecodeController(ControllerPtr ctl);
// This callback gets called any time a new gamepad is connected.
void onConnectedController(ControllerPtr ctl)
{
  checkConnected = true;
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++)
  {
    if (myControllers[i] == nullptr)
    {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot)
  {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl)
{
  checkConnected = false;
  bool foundController = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++)
  {
    if (myControllers[i] == ctl)
    {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }
  if (!foundController)
  {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void processControllers()
{
  for (auto myController : myControllers)
  {
    if (myController && myController->isConnected())
    {

      if (myController->isGamepad())
      {
        DecodeController(myController);
      }
      else
      {
        Serial.println("Unsupported controller");
      }
    }
  }
}

void DecodeController(ControllerPtr ctl)
{
  int buttonsValue = ctl->buttons();
  int dpadValue = ctl->dpad();
  int micsButtonValue = ctl->miscButtons();

  Serial.printf("|>");

  Serial.printf("%4d ", ctl->brake());
  if (buttonsValue & 0x0040)
  {
    Serial.print("L2 ");
  }
  else
  {
    Serial.print("   ");
  }
  if (buttonsValue & 0x0010)
  {
    Serial.print("L1 ");
  }
  else
  {
    Serial.print("   ");
  }

  Serial.print("- ");

  if (dpadValue & 0x01)
  {
    Serial.print("U ");
  }
  else
  {
    Serial.print("  ");
  }
  if (dpadValue & 0x08)
  {
    Serial.print("L ");
  }
  else
  {
    Serial.print("  ");
  }
  if (dpadValue & 0x04)
  {
    Serial.print("R ");
  }
  else
  {
    Serial.print("  ");
  }
  if (dpadValue & 0x02)
  {
    Serial.print("D ");
  }
  else
  {
    Serial.print("  ");
  }

  Serial.print("- ");
  Serial.printf("%4d ", ctl->axisX());
  if (buttonsValue & 0x0100)
  {
    Serial.print("LS ");
  }
  else
  {
    Serial.print("   ");
  }
  Serial.printf("%4d ", ctl->axisY());

  Serial.print("- ");
  if (micsButtonValue & 0x02)
  {
    Serial.print("Sel ");
  }
  else
  {
    Serial.print("    ");
  }
  if (micsButtonValue & 0x04)
  {
    Serial.print("Sta ");
  }
  else
  {
    Serial.print("    ");
  }

  Serial.print("- ");
  Serial.printf("%4d ", ctl->axisRX());
  if (buttonsValue & 0x0200)
  {
    Serial.print("RS ");
  }
  else
  {
    Serial.print("   ");
  }
  Serial.printf("%4d ", ctl->axisRY());

  Serial.print("- ");
  if (buttonsValue & 0x0008)
  {
    Serial.print("Y ");
  }
  else
  {
    Serial.print("  ");
  }
  if (buttonsValue & 0x0004)
  {
    Serial.print("X ");
  }
  else
  {
    Serial.print("  ");
  }
  if (buttonsValue & 0x0002)
  {
    Serial.print("B ");
  }
  else
  {
    Serial.print("  ");
  }
  if (buttonsValue & 0x0001)
  {
    Serial.print("A ");
  }
  else
  {
    Serial.print("  ");
  }

  Serial.print("- ");
  if (buttonsValue & 0x0020)
  {
    Serial.print("R1 ");
  }
  else
  {
    Serial.print("   ");
  }
  if (buttonsValue & 0x0080)
  {
    Serial.print("R2 ");
  }
  else
  {
    Serial.print("   ");
  }
  Serial.printf("%4d ", ctl->throttle());

  Serial.println();
}

void blinkLedWaitingForConnection()
{
  digitalWrite(LED_PIN, LOW);
  delay(300);
  digitalWrite(LED_PIN, HIGH);
  delay(300);
}

void setup()
{
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t *addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  blinkLedWaitingForConnection();

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableNewBluetoothConnections(true);
  BP32.forgetBluetoothKeys(); // must be after enableNewBluetoothConnections(true)
  delay(500);
  Serial.println("1 seconds passed, stop the pairing mode.");
  BP32.enableNewBluetoothConnections(false);
}

void loop()
{
  if (!checkConnected)
    blinkLedWaitingForConnection();
  else
    digitalWrite(LED_PIN, LOW);
  // if (digitalRead(TOUCH_SENSOR_PIN) == HIGH) {
  //     Serial.println("Touch detected, resetting...");
  //     delay(100);
  //     esp_restart(); // Reset ESP32
  // }
  BP32.update();
  processControllers();
  delay(15);
}