#include "aw95238.h"

status_t aw95238_i2c_read(uint8_t reg, uint8_t *data)
{
  status_t status;
  i2c_master_transfer_t *xfer = &(button_handle->xfer);
  memset(xfer, 0, sizeof(*xfer));

  xfer->slaveAddress = KEYBOARD_I2C_ADDRESS;
  xfer->direction = kI2C_Read;
  xfer->subaddress = reg;
  xfer->subaddressSize = 1;
  xfer->data = data;
  xfer->dataSize = 1;
  xfer->flags = kI2C_TransferDefaultFlag;

  status = I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER, xfer);
  if(status < 0)
    PRINTF("%s: I2C read failed...\r\n", __func__);

  return status;
}

status_t aw95238_i2c_write(uint8_t reg, uint8_t *data)
{
  status_t status;
  i2c_master_transfer_t *xfer = &(button_handle->xfer);
  memset(xfer, 0, sizeof(*xfer));

  xfer->slaveAddress = KEYBOARD_I2C_ADDRESS;
  xfer->direction = kI2C_Write;
  xfer->subaddress = reg;
  xfer->subaddressSize = 1;
  xfer->data = data;
  xfer->dataSize;
  xfer->flags = kI2C_TransferDefaultFlag;

  status = I2C_MasterTransferBlocking(EXAMPLE_I2C_MASTER, xfer);
  if(status < 0)
    PRINTF("%s: I2C write failed...\r\n", __func__);

  return status;
}


status_t aw95238_Init(keyborad_handle_t *handle, I2C_Type *base)
{
    i2c_master_transfer_t *xfer = &(handle->xfer);
    status_t status;
    uint8_t mode;
    uint8_t val;

    assert(handle);
    assert(base);
    if (!handle || !base)
    {
        return kStatus_InvalidArgument;
    }
    handle->base = base;
/*do not forget to init i2c master*/
    // status = I2C_Init();
    // if (status != kStatus_Success)
    // {
    //     PRINTF("I2C init failed\n");
    // }
    // assert(status == kStatus_Success);

    // // AW95238 Reset
    // gpio_pin_config_t pin_config = {
    //   kGPIO_DigitalOutput, 0,
    // };
    // GPIO_PinInit(GPIO, 3, 11, &pin_config);
    // GPIO_WritePinOutput(GPIO, 3, 11, 1);

  /*READ ID
  * this register is read only,and its value must be 0x23H
  */
  aw95238_i2c_read(AW95238_ID, &val);
  PRINTF("Read the keyboard ic id is %x\r\n", val);
  if(val != 0x23)
  {
    PRINTF("the id must be 0x23...\r\n");
    return -1;
  }


  /*config the port0&1 output or input
  * each bit: 1-input 0-output
  */
  aw95238_i2c_write(CONFIG_PORT0, 0x38);
  aw95238_i2c_write(CONFIG_PORT1, 0x00);

  /*config port0&1 output state*/
  aw95238_i2c_write(OUTPUT_PORT0, 0xC7);
  aw95238_i2c_write(OUTPUT_PORT1, 0xFF);

  /*config for led mode*/
  aw95238_i2c_write(LED_MODE_PORT0, 0xFC);
  aw95238_i2c_write(LED_MODE_PORT1, 0xFC);

  /*set the led output under the Imax*/
  aw95238_i2c_write(LED_DIM_PORT1_1, 0x40);
  aw95238_i2c_write(LED_DIM_PORT1_0, 0x40);
  aw95238_i2c_write(LED_DIM_PORT0_1, 0x40);
  aw95238_i2c_write(LED_DIM_PORT0_0, 0x40);

  /*enable the interrupts of the aw95238
  * each bit: 0-enable 1-disable
  * only read port0 or port1 can clear corresponding interrupts
  * disable the interrupts of port0
  * enable the interrupts of port1 for bit 3,4,5,6
  */
  aw95238_i2c_write(INT_PORT0, 0xC7);
  aw95238_i2c_write(INT_PORT1, 0xFF);

  /*enable the interrupt of NXP*/

/* attach 12 MHz clock to FLEXCOMM6 (I2C master) */
CLOCK_AttachClk(kFRO12M_to_FLEXCOMM6);

/* attach 12 MHz clock to FLEXCOMM8 (I2C slave) */
CLOCK_AttachClk(kFRO12M_to_FLEXCOMM8);

    return status;
}

/*通过读取寄存器就可以获得哪个按键被按下，但是键值如何设置？*/
status_t AW95238_ReadButtonData(keyborad_handle_t *handle)
{
    assert(handle);
    if (!handle)
    {
      return kStatus_InvalidArgument;
    }

    return I2C_MasterTransferBlocking(handle->base, &handle->xfer);
}

status_t AW95238_GetSingleButton(keyborad_handle_t *handle, button_event_t *button_event)
{
  status_t status;
  button_event_t button_event_local;

  status = AW95238_ReadButtonData(handle);
  if (status == kStatus_Success)
  {
    /**/
  }
  
    /*1.中断：该ic在gpio状态确实发生变化时就会产生中断，而不管是变低了还是变高了
  如果在按下的那一刻产生了中断，去读端口值，可以获知是哪一列的按键被按下但是无法知道是哪一行

  2.由于列线配置为输入，行线配置为输出1，当有按键按下，列线的输入电平变为1，可以确切的知道是哪一列电平发生了变化；
    这时再依次拉低行线，如果列线电平再次发生变化，则说明是这一行，如果不变则说明不是这一行

  3.目前问题在于，ic只有配置输入输出的，不知是否可以直接通过配置输出的高低来控制输出的高低

  */

  return status;
}
