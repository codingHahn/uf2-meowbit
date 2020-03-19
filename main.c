/*
 * STM32F4 board support for the bootloader.
 *
 */

#include "hw_config.h"
#include "bl.h"
#include <string.h>
#include "img.h"


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/scb.h>

#include <libopencmsis/core_cm3.h>

const uint8_t I2C_ADDR = 0x74;

/* flash parameters that we should not really know */
static struct {
	uint32_t	sector_number;
	uint32_t	size;
} flash_sectors[] = {

	/* Physical FLASH sector 0 is reserved for bootloader and is not
	 * the table below.
	 * N sectors may aslo be reserved for the app fw in which case
	 * the zero based define BOARD_FIRST_FLASH_SECTOR_TO_ERASE must
	 * be defined to begin the erase above of the reserved sectors.
	 * The default value of BOARD_FIRST_FLASH_SECTOR_TO_ERASE is 0
	 * and begins flash erase operations at phsical sector 1 the 0th entry
	 * in the table below.
	 * A value of 1 for BOARD_FIRST_FLASH_SECTOR_TO_ERASE would reserve
	 * the 0th entry and begin erasing a index 1 the third physical sector
	 * on the device.
	 *
	 * When BOARD_FIRST_FLASH_SECTOR_TO_ERASE is defined APP_RESERVATION_SIZE
	 * must also be defined to remove that additonal reserved FLASH space
	 * from the BOARD_FLASH_SIZE. See APP_SIZE_MAX below.
	 */

	{0x01, 16 * 1024},
	{0x02, 16 * 1024},
	{0x03, 16 * 1024},
	{0x04, 64 * 1024},
	{0x05, 128 * 1024},
	{0x06, 128 * 1024},
	{0x07, 128 * 1024},
	{0x08, 128 * 1024},
	{0x09, 128 * 1024},
	{0x0a, 128 * 1024},
	{0x0b, 128 * 1024},
	/* flash sectors only in 2MiB devices */
	{0x10, 16 * 1024},
	{0x11, 16 * 1024},
	{0x12, 16 * 1024},
	{0x13, 16 * 1024},
	{0x14, 64 * 1024},
	{0x15, 128 * 1024},
	{0x16, 128 * 1024},
	{0x17, 128 * 1024},
	{0x18, 128 * 1024},
	{0x19, 128 * 1024},
	{0x1a, 128 * 1024},
	{0x1b, 128 * 1024},
};
#define BOOTLOADER_RESERVATION_SIZE	(16 * 1024)

#define OTP_BASE			0x1fff7800
#define OTP_SIZE			512
#define UDID_START		        0x1FFF7A10

// address of MCU IDCODE
#define DBGMCU_IDCODE		0xE0042000
#define STM32_UNKNOWN	0
#define STM32F40x_41x	0x413
#define STM32F42x_43x	0x419
#define STM32F42x_446xx	0x421

#define REVID_MASK	0xFFFF0000
#define DEVID_MASK	0xFFF

#ifndef BOARD_PIN_VBUS
# define BOARD_PIN_VBUS                 GPIO9
# define BOARD_PORT_VBUS                GPIOA
# define BOARD_CLOCK_VBUS               RCC_AHB1ENR_IOPAEN
#endif

/* magic numbers from reference manual */

typedef enum mcu_rev_e {
	MCU_REV_STM32F4_REV_A = 0x1000,
	MCU_REV_STM32F4_REV_Z = 0x1001,
	MCU_REV_STM32F4_REV_Y = 0x1003,
	MCU_REV_STM32F4_REV_1 = 0x1007,
	MCU_REV_STM32F4_REV_3 = 0x2001
} mcu_rev_e;

typedef struct mcu_des_t {
	uint16_t mcuid;
	const char *desc;
	char  rev;
} mcu_des_t;

// The default CPU ID  of STM32_UNKNOWN is 0 and is in offset 0
// Before a rev is known it is set to ?
// There for new silicon will result in STM32F4..,?
mcu_des_t mcu_descriptions[] = {
	{ STM32_UNKNOWN,	"STM32F???",    '?'},
	{ STM32F40x_41x, 	"STM32F40x",	'?'},
	{ STM32F42x_43x, 	"STM32F42x",	'?'},
	{ STM32F42x_446xx, 	"STM32F446XX",	'?'},
};

