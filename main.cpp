
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <array>
#include <vector>

#include "pico/stdlib.h"
#include "hardware/spi.h"

#define ILI9341_RESET 8
#define ILI9341_DC 9
#define ILI9341_SCK 10
#define ILI9341_MOSI 11
#define ILI9341_MISO 12
#define ILI9341_CS 13

#define ILI9341_SPI_PORT spi1

class ILI9341LCD {
public:
	static constexpr int32_t WIDTH = 320;
	static constexpr int32_t HEIGHT = 240;
private:
	void initialize_io()
	{
		spi_init(ILI9341_SPI_PORT, 50 * 1000 * 1000);
		spi_set_format(ILI9341_SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
		gpio_set_function(ILI9341_SCK, GPIO_FUNC_SPI);
		gpio_set_function(ILI9341_MOSI, GPIO_FUNC_SPI);
		gpio_set_function(ILI9341_MISO, GPIO_FUNC_SPI);

		gpio_init(ILI9341_CS);
		gpio_set_dir(ILI9341_CS, GPIO_OUT);
		cs(1);

		gpio_init(ILI9341_DC);
		gpio_set_dir(ILI9341_DC, GPIO_OUT);

		gpio_init(ILI9341_RESET);
		gpio_set_dir(ILI9341_RESET, GPIO_OUT);

		reset();
	}
	void initialize_device(void)
	{
		write_command(0x01);
		sleep_ms(50);
		write_command(0x11);
		sleep_ms(50);

		write_command(0xb6);
		write_data16(0x0ac2);
		write_command(0x36);
		write_data8(0x68);
		write_command(0x3a);
		write_data8(0x55);

		write_command(0x29);
	}
	void reset()
	{
		gpio_put(ILI9341_RESET, 1);
		sleep_ms(1);
		gpio_put(ILI9341_RESET, 0);
		sleep_ms(10);
		gpio_put(ILI9341_RESET, 1);
		sleep_ms(120);
	}
	void dc(bool f)
	{
		gpio_put(ILI9341_DC, f);
	}
	void cs(bool f)
	{
		gpio_put(ILI9341_CS, f);
	}
	void write_command(uint8_t cmd)
	{
		dc(0);
		cs(0);
		spi_write_blocking(ILI9341_SPI_PORT, &cmd, 1);
		cs(1);
	}
	void write_data8(uint8_t data)
	{
		dc(1);
		cs(0);
		spi_write_blocking(ILI9341_SPI_PORT, &data, 1);
		cs(1);
	}
	void write_data16(uint16_t data)
	{
		uint8_t tmp[2];
		tmp[0] = data >> 8;
		tmp[1] = data;
		dc(1);
		cs(0);
		spi_write_blocking(ILI9341_SPI_PORT, tmp, 2);
		cs(1);
	}
	void write_data(const uint8_t *ptr, int32_t len)
	{
		dc(1);
		cs(0);
		spi_write_blocking(ILI9341_SPI_PORT, ptr, len);
		cs(1);
	}
	void read_data(uint8_t cmd, uint8_t *ptr, int32_t len)
	{
		dc(0);
		cs(0);
		spi_write_blocking(ILI9341_SPI_PORT, &cmd, 1);

		gpio_put(ILI9341_DC, 1);
		spi_read_blocking(ILI9341_SPI_PORT, 0, ptr, len);
		cs(1);
	}
	void set_area(int32_t x, int32_t y, int32_t w, int32_t h)
	{
		write_command(0x2a);
		write_data16(x);
		write_data16(x + w - 1);
		write_command(0x2b);
		write_data16(y);
		write_data16(y + h - 1);
	}
public:
	ILI9341LCD() {}
	~ILI9341LCD() {}
	void initialize()
	{
		initialize_io();
		initialize_device();
		set_area(0, 0, WIDTH, HEIGHT);
	}
	void finalize()
	{
		spi_deinit(ILI9341_SPI_PORT);
	}
	static uint16_t color(uint8_t r, uint8_t g, uint8_t b)
	{
		return ((r << 8) & 0xf800) | ((g << 3) & 0x07e0) | ((b >> 3) & 0x001f);
	}
	void clear(uint16_t color)
	{
		fill_rect(0, 0, WIDTH, HEIGHT, color);
	}
	void draw_pixel(int32_t x, int32_t y, uint16_t color)
	{
		set_area(x, y, 1, 1);
		write_command(0x2c);
		write_data16(color);
	}
	void fill_rect(int x, int y, int w, int h, uint16_t color)
	{
		set_area(x, y, w, h);
		write_command(0x2c);
		for (int32_t i = 0; i < w * h; i++) {
			write_data16(color);
		}
	}
	void draw_buffer(int x, int y, int w, int h, uint16_t const *p)
	{
		set_area(x, y, w, h);
		write_command(0x2c);
		for (int i = 0; i < w * h; i++) {
			write_data16(p[i]);
		}
	}
	void draw_line(int x0, int y0, int x1, int y1, uint16_t color)
	{
		int dx = abs(x1 - x0);
		int sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0);
		int sy = y0 < y1 ? 1 : -1;
		int e = dx + dy;
		while (1) {
			draw_pixel(x0, y0, color);
			if (x0 == x1 && y0 == y1) break;
			int e2 = 2 * e;
			if (e2 >= dy) {
				e += dy;
				x0 += sx;
			}
			if (e2 <= dx) {
				e += dx;
				y0 += sy;
			}
		}
	}
	void test()
	{
		int x0 = rand() % 320;
		int y0 = rand() % 240;
		int x1 = rand() % 320;
		int y1 = rand() % 240;
		int r = rand() % 256;
		int g = rand() % 256;
		int b = rand() % 256;
		draw_line(x0, y0, x1, y1, color(r, g, b));
	}
};


ILI9341LCD *lcd;


extern "C" int main()
{
	lcd = new ILI9341LCD();
	lcd->initialize();
	lcd->clear(ILI9341LCD::color(0, 0, 0));

	while (1) {
		lcd->test();
	}

	return 0;
}

