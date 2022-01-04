/*
 * memory.x -- The default memory description
 *
 * Copyright (C) 2009-2014 HighTec EDV-Systeme GmbH.
 *
 */

/*
    E_._._.: eXternal memory
    I_._._.: Internal memory
    P_._._.: PCP memory

    ._C_._.: Code
    ._D_._.: Data

    ._._R_.: RAM
    ._._F_.: Flash

    ._._._B: Base address
    ._._._S: Size
*/

#if __TRICORE_NAME__ == 0x13 || __TRICORE_NAME__ == 0x1920
/* __TC13__ like __TC1920__ with Core TC1.3 */
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC13 like TC1920"
#define E_C_R_B	0xa0000000
#define E_C_R_S	512K
#define E_D_R_B	0xa0080000
#define E_D_R_S	512K
#define I_C_F_B	0xa0000000
#define I_C_F_S	512K
#define I_D_F_B	0xa0000000
#define I_D_F_S	512K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	24K
#define P_C_R_B	0xf0020000
#define P_C_R_S	16K
#define P_D_R_B	0xf0010000
#define P_D_R_S	4K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 4K;

#elif __TRICORE_NAME__ == 0x1130
/* __TC1130__ __TC13__ with Core TC1.3 */
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1130"
#define E_C_R_B	0xa0000000
#define E_C_R_S	512K
#define E_D_R_B	0xa0080000
#define E_D_R_S	512K
#define I_C_F_B	0xa0000000
#define I_C_F_S	512K
#define I_D_F_B	0xa0000000
#define I_D_F_S	512K
#define I_C_R_B	0xd4000000
#define I_C_R_S	32K
#define I_D_R_B	0xd0000000
#define I_D_R_S	28K
#define P_C_R_B	0
#define P_C_R_S	0
#define P_D_R_B	0
#define P_D_R_S	0

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 4K;

#elif __TRICORE_NAME__ == 0x1792
/* __TC1792__ __TC13__ with Core TC1.3 */
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1792"
#define E_C_R_B 0xa1000000
#define E_C_R_S 512K
#define E_D_R_B 0xa1080000
#define E_D_R_S 512K
#define I_C_F_B	0x80000000
#define I_C_F_S	2M
#define I_D_F_B	0xafe00000
#define I_D_F_S	32K
#define I_D_F_B_0	0xafe00000
#define I_D_F_S_0	32K
#define I_D_F_B_1	0xafe10000
#define I_D_F_S_1	32K
#define I_C_R_B 0xd4000000
#define I_C_R_S 24K
#define I_D_R_B 0xd0000000
#define I_D_R_S 64K
#define P_C_R_B 0xf0060000
#define P_C_R_S 16K
#define P_D_R_B 0xf0050000
#define P_D_R_S 8K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 4K;

#elif __TRICORE_NAME__ == 0x1796
/* __TC1796__ __TC13__ with Core TC1.3 */
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1796"
#define E_C_R_B	0xa1000000
#define E_C_R_S	512K
#define E_D_R_B	0xa1080000
#define E_D_R_S	512K
#define I_C_F_B	0x80000000
#define I_C_F_S	2M
#define I_D_F_B	0xafe00000
#define I_D_F_S	128K
#define I_C_R_B	0xd4000000
#define I_C_R_S	48K
#define I_D_R_B	0xd0000000
#define I_D_R_S	56K
#define P_C_R_B	0xf0060000
#define P_C_R_S	32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 4K;

#elif __TRICORE_NAME__ == 0x1766
/* __TC1766__ __TC13__ with Core TC1.3 */
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1766"
#define E_C_R_B	0x80000000
#define E_C_R_S	1504K
#define E_D_R_B	0xafe00000
#define E_D_R_S	16K
#define I_C_F_B	0x80000000
#define I_C_F_S	1504K
#define I_D_F_B	0xafe00000
#define I_D_F_S	16K
#define I_D_F_B_0	0xafe00000
#define I_D_F_S_0	16K
#define I_D_F_B_1	0xafe10000
#define I_D_F_S_1	16K
#define I_C_R_B	0xd4000000
#define I_C_R_S	16K
#define I_D_R_B	0xd0000000
#define I_D_R_S	56K
#define P_C_R_B	0xf0060000
#define P_C_R_S	12K
#define P_D_R_B	0xf0050000
#define P_D_R_S	8K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;

