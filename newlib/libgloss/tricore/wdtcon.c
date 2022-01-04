/*  wdtcon.c -- Lock/unlock the ENDINIT bit in WDTCON to gain write access
                for ENDINIT-protected SFRs (such as BIV, BTV and the CLC
                registers of the various core functional blocks).

    Copyright (C) 2000 - 2013 HighTec EDV-Systeme GmbH.

    This file is part of GCC.

    GCC is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3, or (at your option)
    any later version.

    GCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    Under Section 7 of GPL version 3, you are granted additional
    permissions described in the GCC Runtime Library Exception, version
    3.1, as published by the Free Software Foundation.

    You should have received a copy of the GNU General Public License and
    a copy of the GCC Runtime Library Exception along with this program;
    see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
    <http://www.gnu.org/licenses/>.  */


#if defined(__TC161__) || defined(__TC162__)

#define  _DSYNC() __asm__ volatile ("dsync" ::: "memory")
#if !defined (__MFCR)
#undef  __STRINGIFY
#define __STRINGIFY(x)    #x
#define __MFCR(regaddr) __tric_mfcr (regaddr)
#define __tric_mfcr(regaddr)                                            \
  (__extension__({                                                      \
      unsigned __res;                                                   \
      __asm__ volatile ("mfcr %0, LO:" __STRINGIFY (regaddr)            \
                        : "=d" (__res) :: "memory");                    \
      __res;                                                            \
    }))
#endif /* __MFCR */

typedef struct
{
    unsigned int _con0;
} WdtCon_t;


/* on the AURIX (TC27XX) and AURIX 2G (TC39XX) each core has its own WDTCON address
   therefore we need special read/write-function to the wdtcon0/1
   registers
   */
#define CORE_ID_ADDR            0xFE1C
#define CORE_ID_MASK            0x00000007
#if defined(__TC161__)
#define WDTCON_BASE_ADDR        0xF0036100
#define WDTCON_CPU_STRIDE       0x0C
/* safety watchdog */
#define WDTS_BASE_ADDR          0xF00360F0
#elif defined(__TC162__)
#define WDTCON_BASE_ADDR        0xF003624C
#define WDTCON_CPU_STRIDE       0x0C
/* safety watchdog */
#define WDTS_BASE_ADDR          0xF00362A8
#endif /* __TC161__ */

static inline WdtCon_t * __getWdtBase(void)
{
    unsigned int coreId;

    /* get the core ID from CPU_ID (csfr_base + 0xfe1c) */
    coreId = __MFCR(CORE_ID_ADDR) & CORE_ID_MASK;

#if defined(__TC162__)
    /* special mapping for CPU5: CORE_ID == 6 */
    if (coreId == 6)
    {
        coreId = 5;
    }
#endif /* __TC162__ */

    return (WdtCon_t *)(WDTCON_BASE_ADDR + (coreId * WDTCON_CPU_STRIDE));
}

static inline WdtCon_t * __getWdtsBase(void)
{
    return (WdtCon_t *)(WDTS_BASE_ADDR);
}


/* Unlock (reset) the ENDINIT bit.  */
void unlock_wdtcon(void)
{
    unsigned int passwd;
    volatile WdtCon_t *wdtaddr;

    wdtaddr = __getWdtBase();

    passwd  = wdtaddr->_con0;
    passwd &= 0xffffff00;
    wdtaddr->_con0 = passwd | 0xf1;
    _DSYNC();
    wdtaddr->_con0 = passwd | 0xf2;
    /* read back new value ==> synchronise LFI */
    passwd = wdtaddr->_con0;
}

/* Lock (set) the ENDINIT bit.  */
void lock_wdtcon(void)
{
    unsigned int passwd;
    volatile WdtCon_t *wdtaddr;

    wdtaddr = __getWdtBase();

    passwd  = wdtaddr->_con0;
    passwd &= 0xffffff00;
    wdtaddr->_con0 = passwd | 0xf1;
    _DSYNC();
    passwd |= 3;
    wdtaddr->_con0 = passwd | 0xf2;
    /* read back new value ==> synchronise LFI */
    passwd = wdtaddr->_con0;
}

