    IF :LNOT::DEF:__ARM9_INC_
	GBLA  __ARM9_INC_
	
ARM9_USR_MODE        EQU    0x10
ARM9_FIQ_MODE        EQU    0x11
ARM9_IRQ_MODE        EQU    0x12
ARM9_SVC_MODE        EQU    0x13
ARM9_ABT_MODE        EQU    0x17
ARM9_UND_MODE        EQU    0x1b
ARM9_SYSTEM_MODE     EQU    0x1f
ARM9_MODE_MASK       EQU    0x1f
ARM9_FIQ_MASK         EQU    0x40
ARM9_IRQ_MASK         EQU    0x80
ARM9_CC_V_BIT        EQU    0x10000000
ARM9_CC_C_BIT        EQU    0x20000000
ARM9_CC_Z_BIT        EQU    0x40000000
ARM9_CC_N_BIT        EQU    0x80000000

    
    
;//coprocessor CP15
;//C1
C1_M_BIT        EQU    ( 1 << 0 )
C1_A_BIT        EQU    ( 1 << 1 )
C1_C_BIT        EQU    ( 1 << 2 )
C1_W_BIT        EQU    ( 1 << 3 )
C1_P_BIT        EQU    ( 1 << 4 )
C1_D_BIT        EQU    ( 1 << 5 )
C1_L_BIT        EQU    ( 1 << 6 )
C1_B_BIT        EQU    ( 1 << 7 )
C1_S_BIT        EQU    ( 1 << 8 )
C1_R_BIT        EQU    ( 1 << 9 )
C1_F_BIT        EQU    ( 1 << 10 )
C1_Z_BIT        EQU    ( 1 << 11 )
C1_I_BIT        EQU    ( 1 << 12 )
C1_V_BIT        EQU    ( 1 << 13 )
C1_RR_BIT       EQU    ( 1 << 14 )
C1_L4_BIT       EQU    ( 1 << 15 )


    endif
    end

