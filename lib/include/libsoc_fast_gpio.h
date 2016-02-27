#ifndef _LIBSOC_FAST_GPIO_H_
#define _LIBSOC_FAST_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int mul_sel;
	int pull;
	int drv_level;
	int data;
	char port;
	unsigned int pin;
} fast_gpio;

typedef enum {
	DIRECTION_ERROR = -1,
	INPUT = 0,
	OUTPUT = 1,
} fast_gpio_direction;

typedef enum {
	LEVEL_ERROR = -1,
	LOW = 0,
	HIGH = 1,
} fast_gpio_level;

int libsoc_fast_gpio_init();
void libsoc_fast_gpio_shutdown();

fast_gpio* libsoc_fast_gpio_request(char port, unsigned int pin);
void libsoc_fast_gpio_free(fast_gpio* gpio);

int libsoc_fast_gpio_set_direction(fast_gpio* gpio, fast_gpio_direction direction);
fast_gpio_direction libsoc_fast_gpio_get_direction(fast_gpio* gpio);

int libsoc_fast_gpio_set_level(fast_gpio* gpio, fast_gpio_level level);
fast_gpio_level libsoc_fast_gpio_get_level(fast_gpio* gpio);

#ifdef __cplusplus
}
#endif
#endif // _LIBSOC_FAST_GPIO_H_