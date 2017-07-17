/*
*********************************************************************************************************
*                                                    eMOD
*                                         the melis Operation System
*                                               HID sub-system
*                                          HID event classes and types
*
*                                    (c) Copyright 2010-2012, sunny China
*                                              All Rights Reserved
*
* File   : sys_event.h
* Version: V1.0
* By     : Sunny
* Date   : 2010-7-14 19:21:38
* Note   : define input device event classes and types.
*********************************************************************************************************
*/

#ifndef __SYS_EVENT_H__
#define __SYS_EVENT_H__

/* key action value define */
#define KEY_UP_ACTION        0
#define KEY_DOWN_ACTION      1
#define KEY_REPEAT_ACTION    2

/*
 * Event types
 */
#define EV_SYN			0x00
#define EV_KEY			0x01
#define EV_REL			0x02
#define EV_ABS			0x03
#define EV_MSC			0x04
#define EV_SW			0x05
#define EV_LED			0x11
#define EV_SND			0x12
#define EV_REP			0x14
#define EV_FF			0x15
#define EV_PWR			0x16
#define EV_FF_STATUS	0x17
#define EV_MAX			0x1f
#define EV_CNT			(EV_MAX+1)

/*
 * Synchronization events.
 */
#define SYN_REPORT		0
#define SYN_CONFIG		1
#define SYN_MT_REPORT   2

/*
 * Keys and buttons,
 * Standard Set.
 */
#define KEY_RESERVED        0x00
#define KEY_LBUTTON         0x01
#define KEY_RBUTTON         0x02
#define KEY_CANCEL          0x03
#define KEY_MBUTTON         0x04    /* NOT contiguous with L & RBUTTON */
#define KEY_XBUTTON1        0x05    /* NOT contiguous with L & RBUTTON */
#define KEY_XBUTTON2        0x06    /* NOT contiguous with L & RBUTTON */

#define KEY_BACK            0x08
#define KEY_TAB             0x09

#define KEY_CLEAR           0x0C
#define KEY_RETURN          0x0D

#define KEY_SHIFT           0x10
#define KEY_CONTROL         0x11
#define KEY_MENU            0x12
#define KEY_PAUSE           0x13
#define KEY_CAPITAL         0x14

#define	KEY_KANA            0x15
#define KEY_HANGEUL         0x15  /* old name - should be here for compatibility */
#define KEY_HANGUL          0x15
#define KEY_JUNJA           0x17
#define KEY_FINAL           0x18
#define KEY_HANJA           0x19
#define	KEY_KANJI	        0x19

#define KEY_ESCAPE          0x1B

#define	KEY_CONVERT	        0x1c
#define	KEY_NOCONVERT	    0x1d

#define KEY_SPACE           0x20
#define KEY_PRIOR           0x21
#define KEY_NEXT            0x22
#define KEY_END             0x23
#define KEY_HOME            0x24
#define KEY_LEFT            0x25
#define KEY_UP              0x26
#define KEY_RIGHT           0x27
#define KEY_DOWN            0x28
#define KEY_SELECT          0x29
#define KEY_PRINT           0x2A
#define KEY_EXECUTE         0x2B
#define KEY_SNAPSHOT        0x2C
#define KEY_INSERT          0x2D
#define KEY_DELETE          0x2E
#define KEY_HELP            0x2F

/* KEY_0 thru KEY_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
#define KEY_0               0x30
#define KEY_1               0x31
#define KEY_2               0x32
#define KEY_3               0x33
#define KEY_4               0x34
#define KEY_5               0x35
#define KEY_6               0x36
#define KEY_7               0x37
#define KEY_8               0x38
#define KEY_9               0x39

/* KEY_A thru KEY_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */
#define KEY_A               0x41
#define KEY_B               0x42
#define KEY_C               0x43
#define KEY_D               0x44
#define KEY_E               0x45
#define KEY_F               0x46
#define KEY_G               0x47
#define KEY_H               0x48
#define KEY_I               0x49
#define KEY_J               0x4A
#define KEY_K               0x4B
#define KEY_L               0x4C
#define KEY_M               0x4D
#define KEY_N               0x4E
#define KEY_O               0x4F
#define KEY_P               0x50
#define KEY_Q               0x51
#define KEY_R               0x52
#define KEY_S               0x53
#define KEY_T               0x54
#define KEY_U               0x55
#define KEY_V               0x56
#define KEY_W               0x57
#define KEY_X               0x58
#define KEY_Y               0x59
#define KEY_Z               0x5A

