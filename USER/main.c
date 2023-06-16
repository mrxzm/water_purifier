#include "stm32f10x.h"
#include "core_cm3.h"
// #include <stdio.h>


uint32_t time_count = 0; // 计时秒
uint32_t run_time = 0; // 总运行时间秒
uint32_t run_wait_time = 0; // 运行等待时间
uint32_t model = 0; // 当前运行状态 0 停止 1 即将正常制水 2即将清洗模式 3休眠模式 4低压开关故障 5正在制水 6正在清洗

//微秒级的延时
void delay_us(uint32_t delay_us)
{
  volatile unsigned int num;
  volatile unsigned int t;


  for (num = 0; num < delay_us; num++)
  {
    t = 11;
    while (t != 0)
    {
      t--;
    }
  }
}

void delay_ms(uint16_t delay_ms)
{
  volatile unsigned int num;
  for (num = 0; num < delay_ms; num++)
  {
    delay_us(1000);
  }
}

// pc13 led 睡眠的时候把他关了
void led_init13(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

// 引脚 10 到19 不要用，ADC引脚接5v会烧

/*高压开关初始化函数*/
void KEY_Init4(void)
{
	//1.打开控制GPIOA的时钟(APB2)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	//2.配置结构体	
	GPIO_InitTypeDef key_init;
	key_init.GPIO_Pin   = GPIO_Pin_4;      	//GPIO 5引脚
	key_init.GPIO_Mode  = GPIO_Mode_IPU; 	//GPIO_Mode_IPU上拉输入1
	
	//3.对成员进行初始化
	GPIO_Init(GPIOB, &key_init);
}

/*低压开关初始化函数*/
void KEY_Init6(void)
{
	//1.打开控制GPIOA的时钟(APB2)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//2.配置结构体	
	GPIO_InitTypeDef key_init;
	key_init.GPIO_Pin   = GPIO_Pin_6;      	//GPIO 6引脚
	key_init.GPIO_Mode  = GPIO_Mode_IPU; 	//下拉输入
	
	//3.对成员进行初始化
	GPIO_Init(GPIOB, &key_init);
}



// 进水电磁阀 7
void valve_init7(void)//对继电器初始化，即对PB7口初始化
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
// 冲洗电磁阀8
void valve_init8(void)
{
	//打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//配置结构体
	GPIO_InitTypeDef relay_init;
	relay_init.GPIO_Pin   = GPIO_Pin_8;			//引脚
	relay_init.GPIO_Mode  = GPIO_Mode_Out_PP; 	//推挽输出
	relay_init.GPIO_Speed = GPIO_Speed_50MHz;
	
	//成员初始化
	GPIO_Init(GPIOB, &relay_init);
}

// 废水电磁阀9
void valve_init9(void)
{
	//打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//配置结构体
	GPIO_InitTypeDef relay_init;
	relay_init.GPIO_Pin   = GPIO_Pin_9;			//引脚
	relay_init.GPIO_Mode  = GPIO_Mode_Out_PP; 	//推挽输出
	relay_init.GPIO_Speed = GPIO_Speed_50MHz;
	
	//成员初始化
	GPIO_Init(GPIOB, &relay_init);
}

//PB13 和PB14 是水泵
void pump_init13(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef gpio13;
	gpio13.GPIO_Pin		=	GPIO_Pin_13;
	gpio13.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio13.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio13);
}
void pump_init14(void)
{
	// 使能 GPIOB 时钟
  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef gpio14;
	gpio14.GPIO_Pin		=	GPIO_Pin_14;
	gpio14.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio14.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio14);
}

// 定时器
void time3_init(void)
{
	// 使能 TIM3 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	NVIC_InitTypeDef NVIC_InitTypeDef;
	
	// 配置 TIM3 为定时器模式
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 10000 - 1; // 自动重载值
	TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1; // 分频器，72MHz / 7200 = 10KHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	// 配置 TIM3 的中断，并使能更新中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_InitTypeDef.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitTypeDef.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitTypeDef.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitTypeDef.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitTypeDef);

	// 启动 TIM3
	TIM_Cmd(TIM3, ENABLE);
}

// 定时器中段入口函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		time_count ++;
		 TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
	}
}

// 定时器复位
void time_reset(void)
{
	// 停止计数器
	TIM_Cmd(TIM3, DISABLE);
	time_count = 0;
	// 重新启动计数器，定时 15 秒
	TIM_SetCounter(TIM3, 0);
	TIM_Cmd(TIM3, ENABLE);
	
}


//////////////////////////////////////////////////////////////////////////////////////

