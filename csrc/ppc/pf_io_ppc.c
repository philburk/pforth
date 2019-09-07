/***************************************************************
** I/O subsystem for PForth based on 'C'
**
** Author: Jordan Niethe
** Copyright 2019, Jordan Niethe, IBM Corporation
**
** The pForth software code is dedicated to the public domain,
** and any third party may reproduce, distribute and modify
** the pForth software code or any derivative works thereof
** without any compensation or license.  The pForth software
** code is provided on an "as is" basis without any warranty
** of any kind, including, without limitation, the implied
** warranties of merchantability and fitness for a particular
** purpose and their equivalents under the laws of any jurisdiction.
**
****************************************************************/

#include "../pf_all.h"

int qemu_console;               /* Set in head.S */

#define PROC_FREQ 50000000
#define UART_FREQ 115200
#define POTATO_UART_BASE 0xc0002000
#define QEMU_UART_BASE 0x60300d00103f8

#define POTATO_CONSOLE_TX		0x00
#define POTATO_CONSOLE_RX		0x08
#define POTATO_CONSOLE_STATUS		0x10
#define   POTATO_CONSOLE_STATUS_RX_EMPTY		0x01
#define   POTATO_CONSOLE_STATUS_TX_EMPTY		0x02
#define   POTATO_CONSOLE_STATUS_RX_FULL			0x04
#define   POTATO_CONSOLE_STATUS_TX_FULL			0x08
#define POTATO_CONSOLE_CLOCK_DIV	0x18
#define POTATO_CONSOLE_IRQ_EN		0x20

static uint64_t potatoUARTRegRead( int offset )
{
    uint64_t addr;
    uint64_t val;

    addr = POTATO_UART_BASE + offset;

    val = *(volatile uint64_t * ) addr;

    return val;
}

static void potatoUARTRegWrite( int offset, uint64_t val )
{
    uint64_t addr;

    addr = POTATO_UART_BASE + offset;

    *(volatile uint64_t *) addr = val;
}

static int potatoUARTRXEmpty( void )
{
    uint64_t val;

    val = potatoUARTRegRead(POTATO_CONSOLE_STATUS);

    if (val & POTATO_CONSOLE_STATUS_RX_EMPTY)
        return 1;

    return 0;
}

static int potatoUARTTXFull( void )
{
    uint64_t val;

    val = potatoUARTRegRead(POTATO_CONSOLE_STATUS);

    if (val & POTATO_CONSOLE_STATUS_TX_FULL)
        return 1;

    return 0;
}

static char potatoUARTRead( void )
{
    uint64_t val;

    val = potatoUARTRegRead(POTATO_CONSOLE_RX);

    return (char) (val & 0x000000ff);
}

static void potatoUARTWrite( char c )
{
    uint64_t val;

    val = c;

    potatoUARTRegWrite(POTATO_CONSOLE_TX, val);
}

static unsigned long
potato_uart_divisor( unsigned long proc_freq, unsigned long uart_freq )
{
    return proc_freq / (uart_freq * 16) - 1;
}


/* Values taken from skiboot */
#define REG_RBR		0
#define REG_THR		0
#define REG_DLL		0
#define REG_IER		1
#define REG_DLM		1
#define REG_FCR		2
#define REG_IIR		2
#define REG_LCR		3
#define REG_MCR		4
#define REG_LSR		5
#define REG_MSR		6
#define REG_SCR		7

#define LSR_DR		0x01    /* Data ready */
#define LSR_OE		0x02    /* Overrun */
#define LSR_PE		0x04    /* Parity error */
#define LSR_FE		0x08    /* Framing error */
#define LSR_BI		0x10    /* Break */
#define LSR_THRE	0x20    /* Xmit holding register empty */
#define LSR_TEMT	0x40    /* Xmitter empty */
#define LSR_ERR		0x80    /* Error */

#define LCR_DLAB 	0x80    /* DLL access */

#define IER_RX		0x01
#define IER_THRE	0x02
#define IER_ALL		0x0f

static void qemuUARTRegWrite( uint64_t offset, uint8_t val )
{
    uint64_t addr;

    addr = QEMU_UART_BASE + offset;

    *(volatile uint8_t *) addr = val;
}

static uint8_t qemuUARTRegRead( uint64_t offset )
{
    uint64_t addr;
    uint8_t val;

    addr = QEMU_UART_BASE + offset;

    val = *(volatile uint8_t *) addr;

    return val;
}

static int qemuUARTTXFull( void )
{
    return !(qemuUARTRegRead(REG_LSR) & LSR_THRE);
}

static int qemuUARTRXEmpty( void )
{
    return !(qemuUARTRegRead(REG_LSR) & LSR_DR);
}

static char qemuUARTRead( void )
{
    return qemuUARTRegRead(REG_THR);
}

static void qemuUARTWrite( char c )
{
    qemuUARTRegWrite(REG_RBR, c);
}

int sdTerminalOut( char c )
{
    if (qemu_console)
    {
          while (qemuUARTTXFull());
          qemuUARTWrite(c);
    }
    else
    {
          if ( c == '\n' )
		  sdTerminalOut('\r');
          while (potatoUARTTXFull());
          potatoUARTWrite(c);
    }

    return c;
}

int sdTerminalEcho( char c )
{
    if (qemu_console)
    {
          while(qemuUARTTXFull());
          qemuUARTWrite(c);
    }
    else
    {
          while (potatoUARTTXFull());
          potatoUARTWrite(c);
    }
    return 0;
}

int sdTerminalIn( void )
{
    if (qemu_console)
    {
          while (qemuUARTRXEmpty());
          return qemuUARTRead();
    }
    else
    {
          while (potatoUARTRXEmpty());
          return potatoUARTRead();
    }
}

int sdTerminalFlush( void )
{
    return -1;
}

void sdTerminalInit( void )
{
    if ( qemu_console )
    {
        ;
    }
    else
    {
          potatoUARTRegWrite(POTATO_CONSOLE_CLOCK_DIV,
                             potato_uart_divisor(PROC_FREQ,
                                                 UART_FREQ));
     }
}

void sdTerminalTerm( void )
{
    return;
}

int sdQueryTerminal( void )
{
    return 0;
}
