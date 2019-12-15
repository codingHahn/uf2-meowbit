# Demo App for Meowbit based on UF2 Bootloaders for STM32

This is a simple demo that creates an app in UF2 format for meowbit.

## Big Notice

You may get the latest compiled bootloader flasher from release page:
https://github.com/KittenBot/uf2-meowbit/releases/

or from our web site download page:
https://cdn.kittenbot.cn/meowbit/meowbit-flasher.uf2

And you may go into spi-flash mode by holding the `left button` during reset.

![](https://s2.ax1x.com/2019/02/18/k6llTg.jpg)

The internal spi-flash will mount as a thumb disk, format it or copy anything you like to it. 

## Static resource and firmware

* 12x12 unicode binary https://cdn.kittenbot.cn/meowbit/unicode12.bin
* micropython firmware https://cdn.kittenbot.cn/meowbit/meowpy.uf2

## Build instructions

To build just run ``make`` in this folder.
This will fetch git submodules if needed and build libopenocm3
(which only happens once).

The binaries will be in `build/meowbit`. 
The following files will be built:
* `bootloader.elf` - for use with JTAG adapters
* `bootloader.bin` - for direct onboard upgrading
* `flasher.uf2` - if you already have a UF2 bootloader, you can just drop this on board and it will update the bootloader
* `flasher16.uf2` - this is similar, but if you have an older UF2 bootloader on the board, that expects the app to start at 16k (`0x08004000`)

## License

License: LGPL for libopencm3, BSD for what was left of the PX4 bootloader (see LICENSE.md),
MIT for the UF2 stuff, and ISC for the bits from dapboot.

## Credits

This repo was forked from https://github.com/PX4/Bootloader

It contains small bits from https://github.com/devanlai/dapboot,
mostly the WebUSB, that has been heavily modified though.

The UF2 stuff was mostly lifted from https://github.com/Microsoft/uf2-samdx1
by way of (non-working) https://github.com/Microsoft/uf2-nrf5

The dmesg subsystem is adapted from https://github.com/lancaster-university/codal-core
