

#include "usart.h"
#include "stdarg.h"
#include "stdio.h"


//========================================================================
// ����: void USART1_init(u8 F,u32	BPS)��
// ����: ����ͨ�ų�ʼ������
// ����: F��ϵͳʱ��Ƶ�ʣ�BPS�����ڲ����ʣ�
// ����: ��
// �汾: V1.0, 2013-7-36
// ���ߣ�����
//========================================================================
void USART1_init(u32 BaudRate)
{
	
	USART_InitTypeDef USART1_Struct; 
	GPIO_InitTypeDef  GPIO_Struct;
//	NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);			//ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

//-----------USART1_Tx == PB6-------------//
  GPIO_Struct.GPIO_Pin		= GPIO_Pin_6;
  GPIO_Struct.GPIO_Mode		= GPIO_Mode_AF;
  GPIO_Struct.GPIO_OType 	= GPIO_OType_PP;
  GPIO_Struct.GPIO_PuPd		= GPIO_PuPd_UP;
  GPIO_Struct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_7);		//����PB2ΪUSART1_Tx
  GPIO_Init(GPIOB, &GPIO_Struct);

//-----------USART1_Rx == PB7-------------//
//  GPIO_Struct.GPIO_Pin		= GPIO_Pin_7;
//  GPIO_Struct.GPIO_Mode		= GPIO_Mode_IN;
//  GPIO_Struct.GPIO_OType 	= GPIO_OType_PP;
//  GPIO_Struct.GPIO_PuPd		= GPIO_PuPd_UP;
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_7);		//����PB3ΪUSART1_Rx
//  GPIO_Init(GPIOB, &GPIO_Struct);
	
  USART1_Struct.USART_BaudRate						= BaudRate;												//������
  USART1_Struct.USART_WordLength					= USART_WordLength_8b;						//����λ
  USART1_Struct.USART_StopBits						= USART_StopBits_1;								//ֹͣλ
  USART1_Struct.USART_Parity							= USART_Parity_No ;								//����λ
  USART1_Struct.USART_Mode								= USART_Mode_Tx ;//| USART_Mode_Rx;	//ģʽ
  USART1_Struct.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;	//������
	USART_Init(USART1, &USART1_Struct);	//��������USART

	USART_Cmd(USART1, ENABLE);					//����USRAT1

//	NVIC_InitStructure.NVIC_IRQChannel										= 	USART1_IRQn;	//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 	0;						//��ռ���ȼ�2 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 	2;						//�����ȼ�2 
//	NVIC_InitStructure.NVIC_IRQChannelCmd 								= 	ENABLE;				//ʹ���ⲿ�ж�ͨ��
//	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	USART_ClearFlag(USART1,USART_FLAG_TC);	//���USARTx�Ĵ������־λ,�����һ���ַ���ʧ
}

//========================================================================
// ����: void USART_send(u8 F,u32 V)��
// ����: ����2�������ݺ���
// ����: p��ָ�����
// ����: ��
// �汾: V1.0, 2013-7-36
// ���ߣ�����
//========================================================================
void USART1_TxSend(u8 Data)		
{
	u16 i = 0;
	USART_SendData(USART1, Data);							//��������
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)//�ȴ����ͳɹ�
	{
		i++;
		if(i > 255)
		{
			USART_ClearFlag(USART1,USART_FLAG_TC);		//���USARTx�Ĵ������־λ
			return;
		}		
	}
}
//========================================================================
// ����: void UART_PrintString(unsigned char code *puts)
// ����: ����2����һ���ַ�����
// ����: *puts: Ҫ���͵��ַ�ָ��.
// ����: none.
// �汾: V1.0, 2013-7-36
// ���ߣ�����
//========================================================================
void USART1_PrintString(u8 *puts, u16 length)
{
	while(length--)
	{
		USART1_TxSend(*puts++);
	}
}
//========================================================================
// ����: USART1_IRQHandler(void)
// ����: ����1�����жϷ�����
// ����: ��
// ����: ��
// �汾: V1.0, 2013-7-36
// ���ߣ�����
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
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART1_printf()����
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
 * ��������USART1_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���1����USART1
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART1_printf( USART1, "\r\n this is a demo \r\n" );
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

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
          }
					Data++;
          break;

        case 'd':										//ʮ����
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







