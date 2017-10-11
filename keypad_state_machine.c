/**
 * 键盘扫描处理的设计原则是，既要保证MCU能及时地判别按键的动作，处理按键输入的操作，
 * 又不能过多的占用MUC的工作时间，让它有充裕的时间去处理其它的操作
 *
 * 通常，完成键盘扫描和处理的程序是系统程序中的一个专用子程序，MCU调用该键盘扫描子程序
 * 对键盘进行扫描和处理的方式有3种：程序控制扫描、定时扫描、中断扫描
 * 1.程序控制扫描方式：在主控程序中的适当位置调用键盘扫描程序，对键盘进行读取和处理
 * 2.定时扫描方式：需要使用MCU的一个定时器，使其产生一个10ms的定时中断，MCU响应定时中断，执行键盘扫描，
 * 当在连续两次中断中都读到相同的按键按下（间隔10ms作为消抖），MCU才执行相应的键处理程序
 * 3.中断方式：需要键盘电路作改动，当有按键按下时，键盘硬件电路产生一个外部的中断信号，MCU响应外部中断
 * 进行键盘处理
 *
 *
 * 状态0：键盘扫描检测，控制PD3~PD6，4根行线逐行输出低电平，对键盘进行扫描检测，一旦检测到键按下(key_value),
 *  立即停止键盘的扫描，状态转换到状态1，注意此时变量key_value中保存着读到的列线输入值，且该行线低电平输出维持
 *
 * 状态1：消抖处理和键盘编码。再次检测键盘列线的输入，并与状态0的key_value比较，不相等则返回状态0，实现消抖；
 *  相等则确认按键输入，进行键值设置，转换到状态2
 *
 * 状态2：等待按键释放。控制PD3~PD6，4跟行线全部输出低电平，检测3根列线输入全部为高电平（无按键按下）时状态返回
 *  到状态0
 *
 *
 * return:
 * 255:表示五按键按下
 * 0~11：相应的按键按下
 */

#define No_key 255
#define K1_1    1
#define K1_2    2
#define K1_3    3
#define K2_1    4
#define K2_2    5
#define K2_3    6
#define K3_1    7
#define K3_2    8
#define K3_3    9
#define K4_1    10   // *
#define K4_2    0
#define K4_3    11   // #
#define Key_mask  0x07 // 0b00000111


char read_keyboard(void)
{
  static char key_state = 0, key_value, key_line;
  char key_return = No_key, i;
  switch (key_state) {
    case 0:
      key_line = 0x08;// 0000 1000
      for(i = 1; i <= 4; i++) {//扫描键盘
        PORTD = ~key_line;     //输出行线电平
        PORTD = ~key_line;     //必须送2次！！！这样可以使得电平得以稳定后正确读取键值，相当于NOP
        key_value = Key_mask & PIND;//读列电平，PIND与按键是否按下有关，按下则该列被拉低
        if(key_value == Key_mask)//如果相等说明列线仍然都是高，即此列无按键按下
          key_line <<= 1;      //该行没有按键，继续扫描下一行
        else {
          key_state++;        //有按键，切换状态，停止扫描
          break;              //转消抖确认状态
        }
      }
      break;

    case 1:
      if(key_value == (Key_mask & PIND)) {//再次读列电平
        switch (key_line | key_value) {//与状态0读到的key_value相同，则确认按键
          case 0x0e://0000 1110       键盘编码，返回编码值
            key_return = K1_1;
            break;
          case 0x0d://0000 1101
            key_return = K1_2;
            break;

          /*...*/
          case 0x43://0100 0011
            key_return = K4_3;
            break;
        }
        key_state++;                  //转入等待按键释放状态
      } else {
        key_state--;                  //两次列电平不同，返回状态0(消抖处理)
      }
      break;

    case 2:                           //等待按键释放状态
      PORTD = 0x07;                   //0000 0111, 行线全部输出低电平
      PORTD = 0x07;                   //重复一次，相对于NOP
      if((Key_mask & PIND) == Key_mask)
        key_state = 0;                //行线全部为高电平，返回状态0
      break;
  }

  return key_return;
}
