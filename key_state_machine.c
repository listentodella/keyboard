/**
 *独立按键按下不松手，也只会上报一次，即不支持长按
 *
 * 该简单按键接口函数read_key()在整个系统程序中应每隔10ms调用一次
 * 每次执行时将先读取与按键连接的IO电平到变量key_press中，然后进入用switch结构构成的状态机
 * swtich-case 结构分别实现了3个不同状态的处理判别过程，在每个状态中将根据状态的不同，
 * 以及key_press的值（状态机的输入）确定输出值（key_return)和确定下一次按键的状态值（key_state)
 *
 * @return
 * 0：表示无按键动作
 * 1：表示有一次按键闭合动作
 */

#define key_input PIND.7  //按键输入口
#define key_state_0 0     //无按键
#define key_state_1 1     //确认有按键
#define key_state_2 2     //松开，判断是否松开状态

char read_key(void)
{
  static char key_state = 0;
  char key_press, key_return = 0;

  key_press = key_input;    //读按键IO电平
  switch (key_state) {
    case key_state_0://按键初始态，即无按键
      if(!key_press)
        key_state = key_state_1;//按键被按下，状态转换到键确认态
        break;
    case key_state_1://按键确认态
      if(!key_press) {
        key_return = 1;//按键仍然被按下，按键确认输出为 1
        key_state = key_state_2;//状态转换到键释放态，判断是否松开
      } else
        key_state = key_state_0;//按键已抬起，转换到按键初始态
      break;
    case key_state_2://判断是否松开
      if(key_press)
        key_state = key_state_0;//按键已释放，转换到按键初始态
      break;
  }

  return key_return;
}
