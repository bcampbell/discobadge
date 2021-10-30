#ifndef F_CPU
#error //F_CPU not set
#endif
 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "light_ws2812/light_ws2812.h"

#define NPIX 8
struct cRGB led[NPIX];

uint8_t tick = 0;
uint8_t mode = 0;
uint8_t butt = 0;   // button (count how many frames it's been pressed down)

const struct cRGB rainbowTable[8] PROGMEM = {
    {255,0,0},      // r
    {255,128,0},    // o
    {128,255,0},    // y
    {0,255,0},      // g
    {0,128,255},    // light b
    {0,0,255},      // b
    {128,0,255},    // i
    {200,0,100},    // v
};

void clr();
void cylon();
void cycle( const struct cRGB* tab);
void pulse();
void fadeum();
void wander();
void sparkles();

// by darsie,
// https://www.avrfreaks.net/forum/tiny-fast-prng
uint8_t rnd() {
        static uint8_t s=0xaa,a=0;
        s^=s<<3;
        s^=s>>5;
        s^=a++>>2;
        return s;
}

void show() {
    ws2812_sendarray((uint8_t *)led, NPIX*3);
}

int main(void)
{
	DDRB = _BV(ws2812_pin);
    //DDRB &= ~(1<<PB4);    // inputs all
    PORTB |= (1<<PB3);  // enable internal pullup on pb3

    clr();
    show();
    while(1) {
        ++tick;
        if ((PINB & (1<<PB3)) ==0) {
            butt++;
            if (butt == 5 ) {
                mode++;
                tick = 0;
                clr();
                if (mode>4) {
                    mode = 0;
                }
            }
        } else {
            butt = 0;
        }
        switch (mode) {
            case 0: cylon(); break;
            case 1: cycle(rainbowTable); break;
            case 2: sparkles(); break;
            case 3: fadeum(); break;
            case 4: wander(); break;
            default:
                break;
        }
        show();
        _delay_ms(20);
    }
}

void clr()
{
    for( uint8_t i=0; i<NPIX; ++i) {
        led[i].r=0;
        led[i].g=0;
        led[i].b=0;
    }
}



void cylon()
{
    uint8_t i = tick & 0x2f;
    if( i<32) {
        i = (tick&0x1f);
    } else {
        i = 32-(tick&0x1f);
    }

    i = i/4;

    clr();
    led[i].r=32;
    led[(NPIX-1)-i].g = 32;
}

void cycle( const struct cRGB* tab) {
    for (uint8_t i=0; i<NPIX; ++i) {
        uint8_t idx = (i+(tick>>3)) & 0x07;
        led[i].r = pgm_read_byte(&(tab[idx].r)) >> 4;
        led[i].g = pgm_read_byte(&(tab[idx].g)) >>4;
        led[i].b = pgm_read_byte(&(tab[idx].b)) >>4;
    }
}


void pulse() {
    for (uint8_t i=0; i<NPIX; ++i) {
        uint16_t f = tick;//&0x0f;
        f *= f;
        led[i].r = (f>>10);    //&0xf;
        led[i].g = 0;
        led[i].b = 0;
    }
}

const uint8_t sparkleTable[16] PROGMEM = {
    255,200,220,128,100,150,96,80,
    70,60,50,42,34,12,5,0
};
uint8_t targ = 0;
void sparkles() {
   if (tick > 31) {
       targ = rnd() & 0x7;
       clr();
       tick=0;
   }
   uint8_t idx = tick;
   uint8_t f = 0;
   if( idx<16) {
    f = pgm_read_byte(&sparkleTable[idx]);
   }
   led[targ].r = f/4;
   led[targ].g = f/4;
   led[targ].b = f/4;
   //targ = (targ+1) & 0x7;
}

void fadeum() {
    uint8_t* p = (uint8_t*)led;
    for (uint8_t i=0; i<NPIX*3; ++i) {
        if (*p > 0) {
            *p = *p - 1;
        }
        ++p;
    }
    if (tick > 8) {
        targ = rnd() & 0x7;
        led[targ].r = 0;//rnd() & 0x1f;
        led[targ].g = 0;//rnd() & 0x1f;
        led[targ].b = 32;//rnd() & 0x1f;
        tick=0;
    }
}




void wander() {
    //if ((tick &0x7) !=0) {
    //    return;
    //}
    if(rnd() > 32) {
        ++targ;
    } else {
        --targ;
    }
    targ = targ & 0x7;


    uint8_t c = rnd();
    uint8_t* dest;
    if (c < 110) {
        dest = &(led[targ].r);
    } else if (c < 220) {
        dest = &(led[targ].b);
    } else {
        return;
        //dest = &(led[targ].g);
    }

    if( (tick & 0x3f) > 0x20) {
        if( *dest <32) {++(*dest);}
    } else {
        if( *dest>0) {--(*dest);}
    }
    
}




/*
ISR(PCINT1_vect)
{
    PCMSK1 = 0; // don't need more interrupts
}
*/
