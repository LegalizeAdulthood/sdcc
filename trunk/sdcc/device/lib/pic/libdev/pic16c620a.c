/* Register definitions for pic16c620a.
 * This file was automatically generated by:
 *   inc2h.pl V4585
 *   Copyright (c) 2002, Kevin L. Pauba, All Rights Reserved
 */
#include <pic16c620a.h>

__sfr  __at (INDF_ADDR)                    INDF;
__sfr  __at (TMR0_ADDR)                    TMR0;
__sfr  __at (PCL_ADDR)                     PCL;
__sfr  __at (STATUS_ADDR)                  STATUS;
__sfr  __at (FSR_ADDR)                     FSR;
__sfr  __at (PORTA_ADDR)                   PORTA;
__sfr  __at (PORTB_ADDR)                   PORTB;
__sfr  __at (PCLATH_ADDR)                  PCLATH;
__sfr  __at (INTCON_ADDR)                  INTCON;
__sfr  __at (PIR1_ADDR)                    PIR1;
__sfr  __at (CMCON_ADDR)                   CMCON;
__sfr  __at (OPTION_REG_ADDR)              OPTION_REG;
__sfr  __at (TRISA_ADDR)                   TRISA;
__sfr  __at (TRISB_ADDR)                   TRISB;
__sfr  __at (PIE1_ADDR)                    PIE1;
__sfr  __at (PCON_ADDR)                    PCON;
__sfr  __at (VRCON_ADDR)                   VRCON;

// 
// bitfield definitions
// 
volatile __CMCON_bits_t __at(CMCON_ADDR) CMCON_bits;
volatile __INTCON_bits_t __at(INTCON_ADDR) INTCON_bits;
volatile __OPTION_REG_bits_t __at(OPTION_REG_ADDR) OPTION_REG_bits;
volatile __PCON_bits_t __at(PCON_ADDR) PCON_bits;
volatile __PIE1_bits_t __at(PIE1_ADDR) PIE1_bits;
volatile __PIR1_bits_t __at(PIR1_ADDR) PIR1_bits;
volatile __PORTA_bits_t __at(PORTA_ADDR) PORTA_bits;
volatile __PORTB_bits_t __at(PORTB_ADDR) PORTB_bits;
volatile __STATUS_bits_t __at(STATUS_ADDR) STATUS_bits;
volatile __TRISA_bits_t __at(TRISA_ADDR) TRISA_bits;
volatile __TRISB_bits_t __at(TRISB_ADDR) TRISB_bits;
volatile __VRCON_bits_t __at(VRCON_ADDR) VRCON_bits;