char serial_number[32];
#define STM32_UUID ((uint32_t *)UDID_START)

unsigned bootFlag = 0;

static void initSerialNumber()
{
	writeHex(serial_number, STM32_UUID[0]);
	writeHex(serial_number+8, STM32_UUID[1]);
	writeHex(serial_number+16, STM32_UUID[2]);
}


typedef struct mcu_rev_t {
	mcu_rev_e revid;
	char  rev;
} mcu_rev_t;

/* context passed to cinit */
#if INTERFACE_USB
# define BOARD_INTERFACE_CONFIG_USB  	NULL
#endif

/* board definition */
struct boardinfo board_info = {
	.board_rev	= 0,

#ifdef STM32F401
	.systick_mhz	= 84,
#else
	.systick_mhz	= 168,
#endif
};

static void board_init(void);

#define BOOT_RTC_SIGNATURE          0x71a21877
#define APP_RTC_SIGNATURE           0x24a22d12
#define POWER_DOWN_RTC_SIGNATURE    0x5019684f // Written by app fw to not re-power on.
#define HF2_RTC_SIGNATURE           0x39a63a78
#define SLEEP_RTC_ARG               0x10b37889
#define SLEEP2_RTC_ARG              0x7e3353b7

#define BOOT_RTC_REG                MMIO32(RTC_BASE + 0x50)
#define ARG_RTC_REG                MMIO32(RTC_BASE + 0x54)

/* standard clocking for all F4 boards */
static struct rcc_clock_scale clock_setup = {
	.pllm = 0,
	.plln = 336,
#if defined(STM32F401)
	.pllp = 4,
	.pllq = 7,
	.pllr = 0,
	.hpre = RCC_CFGR_HPRE_DIV_NONE,
	.ppre1 = RCC_CFGR_PPRE_DIV_2,
	.ppre2 = RCC_CFGR_PPRE_DIV_NONE,
	.flash_config = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_2WS,
	.ahb_frequency  = 84000000,
#else
	.pllp = 2,
	.pllq = 7,
#if defined(STM32F446) || defined(STM32F469)
	.pllr = 2,
#endif
	.hpre = RCC_CFGR_HPRE_DIV_NONE,
	.ppre1 = RCC_CFGR_PPRE_DIV_4,
	.ppre2 = RCC_CFGR_PPRE_DIV_2,
	.flash_config = FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_5WS,
#endif
	.voltage_scale = PWR_SCALE1,
	.apb1_frequency = 42000000,
	.apb2_frequency = 84000000,
};

static uint32_t
board_get_rtc_signature(uint32_t *arg)
{
	/* enable the backup registers */
	PWR_CR |= PWR_CR_DBP;
	RCC_BDCR |= RCC_BDCR_RTCEN;

	uint32_t result = BOOT_RTC_REG;
	if (arg)
		*arg = ARG_RTC_REG;

	/* disable the backup registers */
	RCC_BDCR &= RCC_BDCR_RTCEN;
	PWR_CR &= ~PWR_CR_DBP;

	return result;
}

void
board_set_rtc_signature(uint32_t sig, uint32_t arg)
{
	/* enable the backup registers */
	PWR_CR |= PWR_CR_DBP;
	RCC_BDCR |= RCC_BDCR_RTCEN;

	BOOT_RTC_REG = sig;
	ARG_RTC_REG = arg;

	/* disable the backup registers */
	RCC_BDCR &= RCC_BDCR_RTCEN;
	PWR_CR &= ~PWR_CR_DBP;
}