#define KEY_LWIN            0x5B
#define KEY_RWIN            0x5C
#define KEY_APPS            0x5D

#define KEY_SLEEP           0x5F

#define KEY_NUMPAD0         0x60
#define KEY_NUMPAD1         0x61
#define KEY_NUMPAD2         0x62
#define KEY_NUMPAD3         0x63
#define KEY_NUMPAD4         0x64
#define KEY_NUMPAD5         0x65
#define KEY_NUMPAD6         0x66
#define KEY_NUMPAD7         0x67
#define KEY_NUMPAD8         0x68
#define KEY_NUMPAD9         0x69
#define KEY_MULTIPLY        0x6A
#define KEY_ADD             0x6B
#define KEY_SEPARATOR       0x6C
#define KEY_SUBTRACT        0x6D
#define KEY_DECIMAL         0x6E
#define KEY_DIVIDE          0x6F
#define KEY_F1              0x70
#define KEY_F2              0x71
#define KEY_F3              0x72
#define KEY_F4              0x73
#define KEY_F5              0x74
#define KEY_F6              0x75
#define KEY_F7              0x76
#define KEY_F8              0x77
#define KEY_F9              0x78
#define KEY_F10             0x79
#define KEY_F11             0x7A
#define KEY_F12             0x7B
#define KEY_F13             0x7C
#define KEY_F14             0x7D
#define KEY_F15             0x7E
#define KEY_F16             0x7F
#define KEY_F17             0x80
#define KEY_F18             0x81
#define KEY_F19             0x82
#define KEY_F20             0x83
#define KEY_F21             0x84
#define KEY_F22             0x85
#define KEY_F23             0x86
#define KEY_F24             0x87

#define KEY_NUMLOCK         0x90
#define KEY_SCROLL          0x91

/* KEY_L* & KEY_R* - left and right Alt, Ctrl and Shift virtual keys. */
#define KEY_LSHIFT          0xA0
#define KEY_RSHIFT          0xA1
#define KEY_LCONTROL        0xA2
#define KEY_RCONTROL        0xA3
#define KEY_LMENU           0xA4
#define KEY_RMENU           0xA5

#define	KEY_EXTEND_BSLASH   0xE2
#define	KEY_OEM_102         0xE2

#define KEY_PROCESSKEY      0xE5

#define KEY_ATTN            0xF6
#define KEY_CRSEL           0xF7
#define KEY_EXSEL           0xF8
#define KEY_EREOF           0xF9
#define KEY_PLAY            0xFA
#define KEY_ZOOM            0xFB
#define KEY_NONAME          0xFC
#define KEY_PA1             0xFD
#define KEY_OEM_CLEAR       0xFE

#define KEY_SEMICOLON		0xBA
#define KEY_EQUAL			0xBB
#define KEY_COMMA			0xBC
#define KEY_HYPHEN			0xBD
#define KEY_PERIOD			0xBE
#define KEY_SLASH			0xBF
#define KEY_BACKQUOTE		0xC0

#define KEY_BROWSER_BACK                0xA6
#define KEY_BROWSER_FORWARD             0xA7
#define KEY_BROWSER_REFRESH             0xA8
#define KEY_BROWSER_STOP                0xA9
#define KEY_BROWSER_SEARCH              0xAA
#define KEY_BROWSER_FAVORITES           0xAB
#define KEY_BROWSER_HOME                0xAC
#define KEY_VOLUME_MUTE                 0xAD
#define KEY_VOLUME_DOWN                 0xAE
#define KEY_VOLUME_UP                   0xAF
#define KEY_MEDIA_NEXT_TRACK            0xB0
#define KEY_MEDIA_PREV_TRACK            0xB1
#define KEY_MEDIA_STOP                  0xB2
#define KEY_MEDIA_PLAY_PAUSE            0xB3
#define KEY_LAUNCH_MAIL                 0xB4
#define KEY_LAUNCH_MEDIA_SELECT         0xB5
#define KEY_LAUNCH_APP1                 0xB6
#define KEY_LAUNCH_APP2                 0xB7

