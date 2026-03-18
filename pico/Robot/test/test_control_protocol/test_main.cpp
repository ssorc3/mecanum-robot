#include <unity.h>

#include "protocol/control_protocol.h"

static void test_parse_control_valid_drive() {
    uint8_t frame[] = {
        'M', 'R', 1, 0x01, 0x10, 0x08,
        0x01, 0x06,
        0x64, 0x00,
        0x9C, 0xFF,
        0x00, 0x00
    };

    ControlCommand cmd;
    uint8_t seq = 0;

    bool ok = parseControlFrame(frame, sizeof(frame), cmd, seq);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT8(0x10, seq);
    TEST_ASSERT_TRUE(cmd.has_drive);
    TEST_ASSERT_EQUAL_INT16(100, cmd.vx);
    TEST_ASSERT_EQUAL_INT16(-100, cmd.vy);
    TEST_ASSERT_EQUAL_INT16(0, cmd.wz);
}

static void test_parse_control_bad_magic() {
    uint8_t frame[] = {
        'X', 'R', 1, 0x01, 0x00, 0x08,
        0x01, 0x06,
        0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00
    };

    ControlCommand cmd;
    uint8_t seq = 0;

    bool ok = parseControlFrame(frame, sizeof(frame), cmd, seq);
    TEST_ASSERT_FALSE(ok);
}

static void test_parse_control_bad_tlv_len() {
    uint8_t frame[] = {
        'M', 'R', 1, 0x01, 0x00, 0x07,
        0x01, 0x05,
        0x00, 0x00,
        0x00, 0x00,
        0x00
    };

    ControlCommand cmd;
    uint8_t seq = 0;

    bool ok = parseControlFrame(frame, sizeof(frame), cmd, seq);
    TEST_ASSERT_FALSE(ok);
}

static void test_parse_control_ignores_unknown_tlv() {
    uint8_t frame[] = {
        'M', 'R', 1, 0x01, 0x02, 0x0B,
        0x99, 0x01, 0xAA,
        0x01, 0x06,
        0xFF, 0x00,
        0x00, 0x00,
        0x00, 0x00
    };

    ControlCommand cmd;
    uint8_t seq = 0;

    bool ok = parseControlFrame(frame, sizeof(frame), cmd, seq);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT8(0x02, seq);
    TEST_ASSERT_TRUE(cmd.has_drive);
    TEST_ASSERT_EQUAL_INT16(255, cmd.vx);
    TEST_ASSERT_EQUAL_INT16(0, cmd.vy);
    TEST_ASSERT_EQUAL_INT16(0, cmd.wz);
}

static void test_encode_telemetry_all_fields() {
    TelemetryData tlm;
    resetTelemetryData(tlm);

    tlm.has_battery_mv = true;
    tlm.battery_mv = 12345;

    tlm.has_rssi_dbm = true;
    tlm.rssi_dbm = -42;

    tlm.has_uptime_ms = true;
    tlm.uptime_ms = 1000;

    tlm.has_error_flags = true;
    tlm.error_flags = 0xA5A5;

    uint8_t out[64] = {0};
    size_t len = encodeTelemetryFrame(out, sizeof(out), 0x55, tlm);

    TEST_ASSERT_EQUAL_UINT32(23, len);
    TEST_ASSERT_EQUAL_UINT8('M', out[0]);
    TEST_ASSERT_EQUAL_UINT8('R', out[1]);
    TEST_ASSERT_EQUAL_UINT8(1, out[2]);
    TEST_ASSERT_EQUAL_UINT8(0x02, out[3]);
    TEST_ASSERT_EQUAL_UINT8(0x55, out[4]);
    TEST_ASSERT_EQUAL_UINT8(17, out[5]);

    TEST_ASSERT_EQUAL_UINT8(0x10, out[6]);
    TEST_ASSERT_EQUAL_UINT8(2, out[7]);
    TEST_ASSERT_EQUAL_UINT8(0x39, out[8]);
    TEST_ASSERT_EQUAL_UINT8(0x30, out[9]);

    TEST_ASSERT_EQUAL_UINT8(0x11, out[10]);
    TEST_ASSERT_EQUAL_UINT8(1, out[11]);
    TEST_ASSERT_EQUAL_UINT8(0xD6, out[12]);

    TEST_ASSERT_EQUAL_UINT8(0x12, out[13]);
    TEST_ASSERT_EQUAL_UINT8(4, out[14]);
    TEST_ASSERT_EQUAL_UINT8(0xE8, out[15]);
    TEST_ASSERT_EQUAL_UINT8(0x03, out[16]);
    TEST_ASSERT_EQUAL_UINT8(0x00, out[17]);
    TEST_ASSERT_EQUAL_UINT8(0x00, out[18]);

    TEST_ASSERT_EQUAL_UINT8(0x13, out[19]);
    TEST_ASSERT_EQUAL_UINT8(2, out[20]);
    TEST_ASSERT_EQUAL_UINT8(0xA5, out[21]);
    TEST_ASSERT_EQUAL_UINT8(0xA5, out[22]);
}

static void test_encode_telemetry_empty() {
    TelemetryData tlm;
    resetTelemetryData(tlm);

    uint8_t out[16] = {0};
    size_t len = encodeTelemetryFrame(out, sizeof(out), 0x00, tlm);

    TEST_ASSERT_EQUAL_UINT32(6, len);
    TEST_ASSERT_EQUAL_UINT8('M', out[0]);
    TEST_ASSERT_EQUAL_UINT8('R', out[1]);
    TEST_ASSERT_EQUAL_UINT8(1, out[2]);
    TEST_ASSERT_EQUAL_UINT8(0x02, out[3]);
    TEST_ASSERT_EQUAL_UINT8(0x00, out[4]);
    TEST_ASSERT_EQUAL_UINT8(0, out[5]);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_parse_control_valid_drive);
    RUN_TEST(test_parse_control_bad_magic);
    RUN_TEST(test_parse_control_bad_tlv_len);
    RUN_TEST(test_parse_control_ignores_unknown_tlv);
    RUN_TEST(test_encode_telemetry_all_fields);
    RUN_TEST(test_encode_telemetry_empty);
    UNITY_END();
}

void loop() {}
