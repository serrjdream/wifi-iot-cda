uint32_t current_value; 				// Инициализация переменной текущего значения счетчика
uint32_t previous_value;				// Инициализация переменной предыдущего значения счетчика
uint32_t counter_overflows;			// Инициализация счетчика переполнений
uint32_t division_ratio;			  // Инициализация коэффициента деления
uint32_t initial_indication;		// Инициализация переменной начального показания счетчика
uint8_t value_1,value_2,value_3;

void ICACHE_FLASH_ATTR
startfunc()
{  
// выполняется один раз при старте модуля.

}

PCF8583_read_my(uint8_t adr)
 {
i2c_start();										// Устанавливаем на шине Start состояние
i2c_writeByte(adr*2);						// отправляем байт на шину 0b101000yх (откликается любой RTC)
  if(i2c_getAck()){return(-1);} // slave не откликнулся
i2c_writeByte(01);							// устанавливаем регистр для чтения
  if(i2c_getAck()){return(-1);} // slave not ack
i2c_stop();											// Формируем на шине Stop состояние
// ------------------------------------------------------
i2c_start(); 										// Устанавливаем на шине Start состояние
 i2c_writeByte(adr*2+1);
   if(i2c_getAck()){return(-1);} // slave не откликнулся
uint8_t Val;
Val= i2c_readByte(); //ACK
current_value=(Val&15) + (Val/16)*10; // Поразрядное логическое "И" +
value_1=Val;
i2c_setAck(0);
Val= i2c_readByte(); //ACK
current_value=current_value+ (Val&15)*100 + (Val/16)*1000;
value_2=Val;
i2c_setAck(0);
Val= i2c_readByte(); //ACK
current_value=current_value+ (Val&15)*10000 + (Val/16)*100000;
value_3=Val;
i2c_setAck(0);
Val= i2c_readByte(); //ACK
previous_value=(Val&15) + (Val/16)*10; // Поразрядное логическое "И" +
i2c_setAck(0);
Val= i2c_readByte(); //ACK
previous_value=previous_value+ (Val&15)*100 + (Val/16)*1000;
i2c_setAck(0);
Val= i2c_readByte(); //ACK
previous_value=previous_value+ (Val&15)*10000 + (Val/16)*100000;
i2c_setAck(0);

//------------- читаем содержимое счетчика переполнений ------------
Val = i2c_readByte(); //ACK
counter_overflows = Val;
i2c_setAck(0);

//------------ читаем содержимое коэффициента деления --------------
Val = i2c_readByte(); //ACK 
division_ratio=Val; // Первые 8 бит всунули в 4 байтовое слово
i2c_setAck(0);
Val = i2c_readByte(); //ACK 
division_ratio=division_ratio+(Val<<8); // Вторые 8 бит всунули в 4 байтовое слово
i2c_setAck(0);
Val = i2c_readByte(); //ACK 
division_ratio=division_ratio+(Val<<16); // третьи 8 бит всунули в 4 байтовое слово
i2c_setAck(0);
Val = i2c_readByte(); //ACK 
division_ratio=division_ratio+(Val<<24); // Четвартые 8 бит всунули в 4 байтовое слово
i2c_setAck(0);

//----------- читаем начальное показание счетчика --------------
Val = i2c_readByte(); //ACK 
initial_indication=Val; // Первые 8 бит всунули в 4 байтовое слово
i2c_setAck(0);
Val = i2c_readByte(); //ACK 
initial_indication=initial_indication+(Val<<8); // Вторые 8 бит всунули в 4 байтовое слово
i2c_setAck(0);
Val = i2c_readByte(); //ACK 
initial_indication=initial_indication+(Val<<16); // третьи 8 бит всунули в 4 байтовое слово
i2c_setAck(0);
Val = i2c_readByte(); //ACK 
initial_indication=initial_indication+(Val<<24); // Четвартые 8 бит всунули в 4 байтовое слово

//---------------------------------------------------------------------------
i2c_setAck(1);
i2c_stop();
// ------- Теперь проверяем не было ли переполнения -------------------------
return current_value;
}

PCF8583_upd_previous(uint8_t adr)
{
i2c_start();			// Устанавливаем на шине Start состояние
i2c_writeByte(adr*2);	// отправляем байт на шину 0b101000yх (откллюбой RTC)
  if(i2c_getAck()){return;} 	// slave не откликнулся
i2c_writeByte(04);		//  устанавливаем адрес регистра 0x00 для чтения
  if(i2c_getAck()){return;}   // slave не откликнулся
i2c_writeByte(value_1);	//  записываем по адресу 0x00 байт 0x20
	if(i2c_getAck()){return;}   // slave не откликнулся
i2c_writeByte(value_2);	//  записываем по адресу 0x00 байт 0x20
	if(i2c_getAck()){return;}   // slave не откликнулся
i2c_writeByte(value_3);	//  записываем по адресу 0x00 байт 0x20
  if(i2c_getAck()){} 	// slave не откликнулся
i2c_stop();			// Формируем на шине Stop состояние
}

PCF8583_upd_counter_overflows(uint8_t adr)
{
i2c_start();			// Устанавливаем на шине Start состояние
i2c_writeByte(adr*2);	// отправляем байт на шину 0b101000yх (отк любой RTC)
  if(i2c_getAck()){return;} 	// slave не откликнулся
i2c_writeByte(07);		//  устанавливаем адрес регистра 0x00 для чтения
  if(i2c_getAck()){return;}   // slave не откликнулся
i2c_writeByte(counter_overflows);	//  записываем по адресу 0x00 байт 0x20
  if(i2c_getAck()){} 			// slave не откликнулся
i2c_stop();					// Формируем на шине Stop состояние
}

void ICACHE_FLASH_ATTR
 timerfunc(uint32_t  timersrc) {
// выполнение кода каждую 1 секунду
if(timersrc%10==0){
// выполнение кода каждые 10 секунд
PCF8583_read_my (0x50);
// --- Логика работы счетчика ---------

if (current_value>previous_value)
{
	PCF8583_upd_previous(0x50);// Перезапись предыдущего значения счетчика текущим 
}
else 
{ 
	if(current_value==previous_value)
	{
		return;
	}
	counter_overflows++;
	PCF8583_upd_previous(0x50);// Перезапись предыдущего значения счетчика текущим 
	PCF8583_upd_counter_overflows(0x50);// Перезапись счетчика переполнений counter_overflows
valdes[0]= initial_indication+(current_value*counter_overflows*10)/division_ratio; //Добавить проверку деления на ноль !!!!!!!!
}
}
}

void webfunc(char *pbuf) {
os_sprintf(HTTPBUFF,"Горячая вода: <b>%s</b> ,", fltostr(initial_indication+(current_value*counter_overflows*10)/division_ratio));

os_sprintf(HTTPBUFF,"<br>Счетчик:%d",current_value);
os_sprintf(HTTPBUFF,"<br>Предыдущее:%d",previous_value);
os_sprintf(HTTPBUFF,"<br>Начальное:%d",initial_indication);
os_sprintf(HTTPBUFF,"<br>Делитель:%d",division_ratio);
os_sprintf(HTTPBUFF,"<br>Переполнения:%d",counter_overflows);
}