#elif __TRICORE_NAME__ == 0x1762
/* __TC1762__ __TC13__ with Core TC1.3 */
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1762"
#define E_C_R_B	0x80000000
#define E_C_R_S	1024K
#define E_D_R_B	0xd0000000
#define E_D_R_S	32K
#define I_C_F_B	0x80000000
#define I_C_F_S	1024K
#define I_D_F_B	0x8fe00000
#define I_D_F_S	8K
#define I_D_F_B_0	0x8fe00000
#define I_D_F_S_0	8K
#define I_D_F_B_1	0x8fe10000
#define I_D_F_S_1	8K
#define I_C_R_B	0xd4000000
#define I_C_R_S	8K
#define I_D_R_B	0xd0000000
#define I_D_R_S	32K
#define P_C_R_B	0
#define P_C_R_S	0
#define P_D_R_B	0
#define P_D_R_S	0

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;

#elif __TRICORE_NAME__ == 0x1764
/* __TC1764__ __TC13__ with Core TC1.3 */
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1764"
#define E_C_R_B	0x80000000
#define E_C_R_S	1024K
#define E_D_R_B	0xd0000000
#define E_D_R_S	40K
#define I_C_F_B	0x80000000
#define I_C_F_S	1024K
#define I_D_F_B	0x8fe00000
#define I_D_F_S	8K
#define I_D_F_B_0	0x8fe00000
#define I_D_F_S_0	8K
#define I_D_F_B_1	0x8fe10000
#define I_D_F_S_1	8K
#define I_C_R_B	0xd4000000
#define I_C_R_S	8K
#define I_D_R_B	0xd0000000
#define I_D_R_S	40K
#define P_C_R_B	0xf006000
#define P_C_R_S	12K
#define P_D_R_B	0xf005000
#define P_D_R_S	8K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;

#elif __TRICORE_NAME__ == 0x116f    \
      || __TRICORE_NAME__ == 0x1161 \
      || __TRICORE_NAME__ == 0x1162
/* __TC116X__ __TC13__ with Core TC1.3 */
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC116X"
#define E_C_R_B 0x80000000
#define E_C_R_S 1024K
#define E_D_R_B 0xafe00000
#define E_D_R_S 16K
#define I_C_F_B 0x80000000
#define I_C_F_S 1024K
#define I_D_F_B 0xafe00000
#define I_D_F_S 16K
#define I_C_R_B 0xd4000000
#define I_C_R_S 8K
#define I_D_R_B 0xd0000000
#define I_D_R_S 32K
#define P_C_R_B 0
#define P_C_R_S 0
#define P_D_R_B 0
#define P_D_R_S 0

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;

#elif __TRICORE_NAME__ == 0x1167
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1167"
#define I_C_F_B	0x80000000
#define I_C_F_S	1M
#define I_D_F_B 0x8fe00000
#define I_D_F_S	32K
#define I_D_F_B_0 0x8fe00000
#define I_D_F_S_0	32K
#define I_D_F_B_1 0x8fe10000
#define I_D_F_S_1	32K
#define E_C_R_B	0xa0000000
#define E_C_R_S	1M
#define E_D_R_B	0xd0000000
#define E_D_R_S	72K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	72K
#define P_C_R_B	0xf0060000
#define P_C_R_S	16K
#define P_D_R_B	0xf0050000
#define P_D_R_S	8K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;

#elif __TRICORE_NAME__ == 0x1197
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1197"
#define I_C_F_B	0x80000000
#define I_C_F_S	2M
#define I_C_F_B_0	0x80000000
#define I_C_F_S_0	2M
#define I_C_F_B_1	0x80200000
#define I_C_F_S_1	2M
#define I_D_F_B 0x8fe00000
#define I_D_F_S	32K
#define I_D_F_B_0 0x8fe00000
#define I_D_F_S_0	32K
#define I_D_F_B_1 0x8fe10000
#define I_D_F_S_1	32K
#define E_C_R_B	0x81000000
#define E_C_R_S	1M
#define E_D_R_B	0x81100000
#define E_D_R_S	1M
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 2K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 16K;