#define KEY_LBRACKET			        0xDB
#define KEY_BACKSLASH		            0xDC
#define KEY_RBRACKET			        0xDD
#define KEY_APOSTROPHE		            0xDE
#define KEY_OFF                         0xDF

/* buttons */
#define BTN_MOUSE	        0x110
#define BTN_LEFT	        0x110
#define BTN_RIGHT	        0x111
#define BTN_MIDDLE	        0x112
#define BTN_SIDE	        0x113
#define BTN_EXTRA	        0x114
#define BTN_FORWARD	        0x115
#define BTN_BACK	        0x116
#define BTN_TASK	        0x117

#define BTN_DIGI		    0x140
#define BTN_TOOL_PEN		0x140
#define BTN_TOOL_RUBBER		0x141
#define BTN_TOOL_BRUSH		0x142
#define BTN_TOOL_PENCIL		0x143
#define BTN_TOOL_AIRBRUSH	0x144
#define BTN_TOOL_FINGER		0x145
#define BTN_TOOL_MOUSE		0x146
#define BTN_TOOL_LENS		0x147
#define BTN_TOUCH		    0x14a
#define BTN_STYLUS		    0x14b
#define BTN_STYLUS2		    0x14c
#define BTN_TOOL_DOUBLETAP	0x14d
#define BTN_TOOL_TRIPLETAP	0x14e

#define BTN_WHEEL		    0x150
#define BTN_GEAR_DOWN		0x150
#define BTN_GEAR_UP		    0x151

/* keypad keys, use for PVP remote and so on */
#define KPAD_MENU           0x200
#define KPAD_ZOOM           0x201
#define KPAD_ENTER          0x202
#define KPAD_RETURN         0x203
#define KPAD_LEFT           0x204
#define KPAD_RIGHT          0x205
#define KPAD_UP             0x206
#define KPAD_DOWN           0x207
#define KPAD_VOICEUP        0x208
#define KPAD_VOICEDOWN      0x209
#define	KPAD_MUSIC			0x20a
#define	KPAD_MOVIE			0x20b
#define	KPAD_FMTUNER		0x20c
#define	KPAD_SETTING		0x20d
#define KPAD_POWER          0x20e
#define KPAD_POWEROFF       0x20f
#define KPAD_MODE           0x210
#define KPAD_FM         	0x211
#define KPAD_PHOTO          0X212
#define KPAD_EBOOK			0X213
#define KPAD_NUM0           0X214
#define KPAD_NUM1           0X215
#define KPAD_NUM2           0X216
#define KPAD_NUM3           0X217
#define KPAD_NUM4           0X218
#define KPAD_NUM5           0X219
#define KPAD_NUM6           0X21a
#define KPAD_NUM7           0X21b
#define KPAD_NUM8           0X21c
#define KPAD_NUM9           0X21d
#define KPAD_RECORD			0X21e
#define KPAD_LR	    		0X21f
#define KPAD_MUTE	    	0X220
#define KPAD_SWITCH         0x221
#define KPAD_PLAY_PAUSE     0X222
#define KPAD_FF             0X223
#define KPAD_RR             0X224
#define KPAD_PREV           0X225
#define KPAD_NEXT           0X226
#define IR_KPAD_LEFT        0x227
#define IR_KPAD_RIGHT       0x228
#define KPAD_TVOUT          0x229
#define KPAD_SUBMENU		0x22a
#define KPAD_EPG            0x22b
#define KPAD_SCAN           0x22c
#define KPAD_SUB            0x22d
#define KPAD_LIST           0x22e
#define KPAD_AUDIO          0x22f
#define KPAD_RED                   0x2230
#define KPAD_GREEN          0x231
#define KPAD_YELLOW         0x232
#define KPAD_BLUE           0x233
#define KPAD_FAVORITE           0x234
#define KPAD_TV_RADIO           0x235
#define KPAD_RECALL         0x236
#define KPAD_VIDEO          0x237
#define KPAD_DTVREC					0x238

