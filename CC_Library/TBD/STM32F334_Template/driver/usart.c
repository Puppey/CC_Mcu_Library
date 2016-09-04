

#include "usart.h"
#include "stdarg.h"
#include "stdio.h"


//========================================================================
// 函数: void USART1_init(u8 F,u32	BPS)；
// 描述: 串口通信初始化函数
// 参数: F：系统时钟频率，BPS：串口波特率；
// 返回: 无
// 版本: V1.0, 2013-7-36
// 作者：何鹏
//========================================================================
void USART1_init(u32 BaudRate)
{
	
	USART_InitTypeDef USART1_Struct; 
	GPIO_InitTypeDef  GPIO_Struct;
//	NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);			//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

//-----------USART1_Tx == PB6-------------//
  GPIO_Struct.GPIO_Pin		= GPIO_Pin_6;
  GPIO_Struct.GPIO_Mode		= GPIO_Mode_AF;
  GPIO_Struct.GPIO_OType 	= GPIO_OType_PP;
  GPIO_Struct.GPIO_PuPd		= GPIO_PuPd_UP;
  GPIO_Struct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_7);		//复用PB2为USART1_Tx
  GPIO_Init(GPIOB, &GPIO_Struct);

//-----------USART1_Rx == PB7-------------//
//  GPIO_Struct.GPIO_Pin		= GPIO_Pin_7;
//  GPIO_Struct.GPIO_Mode		= GPIO_Mode_IN;
//  GPIO_Struct.GPIO_OType 	= GPIO_OType_PP;
//  GPIO_Struct.GPIO_PuPd		= GPIO_PuPd_UP;
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_7);		//复用PB3为USART1_Rx
//  GPIO_Init(GPIOB, &GPIO_Struct);
	
  USART1_Struct.USART_BaudRate						= BaudRate;												//波特率
  USART1_Struct.USART_WordLength					= USART_WordLength_8b;						//数据位
  USART1_Struct.USART_StopBits						= USART_StopBits_1;								//停止位
  USART1_Struct.USART_Parity							= USART_Parity_No ;								//检验位
  USART1_Struct.USART_Mode								= USART_Mode_Tx ;//| USART_Mode_Rx;	//模式
  USART1_Struct.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	//流控制
	USART_Init(USART1, &USART1_Struct);	//按需配置USART

	USART_Cmd(USART1, ENABLE);					//启动USRAT1

//	NVIC_InitStructure.NVIC_IRQChannel										= 	USART1_IRQn;	//使能按键所在的外部中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 	0;						//抢占优先级2 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 	2;						//子优先级2 
//	NVIC_InitStructure.NVIC_IRQChannelCmd 								= 	ENABLE;				//使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	USART_ClearFlag(USART1,USART_FLAG_TC);	//清除USARTx的待处理标志位,解决第一个字符丢失
}

//========================================================================
// 函数: void USART_send(u8 F,u32 V)；
// 描述: 串口2发送数据函数
// 参数: p：指针参数
// 返回: 无
// 版本: V1.0, 2013-7-36
// 作者：何鹏
//========================================================================
void USART1_TxSend(u8 Data)		
{
	u16 i = 0;
	USART_SendData(USART1, Data);							//发送数据
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)//等待发送成功
	{
		i++;
		if(i > 255)
		{
			USART_ClearFlag(USART1,USART_FLAG_TC);		//清除USARTx的待处理标志位
			return;
		}		
	}
}
//========================================================================
// 函数: void UART_PrintString(unsigned char code *puts)
// 描述: 串口2发送一串字符串。
// 参数: *puts: 要发送的字符指针.
// 返回: none.
// 版本: V1.0, 2013-7-36
// 作者：何鹏
//========================================================================
void USART1_PrintString(u8 *puts, u16 length)
{
	while(length--)
	{
		USART1_TxSend(*puts++);
	}
}
//========================================================================
// 函数: USART1_IRQHandler(void)
// 描述: 串口1接收中断服务函数
// 参数: 无
// 返回: 无
// 版本: V1.0, 2013-7-36
// 作者：何鹏
//========================================================================
void USART1_IRQHandler(void)
{

}








int fputc(int ch, FILE *f);

int fputc(int ch, FILE *f)
{
	USART1_TxSend((u8)ch);	
	return ch;
}


/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART1_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */
/*
 * 函数名：USART1_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *            		 USART1_printf( USART1, "\r\n %d \r\n", i );
 *            		 USART1_printf( USART1, "\r\n %s \r\n", j );
 */
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}
/*			 [] END OF FILE					*/







