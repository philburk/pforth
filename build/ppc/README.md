# pForth for Bare Metal POWER

This lets pForth run on bare metal POWER. You can run it in QEMU, the microwatt
simulator with ghdl or microwatt on Xilinx FPGA with potato UART.

## Building

The steps for building pForth are:
    1. Build pForth for the host
    2. Use host binary to create an embeddable dictionary
    3. Build pForth for POWER and include the dictionary

You will need a compiler for POWER. To build run:

    $ make

If you need to specify the path to your POWER compiler run:

    $ X=/your/path/powerpc64-linux-gnu- make

Building will produce the pforth.bin file which can be used
QEMU or microwatt

To run in QEMU use:

    $ ./qemu-system-ppc64 -M powernv -cpu POWER9 -m 512M -nographic \
    -bios ./pforth.bin

## Caveats
    * If running in the microwatt simulator you will need to make sure the
    potato UART is modelled and hooked up

    * In microwatt you need to increase the space for the simulation ram in
    `core_tb.vhdl` to `2097152`.

    * pForth allocates a new dictionary at run time and copies the embedded
    dictionary there. This is too slow on microwatt so the embedded dictionary
    is used directly. SDAD_NAMES_EXTRA and SDAD_CODE_EXTRA in fth/savedicd.fth
    need to be set to allocate space for new words.