#elif __TRICORE_NAME__ == 0x1736
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1736"
#define E_C_R_B	0x80000000
#define E_C_R_S	1M
#define E_D_R_B	0xafe00000
#define E_D_R_S	16K
#define I_C_F_B	0x80000000
#define I_C_F_S	1M
#define I_D_F_B 0xafe00000
#define I_D_F_S	16K
#define I_D_F_B_0 0xafe00000
#define I_D_F_S_0	16K
#define I_D_F_B_1 0xafe10000
#define I_D_F_S_1	16K
#define I_C_R_B	0xd4000000
#define I_C_R_S	8K
#define I_D_R_B	0xd0000000
#define I_D_R_S	36K
#define P_C_R_B	0
#define P_C_R_S	0
#define P_D_R_B	0
#define P_D_R_S	0

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;

#elif __TRICORE_NAME__ == 0x1767
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1767"
#define I_C_F_B	0x80000000
#define I_C_F_S	2M
#define I_D_F_B 0xafe00000
#define I_D_F_S	32K
#define I_D_F_B_0 0xafe00000
#define I_D_F_S_0	32K
#define I_D_F_B_1 0xafe10000
#define I_D_F_S_1	32K
#define E_C_R_B	0xa1000000
#define E_C_R_S	512K
#define E_D_R_B	0xa1080000
#define E_D_R_S	512K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	68K
#define P_C_R_B	0xf0060000
#define P_C_R_S	16K
#define P_D_R_B	0xf0050000
#define P_D_R_S	8K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;

#elif __TRICORE_NAME__ == 0x1797
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1797"
#define I_C_F_B	0x80000000
#define I_C_F_S	2M
#define I_C_F_B_0	0xa0000000
#define I_C_F_S_0	2M
#define I_C_F_B_1	0xa0200000
#define I_C_F_S_1	2M
#define I_D_F_B 0xafe00000
#define I_D_F_S	32K
#define I_D_F_B_0 0xafe00000
#define I_D_F_S_0	32K
#define I_D_F_B_1 0xafe10000
#define I_D_F_S_1	32K
#define E_C_R_B	0xa1000000
#define E_C_R_S	512K
#define E_D_R_B	0xa1080000
#define E_D_R_S	512K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;

#elif __TRICORE_NAME__ == 0x1782
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1782"
#define I_C_F_B	0x80000000
#define I_C_F_S	0x280000
#define I_C_F_B_0	0x80000000
#define I_C_F_S_0	0x280000
#define I_C_F_B_1	0
#define I_C_F_S_1	0
#define I_D_F_B 0x8fe00000
#define I_D_F_S	128K
#define I_D_F_B_0 0x8fe00000
#define I_D_F_S_0	64K
#define I_D_F_B_1 0x8fe10000
#define I_D_F_S_1	64K
#define E_C_R_B	0x80000000
#define E_C_R_S	0x280000
#define E_D_R_B	0xd0000000
#define E_D_R_S	124K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;


#elif  __TRICORE_NAME__ == 0x1783
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1783"
#define I_C_F_B	0x80000000
#define I_C_F_S	0x280000
#define I_C_F_B_0	0x80000000
#define I_C_F_S_0	0x280000
#define I_C_F_B_1	0
#define I_C_F_S_1	0
#define I_D_F_B 0x8fe00000
#define I_D_F_S	128K
#define I_D_F_B_0 0x8fe00000
#define I_D_F_S_0	64K
#define I_D_F_B_1 0x8fe10000
#define I_D_F_S_1	64K
#define E_C_R_B	0x80000000
#define E_C_R_S	0x280000
#define E_D_R_B	0xd0000000
#define E_D_R_S	124K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;


#elif  __TRICORE_NAME__ == 0x1784
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1784"
#define I_C_F_B	0x80000000
#define I_C_F_S	0x280000
#define I_C_F_B_0	0x80000000
#define I_C_F_S_0	0x280000
#define I_C_F_B_1	0
#define I_C_F_S_1	0
#define I_D_F_B 0x8fe00000
#define I_D_F_S	128K
#define I_D_F_B_0 0x8fe00000
#define I_D_F_S_0	64K
#define I_D_F_B_1 0x8fe10000
#define I_D_F_S_1	64K
#define E_C_R_B	0x80000000
#define E_C_R_S	0x280000
#define E_D_R_B	0xd0000000
#define E_D_R_S	124K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 1K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 1K;


