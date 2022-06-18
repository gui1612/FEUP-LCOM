#ifndef _LCOM_KBD_MACROS_H_
#define _LCOM_KBD_MACROS_H_

#include <lcom/lcf.h>


#define ESC                 0x81                // breakcode of the ESC key
#define PREFIX_2B_SCANCODE  0xE0                // The first byte of two bytes scancodes (applies to both make and break codes)
#define KBC_IRQ             1                   // Keyboard Controller IRQ Line
#define MAKE_BREAK_DIFF     BIT(7)              // Bit that usually differs from the break code (key released) to the make code
// Note: Usually the scancode of when a key is released (break code) is the make code of that key with the MSB set to 1


/** Ports */
#define KBC_OUT_BUF         0x60                // KBC output buffer
#define KBC_ST_REG          0x64                // KBC Status register
#define KBC_IN_CMD          0x64                // KBC input buffer for commands
#define KBC_IN_ARG          0x60                // KBC input buffer for arguments

/** Status Reg */

#define KBC_PARITY_ERR      BIT(7)              // KBC Parity Error
#define KBC_TIMEOUT_ERR     BIT(6)              // KBC Timeout Error
#define KBC_ERROR_SIG       (BIT(6) | BIT(7))   // Error in the serial communication between the keyboarad and the KBC
#define KBC_AUX             BIT(5)              // KBC mouse data bit
#define KBC_INH             BIT(4)              // KBC Inhibit flag: 0 if keyboard is inhibited
#define KBC_A2              BIT(3)              // KBC A2 Input Line
#define KBC_SYS             BIT(4)              // KBC System Flag
#define KBC_IBF             BIT(1)              // KBC Input Buffer Full
#define KBC_OBF             BIT(0)              // KBC Output Buffer Full


/** KBC commands for PC-AT/PS2 
 * 
 * @note This commands must be written using the address 0x64
 * @note Arguments (if they exist) must be passed to the address 0x60
 * 
*/


#define R_CMD_B             0x20                // Returns Command Byte
#define W_CMD_B             0x60                // Writes command byte (Takes A and returns the command byte)

#define CHK_CMD_B           0xAA                // Checks Command Byte (Returns CHK_CMD_OK if OK and CHK_CMD_ERR if error)
#define CHK_CMD_OK          0x55
#define CHK_CMD_ERR         0xFC

#define DIS_KBD_INT         0xAD                // Disables KBD interface
#define EN_KBD_INT          0xAD                // Enables KBD interface

/** KBC Command Byte */

#define DIS2                BIT(5)              // disable mouse interface
#define DIS                 BIT(4)              // disable keyboard interface
#define INT2                BIT(1)              // enable interrupt on OBF, from mouse
#define INT                 BIT(0)              // enable interrupt on OBF, from keyboard


#endif /* _LCOM_KBD_MACROS_H */