static bool
board_test_force_pin()
{
#if defined(BOARD_FORCE_BL_PIN_IN) && defined(BOARD_FORCE_BL_PIN_OUT)
	/* two pins strapped together */
	volatile unsigned samples = 0;
	volatile unsigned vote = 0;

	for (volatile unsigned cycles = 0; cycles < 10; cycles++) {
		gpio_set(BOARD_FORCE_BL_PORT, BOARD_FORCE_BL_PIN_OUT);

		for (unsigned count = 0; count < 20; count++) {
			if (gpio_get(BOARD_FORCE_BL_PORT, BOARD_FORCE_BL_PIN_IN) != 0) {
				vote++;
			}

			samples++;
		}

		gpio_clear(BOARD_FORCE_BL_PORT, BOARD_FORCE_BL_PIN_OUT);

		for (unsigned count = 0; count < 20; count++) {
			if (gpio_get(BOARD_FORCE_BL_PORT, BOARD_FORCE_BL_PIN_IN) == 0) {
				vote++;
			}

			samples++;
		}
	}

	/* the idea here is to reject wire-to-wire coupling, so require > 90% agreement */
	if ((vote * 100) > (samples * 90)) {
		return true;
	}

#endif
#if defined(BOARD_FORCE_BL_PIN)
	/* single pin pulled up or down */
	volatile unsigned samples = 0;
	volatile unsigned vote = 0;

	for (samples = 0; samples < 200; samples++) {
		if ((gpio_get(BOARD_FORCE_BL_PORT, BOARD_FORCE_BL_PIN) ? 1 : 0) == BOARD_FORCE_BL_STATE) {
			vote++;
		}
	}

	/* reject a little noise */
	if ((vote * 100) > (samples * 90)) {
		return true;
	}

#endif
	return false;
}


static void
board_init(void)
{
	RCC_APB1ENR |= RCC_APB1ENR_PWREN;
	RCC_APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// enable all GPIO clocks
	RCC_AHB1ENR |= RCC_AHB1ENR_IOPAEN|RCC_AHB1ENR_IOPBEN|RCC_AHB1ENR_IOPCEN|BOARD_CLOCK_VBUS;

	// make sure JACDAC line is up, otherwise trashes the bus
	setup_input_pin(CFG_PIN_BTN_LEFT); // use left to detect bootloader mode

	setup_input_pin(CFG_PIN_JACK_TX);

	setup_output_pin(CFG_PIN_LED);
	setup_output_pin(CFG_PIN_LED1);

	setup_output_pin(CFG_PIN_JACK_SND);


	initSerialNumber();

	start_systick();
}

static void initSpi(){
    // spi2 pin pb12~15
    //rcc_periph_clock_enable(RCC_GPIOB);
    //rcc_periph_clock_enable(RCC_SPI2);
    //setup_output_pin(CFG_PIN_FLASH_CS);
    //pin_set(CFG_PIN_FLASH_CS, 1);

    //gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13 | GPIO14 | GPIO15);
    //gpio_set_af(GPIOB, GPIO_AF5, GPIO13 | GPIO14 | GPIO15);

    //spi_reset(SPI2);
    //spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_4, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
    //                SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
    //spi_enable_software_slave_management(SPI2);
    //spi_set_nss_high(SPI2);
    //spi_enable(SPI2);
    DMESG("SPI2 init");
}

static void initI2C(){
	rcc_periph_clock_enable(RCC_I2C1);
	rcc_periph_clock_enable(RCC_GPIOB);
	

	i2c_peripheral_disable(I2C1);
	i2c_reset(I2C1);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6 | GPIO7);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
	i2c_set_speed(I2C1, i2c_speed_sm_100k, 16);
	/* HSI is at 8Mhz */
	i2c_peripheral_enable(I2C1);

}

/*
 * Inspiried by the Adafruit lib for the IS31FL3731
 * basically a C port minus a few functions
 */
#define SCROLL_BANK_FUNCTIONREG 0x0B // page 'nine'
#define SCROLL_REG_SHUTDOWN 0x0A
#define SCROLL_REG_CONFIG 0x00
#define SCROLL_REG_CONFIG_PICTUREMODE 0x00
#define SCROLL_REG_PICTURE_FRAME 0x01
#define SCROLL_CMD_REG 0xFD

static void select_bank(uint8_t bank)
{
	uint8_t data[2];
	data[0] = SCROLL_CMD_REG;
	data[1] = bank;
	i2c_transfer7(I2C1, I2C_ADDR, data, 2, NULL, 0);
}

