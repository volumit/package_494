/* PowerPC Z42xn3 user include file.
   Copyright (C) 2011 HighTec EDV Systeme GmbH
   Contributed by Horst Lehser (Horst.Lehser@hightec-rt.com).
*/

#ifndef _ZEN_H
#define _ZEN_H


#define __debug()       __asm__ volatile ("se_illegal"::: "memory");
#define __nop()       __asm__ volatile ("nop"::: "memory");

/* Access Device Control Register */
#define __mfdcr(dcnr)       ({ uint32_t ret; __asm__ volatile("mfdcr %0, %1":"=r" (ret): "i" (dcnr)); ret;} )
#define __mtdcr(dcnr,value) __asm__ volatile("mtdcr %0, %1":: "i" (dcnr), "r" (value))

/* Debug APU */
/* Debugger Notify Halt */
#define __e_dnh(dui)    __asm__ volatile ("e_dnh %0":: "i" (dui & 0x1f))
#define __se_dnh()      __asm__ volatile ("se_dnh")

/* Debugger Notify Interrupt */
#define __e_dni(dui)    __asm__ volatile ("e_dni %0":: "i" (dui & 0x1f))
#define __se_dni()      __asm__ volatile ("se_dni")

/* WAIT APU */
/* Wait for Interrupt */
#define __wait()        __asm__ volatile ("wait")

/* Decorated Storage APU */

/* Load Byte with Decoration Indexed */
static uint8_t inline __lbdx(uint32_t dc, uint8_t *m)
{
  uint8_t ret;
  __asm__ volatile ("lbdx\t%0,%1,%2": "=r" (ret): "r" (dc), "r" (m));
  return ret;
}

/* Load Halfword with Decoration Indexed */
static uint16_t inline __lhdx(uint32_t dc, uint16_t *m)
{
  uint16_t ret;
  __asm__ volatile ("lhdx\t%0,%1,%2": "=r" (ret): "r" (dc), "r" (m));
  return ret;
}

/* Load Word with Decoration Indexed */
static uint32_t inline __lwdx(uint32_t dc, uint32_t *m)
{
  uint32_t ret;
  __asm__ volatile ("lwdx\t%0,%1,%2": "=r" (ret): "r" (dc), "r" (m));
  return ret;
}

/* Store Byte with Decoration Indexed */
static void inline __stbdx(uint8_t val, uint32_t dc, uint8_t *m)
{
  __asm__ volatile ("stbdx\t%0,%1,%2": : "r" (val), "r" (dc), "r" (m) :
                    "memory");
}

/* Store Halfword with Decoration Indexed */
static void inline __sthdx(uint16_t val, uint32_t dc, uint16_t *m)
{
  __asm__ volatile ("sthdx\t%0,%1,%2": : "r" (val), "r" (dc), "r" (m) :
                    "memory");
}

/* Store Word with Decoration Indexed */
static void inline __stwdx(uint32_t val, uint32_t dc, uint32_t *m)
{
  __asm__ volatile ("stwdx\t%0,%1,%2": : "r" (val), "r" (dc), "r" (m) :
                    "memory");
}

/* Decorated Storage Notify */
static void inline __dsn(uint32_t dc, uint8_t *m)
{
  __asm__ volatile ("dsn\t%0,%1": : "r" (dc), "r" (m));
}

/* Cache Bypass Storage APU */
/* Cache Bypass with Decoration Instructions */

/* Load Byte with Decoration Cached Bypassed Indexed */
static uint8_t inline __lbdcbx(uint32_t dc, uint8_t *m)
{
  uint8_t ret;
  __asm__ volatile ("lbdcbx\t%0,%1,%2": "=r" (ret): "r" (dc), "r" (m));
  return ret;
}

/* Load Halfword with Decoration Cached Bypassed Indexed */
static uint16_t inline __lhdcbx(uint32_t dc, uint16_t *m)
{
  uint16_t ret;
  __asm__ volatile ("lhdcbx\t%0,%1,%2": "=r" (ret): "r" (dc), "r" (m));
  return ret;
}

/* Load Word with Decoration Cached Bypassed Indexed */
static uint32_t inline __lwdcbx(uint32_t dc, uint32_t *m)
{
  uint32_t ret;
  __asm__ volatile ("lwdcbx\t%0,%1,%2": "=r" (ret): "r" (dc), "r" (m));
  return ret;
}

/* Store Byte with Decoration Cached Bypassed Indexed */
static void inline __stbdcbx(uint8_t val, uint32_t dc, uint8_t *m)
{
  __asm__ volatile ("stbdcbx\t%0,%1,%2": : "r" (val), "r" (dc), "r" (m) :
                    "memory");
}

/* Store Halfword with Decoration Cached Bypassed Indexed */
static void inline __sthdcbx(uint16_t val, uint32_t dc, uint16_t *m)
{
  __asm__ volatile ("sthdcbx\t%0,%1,%2": : "r" (val), "r" (dc), "r" (m) :
                    "memory");
}

