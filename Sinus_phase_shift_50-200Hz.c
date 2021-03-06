#include "stm32f767xx.h"

int sin_value[90] =
{
	0,
	243595,
	9731931,
	21852399,
	38738304,
	60307379,
	86454542,
	117052407,
	151951904,
	190983006,
	233955557,
	280660200,
	330869394,
	384338525,
	440807097,
	500000000,
	561628853,
	625393407,
	690983006,
	758078104,
	826351822,
	895471537,
	965100503,
	1034899497,
	1090452846,
	1173648178,
	1241921896,
	1309016994,
	1374606593,
	1438371147,
	1500000000,
	1559192903,
	1615661475,
	1669130606,
	1719339800,
	1766044443,
	1809016994,
	1848048096,
	1882947593,
	1913545458,
	1939692621,
	1961261696,
	1978147601,
	1990268069,
	1997564050,
	2000000000,
	1997564050,
	1990268069,
	1978147601,
	1961261696,
	1939692621,
	1913545458,
	1882947593,
	1848048096,
	1809016994,
	1766044443,
	1719339800,
	1669130606,
	1615661475,
	1559192903,
	1500000000,
	1438371147,
	1374606593,
	1309016994,
	1241921896,
	1173648178,
	1104528463,
	1034899497,
	965100503,
	895471537,
	826351822,
	758078104,
	690983006,
	625393407,
	561628853,
	500000000,
	440807097,
	384338525,
	330869394,
	280660200,
	233955557,
	190983006,
	151951904,
	117052407,
	86454542,
	60307379,
	38738304,
	21852399,
	9731931,
	243595
}, i = 0, adc_value = 0, psc = 2000000000/133;

void TIM4_IRQHandler(void)
{
	TIM4->CCR3 = sin_value[i]/psc;
	++i;
	if(i == 89)
		i = 0;
	TIM4->SR &= ~TIM_SR_UIF;
}

void SysTick_Handler(void)
{
	for(int i = 0; i < 5; ++i)
	{
		ADC1->CR2 |= ADC_CR2_JSWSTART;
		adc_value += ADC1->JDR1;
	}
	TIM4->ARR = 133 + adc_value/102;
	psc = 2000000000/(133 + adc_value/102);
	adc_value = 0;
}

void init_all()
{
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4 | RCC_PLLCFGR_PLLN_7 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLSRC;
	RCC->CFGR |= RCC_CFGR_PPRE1 | RCC_CFGR_HPRE_0 | RCC_CFGR_HPRE_3;
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
	RCC->CR |= RCC_CR_HSEON;
	while((RCC->CR & RCC_CR_HSERDY) == 0);
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	RCC->CFGR |= RCC_CFGR_SW_1;
	while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_1);
	RCC->CR &= ~RCC_CR_HSION;

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_ADC1EN;

	SysTick_Config(1200000);

	GPIOB->MODER |= GPIO_MODER_MODER8_1;
	GPIOB->AFR[1] |= GPIO_AFRH_AFRH0_1;
	TIM4->ARR = 133;
	TIM4->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
	TIM4->CCER |= TIM_CCER_CC3E;
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM4->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM4_IRQn);

	GPIOA->MODER |= GPIO_MODER_MODER3;
	ADC1->JSQR |= ADC_JSQR_JSQ4_0 | ADC_JSQR_JSQ4_1;
	ADC1->CR2 |= ADC_CR2_ADON;
}

int main(void)
{
	init_all();
	while(1);
}
