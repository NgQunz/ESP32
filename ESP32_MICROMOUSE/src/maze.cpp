#include "maze.h"
#include "config.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
// ===================== STATE =====================
int scoreMap[16][16];
int historyMap[16][16][3];
bool wallMap[16][16][4]; // [x][y][N=0,E=1,S=2,W=3]

int x = 0;
int y = 0;
int facing = 0; // 0=N 1=E 2=S 3=W
int moveCount = 1;
bool inDeadEnd = false;
bool goalReached = false;

// ===================== WALL =====================
/*
 * Lưu tường vào wallMap cho ô hiện tại (x,y) và ô kề bên (tường chung).
 * relativeDir: 'f'=trước, 'l'=trái, 'r'=phải
 */
static void setWall(int dir, char relativeDir)
{
    // Tính hướng tuyệt đối của tường
    int wallDir = dir;
    if (relativeDir == 'l')
        wallDir = (dir + 3) % 4;
    else if (relativeDir == 'r')
        wallDir = (dir + 1) % 4;
    // 'f' → giữ nguyên dir

    // Lưu tường cho ô hiện tại
    if (x >= 0 && x < 16 && y >= 0 && y < 16)
        wallMap[x][y][wallDir] = true;

    // Tính ô kề để lưu tường chung (tường có 2 mặt)
    int nx = x, ny = y;
    int oppositeDir = (wallDir + 2) % 4; // hướng ngược lại
    if (wallDir == 0)
        ny = y + 1; // N → ô phía trên
    else if (wallDir == 1)
        nx = x + 1; // E → ô bên phải
    else if (wallDir == 2)
        ny = y - 1; // S → ô phía dưới
    else if (wallDir == 3)
        nx = x - 1; // W → ô bên trái

    if (nx >= 0 && nx < 16 && ny >= 0 && ny < 16)
        wallMap[nx][ny][oppositeDir] = true;
}

// ===================== INIT =====================
void initMazeScore()
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            // Score heuristic: cao nhất ở trung tâm (7,7)-(8,8)
            scoreMap[i][j] = 128 - (abs(2 * i - 15) + abs(2 * j - 15)) / 2;

            historyMap[i][j][0] = 0;
            historyMap[i][j][1] = 0;
            historyMap[i][j][2] = 0;

            wallMap[i][j][0] = false;
            wallMap[i][j][1] = false;
            wallMap[i][j][2] = false;
            wallMap[i][j][3] = false;
        }
    }

    x = 0;
    y = 0;
    facing = 0;
    moveCount = 1;
    inDeadEnd = false;
    goalReached = false;
}

// ===================== MOTION =====================
void goCell()
{
    resetEncoders();
    float targetYaw = yawDeg;
    unsigned long startMs = millis();

    while (avgAbsEncoder() < CELL_PULSE)
    {
        // Timeout 3 giây phòng robot bị kẹt
        if (millis() - startMs > 3000)
            break;

        updateYaw();
        readTof();

        // encoder1 = bánh PHẢI, encoder2 = bánh TRÁI (đo thực tế)
        long encError = encoder2Count - encoder1Count; // dương → trái chạy nhanh hơn → giảm trái, tăng phải

        float yawError = targetYaw - yawDeg;

        int encCorrection = (int)(KP_ENC * encError);
        int gyroCorrection = (int)(KP_GYRO * yawError);

        int wallCorrection = 0;
        if (LeftMM < SIDE_WALL_USE_MM && RightMM < SIDE_WALL_USE_MM)
        {
            int wallError = LeftMM - RightMM;
            wallCorrection = (int)(KP_WALL * wallError);
        }

        // leftPWM  = Motor2, rightPWM = Motor1
        int leftPWM = BASE_PWM_FORWARD - encCorrection + gyroCorrection + wallCorrection;
        int rightPWM = BASE_PWM_FORWARD + encCorrection - gyroCorrection - wallCorrection;

        leftPWM = constrain(leftPWM, MIN_PWM_FORWARD, MAX_PWM_FORWARD);
        rightPWM = constrain(rightPWM, MIN_PWM_FORWARD, MAX_PWM_FORWARD);

        setMotor(leftPWM, rightPWM);
    }

    brakeStop();
}

void turnLeft90()
{
    float targetYaw = yawDeg + 90.0f;
    unsigned long startMs = millis();
    unsigned long lastPrint = 0;

    while (yawDeg < targetYaw - TURN_TOLERANCE_DEG)
    {
        if (millis() - startMs > 2000)
            break;
        updateYaw();
        setMotor(-BASE_PWM_TURN, BASE_PWM_TURN);

        if (millis() - lastPrint >= 50)
        {
            SerialBT.println("R yaw=" + String(yawDeg, 1));
            lastPrint = millis();
        }
    }

    brakeStop();
    SerialBT.println("R done=" + String(yawDeg, 2) + " target=" + String(targetYaw, 2));
}

void turnRight90()
{
    float targetYaw = yawDeg - 90.0f;
    unsigned long startMs = millis();
    unsigned long lastPrint = 0;

    while (yawDeg > targetYaw + TURN_TOLERANCE_DEG)
    {
        if (millis() - startMs > 2000)
            break;
        updateYaw();
        setMotor(BASE_PWM_TURN, -BASE_PWM_TURN);

        // In góc mỗi 50ms
        if (millis() - lastPrint >= 50)
        {
            SerialBT.println("L yaw=" + String(yawDeg, 1));
            lastPrint = millis();
        }
    }

    brakeStop();
    SerialBT.println("L done=" + String(yawDeg, 2) + " target=" + String(targetYaw, 2));
}