#elif __TRICORE_NAME__  == 0x1387
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1387"
#define I_C_F_B	0x80000000
#define I_C_F_S	0x180000
#define I_C_F_B_0	0x80000000
#define I_C_F_S_0	0x180000
#define I_C_F_B_1	0x80400000
#define I_C_F_S_1	0x180000
#define I_D_F_B 0x8fe00000
#define I_D_F_S	32K
#define I_D_F_B_0 0x8fe00000
#define I_D_F_S_0	32K
#define I_D_F_B_1 0x8fe10000
#define I_D_F_S_1	32K
#define E_C_R_B	0x80800000
#define E_C_R_S	1M
#define E_D_R_B	0x80900000
#define E_D_R_S	1M
#define I_C_R_B	0xd4000000
#define I_C_R_S	80K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	24K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 32K;

#elif __TRICORE_NAME__  == 0x1367
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1367"
#define I_C_F_B	0x80000000
#define I_C_F_S	0x180000
#define I_C_F_B_0	0x80000000
#define I_C_F_S_0	0x180000
#define I_C_F_B_1	0x80400000
#define I_C_F_S_1	0x180000
#define I_D_F_B 0x8fe00000
#define I_D_F_S	32K
#define I_D_F_B_0 0x8fe00000
#define I_D_F_S_0	32K
#define I_D_F_B_1 0x8fe10000
#define I_D_F_S_1	32K
#define E_C_R_B	0x80800000
#define E_C_R_S	1M
#define E_D_R_B	0x80900000
#define E_D_R_S	1M
#define I_C_R_B	0xd4000000
#define I_C_R_S	80K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	24K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 32K;

#elif __TRICORE_NAME__  == 0x1337
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1337"
#define I_C_F_B	0x80000000
#define I_C_F_S	0x180000
#define I_C_F_B_0	0x80000000
#define I_C_F_S_0	0x180000
#define I_C_F_B_1	0x80400000
#define I_C_F_S_1	0x180000
#define I_D_F_B 0x8fe00000
#define I_D_F_S	32K
#define I_D_F_B_0 0x8fe00000
#define I_D_F_S_0	32K
#define I_D_F_B_1 0x8fe10000
#define I_D_F_S_1	32K
#define E_C_R_B	0x80800000
#define E_C_R_S	1M
#define E_D_R_B	0x80900000
#define E_D_R_S	1M
#define I_C_R_B	0xd4000000
#define I_C_R_S	80K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	24K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 32K;

#elif __TRICORE_NAME__  == 0x1798
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1798"
#define I_C_F_B	0x80000000
#define I_C_F_S	4M
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0  2M
#define I_C_F_B_1 0x80200000
#define I_C_F_S_1  2M
#define I_D_F_B   0xaf000000
#define I_D_F_S	   96K
#define I_D_F_B_0 0xaf000000
#define I_D_F_S_0  96K
#define I_D_F_B_1 0xaf080000
#define I_D_F_S_1  96K
#define E_C_R_B	0x83000000
#define E_C_R_S	 2M
#define E_D_R_B	0x83200000
#define E_D_R_S	 2M
#define I_C_R_B	0xc0000000
#define I_C_R_S	 32K
#define I_D_R_B	0xd0000000
#define I_D_R_S	 128K
#define P_C_R_B	0xf0060000
#define P_C_R_S	 32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	 16K

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 32K;

#elif __TRICORE_NAME__  == 0x1793
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1793"
#define I_C_F_B	0x80000000
#define I_C_F_S	4M
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0  2M
#define I_C_F_B_1 0x80200000
#define I_C_F_S_1  2M
#define I_D_F_B   0xaf000000
#define I_D_F_S	   96K
#define I_D_F_B_0 0xaf000000
#define I_D_F_S_0  96K
#define I_D_F_B_1 0xaf080000
#define I_D_F_S_1  96K
#define E_C_R_B	0x83000000
#define E_C_R_S	 2M
#define E_D_R_B	0x83200000
#define E_D_R_S	 2M
#define I_C_R_B	0xc0000000
#define I_C_R_S	 32K
#define I_D_R_B	0xd0000000
#define I_D_R_S	 128K
#define P_C_R_B	0xf0060000
#define P_C_R_S	 32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	 16K

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 32K;

