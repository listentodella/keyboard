/**
 *独立按键按下不松手，支持长按
 *
 * 该简单按键接口函数read_key_n()在整个系统程序中应每隔10ms调用一次
 *
 * @return
 * 0：表示无按键动作
 * 1：表示有一次按键闭合动作，“单发”（小于1s）动作
 * 2：表示按键的“连发”动作（大于1s）
 */

#define key_input PIND.7  //按键输入口
#define key_state_0 0     //无按键
#define key_state_1 1     //确认有按键
#define key_state_2 2     //判断是否松开状态
#define key_state_3 3     //长按状态，计时2

char read_key_n(void)
{
  static char key_state = 0, key_time = 0;
  char key_press, key_return = 0;

  key_press = key_input;    //读按键IO电平
  switch (key_state) {
    case key_state_0://按键初始态，即无按键
      if(!key_press)
        key_state = key_state_1;//按键被按下，状态转换到键确认态
        break;
    case key_state_1://按键确认态
      if(!key_press) {
        key_state = key_state_2;//状态转换到键释放态
        key_time = 0;           //清零按键时间计数器
      } else
        key_state = key_state_0;//按键已抬起，转换到按键初始态
      break;
    case key_state_2://判断是否松开，只要未达到长按的时限则判断为松开，还原状态：否则进入长按计时状态
      if(key_press) {
        key_state = key_state_0;//按键已释放，转换到按键初始态
        key_return = 1;         //输出 “1”
      } else if(++key_time >= 100) {//若未松开，开始按键时间计数
        key_state = key_state_3;//按下时间大于1s，状态转换到计时2，进入长按状态
        key_time = 0;           //清零按键计数器
        key_return = 2;         //输出 “2”
      }
      break;
    case key_state_3:
      if(key_press)
        key_state = key_state_0;//按键已释放，转换到按键初始态
      else {
        if(++key_time >= 50) {  //按键时间计数，准备连发
          key_time = 0;         //按下时间大于 0.5s，清零按键计数器
          key_return = 2;       //输出 “2”，连发
        }
      }
      break;
  }

  return key_return;
}
