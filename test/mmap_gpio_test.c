#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libsoc_mmap_gpio.h"

int main()
{
	int ret = -1;

	ret = libsoc_mmap_gpio_init();
	if (ret != 0)
	{
		printf("Fail to init GPIO memory map");
		goto fail;
	}

	mmap_gpio *gpio_output = libsoc_mmap_gpio_request('E', 4);
	if (gpio_output == NULL)
	{
		printf("Faile to open GPIO\n");
		goto fail;
	}

	libsoc_mmap_gpio_set_direction(gpio_output, OUTPUT);
	int  i;
	for (i = 0; i < 1000; ++i)
	{
		libsoc_mmap_gpio_set_level(gpio_output, HIGH);
		sleep(3);
		libsoc_mmap_gpio_set_level(gpio_output, LOW);
		sleep(3);
	}

	ret = 0;

	fail:
	if (gpio_output)
	{
		libsoc_mmap_gpio_free(gpio_output);
	}
	libsoc_mmap_gpio_shutdown();

	return ret;
}