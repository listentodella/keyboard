#include "remote_keyboard.h"

keyborad_handle_t key_handle;








void Keyboard_Task(void *pvParameters)
{
  struct 
}





status_t Keyboard_I2C_Master_Init(void)
{
  i2c_master_config_t masterConfig;
  I2C_MasterGetDefaultConfig(&masterConfig);
  masterConfig.baudRate_Bps = I2C_BAUDRATE;
  I2C_MasterInit(NXP_I2C_MASTER, &masterConfig, I2C_MASTER_CLOCK_FREQUENCY);

  return kStatus_Success;
}

int Keyboard_Init(void)
{
  status_t status;
  gpio_pin_config_t pin_config = {
    kGPIO_DigitalOutput, 0,
  };

  CLOCK_AttachClk(kMCLK_to_FLEXCOMM6);
  CLOCK_SetClkDiv(kCLOCK_DivLcdClk, 1, true);

  /*Initialize I2C for Keyboard IC*/
  status = Keyboard_I2C_Master_Init();
  if (status != kStatus_Success)
    PRINTF("I2C init failed\n");
  assert(status == kStatus_Success);

  /*Reset the keyboard IC*/
   GPIO_PinInit(GPIO, 3, 11, &pin_config);
   GPIO_WritePinOutput(GPIO, 3, 11, 1);

   /*Initialize aw95238*/
   status = AW95238_Init(&key_handle, NXP_I2C_MASTER);
   if(status != kStatus_Success)
    PRINTF("Keyboard IC init failed\n");

  return status;
}
