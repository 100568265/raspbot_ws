#pragma once
#include <cstdint>
#include <string>

class RaspbotBoard{
public:
    explicit RaspbotBoard(const std::string& dev = "/dev/i2c-1", uint8_t addr = 0x2B);
    ~RaspbotBoard();

    // 禁止拷贝
    RaspbotBoard(const RaspbotBoard &) = delete;
    RaspbotBoard & operator=(const RaspbotBoard &) = delete;

    // 电机：id 0~3, dir 0前/1后, speed 0~255
    void set_motor(uint8_t id, uint8_t dir, uint8_t speed);
    // 便捷：带符号速度 -255~255，自动拆方向
    void set_motor_signed(uint8_t id, int16_t speed);
    void stop_all();

    // 传感器
    void ultrasonic_switch(bool on);
    uint16_t read_distance_mm();
    uint8_t read_tracking();    // 返回原始字节，bit3..0 = x1..x4

private:
    void write_reg_bytes(uint8_t reg, const uint8_t* data, uint8_t len);
    uint8_t read_reg_byte(uint8_t reg);

    int fd_{-1};
    uint8_t addr_;
};