#define KPAD_BAND  			0x239
#define KPAD_SEL			0x23a
#define KPAD_EQ				0x23b
#define KPAD_INT			0x23c
#define IR_KPAD_PLAY		0x23d
#define KPAD_REPEAT			0x23e
#define KPAD_ACC_OFF		0x23f
//by wanggang
#define KPAD_MUT		0x240
#define KPAD_CLK		0x241
#define KPAD_SINGLE0    0x242
#define	KPAD_SINGLE1    0x243
#define	KPAD_SINGLE2    0x244
#define	KPAD_SINGLE3    0x245
#define	KPAD_SINGLE4    0x246
#define KPAD_SETTING	0x247



/* We avoid low common keys in module aliases so they don't get huge. */
#define KEY_MAX			    0x2ff
#define KEY_CNT			    (KEY_MAX+1)

/*
 * Relative axes
 */
#define REL_X			    0x00
#define REL_Y			    0x01
#define REL_Z			    0x02
#define REL_RX			    0x03
#define REL_RY			    0x04
#define REL_RZ			    0x05
#define REL_HWHEEL		    0x06
#define REL_DIAL		    0x07
#define REL_WHEEL		    0x08
#define REL_MISC		    0x09
#define REL_MAX			    0x0f
#define REL_CNT			    (REL_MAX+1)

/*
 * Absolute axes
 */
#define ABS_X			    0x00
#define ABS_Y			    0x01
#define ABS_Z			    0x02
#define ABS_RX			    0x03
#define ABS_RY			    0x04
#define ABS_RZ			    0x05
#define ABS_THROTTLE	    0x06
#define ABS_RUDDER		    0x07
#define ABS_WHEEL		    0x08
#define ABS_GAS			    0x09
#define ABS_BRAKE		    0x0a
#define ABS_HAT0X		    0x10
#define ABS_HAT0Y		    0x11
#define ABS_HAT1X		    0x12
#define ABS_HAT1Y		    0x13
#define ABS_HAT2X		    0x14
#define ABS_HAT2Y		    0x15
#define ABS_HAT3X		    0x16
#define ABS_HAT3Y		    0x17
#define ABS_PRESSURE	    0x18
#define ABS_DISTANCE	    0x19
#define ABS_TILT_X		    0x1a
#define ABS_TILT_Y		    0x1b
#define ABS_TOOL_WIDTH	    0x1c
#define ABS_VOLUME		    0x20
#define ABS_MISC		    0x28
#define ABS_MAX			    0x3f
#define ABS_CNT			    (ABS_MAX+1)

/*
 * Misc events
 */
#define MSC_SERIAL		0x00
#define MSC_PULSELED	0x01
#define MSC_GESTURE		0x02
#define MSC_RAW			0x03
#define MSC_SCAN		0x04
#define MSC_MAX			0x07
#define MSC_CNT			(MSC_MAX+1)

/*
 * LEDs
 */
#define LED_NUML		    0x00
#define LED_CAPSL		    0x01
#define LED_SCROLLL		    0x02
#define LED_COMPOSE		    0x03
#define LED_KANA		    0x04
#define LED_SLEEP		    0x05
#define LED_SUSPEND		    0x06
#define LED_MUTE		    0x07
#define LED_MISC		    0x08
#define LED_MAIL		    0x09
#define LED_CHARGING	    0x0a
#define LED_MAX			    0x0f
#define LED_CNT			    (LED_MAX+1)

/*
 * Autorepeat values
 */
#define REP_DELAY		    0x00
#define REP_PERIOD		    0x01
#define REP_MAX			    0x01

/*
 * Sounds
 */
#define SND_CLICK		    0x00
#define SND_BELL		    0x01
#define SND_TONE		    0x02
#define SND_MAX			    0x07
#define SND_CNT			    (SND_MAX+1)

#define FF_MAX		        0x7f
#define FF_CNT		        (FF_MAX+1)

#endif  /* __SYS_EVENT_H__ */