void Usart1_Init(uint32_t bound)
{
  /*开启USART1、GPIOA时钟USART1*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	
	
	USART_Cmd(USART1,ENABLE); //使能USART1
	USART_ClearFlag(USART1,USART_FLAG_TC);//清除发送溢出标志位
}
void put_s(uint8_t *s)  //发送字符串函数
{
	while(*s)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
		USART_SendData(USART1,*s);
		s++;
	}
}
 ////////////////////////////////////////////////////////////

// 运行制水模式
void run(void)
{
	// 开启进水阀 
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	
	// 关闭回流阀 & 废水阀
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	model = 1;
}
// 运行清洗模式
void run_clean(void)
{
	// 关闭进水阀 
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	// 开启回流阀 & 废水阀 & 水泵1 & 水泵2
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	model = 2;
}
// 运行抽水泵
void run_pump(void)
{
	// & 水泵1 & 水泵2
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
}
// 停止水泵
void stop_pump(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
}
// 停止模式
void stop(void)
{
	// 关闭回流阀 & 废水阀& 进水阀 & 水泵1 & 水泵2
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	model = 0;
}
// 睡眠模式 等待高压开关唤醒
void sleep(void)
{
	// 停止计数器
	TIM_Cmd(TIM3, DISABLE);
	model = 3;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);  //使能 PWR 外设时钟
	

// 使能AFIO和EXTI时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4;      	//GPIO 5引脚
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU; 	//GPIO_Mode_IPU上拉输入1
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//默认即可
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	// 将PB4映射到EXTI4中断线
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	
	// 配置EXTI4中断线为下降沿触发
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	// 使能EXTI4中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	// 禁止所有外设时钟
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, DISABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1, DISABLE);

	// 配置唤醒源为EXTI4中断
	PWR_WakeUpPinCmd(ENABLE);
	PWR_ClearFlag(PWR_FLAG_WU);
	EXTI_ClearITPendingBit(EXTI_Line4);
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	PWR_WakeUpPinCmd(ENABLE);                 //使能唤醒管脚功能
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
	// PWR_EnterSTANDBYMode();                 //进入待命（STANDBY）模式
}

void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line4);//清除中断函数的标志位，否则程序将一直卡在中断函数中
	}
}



int main(void)
{
	Usart1_Init(9600);  //波特率
	
	
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	// 初始化
	led_init13();
	KEY_Init4(); // 高压开关 压力高时断开 关水
	KEY_Init6(); // 低压开关 压力低时断开  
	valve_init7(); // 4分原进水电磁阀
	valve_init8(); // 3分水桶进水电磁阀
	valve_init9(); // 废水电磁阀
	pump_init13(); // 水泵1
	pump_init14(); // 水泵2
	//GPIO_SetBits(GPIOA,GPIO_Pin_5);		//按键设置高电平
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	time3_init();
	// 开机初始化机器关闭所有操作
	GPIO_SetBits(GPIOB,GPIO_Pin_4); // 开关默认关闭状态
	GPIO_SetBits(GPIOB,GPIO_Pin_6); // 开关默认关闭
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	stop();
	
	while(1)
	{
		// 水泵控制
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6) == 0 && (model == 1 || model == 2))
		{
			run_pump();
			time_reset();
			model += 4;
		}
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6) != 0 && (model == 5 || model == 6) )
		{
			stop_pump();
			model -= 4;
			run_time += time_count;
			time_reset();
		}
		
		// 制水模式控制 当前处于 0停止状态 2准备清洗 3睡眠模式 6正在清洗时 都停止启动准备制水模式
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 0 && (model == 0 || model == 2 || model == 3 || model == 6))
		{
			delay_ms(20); //消抖再次判断是否打开水龙头 
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) == 0  )
			{
				// 运行制水模式
				put_s("1\r\n");
				run();
				time_reset();
				run_wait_time = 0;
			}
		}
		else if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4) != 0 && model == 5)
		{
			put_s("0\r\n");
			stop_pump();
			stop();
			run_time += time_count;
			time_reset();
		}
		else if(model == 1)
		{
			run_wait_time = time_count;
			// 5秒后低压开关仍然未导通 判断为低压开关故障
			if(run_wait_time >= 5){
				stop_pump();
				stop();
				time_reset();
				run_wait_time = 0;
				model = 4;
			}
		}
		
		//清洗模式
		if(model == 0)
		{
			// 当处于停止模式时查看
			if(time_count == 1800) //60 1分钟 1800 30分钟 2400 40分钟 
			{
				run_clean();
				time_reset();
			}
		}
		else if(model == 6)
		{
			// 当前清洗模式 15秒后关闭机器 后睡眠
			if(time_count > 15)
			{
				stop_pump();
				stop();
				time_count = 0;
				delay_ms(20);
				GPIO_SetBits(GPIOC,GPIO_Pin_13);
				sleep();
				// 在唤醒事件发生后，需要重新开启USART1和GPIOA口时钟
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);//重新失能PWR	
				SystemInit();	//退出停止模式要重新初始化总线时钟
				Usart1_Init(9600);
				time3_init();
				GPIO_ResetBits(GPIOC,GPIO_Pin_13);
				model = 0;
			}
		}
		
		// 低压开关故障模式
		if(model == 4)
		{
			// led 1秒一次闪烁
			if(time_count % 2 == 0)
			{
				GPIO_SetBits(GPIOC,GPIO_Pin_13);
			}
			else
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			}
			// 退出
			if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6) == 0){
				model = 0;
			}
		}
		
	}
		return 1;
}