static void write_register8(uint8_t bank, uint8_t reg, uint8_t data)
{
	select_bank(bank);
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = data;

	i2c_transfer7(I2C1, I2C_ADDR, buf, 2, NULL, 0);
}

uint8_t reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

static void clear(){
	for(uint8_t frame = 0; frame < 8; frame++)
	{
		uint8_t charcount = 0;
		uint8_t row = 0;
		for(uint8_t i = 0; i <= 0x11; i++)
		{
			if(i % 5 == 0 && i != 0)
				charcount++;
			if(row % 8 == 0)
				row = 0;
			write_register8(frame, i, 0x00);
		}
	}
}

void show(uint8_t* buffer, uint8_t length){
	uint8_t tmp;
	uint8_t result[length];
	result[0] = buffer[0];
	for(uint8_t i = 1; i < 9; i++)
	{
		result[2*i] = buffer[i];
	}
	for(uint8_t i = 9; i < 17; i++)
	{
		result[((i-9)*2)+1] = buffer[i];
	}
	buffer = result;
	//swap(&buffer);
	for(uint8_t i = 0; i < length; i++)
	{
		if(i % 2 != 0)
			buffer[i] = reverse(buffer[i])>>1;
		else{
			if(i < 7){
				tmp = buffer[i];
				buffer[i] = buffer[16-i];
				buffer[16-i] = tmp;
			}
		}
	}
	write_register8(SCROLL_BANK_FUNCTIONREG, SCROLL_REG_SHUTDOWN, 0x00);
	delay(10);
	write_register8(SCROLL_BANK_FUNCTIONREG, SCROLL_REG_SHUTDOWN, 0x01);
	delay(10);

	write_register8(SCROLL_BANK_FUNCTIONREG, SCROLL_REG_CONFIG, SCROLL_REG_CONFIG_PICTUREMODE);

	write_register8(SCROLL_BANK_FUNCTIONREG,  SCROLL_REG_PICTURE_FRAME, 0x00);
	clear();
	for(uint8_t frame = 0; frame < 8; frame++)
	{
		uint8_t charcount = 0;
		uint8_t column = 0;
		for(uint8_t i = 0; i <= 0x11; i++)
		{
			write_register8(frame, i, buffer[i]); //CHACRACTERS[charcount][column]);
		}
	}
}
/**
  * @brief  Initializes the RCC clock configuration.
  *
  * @param  clock_setup : The clock configuration to set
  */
static inline void
clock_init(void)
{
	uint32_t pllm = BOOT_SETTINGS->hseValue / 1000000;
	if (pllm < 4 || pllm > 60 || pllm * 1000000 != BOOT_SETTINGS->hseValue)
		pllm = OSC_FREQ;
	clock_setup.pllm = pllm;
	rcc_clock_setup_pll(&clock_setup);
}

void
led_on(unsigned led)
{
	switch (led) {
	case LED_ACTIVITY:
		pin_set(CFG_PIN_LED, 1);
		break;

	case LED_BOOTLOADER:
		pin_set(CFG_PIN_LED1, 1);
		break;
	}
}

void
led_off(unsigned led)
{
	switch (led) {
	case LED_ACTIVITY:
		pin_set(CFG_PIN_LED, 0);
		break;

	case LED_BOOTLOADER:
		pin_set(CFG_PIN_LED1, 0);
		break;
	}
}

/* we should know this, but we don't */
#ifndef SCB_CPACR
# define SCB_CPACR (*((volatile uint32_t *) (((0xE000E000UL) + 0x0D00UL) + 0x088)))
#endif

#define PWR_CR_LPLVDS (1 << 10)