/* Unlock (reset) the SAFETY ENDINIT bit.  */
void unlock_safety_wdtcon(void)
{
    unsigned int passwd;
    volatile WdtCon_t *wdtaddr;

    wdtaddr = __getWdtsBase();

    passwd  = wdtaddr->_con0;
    passwd &= 0xffffff00;
    wdtaddr->_con0 = passwd | 0xf1;
    _DSYNC();
    wdtaddr->_con0 = passwd | 0xf2;
    /* read back new value ==> synchronise LFI */
    passwd = wdtaddr->_con0;
}

/* Lock (set) the SAFETY ENDINIT bit.  */
void lock_safety_wdtcon(void)
{
    unsigned int passwd;
    volatile WdtCon_t *wdtaddr;

    wdtaddr = __getWdtsBase();

    passwd  = wdtaddr->_con0;
    passwd &= 0xffffff00;
    wdtaddr->_con0 = passwd | 0xf1;
    _DSYNC();
    passwd |= 3;
    wdtaddr->_con0 = passwd | 0xf2;
    /* read back new value ==> synchronise LFI */
    passwd = wdtaddr->_con0;
}

#else

typedef struct
{
    unsigned int _con0;
    unsigned int _con1;
} WdtCon_t;

static inline WdtCon_t * __getWdtBase(void)
{
  WdtCon_t *addr;

  __asm__ volatile ("movh.a     %0, HI:$wdtcon0\n\t"
                    "lea        %0, [%0] LO:$wdtcon0"
                    : "=a" (addr));
  return addr;
}


/* Unlock (reset) the ENDINIT bit.  */

void
unlock_wdtcon (void)
{
  unsigned int wcon0, wcon1;
  volatile WdtCon_t *wdtaddr;

  wdtaddr = __getWdtBase();

  wcon0 = wdtaddr->_con0;
  wcon1 = wdtaddr->_con1;
  wcon0 &= 0xffffff03;
  wcon0 |= 0xf0;
  wcon0 |= (wcon1 & 0xc);
  wcon0 ^= 0x2;
  wdtaddr->_con0 = wcon0;
  wcon0 &= 0xfffffff0;
  wcon0 |= 0x2;
  wdtaddr->_con0 = wcon0;
  /* read back new value ==> synchronise LFI */
  (void) wdtaddr->_con0;
}

/* Lock (set) the ENDINIT bit.  */

void
lock_wdtcon (void)
{
  unsigned int wcon0, wcon1;
  volatile WdtCon_t *wdtaddr;

  wdtaddr = __getWdtBase();

  wcon0 = wdtaddr->_con0;
  wcon1 = wdtaddr->_con1;
  wcon0 &= 0xffffff03;
  wcon0 |= 0xf0;
  wcon0 |= (wcon1 & 0xc);
  wcon0 ^= 0x2;
  wdtaddr->_con0 = wcon0;
  wcon0 &= 0xfffffff0;
  wcon0 |= 0x3;
  wdtaddr->_con0 = wcon0;

  /* read back new value ==> synchronise LFI */
  (void) wdtaddr->_con0;
}

/* These functions are taken from the TriCore Application Note AP321201.pdf.
   WDT_Passwd() is essentially another implementation of unlock_wdtcon(),
   while WDT_Modify() allows to set or reset certain bits in WDTCON0 after
   it was unlocked with WDT_Modify().  */

void
WDT_Passwd (void)
{
  unsigned int passwd;
  volatile WdtCon_t *wdtaddr;

  wdtaddr = __getWdtBase();

  passwd = wdtaddr->_con0 | 0xf0;
  passwd = (wdtaddr->_con1 & 0xc) | (passwd & 0xfffffff3);
  wdtaddr->_con0 = passwd ^ 0x2;
  /* read back new value ==> synchronise LFI */
  (void) wdtaddr->_con0;
}

void
WDT_Modify (unsigned long modify, unsigned long mask)
{
  unsigned int newvalue;
  volatile WdtCon_t *wdtaddr;

  wdtaddr = __getWdtBase();

  newvalue = wdtaddr->_con0 & ~mask;
  newvalue |= (modify & mask);
  newvalue = (newvalue | 0xf2) & 0xfffffff3;
  wdtaddr->_con0 = newvalue;
  /* read back new value ==> synchronise LFI */
  (void) wdtaddr->_con0;
}
#endif /* __TC161__ || __TC162__ */
