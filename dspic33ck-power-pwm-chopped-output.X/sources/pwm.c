/*
 * File:   pwm.c
 * Author: A14426
 *
 * Created on Oct 20, 2020, 12:20 PM
 */


#include <xc.h>

#include "pwm.h"

/* Declare global, user-defined PWM generator object */
volatile struct P33C_PWM_GENERATOR_s* my_pg1;    // user-defined PWM generator 1 object 
volatile struct P33C_PWM_GENERATOR_s* my_pg3;    // user-defined PWM generator 4 object
volatile struct P33C_PWM_MODULE_SFRSET_s* my_ol; // use-defined PWM output logic object

volatile uint16_t PWM_Initialize(void) {
    
    volatile uint16_t retval=1;

    // Default PWM Initialization for 500 MHz input clock from AUX PLL
    p33c_PwmModule_Initialize();

    // Capture handle to the desired PWM generator 
    my_pg1 = p33c_PwmGenerator_GetHandle(MOTHER_PWM_GENERATOR);
    my_pg3 = p33c_PwmGenerator_GetHandle(CHILD_PWM_GENERATOR);
    
    //Capture handle to define the desired PWM output logic
    my_ol = p33c_PwmModule_GetHandle();
    
    // Reset PGx SFRs to RESET conditions
    p33c_PwmModule_ConfigWrite(pwmConfigDefault);
    p33c_PwmGenerator_ConfigWrite(MOTHER_PWM_GENERATOR, pgConfigClear);
    p33c_PwmGenerator_ConfigWrite(CHILD_PWM_GENERATOR, pgConfigClear);
   
    // Set individual PWM generator configuration for PG1
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // PGxIOCONH: PWM GENERATOR x I/O CONTROL REGISTER HIGH
    my_pg1->PGxIOCONH.bits.PMOD = 0b00; // PWM Generator outputs operate in Complementary mode
    
    // PGxCONL: PWM GENERATOR x CONTROL REGISTER LOW
    my_pg1->PGxCONL.bits.CLKSEL = 0b01; // PWM Generator uses Master clock selected by the MCLKSEL[1:0] (PCLKCON[1:0]) control bits
    my_pg1->PGxCONL.bits.MODSEL = 0b000; // Independent Edge PWM mode
    my_pg1->PGxCONL.bits.HREN = 1; // PWM Generator x operates in High-Resolution mode
    
    // PGxCONL: PWM GENERATOR x CONTROL REGISTER LOW
    my_pg1->PGxCONL.bits.CLKSEL = 0b01; // PWM Generator uses Master clock selected by the MCLKSEL[1:0] (PCLKCON[1:0]) control bits
    my_pg1->PGxCONL.bits.MODSEL = 0b000; // Independent Edge PWM mode
    my_pg1->PGxCONL.bits.HREN = 1; // PWM Generator x operates in High-Resolution mode
    
    // PGxCONH: PWM GENERATOR x CONTROL REGISTER HIGH
    my_pg1->PGxCONH.bits.MSTEN = 1;
    my_pg1->PGxCONH.bits.UPDMOD = 0b00; // SOC update: Data registers at start of next PWM cycle if UPDREQ = 1
    my_pg1->PGxCONH.bits.TRGMOD = 0b0; // PWM Generator operates in Single Trigger mode
    my_pg1->PGxCONH.bits.SOCS = 0b0011; // SOCS[3:0]: Start-of-Cycle Selection: Local EOC ? PWM Generator is self-triggered
    
    // PGxIOCONL: PWM GENERATOR x I/O CONTROL REGISTER LOW
    my_pg1->PGxIOCONL.bits.OSYNC = 0b00; // User output overrides via the OVRENH/L and OVRDAT[1:0] 
                                        // bits are synchronized to the local PWM time base (next Start-of-Cycle)
    my_pg1->PGxIOCONL.bits.OVRDAT = 0b00; // Both PWM outputs are LOW in override mode
    my_pg1->PGxIOCONL.bits.OVRENL = 1; // OVRDAT0 provides data for output on the PWMxL pin
    my_pg1->PGxIOCONL.bits.OVRENH = 1; // OVRDAT1 provides data for output on the PWMxH pin
    
    // Set PWM signal generation timing of this generator 
    my_pg1->PGxPER.value = 40000;   // Set Period (n = 20000 => 1 / (n * 250 ps) = 200 kHz )
    my_pg1->PGxDC.value  = 20000;    // Set Duty Cycle (n = 10000 => n / PGxPER = 50.0 %)
    my_pg1->PGxDTH.value = 0;     // Set Rising Edge Dead Time (n = 0 => n * 250 ps = 0 ns)
    my_pg1->PGxDTL.value = 0;     // Set Falling Edge Dead Time (n = 0 => n * 250 ps = 0 ns)
    
    // Set PWM signal generation trigger output timing
    my_pg1->PGxTRIGC.value =  0; // Set PGxTRIGC (n = 5000 => n/PGxPER =  25.0% )
    
    // Set individual PWM generator configuration for PG3
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // PGxIOCONH: PWM GENERATOR x I/O CONTROL REGISTER HIGH
    my_pg3->PGxIOCONH.bits.PMOD = 0b00; // PWM Generator outputs operate in Complementary mode
    
    // PGxCONL: PWM GENERATOR x CONTROL REGISTER LOW
    my_pg3->PGxCONL.bits.CLKSEL = 0b01; // PWM Generator uses Master clock selected by the MCLKSEL[1:0] (PCLKCON[1:0]) control bits
    my_pg3->PGxCONL.bits.MODSEL = 0b000; // Independent Edge PWM mode
    my_pg3->PGxCONL.bits.HREN = 1; // PWM Generator x operates in High-Resolution mode
    
    // PGxCONL: PWM GENERATOR x CONTROL REGISTER LOW
    my_pg3->PGxCONL.bits.CLKSEL = 0b01; // PWM Generator uses Master clock selected by the MCLKSEL[1:0] (PCLKCON[1:0]) control bits
    my_pg3->PGxCONL.bits.MODSEL = 0b000; // Independent Edge PWM mode
    my_pg3->PGxCONL.bits.HREN = 1; // PWM Generator x operates in High-Resolution mode
    
    // PGxCONH: PWM GENERATOR x CONTROL REGISTER HIGH
    my_pg3->PGxCONH.bits.UPDMOD = 0b000; // SOC update: Data registers at start of next PWM cycle if UPDREQ = 1
    my_pg3->PGxCONH.bits.TRGMOD = 0b0; // PWM Generator operates in Single Trigger mode
    my_pg3->PGxCONH.bits.SOCS = 0b0000; // SOCS[3:0]: Start-of-Cycle Selection: Local EOC ? PWM Generator is self-triggered
    
    // PGxIOCONL: PWM GENERATOR x I/O CONTROL REGISTER LOW
    my_pg3->PGxIOCONL.bits.OSYNC = 0b00; // User output overrides via the OVRENH/L and OVRDAT[1:0] 
                                        // bits are synchronized to the local PWM time base (next Start-of-Cycle)
    my_pg3->PGxIOCONL.bits.OVRDAT = 0b00; // Both PWM outputs are LOW in override mode
    my_pg3->PGxIOCONL.bits.OVRENL = 1; // OVRDAT0 provides data for output on the PWMxL pin
    my_pg3->PGxIOCONL.bits.OVRENH = 1; // OVRDAT1 provides data for output on the PWMxH pin
    
    // Set PWM signal generation timing of this generator 
    my_pg3->PGxPER.value = 4000;   // Set Period (n = 4000 => 1 / (n * 250 ps) = 1MHz )
    my_pg3->PGxDC.value  = 2000;    // Set Duty Cycle (n = 2000 => n / PGxPER = 50.0 %)
    my_pg3->PGxDTH.value = 0;     // Set Rising Edge Dead Time (n = 0 => n * 250 ps = 0 ns)
    my_pg3->PGxDTL.value = 0;     // Set Falling Edge Dead Time (n = 0 => n * 250 ps = 0 ns)
      
    // Set PWM output logic using combinatorial logic for PWM1 and PWM3
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Combinatorial trigger registers enabled for PWM1 and PWM3
    my_ol->vCMBTRIGL.bits.CTA1EN = 1; // Disable Trigger Output from PWM Generator #1 as Source for Combinational Trigger A
    my_ol->vCMBTRIGL.bits.CTA3EN = 1; // Disable Trigger Output from PWM Generator #2 as Source for Combinational Trigger A
    
    // combinatorial PWM logic B control registers set to PWM1H & PWM3H
    my_ol->LOGCON_A.bits.PWMS1A = 0b0100; // Combinatorial PWM Logic Source #1 Selection: PWM3H
    my_ol->LOGCON_A.bits.S1APOL = 0;      // Combinatorial PWM Logic Source #1 Polarity: Input is positive logic
    my_ol->LOGCON_A.bits.PWMS2A = 0b0000; // Combinatorial PWM Logic Source #2 Selection: PWM1H
    my_ol->LOGCON_A.bits.S2APOL = 0;      // Combinatorial PWM Logic Source #2 Polarity: Input is positive logic
    my_ol->LOGCON_A.bits.PWMLFA = 0b01;   // Combinatorial PWM Logic Function Selection: PWMS1y & PWMS3y (AND)
    my_ol->LOGCON_A.bits.PWMLFAD = 0b010; // Logic function is assigned to PWM3H pin
    
    // Enable PWM generators with outputs DISABLED
    p33c_PwmGenerator_Enable(my_pg1);
    p33c_PwmGenerator_Enable(my_pg3); 
    
    Nop(); // Place breakpoint to review PWM configuration
    Nop(); // using the Watch Window
    Nop();
    
    // Enable PWM generator outputs (starts signal generation at next period transition)
    p33c_PwmGenerator_Resume(my_pg1);
    p33c_PwmGenerator_Resume(my_pg3);
    
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PLEASE NOTE:
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Digital Power Plug-In Modules may have additional circuits dedicated to 
    // specific functions connected to some of their device pins. The following  
    // pre-compiler instructions disabling certain PWM outputs interfering with  
    // analog filter circuits and are therefore not available on the hardware
    // you may be using.
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    #if defined (__MA330048_dsPIC33CK_DPPIM__)

    // On dsPIC33CK DP PIM (MA330048) PWM generator output PWM7L is shared
    // with the on-board push button of the Digital Power Development
    // Board (DM330029). The PWM7L output signal will get filtered and
    // distorted by the switch debounce capacitor and is therefore 
    // being turned off here.
    
    if(MOTHER_PWM_GENERATOR == 7) 
    {
        my_pg1->PGxIOCONL.bits.OVRENL = 1;
        my_pg1->PGxIOCONH.bits.PENL = 0;
    }
    if(CHILD_PWM_GENERATOR == 7) 
    {
        my_pg3->PGxIOCONL.bits.OVRENL = 1;
        my_pg3->PGxIOCONH.bits.PENL = 0;
    }
    
    #elif defined (__MA330049_dsPIC33CH_DPPIM__)

    // On dsPIC33CH DP PIM (MA330049) PWM generator output PWM2L is shared
    // with the on-board push button of the Digital Power Development
    // Board (DM330029). The PWM7L output signal will get filtered and
    // distorted by the switch debounce capacitor and is therefore 
    // being turned off here.

    if(MOTHER_PWM_GENERATOR == 2) 
    {
        my_pg1.pgHandle->PGxIOCONL.bits.OVRENL = 1;
        my_pg1.pgHandle->PGxIOCONH.bits.PENL = 0;
    }
    if(CHILD_PWM_GENERATOR == 2) 
    {
        my_pg1.pgHandle->PGxIOCONL.bits.OVRENL = 1;
        my_pg3.pgHandle->PGxIOCONH.bits.PENL = 0;
    }
    #endif

    // Check return value
    retval = (bool)(my_pg1->PGxCONL.bits.ON) &&         // Check if PWM generator is turned on
                   (!my_pg1->PGxIOCONL.bits.OVRENH) &&    // Check if PWM generator outputs are active
                   (my_pg3->PGxCONL.bits.ON) &&
                    (!my_pg3->PGxIOCONL.bits.OVRENH);  
    
    return(retval); // Return 1=success, 0=failure
}
