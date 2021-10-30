# Default freq is 1MHz. Turn off CKDIV8 fuse to speed it up.
F_CPU = 9600000
DEVICE = attiny13

CFLAGS = -g2 -I. -Ilight_ws2812 -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) 
CFLAGS+= -Os -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions  
CFLAGS+= -Wall -Wno-pointer-to-int-cast

disco.hex: disco.c light_ws2812/light_ws2812.c
	avr-gcc $(CFLAGS) disco.c light_ws2812/light_ws2812.c -o disco.elf
	avr-objcopy -O ihex disco.elf disco.hex

flashdisco: disco.hex
	avrdude -c usbtiny -p t13 -U flash:w:disco.hex

pumpkin.hex: pumpkin.c light_ws2812/light_ws2812.c
	avr-gcc $(CFLAGS) pumpkin.c light_ws2812/light_ws2812.c -o pumpkin.elf
	avr-objcopy -O ihex pumpkin.elf pumpkin.hex

flashpumpkin: pumpkin.hex
	avrdude -c usbtiny -p t13 -U flash:w:pumpkin.hex


