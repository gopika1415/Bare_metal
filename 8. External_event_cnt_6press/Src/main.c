#include <stdint.h>

uint32_t count = 0;

void ms_delay_tim(uint32_t n){
	uint32_t *control_tim4 = (uint32_t*)0x40000800;//tim4
	uint32_t *status_register = (uint32_t*)0x40000810;
	uint32_t *prescalar = (uint32_t*)0x40000828;
	uint32_t *ARR = (uint32_t*)0x4000082C;

	//uint32_t *RCC_APB1 = (uint32_t*)0x40023840;
	//*RCC_APB1 |= (1<<0);
	*prescalar = 16000-1;
	*ARR = 1000-1;
	*control_tim4 |=(1<<0);
	uint32_t *EGR = (uint32_t*)0x40000814;
	*EGR |= (1<<0);

	for(uint32_t i=0;i<n;i++){
		while(!(*status_register & (1<<0)));
		*status_register &= ~(1<<0);
	}

}


int main(void)
{

	uint32_t *clockregA = (uint32_t*)0x40023830;
	*clockregA |=(1<<0);

	uint32_t *RCC_APB1 = (uint32_t*)0x40023840;
	*RCC_APB1 |= (1<<0); //tim2 enable for push button
	*RCC_APB1 |= (1<<2); //tim4 enable for delay

	uint32_t *moderregA = (uint32_t*)0x40020000;
	*moderregA &=~(3<<0);
	//Altenate fn mode
	*moderregA |=(1<<1);
    *moderregA &=~(1<<0);

    uint32_t *afrlA = (uint32_t*)0x40020020;
    *afrlA &=~(15<<0);
	*afrlA &=~(1<<3);
	*afrlA &=~(1<<2);
	*afrlA &=~(1<<1);
	*afrlA |=(1<<0);

    // pull-down input push button
    uint32_t *pupdrA = (uint32_t*)0x4002000C;
    *pupdrA &= ~(3<<0);
    *pupdrA |=  (2<<0);

	uint32_t *tim2_cr = (uint32_t*)0x40000000;//tim2

	uint32_t *tim2_smcr = (uint32_t*)0x40000008;//tim2 slave mode
	//sms111 done4
    *tim2_smcr &=~(7<<0);
	*tim2_smcr |=(1<<2);
	*tim2_smcr |=(1<<1);
	*tim2_smcr |=(1<<0);

	uint32_t *tim2_ccmr = (uint32_t*)0x40000018;//tim2 capture compare done1
	// configure channel1 for input
	*tim2_ccmr &= ~(3<<0);   // clear CC1S bits
	*tim2_ccmr |=(1<<0);

	// filter  1111: fSAMPLING=fDTS/16, N=6
	*tim2_ccmr |= (1<<7);
	*tim2_ccmr |= (1<<6);
	*tim2_ccmr |= (1<<5);
	*tim2_ccmr |= (1<<4);

	uint32_t *tim2_ccer = (uint32_t*)0x40000020;//tim2 capture compare enable
	//capture on rising edge
	*tim2_ccer &=~(1<<1);//cc1p done2

	*tim2_ccer |= (1<<0);   // CC1E enable ---> Here only capturing is start

	//TS=101 done 5
	*tim2_smcr &=~(7<<4);
	*tim2_smcr |=(1<<6);
	*tim2_smcr &=~(1<<5);
	*tim2_smcr |=(1<<4);

	//configure count5
	uint32_t *tim2_cnt = (uint32_t*)0x40000024;
	*tim2_cnt = 0;  //done6

	*tim2_cr |=(1<<0);//counter enable done 7

	//configure LED pd12
    uint32_t *clockreg = (uint32_t*)0x40023830;
    uint32_t *moderreg = (uint32_t*)0x40020C00;
	uint32_t *odrreg =(uint32_t*)0x40020C14;

	*clockreg |=(1<<3);

	*moderreg |=(1<<24);
	*moderreg &=~(1<<25);



   while(1){

	// wait until 6 presses occur
	while(*tim2_cnt < 6){
		count = *tim2_cnt;
	}

	count = *tim2_cnt;

	for(uint32_t i = 0; i < count; i++)
	{
	    *odrreg |= (1<<12);
	    ms_delay_tim(2);

	    *odrreg &= ~(1<<12);
	    ms_delay_tim(2);
	}
	*tim2_cnt=0;
   }

}