void turnBack180()
{
    float targetYaw = yawDeg + 180.0f;
    unsigned long startMs = millis();

    while (yawDeg < targetYaw - 3.0f)
    {
        if (millis() - startMs > 3000)
            break;
        updateYaw();
        setMotor(-BASE_PWM_TURN, BASE_PWM_TURN); // cùng chiều turnRight90
    }

    brakeStop();
}

// ===================== MOVE =====================
void moveRobot(char relativeDir)
{
    scoreMap[x][y] -= TRAVEL_COST;
    moveCount++;

    if (inDeadEnd)
        scoreMap[x][y] = -16383;

    if (relativeDir == 'b')
    {
        turnBack180();
        facing = (facing + 2) % 4;
    }
    else if (relativeDir == 'r')
    {
        turnRight90();
        facing = (facing + 1) % 4;
    }
    else if (relativeDir == 'l')
    {
        turnLeft90();
        facing = (facing + 3) % 4;
    }
    // 'f' → không quay

    goCell();

    // Cập nhật tọa độ theo hướng đang đi
    x = (facing == 1) ? x + 1 : (facing == 3) ? x - 1
                                              : x;
    y = (facing == 0) ? y + 1 : (facing == 2) ? y - 1
                                              : y;

    // Giới hạn tọa độ trong mảng 16x16
    x = constrain(x, 0, 15);
    y = constrain(y, 0, 15);
}

// ===================== THINK =====================
void think()
{
    readTof();

    bool wallFront = (FrontMM < WALL_THRESHOLD_MM);
    bool wallLeft = (LeftMM < WALL_THRESHOLD_MM);
    bool wallRight = (RightMM < WALL_THRESHOLD_MM);

    // Lưu tường vào wallMap
    if (wallFront)
        setWall(facing, 'f');
    if (wallLeft)
        setWall(facing, 'l');
    if (wallRight)
        setWall(facing, 'r');

    int wallCount = wallFront + wallLeft + wallRight;

    // Cập nhật lịch sử để phát hiện loop
    historyMap[x][y][0] = historyMap[x][y][1];
    historyMap[x][y][1] = historyMap[x][y][2];
    historyMap[x][y][2] = moveCount;

    // Phạt nếu đang loop (phải có đủ 3 lần thăm mới tính)
    if (historyMap[x][y][0] != 0 &&
        historyMap[x][y][1] - historyMap[x][y][0] ==
            historyMap[x][y][2] - historyMap[x][y][1])
    {
        scoreMap[x][y] -= LOOP_COST;
    }

    // --- Dead end: 3 tường ---
    if (wallCount == 3)
    {
        inDeadEnd = true;
        moveRobot('b');
        return;
    }

    // --- Chỉ 1 hướng: đi thẳng ---
    if (wallCount == 2)
    {
        if (!wallFront)
            moveRobot('f');
        else if (!wallLeft)
            moveRobot('l');
        else
            moveRobot('r');
        return;
    }

    // --- Nhiều hướng: chọn theo score ---
    inDeadEnd = false;

    int dir, u, v;
    int scoreF = -16383, scoreR = -16383, scoreL = -16383;

    if (!wallFront)
    {
        dir = facing;
        u = (dir == 1) ? x + 1 : (dir == 3) ? x - 1
                                            : x;
        v = (dir == 0) ? y + 1 : (dir == 2) ? y - 1
                                            : y;
        if (u >= 0 && u < 16 && v >= 0 && v < 16)
            scoreF = scoreMap[u][v];
    }
    if (!wallRight)
    {
        dir = (facing + 1) % 4;
        u = (dir == 1) ? x + 1 : (dir == 3) ? x - 1
                                            : x;
        v = (dir == 0) ? y + 1 : (dir == 2) ? y - 1
                                            : y;
        if (u >= 0 && u < 16 && v >= 0 && v < 16)
            scoreR = scoreMap[u][v];
    }
    if (!wallLeft)
    {
        dir = (facing + 3) % 4;
        u = (dir == 1) ? x + 1 : (dir == 3) ? x - 1
                                            : x;
        v = (dir == 0) ? y + 1 : (dir == 2) ? y - 1
                                            : y;
        if (u >= 0 && u < 16 && v >= 0 && v < 16)
            scoreL = scoreMap[u][v];
    }

    // Thưởng điểm chỉ lần đầu thăm ô này, và chỉ khi có ngã rẽ (wallCount < 2)
    if (historyMap[x][y][0] == 0 && historyMap[x][y][1] == 0 && wallCount < 2)
        scoreMap[x][y] += BRANCH_REWARD * (2 - wallCount);

    // Chọn hướng score cao nhất (đã bỏ 3 else-if thừa của bản gốc)
    if (scoreF > -8191 && scoreF >= scoreR && scoreF >= scoreL)
        moveRobot('f');
    else if (scoreR > -8191 && scoreR >= scoreL)
        moveRobot('r');
    else if (scoreL > -8191)
        moveRobot('l');
    else
    {
        inDeadEnd = true;
        moveRobot('b');
    }
}

// ===================== RUN MAZE =====================
/*
 * Chạy tự động cho đến khi đến vùng goal (x=7,8 và y=7,8).
 * Trong lúc chạy vẫn kiểm tra lệnh 's' từ Bluetooth để dừng khẩn cấp.
 */

extern BluetoothSerial SerialBT; // khai báo từ main.cpp

void runMaze()
{
    goalReached = false;

    while (true)
    {
        // --- Dừng khẩn cấp qua Bluetooth ---
        if (SerialBT.available())
        {
            char cmd = SerialBT.read();
            if (cmd == 's')
            {
                stopAllMotors();
                goalReached = false;
                return;
            }
        }

        // --- Đã đến goal ---
        if ((x == 7 || x == 8) && (y == 7 || y == 8))
        {
            stopAllMotors();
            goalReached = true;
            return;
        }

        think();
    }
}