#elif __TRICORE_NAME__  == 0x1791
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1791"
#define I_C_F_B	0x80000000
#define I_C_F_S	4M
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0  2M
#define I_C_F_B_1 0x80200000
#define I_C_F_S_1  2M
#define I_D_F_B   0xaf000000
#define I_D_F_S	   96K
#define I_D_F_B_0 0xaf000000
#define I_D_F_S_0  96K
#define I_D_F_B_1 0xaf080000
#define I_D_F_S_1  96K
#define E_C_R_B	0x83000000
#define E_C_R_S	 2M
#define E_D_R_B	0x83200000
#define E_D_R_S	 2M
#define I_C_R_B	0xc0000000
#define I_C_R_S	 32K
#define I_D_R_B	0xd0000000
#define I_D_R_S	 128K
#define P_C_R_B	0xf0060000
#define P_C_R_S	 32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	 16K

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 32K;

#elif __TRICORE_NAME__  == 0x1724
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1724"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  0x180000
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 0x180000
#define I_C_F_B_1 0
#define I_C_F_S_1 0
#define I_D_F_B   0xaf000000
#define I_D_F_S	   32K
#define I_D_F_B_0 0xafe00000
#define I_D_F_S_0  32K
#define I_D_F_B_1 0xafe10000
#define I_D_F_S_1  32K
#define E_C_R_B	  0x80000000
#define E_C_R_S	  0x180000
#define E_D_R_B	  0xafe00000
#define E_D_R_S	   32K
#define I_C_R_B	  0xc0000000
#define I_C_R_S	   24K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   120K
#define P_C_R_B	  0xf0060000
#define P_C_R_S	   24K
#define P_D_R_B	  0xf0050000
#define P_D_R_S	   8K

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 32K;

#elif __TRICORE_NAME__  == 0x1728
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC1728"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  0x180000
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 0x180000
#define I_C_F_B_1 0
#define I_C_F_S_1 0
#define I_D_F_B   0xaf000000
#define I_D_F_S	   32K
#define I_D_F_B_0 0xafe00000
#define I_D_F_S_0  32K
#define I_D_F_B_1 0xafe10000
#define I_D_F_S_1  32K
#define E_C_R_B	  0x80000000
#define E_C_R_S	  0x180000
#define E_D_R_B	  0xafe00000
#define E_D_R_S	   32K
#define I_C_R_B	  0xc0000000
#define I_C_R_S	   24K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   120K
#define P_C_R_B	  0xf0060000
#define P_C_R_S	   24K
#define P_D_R_B	  0xf0050000
#define P_D_R_S	   8K

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 1K;
__HEAP_MIN = 32K;

#elif __TC13__
/* __TC13__ with Core TC1.3 */
#ifdef __TRICORE_NAME__
#error
#endif
#define __TRICORE_NAME__ 0x13
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC13"
#define E_C_R_B	0xa0000000
#define E_C_R_S	512K
#define E_D_R_B	0xa0080000
#define E_D_R_S	512K
#define I_C_F_B	0xa0000000
#define I_C_F_S	512K
#define I_D_F_B	0xa0000000
#define I_D_F_S	512K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	24K
#define P_C_R_B	0xf0020000
#define P_C_R_S	16K
#define P_D_R_B	0xf0010000
#define P_D_R_S	4K
#elif __TC131__
#define __TRICORE_NAME__ 0x131
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC131"
#define E_C_R_B	0xa1000000
#define E_C_R_S	512K
#define E_D_R_B	0xa1080000
#define E_D_R_S	512K
#define I_C_F_B	0xa0000000
#define I_C_F_S	512K
#define I_D_F_B	0xa0000000
#define I_D_F_S	512K
#define I_C_R_B	0xd4000000
#define I_C_R_S	24K
#define I_D_R_B	0xd0000000
#define I_D_R_S	124K
#define P_C_R_B	0xf0060000
#define P_C_R_S	32K
#define P_D_R_B	0xf0050000
#define P_D_R_S	16K

#elif __TC16__
#define __TRICORE_NAME__ 0x16
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC16"
#define E_C_R_B	0xa0000000
#define E_C_R_S	1M
#define E_D_R_B	0xa0100000
#define E_D_R_S	1M
#define I_C_F_B	0xa0000000
#define I_C_F_S	2M
#define I_D_F_B	0xaf000000
#define I_D_F_S	96K
#define I_C_R_B	0xc0000000
#define I_C_R_S	32K
#define I_D_R_B	0xd0000000
#define I_D_R_S	128K
#define P_C_R_B	0
#define P_C_R_S	0
#define P_D_R_B	0
#define P_D_R_S	0

