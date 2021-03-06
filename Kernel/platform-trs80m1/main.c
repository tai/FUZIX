#include <kernel.h>
#include <timer.h>
#include <kdata.h>
#include <printf.h>
#include <devtty.h>

uint16_t ramtop = PROGTOP;
uint8_t trs80_model;
uint8_t vtattr_cap;

struct blkbuf *bufpool_end = bufpool + NBUFS;

/* We need to spin here because we don't have interrupts for the UART on the
   model I */
void platform_idle(void)
{
  irqflags_t irq = di();
  platform_interrupt();
  irqrestore(irq);
}

void do_beep(void)
{
}

uint8_t platform_param(char *p)
{
    used(p);
    return 0;
}

/* Work around SDCC bugs */
uint8_t sdcc_bug_2753(uint8_t v) __z88dk_fastcall
{
  return v;
}

/* We assign these to dummy to deal with what I think is an SDCC bug */
void platform_interrupt(void)
{
  uint8_t irq = *((volatile uint8_t *)0x37E0);
  uint8_t dummy;

  tty_interrupt();
  kbd_interrupt();

  /* FIXME: do we care about floppy interrupts */

  if (irq & 0x40)
    dummy = sdcc_bug_2753(*((volatile uint8_t *)0x37EC));
  if (irq & 0x80) {	/* FIXME??? */
    timer_interrupt();
    dummy = sdcc_bug_2753(*((volatile uint8_t *)0x37E0));	/* Ack the timer */
  }
}

/*
 *	We can't recover discard space usefully... yet. I have a cunning plan
 *	involving external buffers in the spare bank space 8)
 */

void platform_discard(void)
{
}

#ifdef CONFIG_RTC

__sfr __at 0xB0 rtc_secl;
__sfr __at 0xB1 rtc_sech;

/* FIXME: the RTC is optional so we should test for it first */
uint8_t platform_rtc_secs(void)
{
    uint8_t sl, rv;
    /* BCD encoded */
    do {
        sl = rtc_secl;
        rv = sl + rtc_sech * 10;
    } while (sl != rtc_secl);
    return rv;
}

#endif

/*
 *	So that we don't suck in a library routine we can't use from
 *	the runtime
 */

int strlen(const char *p)
{
  int len = 0;
  while(*p++)
    len++;
  return len;
}
