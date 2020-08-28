/*Reaction Time Game Instructions:
 - Player1(Blue) on SW2; Player2(Red) on SW3
 - Press RESET to begin game
 - Green LED will blink 3 times to signal to players that white LED is coming
 - First player to press their button after white LED comes on wins that round and gets a point
		- Blue LED will blink if Player1 won the round
		- Red LED will blink if Player2 won the round
 - FALSE STARTS:
		- We assume that no player will false start before or during green flashes
		- After the green flashes, if a player presses their button before the white LED comes on, they have false started.
				- If Player1 false starts, Player2 gets wins the round (Red LED flashes and Player2 gets a point)
				- If Player2 false starts, Player1 gets wins the round (Blue LED flashes and Player1 gets a point)
 - First player to 3 points wins!
		- Blue LED will stay on if Player1 wins game
		- Red LED will stay on if Player2 wins game
*/
#include <fsl_device_registers.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

unsigned int Player1Won = 0;
unsigned int Player2Won = 0;
unsigned int Game_On;
unsigned int Player1Score = 0;
unsigned int Player2Score = 0;
unsigned int LED_On = 0;

void start() {
	Game_On = 1;
	
	//indicate to players LED is coming by flashing Green LED 3 times
	LEDGreen_On();
	delay();
	LED_Off();
	delay();
	LEDGreen_On();
	delay();
	LED_Off();
	delay();
	LEDGreen_On();
	delay();
	LED_Off();
	//ASSUME player will not false start during green flashes
	
	//generate random number
	int random = 0;
	//want random to be 10 and 50
	random = rand() %41 + 10;
	
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK / 10 * random; //load value between 1.0 & 5.0 sec
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TIE(1);
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN(1);
	
}

void setup() {
	LED_Initialize();
	
	//set up SW2 (IN1)
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK; //enable SW2
	PORTC->PCR[6] = PORT_PCR_MUX(001);  //select gpio
	PTC->PDDR &= ~GPIO_PDDR_PDD(1<<6);   //assign SW2 to input
	PORTC->PCR[6] |= PORT_PCR_IRQC(1001); //Connect SW2 as interrupt on rising edge
	NVIC_EnableIRQ(PORTC_IRQn); //Configure NVIC
	
	//set up SW3 (IN2)
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; //enable SW3
	PORTA->PCR[4] = PORT_PCR_MUX(001);  //select gpio
	PTA->PDDR &= ~GPIO_PDDR_PDD(1<<4);   //assign SW3 to input
	PORTA->PCR[4] |= PORT_PCR_IRQC(1001); //Connect SW3 as interrupt on rising edge
	NVIC_EnableIRQ(PORTA_IRQn); //Configure NVIC
	
	//set up timer
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; //Enable clock to PIT module
	PIT->MCR = 0;
	NVIC_EnableIRQ(PIT0_IRQn);
	
	//start game
	start();
	
}


/* interrupt handler for SW2 */
void PORTC_IRQHandler(void) {
	//Player 1 pressed
	//First check if Game is active
	if (Game_On) {
		LED_Off();
		//check for false start
		if (LED_On) {
			Player1Score++;
	
			LEDBlue_On();
			delay();
			delay();
			delay();
			LED_Off();
			delay();
			delay();
		}
		else {
			//Player1 has false started, give point to Player2
			Player2Score++;
	
			LEDRed_On();
			delay();
			delay();
			delay();
			LED_Off();
			delay();
			delay();			
		}
	
		if (Player1Score == 3) {
			//Player1 has won
			Player1Won = 1;
			LEDBlue_On();
		}
		else if (Player2Score == 3) {
			//Player2 has won
			Player2Won = 1;
			LEDRed_On();
		}
		
		LED_On = 0;
		Game_On = 0;
	}
	
	//clear interrupt flag
	PORTC->ISFR = (1 << 6);
}
	
/*interrupt handler for SW3 */
void PORTA_IRQHandler(void) {
	//Player 2 pressed
	//First check if Game is active
	if (Game_On) {
		LED_Off();
		//check for false start
		if (LED_On){
			Player2Score++;
	
			LEDRed_On();
			delay();
			delay();
			delay();
			LED_Off();
			delay();
			delay();
		}
		else {
			//Player2 has false started, give point to Player1
			Player1Score++;
	
			LEDBlue_On();
			delay();
			delay();
			delay();
			LED_Off();
			delay();
			delay();
		}
	
		if (Player2Score == 3) {
			//Player2 has won
			Player2Won = 1;
			LEDRed_On();
		}
		else if (Player1Score == 3) {
			//Player1 has won
			Player1Won = 1;
			LEDBlue_On();
		}
		
		LED_On = 0;
		Game_On = 0;
	}
	
	//Clear interrupt flag
	PORTA->ISFR = (1 << 4);
}

/* 
     PIT 0 Interrupt Handler
*/
void PIT0_IRQHandler(void)
{
	if (Player1Won == 0 && Player2Won == 0 && Game_On == 1) {
	LED_On = 1;
	LEDBlue_Toggle();
	LEDGreen_Toggle();
	LEDRed_Toggle();
	
	}
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(0);
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	
}

int main(void) {
	setup();
	
	while (Player1Won == 0 && Player2Won == 0) {
		while (Game_On); //waiting for a player to press button, don't want to start a new game
		if (Player1Won == 0 && Player2Won == 0) {
			PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN(0); //clear timer if still going from false start round
			start();
		}
	}
	
	while (1) {}
		return 0;
}