#elif __TRICORE_NAME__  == 0x2200
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC22XX"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  1M
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 1M
#define I_C_F_B_1 0
#define I_C_F_S_1 0
#define I_D_F_B   0xaf000000
#define I_D_F_S	   96K
#define E_C_R_B	  0
#define E_C_R_S	  0
#define E_D_R_B	  0
#define E_D_R_S	  0
#define I_C_R_B	  0xc0000000
#define I_C_R_S	    8K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   88K
#define P_C_R_B	  0
#define P_C_R_S	  0
#define P_D_R_B	  0
#define P_D_R_S	  0

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 256;
__HEAP_MIN = 8K;
__CSA_SIZE = 16K;

#elif __TRICORE_NAME__  == 0x2300
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC23XX"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  2M
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 2M
#define I_C_F_B_1 0
#define I_C_F_S_1 0
#define I_D_F_B   0xaf000000
#define I_D_F_S	  128K
#define E_C_R_B	  0
#define E_C_R_S	  0
#define E_D_R_B	  0
#define E_D_R_S	  0
#define I_C_R_B	  0xc0000000
#define I_C_R_S	    8K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	  184K
#define P_C_R_B	  0
#define P_C_R_S	  0
#define P_D_R_B	  0
#define P_D_R_S	  0

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 256;
__HEAP_MIN = 8K;
__CSA_SIZE = 16K;

#elif __TRICORE_NAME__  == 0x2600
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC26XX"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  0x100000
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 0x100000
#define I_C_F_B_1 0x80100000
#define I_C_F_S_1 0x180000
#define I_D_F_B   0xaf000000
#define I_D_F_S	   96K
#define E_C_R_B	  0
#define E_C_R_S	  0
#define E_D_R_B	  0
#define E_D_R_S	  0
#define I_C_R_B	  0xc0000000
#define I_C_R_S	   16K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   72K
#define P_C_R_B	  0
#define P_C_R_S	  0
#define P_D_R_B	  0
#define P_D_R_S	  0

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 256;
__HEAP_MIN = 8K;
__CSA_SIZE = 16K;

#elif __TRICORE_NAME__  == 0x2700
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC27XX"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  0x200000
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 0x200000
#define I_C_F_B_1 0x80200000
#define I_C_F_S_1 0x200000
#define I_D_F_B   0xaf000000
#define I_D_F_S	   0x60000
#define I_D_F_B_0 0xaf000000
#define I_D_F_S_0  0x60000
#define I_D_F_B_1 0xaf110000
#define I_D_F_S_1  64K
#define E_C_R_B	  0
#define E_C_R_S	  0
#define E_D_R_B	  0
#define E_D_R_S	  0
#define I_C_R_B	  0xc0000000
#define I_C_R_S	   24K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   112K
#define P_C_R_B	  0
#define P_C_R_S	  0
#define P_D_R_B	  0
#define P_D_R_S	  0

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 256;
__HEAP_MIN = 8K;
__CSA_SIZE = 16K;

#elif __TRICORE_NAME__  == 0x2900
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC29XX"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  0x200000
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 0x200000
#define I_C_F_B_1 0x80200000
#define I_C_F_S_1 0x200000
#define I_C_F_B_2 0x80400000
#define I_C_F_S_2 0x200000
#define I_C_F_B_3 0x80600000
#define I_C_F_S_3 0x200000
#define I_D_F_B   0xaf000000
#define I_D_F_S	   768K
#define I_D_F_B_0 0xaf000000
#define I_D_F_S_0  768K
#define I_D_F_B_1 0xaf110000
#define I_D_F_S_1  64K
#define E_C_R_B	  0
#define E_C_R_S	  0
#define E_D_R_B	  0
#define E_D_R_S	  0
#define I_C_R_B	  0xc0000000
#define I_C_R_S	   32K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   120K
#define P_C_R_B	  0
#define P_C_R_S	  0
#define P_D_R_B	  0
#define P_D_R_S	  0

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 256;
__HEAP_MIN = 8K;
__CSA_SIZE = 16K;

