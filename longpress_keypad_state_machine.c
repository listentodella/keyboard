#define Key_mask  0x38 // 0011 1000, idle column

uint8_t get_keyvalue(uint8_t key_line, uint8_t column)
{
	switch (key_line) {
	case 0x08:
	if(column == 0x18)
		return 1;
	else if(column == 0x28)
		return 2;
	else
		return 3;
	break;

	case 0x10:
	if(column == 0x18)
		return 4;
	else if(column == 0x28)
		return 5;
	else
		return 6;
	break;

	case 0x20:
	if(column == 0x18)
		return 7;
	else if(column == 0x28)
		return 8;
	else
		return 9;
	break;

	case 0x40:
	if(column == 0x18)
		return 10;
	else if(column == 0x28)
		return 11;
	else
		return 12;
	break;
  }
}

uint8_t read_keyboard(void)
{
	static uint8_t key_state = 0, key_press, key_line, key_time;
    uint8_t key_value = 0xFF, i, column;
    switch (key_state) {
		case 0://按键初始态
        key_line = 0x08;
		for(i = 1; i <= 4; i++) {
			aw95238_i2c_write(button_handle, OUTPUT_PORT1, ~key_line);
			aw95238_i2c_write(button_handle, OUTPUT_PORT1, ~key_line);
			aw95238_i2c_read(button_handle, INPUT_PORT0, &column);
			key_press = Key_mask & column;//保存了列位置
			if(key_press == Key_mask)//该行无按键按下
				key_line <<= 1;            //保存了行位置
			else {
				key_state++;//若有键按下，则进入按键确认态
				break;
			}
        }
        break;

		case 1://按键确认态
		aw95238_i2c_read(button_handle, INPUT_PORT0, &column);
		if(key_press == (Key_mask & column)) {//确认有按键按下
			key_value = get_keyvalue(key_line, column);//确认键值
			key_time = 0;
			key_state++;//进入按键是否长按
		} else {
			key_state--;//已松开
		}
		break;

		case 2://判断是否松开，只要未达到长按的时限则认为是松开，还原状态；否则进入长按计时
		aw95238_i2c_read(button_handle, INPUT_PORT0, &column);
		if((Key_mask & column) == Key_mask) {//若松开
			key_state--;
			// key_value = get_keyvalue(key_line, column);//确认键值
		} else if(++key_time >= 40) {//若未松开，进入计时
			key_value = get_keyvalue(key_line, column);//确认键值
			key_state++;//满足计数，进入长按
			key_time = 0;
			key_value |= 0x80;//给键值附上一个长按的标志位
		}
		break;

		case 3://长按状态
		aw95238_i2c_read(button_handle, INPUT_PORT0, &column);
		if((Key_mask & column) == Key_mask)
			key_state = 0;
		else {
			if(++key_time >= 20) {
				key_time = 0;
				key_value = get_keyvalue(key_line, column);//确认键值
				key_value |= 0x80;//给键值附上一个长按的标志位
			}
		}
		break;
    }

    return key_value;
}