/* Store Word with Decoration Cached Bypassed Indexed */
static void inline __stwdcbx(uint32_t val, uint32_t dc, uint32_t *m)
{
  __asm__ volatile ("stwdcbx\t%0,%1,%2": : "r" (val), "r" (dc), "r" (m) :
                    "memory");
}

/* Decorated Storage Cached Bypassed Notify */
static void inline __dsncb(uint32_t dc, uint8_t *m)
{
  __asm__ volatile ("dsncb\t%0,%1": : "r" (dc), "r" (m));
}

/* Cache Bypass Load Instructions */

/* Load Byte Cached Bypassed Indexed */
static uint8_t inline __lbcbx(uint8_t *m, uint32_t idx)
{
  uint8_t ret;
  if (idx == 0)
    __asm__ volatile ("lbcbx\t%0,0,%1": "=r" (ret): "r" (m));
  else
    __asm__ volatile ("lbcbx\t%0,%1,%2": "=r" (ret): "b" (idx), "r" (m));

  return ret;
}

/* Load Halfword Cached Bypassed Indexed */
static uint16_t inline __lhcbx(uint16_t *m, uint32_t idx)
{
  uint16_t ret;
  if (idx == 0)
    __asm__ volatile ("lhcbx\t%0,0,%1": "=r" (ret): "r" (m));
  else
    __asm__ volatile ("lhcbx\t%0,%1,%2": "=r" (ret): "b" (idx), "r" (m));
  return ret;
}

/* Load Word Cached Bypassed Indexed */
static uint32_t inline __lwcbx(uint32_t *m, uint32_t idx)
{
  uint32_t ret;
  if (idx == 0)
    __asm__ volatile ("lwcbx\t%0,0,%1": "=r" (ret): "r" (m));
  else
    __asm__ volatile ("lwcbx\t%0,%1,%2": "=r" (ret): "b" (idx), "r" (m));
  return ret;
}

/* Store with Writetrough Instructions */

/* Store Byte with Writetrough Indexed */
static void inline __stbwtx(uint8_t val, uint32_t idx, uint8_t *m)
{
  if (idx == 0)
  __asm__ volatile ("stbwtx\t%0,0,%1": : "r" (val), "r" (m) :
                    "memory");
  else
  __asm__ volatile ("stbwtx\t%0,%1,%2": : "r" (val), "b" (idx), "r" (m) :
                    "memory");
}

/* Store Halfword Writetrough Indexed */
static void inline __sthwtx(uint16_t val, uint32_t idx, uint16_t *m)
{
  if (idx == 0)
  __asm__ volatile ("sthwtx\t%0,0,%1": : "r" (val), "r" (m) :
                    "memory");
  else
  __asm__ volatile ("sthwtx\t%0,%1,%2": : "r" (val), "b" (idx), "r" (m) :
                    "memory");
}

/* Store Word Writetrough Indexed */
static void inline __stwwtx(uint32_t val, uint32_t idx, uint32_t *m)
{
  if (idx == 0)
  __asm__ volatile ("stwwtx\t%0,0,%1": : "r" (val),  "r" (m) :
                    "memory");
  else
  __asm__ volatile ("stwwtx\t%0,%1,%2": : "r" (val), "b" (idx), "r" (m) :
                    "memory");
}


/*
   Macros to declare an interrupt function
   Function has to of type void func(void)
   Use
   INTERRUPT_FXN(name)
   {
        function body
   }
*/
/* stack layout on interrupt
      r1 = 
      0    R1
      4    LR of called function stores
      8    
     12    SRR0
     16    SRR1
     20    CR
     24    LR
     28    CTR
     32    XER
     36    R0
     40    R3
     44    R4
     48    R5
     52    R6
     56    R7
     60    R8
     64    R9
     68    R10
     72    R11
     74    R12
     80    
*/

#ifndef INTERRUPT_FXN
#define INTR_PROLOG \
  __asm__ volatile ("" \
       "e_stwu      %r1,-(4*20)(%r1)  \n\t" \
       "e_stmvsrrw  12(%r1)          \n\t" \
       "wrteei     1               \n\t" \
       "e_stmvsprw  (12+(2*4))(%r1)  \n\t" \
       "e_stmvgprw  (12+(6*4))(%r1)  \n\t" \
       "")
#define INTR_EPILOG \
  __asm__ volatile ("" \
       "e_lmvgprw  (12+(6*4))(%r1)  \n\t" \
       "e_lmvsprw  (12+(2*4))(%r1)  \n\t" \
       "wrteei     0               \n\t" \
       "e_lmvsrrw  12(%r1)          \n\t" \
       "eaddi       %r1,%r1,(4*20)    \n\t" \
       "se_rfi                      \n\t" \
       "")
#define INTERRUPT_FXN(name)     \
static void ___Intr_##name (void); \
__attribute__ ((naked)) void \
name(void) \
{ \
    INTR_PROLOG; \
    ___Intr_##name(); \
    INTR_EPILOG; \
} \
static void ___Intr_##name(void)
#endif // INTERRUPR_FXN


#endif /* _ZEN_H */