#elif __TRICORE_NAME__  == 0x2d5d
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC2D5D"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  0x200000
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 0x200000
#define I_C_F_B_1 0x80200000
#define I_C_F_S_1 0x200000
#define I_D_F_B   0xaf000000
#define I_D_F_S	   1M
#define I_D_F_B_0 0xaf000000
#define I_D_F_S_0  1M
#define I_D_F_B_1 0xaf100000
#define I_D_F_S_1  1M
#define E_C_R_B	  0
#define E_C_R_S	  0
#define E_D_R_B	  0
#define E_D_R_S	  0
#define I_C_R_B	  0xc0000000
#define I_C_R_S	   8K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   128K
#define P_C_R_B	  0
#define P_C_R_S	  0
#define P_D_R_B	  0
#define P_D_R_S	  0

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 256;
__HEAP_MIN = 8K;
__CSA_SIZE = 16K;

#elif __TRICORE_NAME__  == 0x3800
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC38xx"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  3M
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 3M
#define I_C_F_B_1 0x80300000
#define I_C_F_S_1 3M
#define I_C_F_B_2 0x80600000
#define I_C_F_S_2 3M
#define I_C_F_B_3 0x80900000
#define I_C_F_S_3 1M
#define I_D_F_B   0xaf000000
#define I_D_F_S	   512K
#define I_D_F_B_0 0xaf000000
#define I_D_F_S_0  512K
#define E_C_R_B	  0
#define E_C_R_S	  0
#define E_D_R_B	  0
#define E_D_R_S	  0
#define I_C_R_B	  0xc0000000
#define I_C_R_S	   64K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   96K
#define P_C_R_B	  0
#define P_C_R_S	  0
#define P_D_R_B	  0
#define P_D_R_S	  0

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 256;
__HEAP_MIN = 8K;
__CSA_SIZE = 16K;

#elif __TRICORE_NAME__  == 0x3900
#define __MESSAGE__ "Using wrong Memory Map. This Map is for TC39xx"
#define I_C_F_B	  0x80000000
#define I_C_F_S	  3M
#define I_C_F_B_0 0x80000000
#define I_C_F_S_0 3M
#define I_C_F_B_1 0x80300000
#define I_C_F_S_1 3M
#define I_C_F_B_2 0x80600000
#define I_C_F_S_2 3M
#define I_C_F_B_3 0x80900000
#define I_C_F_S_3 3M
#define I_C_F_B_4 0x80c00000
#define I_C_F_S_4 3M
#define I_C_F_B_5 0x80f00000
#define I_C_F_S_5 1M
#define I_D_F_B   0xaf000000
#define I_D_F_S	   1M
#define I_D_F_B_0 0xaf000000
#define I_D_F_S_0  1M
#define E_C_R_B	  0
#define E_C_R_S	  0
#define E_D_R_B	  0
#define E_D_R_S	  0
#define I_C_R_B	  0xc0000000
#define I_C_R_S	   64K
#define I_D_R_B	  0xd0000000
#define I_D_R_S	   96K
#define P_C_R_B	  0
#define P_C_R_S	  0
#define P_D_R_B	  0
#define P_D_R_S	  0

__USTACK_SIZE = 4K;
__ISTACK_SIZE = 256;
__HEAP_MIN = 8K;
__CSA_SIZE = 16K;

#endif


