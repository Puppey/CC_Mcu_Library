

#include "onboard_led.h"


#define LED_GPIO_CLOCK						RCC_AHBPeriph_GPIOB
#define LED_GPIO_PORT             GPIOB
#define LED1_GPIO_PIN             GPIO_Pin_3
#define LED2_GPIO_PIN             GPIO_Pin_4

void LED_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd(LED_GPIO_CLOCK, ENABLE);

  GPIO_InitStructure.GPIO_Pin		= LED1_GPIO_PIN | LED2_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	
	LED1_Off();
	LED2_Off();
}
void LED1_On(void)
{
	 LED_GPIO_PORT->BSRR = LED1_GPIO_PIN;
}
void LED1_Off(void)
{
  LED_GPIO_PORT->BRR = LED1_GPIO_PIN;
}
void LED1_Toggle(void)
{
  LED_GPIO_PORT->ODR ^= LED1_GPIO_PIN;
}
void LED2_On(void)
{
	 LED_GPIO_PORT->BSRR = LED2_GPIO_PIN;
}
void LED2_Off(void)
{
  LED_GPIO_PORT->BRR = LED2_GPIO_PIN;
}
void LED2_Toggle(void)
{
  LED_GPIO_PORT->ODR ^= LED2_GPIO_PIN;
}

