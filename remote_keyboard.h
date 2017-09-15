#ifndef _REMOTER_KEYBOARD_H_
#define  _REMOTER_KEYBOARD_H_

#include "remoter_base.h"
#include "fsl_i2c.h"
#include "aw95238.h"

#define NXP_I2C_MASTER_BASE (I2C6_BASE)
#define I2C_MASTER_CLOCK_FREQUENCY (12000000)
#define NXP_I2C_MASTER ((I2C_Type *)NXP_I2C_MASTER_BASE)
#define I2C_BAUDRATE 100000U

#endif
