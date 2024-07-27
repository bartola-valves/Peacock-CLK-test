#include <stdio.h>
#include "pico/stdlib.h"
#include <button.h>
// Map all the LEDs in the PCK PCB
#define LED_PIN 25  //on board LED
#define LED_SW1 8  // LED SW1 on GP8
#define LED_SW2 9  // LED SW2 on GP9
#define LED_OUT_7 7  // LED OUT7 on GP7
#define LED_OUT_6 6  // LED OUT7 on GP6

// Map all the Inputs

#define CLK 12  // Clock input
#define RES 13  // Reset input
#define SYNC 14 // Sync input

// Map the switches
#define SW1 10  // Switch 1
#define SW2 11  // Switch 2

// Global variable declaration here

bool latch1; // stores switch 1 latching state
bool latch2; // stores switch 2 latching state

uint read_input(uint input)
{
    return gpio_get(input);
}
void test_LED(uint LED, int time)
{
    // turn on and off the LED for time*2
    sleep_ms(time);
    gpio_put(LED, 1);
    sleep_ms(time);
    gpio_put(LED, 0);
    
}

// init_port initialises the port (either LED or jack) with direction. No logic validation of HW, be careful!
// direction is GPIO_OUT or GPIO_IN
void init_port( uint LED, bool direction)
{
    gpio_init(LED); //initialise the LED
    gpio_set_dir(LED, direction); // set the LED direction
}

void onchange(button_t *button_p) 
{
  button_t *button = (button_t*)button_p;
  
  printf("Button on pin %d changed its state to %d\n", button->pin, button->state);

  if(button->state) 
  {
    gpio_put(LED_SW1, 0);
    gpio_put(LED_SW2, 0);
    // turn off the LED when released
    return; // Ignore button release. Invert the logic if using
            // a pullup (internal or external).

  }
  switch(button->pin){
    case SW1:
    {
        printf("SW1\n");
        gpio_put(LED_SW1, 1);
        // latch state change
        if (!latch1)
            latch1=true; 
        else latch1 = false;
        printf("SW1 latch state is: %d\n", latch1);
        break;
    }
    case SW2:
    {
        printf("SW2\n");
        gpio_put(LED_SW2, 1);
        break;
    }
  }
}

void onchange2(button_t *button_p) 
{
  button_t *button = (button_t*)button_p;
  
  printf("Button on pin %d changed its state to %d\n", button->pin, button->state);

  if(button->state) 
  {
    gpio_put(LED_SW2, 0);
    // turn off the LED when released
    return; // Ignore button release. Invert the logic if using
            // a pullup (internal or external).

  }
  switch(button->pin){    
    case SW2:
    {
        printf("SW2\n");
        gpio_put(LED_SW2, 1);
        break;
    }
  }
}

void onchangeCLK(button_t *button_p) 
{
  button_t *button = (button_t*)button_p;
  
  //printf("Jack on pin %d changed its state to %d\n", button->pin, button->state);

  if(button->state) 
  {
    gpio_put(LED_SW1, 0);
    // turn off the LED when released
    return; // Ignore button release. Invert the logic if using
            // a pullup (internal or external).

  }
  switch(button->pin){    
    case CLK:
    {
        gpio_put(LED_SW1, 1);
        if (!latch1)
            latch1=true; 
        else 
            {
                latch1=false; //reseting latch1
                if  (latch2)
                    latch2=false; // latch 2 reset
                else 
                    latch2=true; //set latch 2
            }    
        //printf("CLK latch state is: %d\n", latch1);


        break;
    }
  }
}


void latch_LEDs(void)
/* very simple testing function to turn on LED when latch is active after pressing button 1
    It uses global variable latch1*/
{
    switch (latch1){
        case true:
            {
                gpio_put(LED_OUT_7, 1);
                break; 
            }
        case false:
            {
                gpio_put(LED_OUT_7, 0);
                break; 
            }    
    }
    
    switch (latch2) 
    {
        case true:
        {
            gpio_put(LED_OUT_6, 1);
            break; 
        }
        case false:
        {
            gpio_put(LED_OUT_6, 0);
            break; 
        } 
    }
}

int main()
{
    stdio_init_all();

    init_port(LED_SW1,GPIO_OUT);
    init_port(LED_SW2,GPIO_OUT);
    init_port(CLK, GPIO_IN); 

    button_t *sw1_button = create_button(SW1, onchange);
    button_t *sw2_button = create_button(SW2, onchange2);
    button_t *CLK_jack = create_button(CLK, onchangeCLK);

    init_port(LED_OUT_7, GPIO_OUT);
    init_port(LED_OUT_6, GPIO_OUT);

    latch1 = false;
    latch2 = false;
    printf("Hello, Peacock CLK testing!\n");

    while (true) {
        latch_LEDs();
 //       sleep_ms(1000); no need to to delay!
    }
}
