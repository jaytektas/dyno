#include "hx711.h"
#include "status.h"

#define PERIOD(now, then) (now < then) ? (now + (0xffffffff - then) + 1) : (now - then)

HX711 hx[4];

void TIM8_CC_IRQHandler(void)
{
	if ((TIM8->SR & TIM_SR_CC1IF) != 0)
	{
		if (hx[0].bitCount < 24)
		{
			if (HAL_GPIO_ReadPin(hx[0].gpioSck, hx[0].pinSck) == GPIO_PIN_SET)
			{
				hx[0].bitCount++;
				hx[0].value = hx[0].value << 1;
				if (HAL_GPIO_ReadPin(hx[0].gpioData, hx[0].pinData) == GPIO_PIN_SET) hx[0].value++;
				HAL_GPIO_WritePin(hx[0].gpioSck, hx[0].pinSck, GPIO_PIN_RESET);
				TIM8->CCR1 = TIM8->CNT + 10;
			}
			else
			{
				HAL_GPIO_WritePin(hx[0].gpioSck, hx[0].pinSck, GPIO_PIN_SET);
				TIM8->CCR1 = TIM8->CNT + 10;
			}
		}
		else
		{
			if (hx[0].gainCount < hx[0].gain)
			{
				if (HAL_GPIO_ReadPin(hx[0].gpioData, hx[0].pinData) == GPIO_PIN_SET)
				{
					hx[0].gainCount++;
			    	HAL_GPIO_WritePin(hx[0].gpioSck, hx[0].pinSck, GPIO_PIN_RESET);
					TIM8->CCR1 = TIM8->CNT + 10;
				}
				else
				{
			    	HAL_GPIO_WritePin(hx[0].gpioSck, hx[0].pinSck, GPIO_PIN_SET);
					TIM8->CCR1 = TIM8->CNT + 10;
				}
			}
			else
			{
				status.hub[0].loadCell = hx[0].value ^ 0x800000;
				// disable timer
				TIM8->CCER &= ~TIM_CCER_CC1E;
				TIM8->DIER &= ~TIM_DIER_CC1IE;
				// enable external interrupt
				EXTI->IMR |= EXTI_IMR_MR6;
				EXTI->PR = EXTI_PR_PR6;
			}
		}
	}

	if ((TIM8->SR & TIM_SR_CC2IF) != 0)
	{

	}

	if ((TIM8->SR & TIM_SR_CC3IF) != 0)
	{

	}

	if ((TIM8->SR & TIM_SR_CC4IF) != 0)
	{

	}
}

void EXTI9_5_IRQHandler(void)
{

	if ((EXTI->PR & EXTI_PR_PR6) != 0)
	{
		hx[0].value = 0;
		hx[0].bitCount = 0;
		hx[0].gainCount = 0;

		// disable interrupt on this channel
		EXTI->IMR &= ~EXTI_IMR_MR6;
		EXTI->PR = EXTI_PR_PR6;
		// set the clock line high for reading this HX711
		TIM8->CCR1 = TIM8->CNT + 10;
		TIM8->CCER |= TIM_CCER_CC1E;
		TIM8->DIER |= TIM_DIER_CC1IE;
		// set a timer to start reading HX711
    	HAL_GPIO_WritePin(hx[0].gpioSck, hx[0].pinSck, GPIO_PIN_SET);
	}
}

void HX711_Init(HX711 data)
{
	HAL_GPIO_WritePin(data.gpioSck, data.pinSck, GPIO_PIN_SET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(data.gpioData, data.pinSck, GPIO_PIN_RESET);
}

int HX711_Average_Value(HX711 data, uint8_t times)
{
    int sum = 0;
    for (int i = 0; i < times; i++)
    {
        sum += HX711_Value(data);
    }

    return sum / times;
}

inline void delay(__volatile TIM_TypeDef* timer, uint32_t tics)
{
	uint32_t start;
	for (start = timer->CNT; PERIOD(timer->CNT, start) < tics;) __asm__("nop");
}

int HX711_Value(HX711 data)
{
    int buffer;
    buffer = 0;

    while (HAL_GPIO_ReadPin(data.gpioData, data.pinData)==1);

    for (uint8_t i = 0; i < 24; i++)
    {
    	HAL_GPIO_WritePin(data.gpioSck, data.pinSck, GPIO_PIN_SET);
        buffer = buffer << 1 ;

        delay(TIM2, 10);

        if (HAL_GPIO_ReadPin(data.gpioData, data.pinData))
        {
            buffer ++;
        }

        HAL_GPIO_WritePin(data.gpioSck, data.pinSck, GPIO_PIN_RESET);

        delay(TIM2, 10);
    }

    for (int i = 0; i < data.gain; i++)
    {
    	HAL_GPIO_WritePin(data.gpioSck, data.pinSck, GPIO_PIN_SET);

        delay(TIM2, 10);

    	HAL_GPIO_WritePin(data.gpioSck, data.pinSck, GPIO_PIN_RESET);

        delay(TIM2, 10);

    }

    buffer = buffer ^ 0x800000;

    return buffer;
}

HX711 HX711_Tare(HX711 data, uint8_t times)
{
    int sum = HX711_Average_Value(data, times);
    data.offset = sum;
    return data;
}
