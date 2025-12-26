#include <cstdint>
#include "sys_led.h"

uint32_t SysLed::_on_ms;
uint32_t SysLed::_off_ms;
uint64_t SysLed::_on_next_ms;
uint64_t SysLed::_off_next_ms;