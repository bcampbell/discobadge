#ifndef WS2812_CONFIG_H_
#define WS2812_CONFIG_H_

// WS2813 needs 300 µs reset time
// WS2812 and clones only need 50 µs
#define ws2812_resettime  300 
#define ws2812_port B     // Data port 
#define ws2812_pin  4     // Data out pin

#endif /* WS2812_CONFIG_H_ */