void playTone()
{
	for (int i=0;i<100;i++)
	{
		pin_set(CFG_PIN_JACK_SND, 1);
		delay(1);
		pin_set(CFG_PIN_JACK_SND, 0);
		delay(1);
	}	
}
int main(void)
{
#define CHARACTER_ARRAY_WIDTH 	5
#define CHARACTER_ARRAY_HEIGHT 	7
#define MAX_CHAR_LENGTH 30

	uint8_t A[CHARACTER_ARRAY_WIDTH] = {0b00011111, 0b00100100, 0b01000100, 0b00100100, 0b00011111};
	uint8_t B[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00001111};
	uint8_t C[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b01000001, 0b01000001, 0b01000001, 0b01000001};
	uint8_t D[CHARACTER_ARRAY_WIDTH] = {0b00001111, 0b00001001, 0b00001001, 0b00001001, 0b01111111};
	uint8_t E[CHARACTER_ARRAY_WIDTH] = {0b01111110, 0b01001001, 0b01001001, 0b01001001, 0b01001001};
	uint8_t F[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b01001000, 0b01001000, 0b01001000, 0b01001000};
	uint8_t G[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b01000001, 0b01000001, 0b01001001, 0b01001111};
	uint8_t H[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111};
	uint8_t I[CHARACTER_ARRAY_WIDTH] = {0b00000000, 0b01000001, 0b01111111, 0b01000001, 0b00000000};
	uint8_t J[CHARACTER_ARRAY_WIDTH] = {0b00000111, 0b00000001, 0b00000001, 0b00000001, 0b01111111};
	uint8_t K[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b00001000, 0b00011000, 0b00100100, 0b01000011};
	uint8_t L[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b00000001, 0b00000001, 0b00000001, 0b00000001};
	uint8_t M[CHARACTER_ARRAY_WIDTH] = {0b00001111, 0b00001000, 0b00001111, 0b00001000, 0b00001111};
	uint8_t N[CHARACTER_ARRAY_WIDTH] = {0b00011111, 0b00001000, 0b00001000, 0b00001000, 0b00001111};
	uint8_t O[CHARACTER_ARRAY_WIDTH] = {0b00011111, 0b00010001, 0b00010001, 0b00010001, 0b00011111};
	uint8_t P[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b01001000, 0b01001000, 0b01001000, 0b01111000};
	uint8_t Q[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b01000001, 0b01000011, 0b01111111, 0b00000001};
	uint8_t R[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b01001100, 0b01001010, 0b01001001, 0b01111000};
	uint8_t S[CHARACTER_ARRAY_WIDTH] = {0b00000000, 0b00011101, 0b00010101, 0b00010111, 0b00000000};
	uint8_t T[CHARACTER_ARRAY_WIDTH] = {0b01100000, 0b01000000, 0b01111111, 0b01000000, 0b01100000};
	uint8_t U[CHARACTER_ARRAY_WIDTH] = {0b01111111, 0b00000001, 0b00000001, 0b00000001, 0b01111111};
	uint8_t V[CHARACTER_ARRAY_WIDTH] = {0b01111000, 0b00000110, 0b00000001, 0b00000110, 0b01111000};
	uint8_t W[CHARACTER_ARRAY_WIDTH] = {0b00001111, 0b00000001, 0b00001111, 0b00000001, 0b00001111};
	uint8_t X[CHARACTER_ARRAY_WIDTH] = {0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011};
	uint8_t Y[CHARACTER_ARRAY_WIDTH] = {0b01100000, 0b00010000, 0b00001111, 0b00010000, 0b01100000};
	uint8_t Z[CHARACTER_ARRAY_WIDTH] = {0b00000000, 0b00011001, 0b00010101, 0b00010011, 0b00000000};

	char* input_string = "abcdefghijklmopqrstuvwxyz";
	uint8_t len = (strlen(input_string) + 17) *6;
	uint8_t text_buffer[len];
	memset(text_buffer, 0, len);
	uint8_t *varptr = text_buffer;

	for(uint8_t i = 0; i < strlen(input_string); i++, varptr+=6){
		switch(input_string[i]){
		case 'a':
			memcpy(varptr, A, sizeof(uint8_t) * 5);
			break;
		case 'b':
			memcpy(varptr, B, sizeof(uint8_t) * 5);
			break;
		case 'c':
			memcpy(varptr, C, sizeof(uint8_t) * 5);
			break;
		case 'd':
			memcpy(varptr, D, sizeof(uint8_t) * 5);
			break;
		case 'e':
			memcpy(varptr, E, sizeof(uint8_t) * 5);
			break;
		case 'f':
			memcpy(varptr, F, sizeof(uint8_t) * 5);
			break;
		case 'g':
			memcpy(varptr, G, sizeof(uint8_t) * 5);
			break;
		case 'h':
			memcpy(varptr, H, sizeof(uint8_t) * 5);
			break;
		case 'i':
			memcpy(varptr, I, sizeof(uint8_t) * 5);
			break;
		case 'j':
			memcpy(varptr, J, sizeof(uint8_t) * 5);
			break;
		case 'k':
			memcpy(varptr, K, sizeof(uint8_t) * 5);
			break;
		case 'l':
			memcpy(varptr, L, sizeof(uint8_t) * 5);
			break;
		case 'm':
			memcpy(varptr, M, sizeof(uint8_t) * 5);
			break;
		case 'n':
			memcpy(varptr, N, sizeof(uint8_t) * 5);
			break;
		case 'o':
			memcpy(varptr, O, sizeof(uint8_t) * 5);
			break;
		case 'p':
			memcpy(varptr, P, sizeof(uint8_t) * 5);
			break;
		case 'q':
			memcpy(varptr, Q, sizeof(uint8_t) * 5);
			break;
		case 'r':
			memcpy(varptr, R, sizeof(uint8_t) * 5);
			break;
		case 's':
			memcpy(varptr, S, sizeof(uint8_t) * 5);
			break;
		case 't':
			memcpy(varptr, T, sizeof(uint8_t) * 5);
			break;
		case 'u':
			memcpy(varptr, U, sizeof(uint8_t) * 5);
			break;
		case 'v':
			memcpy(varptr, V, sizeof(uint8_t) * 5);
			break;
		case 'w':
			memcpy(varptr, W, sizeof(uint8_t) * 5);
			break;
		case 'x':
			memcpy(varptr, X, sizeof(uint8_t) * 5);
			break;
		case 'y':
			memcpy(varptr, Y, sizeof(uint8_t) * 5);
			break;
		case 'z':
			memcpy(varptr, Z, sizeof(uint8_t) * 5);
			break;
		default:
			memset(varptr, 0, sizeof(uint8_t) * 5);
		}
	}
	varptr = text_buffer;


	/* Enable the FPU before we hit any FP instructions */
	SCB_CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 Full Access and set CP11 Full Access */

	/* do board-specific initialisation */
	board_init();

	/* configure the clock for bootloader activity */
	clock_init();

	initSpi();
	initI2C();

	screen_init();
	//draw_drag();
	for(uint8_t i = 0; i < len-17*6; i++, varptr++){
		show(varptr, 17);
		delay(300);

	}

	//playTone();

	// if they hit reset the second time, go to app
	board_set_rtc_signature(APP_RTC_SIGNATURE, 0);
	board_set_rtc_signature(0, 0);
	int x=0;
	int y=0;
	bool right=true;
	bool down=true;
	//light_led_matrix();
	uint8_t data[17] = {0};
	memcpy(data, A, sizeof(uint8_t)*5);
	memcpy(data+6, B, sizeof(uint8_t)*5);
	memcpy(data+12, C, sizeof(uint8_t)*5);
	show(data, 17);
	while (1) {
		if (x==0)
			right=true;
		else if (x==100)
		    right=false;
		
		if (y==0)
			down=true;
		else if (y==80)
		    down=false;

		if (right)
		{
			//drawImage(x,0,45,45,emptyImg);
			drawImage(++x,0,45,45,hamImg);
		}
		else
		{ 
			//drawImage(x,0,45,45,emptyImg);
			drawImage(--x,0,45,45,hamImg);
		}
		if (down)
		{ 
			//drawImage(0,y,45,45,emptyImg);
			drawImage(0,++y,45,45,hamImg);
		}
		else
		{ 
			//drawImage(0,y,45,45,emptyImg);
			drawImage(0,--y,45,45,hamImg);
		}
		
		//led_on(1);
		//led_off(2);
		//delay(100);
		//led_off(1);
		//led_on(2);
		//delay(1);
	}
}