__TRICORE_DERIVATE_MEMORY_MAP__ = __TRICORE_NAME__;
/* the external RAM description */
__EXT_CODE_RAM_BEGIN	= 	E_C_R_B;
__EXT_CODE_RAM_SIZE		=  	E_C_R_S ;
__EXT_DATA_RAM_BEGIN	=	E_D_R_B;
__EXT_DATA_RAM_SIZE		=	E_D_R_S;
__RAM_END = __EXT_DATA_RAM_BEGIN + __EXT_DATA_RAM_SIZE;
/* internal FLASH description */
__INT_CODE_FLASH_BEGIN	=	I_C_F_B;
__INT_CODE_FLASH_SIZE	=	I_C_F_S;
#if defined(I_C_F_B_1)
__INT_CODE_FLASH_BANK1_BEGIN	=	I_C_F_B_1;
__INT_CODE_FLASH_BANK1_SIZE	=	I_C_F_S_1;
#endif
#if defined(I_C_F_B_2)
__INT_CODE_FLASH_BANK2_BEGIN	=	I_C_F_B_2;
__INT_CODE_FLASH_BANK2_SIZE	=	I_C_F_S_2;
#endif
#if defined(I_C_F_B_3)
__INT_CODE_FLASH_BANK3_BEGIN	=	I_C_F_B_3;
__INT_CODE_FLASH_BANK3_SIZE	=	I_C_F_S_3;
#endif
#if defined(I_C_F_B_4)
__INT_CODE_FLASH_BANK4_BEGIN	=	I_C_F_B_4;
__INT_CODE_FLASH_BANK4_SIZE	=	I_C_F_S_4;
#endif
#if defined(I_C_F_B_5)
__INT_CODE_FLASH_BANK5_BEGIN	=	I_C_F_B_5;
__INT_CODE_FLASH_BANK5_SIZE	=	I_C_F_S_5;
#endif
__INT_DATA_FLASH_BEGIN	=	I_D_F_B;
__INT_DATA_FLASH_SIZE	=	I_D_F_S;
#if defined(I_D_F_B_1)
__INT_DATA_FLASH_BANK1_BEGIN =	I_D_F_B_1;
__INT_DATA_FLASH_BANK1_SIZE	 =	I_D_F_S_1;
#endif
/* the internal ram description */
__INT_CODE_RAM_BEGIN	=	I_C_R_B;
__INT_CODE_RAM_SIZE		=	I_C_R_S;
__INT_DATA_RAM_BEGIN	=	I_D_R_B;
__INT_DATA_RAM_SIZE		=	I_D_R_S;
/* the pcp memory description */
__PCP_CODE_RAM_BEGIN	=	P_C_R_B;
__PCP_CODE_RAM_SIZE		=	P_C_R_S;
__PCP_DATA_RAM_BEGIN	=	P_D_R_B;
__PCP_DATA_RAM_SIZE		=	P_D_R_S;

MEMORY
{
  PMU_PFLASH0 (rx!p):	org = I_C_F_B, len = I_C_F_S
#if defined(I_C_F_B_1)
  PMU_PFLASH1 (rx!p):	org = I_C_F_B_1, len = I_C_F_S_1
#endif
#if defined(I_C_F_B_2)
  PMU_PFLASH2 (rx!p):	org = I_C_F_B_2, len = I_C_F_S_2
#endif
#if defined(I_C_F_B_3)
  PMU_PFLASH3 (rx!p):	org = I_C_F_B_3, len = I_C_F_S_3
#endif
#if defined(I_C_F_B_4)
  PMU_PFLASH4 (rx!p):	org = I_C_F_B_4, len = I_C_F_S_4
#endif
#if defined(I_C_F_B_5)
  PMU_PFLASH5 (rx!p):	org = I_C_F_B_5, len = I_C_F_S_5
#endif
  PMU_DFLASH0 (w!xp):	org = I_D_F_B, len = I_D_F_S
#if defined(I_D_F_B_1)
  PMU_DFLASH1 (w!xp):	org = I_D_F_B_1, len = I_D_F_S_1
#endif
  PMI_SPRAM (rx!p):	org = I_C_R_B, len = I_C_R_S
  DMI_LDRAM (w!xp):	org = I_D_R_B, len = I_D_R_S
  PCP_PRAM (wp!x):	org = P_D_R_B, len = P_D_R_S
  PCP_CMEM (rpx):	org = P_C_R_B, len = P_C_R_S
}

REGION_ALIAS("DATA_MEM", DMI_LDRAM)
REGION_ALIAS("CODE_MEM", PMU_PFLASH0)
REGION_ALIAS("SDATA_MEM", DMI_LDRAM)
REGION_ALIAS("BSS_MEM", DMI_LDRAM)
REGION_ALIAS("ZDATA_MEM", DMI_LDRAM)
REGION_ALIAS("CSA_MEM", DMI_LDRAM)
REGION_ALIAS("PCP_CODE", PCP_CMEM)
REGION_ALIAS("PCP_DATA", PCP_PRAM)


/* the symbol __TRICORE_DERIVATE_NAME__ will be defined in the crt0.S and is
 * tested here to confirm that this memory map and the startup file will
 * fit together
*/
_. = ASSERT ((__TRICORE_DERIVATE_MEMORY_MAP__ == __TRICORE_DERIVATE_NAME__), __MESSAGE__);
 
INSERT BEFORE .startup
