#include <Arduino.h>
#include <SoftwareSerial.h>

#ifndef LTM_RX_PIN
#error LTM has no RX pin defined
#endif

#ifndef LTM_TX_PIN
#error LTM has no TX pin defined
#endif

#ifndef LTM_BAUD
#define LTM_BAUD 9600
#endif

enum LTMState {
    LTM_IDLE,
    LTM_HEADER_START1,
    LTM_HEADER_START2,
    LTM_HEADER_MSGTYPE,
    LTM_HEADER_DATA
};

#define LTM_LONGEST_FRAME_LENGTH 18
#define LTM_GFRAMELENGTH 18
#define LTM_AFRAMELENGTH 10
#define LTM_SFRAMELENGTH 11
#define LTM_OFRAMELENGTH 18
#define LTM_NFRAMELENGTH 10
#define LTM_XFRAMELENGTH 10

static const char* LTM_FLIGHT_MODES[] = {
    "Manual", "Rate", "Angle", "Horizon", "Acro",
    "Stabilized1", "Stabilized2", "Stabilized3",
    "Altitude Hold", "GPS Hold", "Waypoints", 
    "Head free", "Circle", "RTH", "Follow me", 
    "Land", "Fly by wire A", "Fly by wire B", 
    "Cruise", "Unknown", "Launch", "Autotune"
};

struct LTMData {
    int pitch;
    int roll;
    int heading;
    uint16_t voltage;
    byte rssi;
    bool armed;
    bool failsafe;
    byte flightmode;
    int32_t latitude;
    int32_t longitude;
    int32_t altitude;
    uint8_t groundSpeed;
    int16_t hdop;
    uint8_t gpsFix;
    uint8_t gpsSats;
    int32_t homeLatitude;
    int32_t homeLongitude;
    uint8_t sensorStatus;
};

LTMData ltmData;

SoftwareSerial ltmSerial(LTM_RX_PIN, LTM_TX_PIN);
uint8_t ltmBuffer[LTM_LONGEST_FRAME_LENGTH];
LTMState ltmState = LTM_IDLE;
char ltmFrameType;
byte ltmFrameLength;
byte ltmReceiverIndex;

uint16_t readUint16(uint8_t offset) {
    return (uint16_t)ltmBuffer[offset] | ((uint16_t)ltmBuffer[offset + 1] << 8);
}

int16_t readInt16(uint8_t offset) {
    return (int16_t)ltmBuffer[offset] | ((int16_t)ltmBuffer[offset + 1] << 8);
}

int32_t readInt32(uint8_t offset) {
    return (int32_t)ltmBuffer[offset] | ((int32_t)ltmBuffer[offset + 1] << 8) | ((int32_t)ltmBuffer[offset + 2] << 16) | ((int32_t)ltmBuffer[offset + 3] << 24);
}

int16_t toAttitude(int16_t v) {
    return v > 32767 ? -1 * (65535 - v) : v;
}

void ltmBegin() {
    ltmSerial.begin(LTM_BAUD);
}

float convertGPS(int32_t raw) {
    return raw / 1e7;
}

void processLTMFrame() {
    switch (ltmFrameType) {
        case 'A':
            ltmData.pitch = toAttitude(readUint16(0));
            ltmData.roll = toAttitude(readUint16(2));
            ltmData.heading = readUint16(4);
            break;
        case 'S':
            ltmData.voltage = readUint16(0);
            ltmData.rssi = ltmBuffer[4];
            ltmData.flightmode = ltmBuffer[6] >> 2;
            break;
        case 'G':
            ltmData.latitude = convertGPS(readInt32(0));
            ltmData.longitude = convertGPS(readInt32(4));
            ltmData.altitude = readInt32(9);
            ltmData.gpsSats = ltmBuffer[13] >> 2;
            ltmData.gpsFix = ltmBuffer[13] & 0x03;
            break;
        case 'O':
            ltmData.latitude = convertGPS(readInt32(0));
            ltmData.longitude = convertGPS(readInt32(4));
            ltmData.altitude = readInt32(8);
            ltmData.groundSpeed = ltmBuffer[12];
            ltmData.gpsSats = ltmBuffer[13];
            break;
        case 'N':
            ltmData.homeLatitude = readInt32(0);
            ltmData.homeLongitude = readInt32(4);
            break;
        case 'X':
            ltmData.hdop = readUint16(0);
            ltmData.sensorStatus = ltmBuffer[2];
            break;
    }
}

void ltmLoop() {
    while (ltmSerial.available()) {
        digitalWrite(LED_BUILTIN, LOW);
        char data = ltmSerial.read();

        switch (ltmState) {
            case LTM_IDLE:
                if (data == '$') ltmState = LTM_HEADER_START1;
                break;
            case LTM_HEADER_START1:
                ltmState = (data == 'T') ? LTM_HEADER_START2 : LTM_IDLE;
                break;
            case LTM_HEADER_START2:
                ltmFrameType = data;
                ltmState = LTM_HEADER_MSGTYPE;
                ltmReceiverIndex = 0;
                switch (data) {
                    case 'G': ltmFrameLength = LTM_GFRAMELENGTH; break;
                    case 'A': ltmFrameLength = LTM_AFRAMELENGTH; break;
                    case 'S': ltmFrameLength = LTM_SFRAMELENGTH; break;
                    case 'O': ltmFrameLength = LTM_OFRAMELENGTH; break;
                    case 'N': ltmFrameLength = LTM_NFRAMELENGTH; break;
                    case 'X': ltmFrameLength = LTM_XFRAMELENGTH; break;
                    default: ltmState = LTM_IDLE; break;
                }
                break;
            case LTM_HEADER_MSGTYPE:
                if (ltmReceiverIndex == ltmFrameLength - 4) {
                    ltmBuffer[ltmReceiverIndex++] = data;
                    processLTMFrame();
                    ltmState = LTM_IDLE;
                    memset(ltmBuffer, 0, LTM_LONGEST_FRAME_LENGTH);
                } else {
                    ltmBuffer[ltmReceiverIndex++] = data;
                }
                break;
            default:
                ltmState = LTM_IDLE;
                break;
        }
        digitalWrite(LED_BUILTIN, HIGH);
    }
}
