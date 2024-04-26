/*************************************************** 
  这是我们的Adafruit 16通道PWM和舵机驱动器的库

  今天就在Adafruit商店中购买一个！
  ------> http://www.adafruit.com/products/815

  这些显示器使用I2C进行通信，需要2个引脚来接口。对于Arduino UNO，SCL -> 模拟5，SDA -> 模拟4

  Adafruit花费了时间和资源提供这个开源代码，请通过购买Adafruit的产品来支持Adafruit和开源硬件！

  由Limor Fried/Ladyada为Adafruit Industries编写。
  BSD许可证，任何重新分发必须包含上面的所有文本
 ****************************************************/

#include "Adafruit_MS_PWMServoDriver.h"
#include <Wire.h>

#if defined(ARDUINO_SAM_DUE)
 #define WIRE Wire1
#else
 #define WIRE Wire
#endif

// 创建Adafruit_MS_PWMServoDriver类的构造函数
Adafruit_MS_PWMServoDriver::Adafruit_MS_PWMServoDriver(uint8_t addr) {
  _i2caddr = addr;
}

// 初始化函数
void Adafruit_MS_PWMServoDriver::begin(void) {
  WIRE.begin(); // 启动I2C通信
  reset(); // 重置驱动器
}

// 重置函数
void Adafruit_MS_PWMServoDriver::reset(void) {
  write8(PCA9685_MODE1, 0x0); // 写入控制寄存器1，将设备重置为初始状态
}

// 设置PWM频率函数
void Adafruit_MS_PWMServoDriver::setPWMFreq(float freq) {
  // 修正频率设置中的过冲
  freq *= 0.9;

  // 计算预分频值
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;
  uint8_t prescale = floor(prescaleval + 0.5); // 取整并四舍五入
  
  // 保存当前模式
  uint8_t oldmode = read8(PCA9685_MODE1);
  // 设置设备进入睡眠模式
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  write8(PCA9685_MODE1, newmode); // 进入睡眠模式
  write8(PCA9685_PRESCALE, prescale); // 设置预分频
  write8(PCA9685_MODE1, oldmode);
  delay(5);
  write8(PCA9685_MODE1, oldmode | 0xa1); // 设置模式，打开自动增量寄存器
}

// 设置PWM函数
void Adafruit_MS_PWMServoDriver::setPWM(uint8_t num, uint16_t on, uint16_t off) {
  WIRE.beginTransmission(_i2caddr);
  WIRE.write(LED0_ON_L+4*num); // 寄存器地址
  WIRE.write(on); // 开始时间
  WIRE.write(on>>8);
  WIRE.write(off); // 结束时间
  WIRE.write(off>>8);
  WIRE.endTransmission();
}

// 读取8位数据函数
uint8_t Adafruit_MS_PWMServoDriver::read8(uint8_t addr) {
  WIRE.beginTransmission(_i2caddr);
  WIRE.write(addr); // 寄存器地址
  WIRE.endTransmission();

  WIRE.requestFrom((uint8_t)_i2caddr, (uint8_t)1); // 请求读取一个字节的数据
  return WIRE.read(); // 读取数据并返回
}

// 写入8位数据函数
void Adafruit_MS_PWMServoDriver::write8(uint8_t addr, uint8_t d) {
  WIRE.beginTransmission(_i2caddr);
  WIRE.write(addr); // 寄存器地址
  WIRE.write(d); // 写入数据
  WIRE.endTransmission();
}
