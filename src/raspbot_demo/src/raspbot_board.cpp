#include "raspbot_demo/raspbot_board.hpp"
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/i2c-dev.h>
extern "C" {
#include <i2c/smbus.h>
}

#include <stdexcept>
#include <cstring>

RaspbotBoard::RaspbotBoard(const std::string& dev, uint8_t addr)
    : addr_(addr)
{
    fd_ = ::open(dev.c_str(), O_RDWR);
    if(fd_ < 0) 
        throw std::runtime_error("打开 " + dev + " 失败: " + std::strerror(errno));
    if(::ioctl(fd_, I2C_SLAVE, addr_) < 0)
    {
        ::close(fd_);
        throw std::runtime_error("设置从机地址失败");
    }
}

RaspbotBoard::~RaspbotBoard()
{
    if(fd_ >= 0)
    {
        stop_all(); // 析构时确保电机停 —— 安全！
        ::close(fd_);
    }
}

void RaspbotBoard::write_reg_bytes(uint8_t reg, const uint8_t* data, uint8_t len){
    if(i2c_smbus_write_i2c_block_data(fd_, reg, len, data) < 0)
        throw std::runtime_error("I2C 写失败 reg=" + std::to_string(reg));
}

uint8_t RaspbotBoard::read_reg_byte(uint8_t reg){
    int v = i2c_smbus_read_byte_data(fd_,reg);
    if(v < 0) throw std::runtime_error("I2C 读失败 reg=" + std::to_string(reg));
    return static_cast<uint8_t>(v);
}

void RaspbotBoard::set_motor(uint8_t id, uint8_t dir, uint8_t speed){
    uint8_t buf[3] = {id, dir, speed};
    write_reg_bytes(0x01, buf, 3);
}

void RaspbotBoard::set_motor_signed(uint8_t id, int16_t speed){
    if(speed > 255) speed = 255;
    if(speed < -255) speed = -255;
    uint8_t dir = (speed < 0) ? 1 : 0;
    set_motor(id, dir, static_cast<uint8_t>(std::abs(speed)));
}

void RaspbotBoard::stop_all(){
    for(uint8_t id = 0; id < 4; ++id) set_motor(id, 0, 0);
}

void RaspbotBoard::ultrasonic_switch(bool on){
    uint8_t v = on ? 1 : 0;
    write_reg_bytes(0x07, &v, 1);
}

uint16_t RaspbotBoard::read_distance_mm(){
    uint8_t lo = read_reg_byte(0x1a);
    uint8_t hi = read_reg_byte(0x1b);
    return static_cast<uint16_t>((hi << 8) | lo);
}

uint8_t RaspbotBoard::read_tracking(){
    return read_reg_byte(0x0a);
}



