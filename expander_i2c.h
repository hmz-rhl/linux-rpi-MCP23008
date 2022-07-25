#ifndef _EXPANDER_I2C_H
#define _EXPANDER_I2C_H

/**
 * @file expander-i2c.h
 * @author Hamza RAHAL
 * @brief 
 * @version 0.2
 * @date 2022-05-19
 * 
 * @copyright Saemload (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <stdarg.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>





#define I2C_DEVICE          "/dev/i2c-1"
#define VERSION_EXPANDER_I2C "1.0"


#define MAX_STRING          255



//l'expander 0x26

#define RCD_RESET       6       //  Pin pour controler le mode de calcul de l'ADE
#define RCD_TST         5       //  Pin pour controler le mode de calcul de l'ADE
#define RCD_DIS         4      //  Pin pour controler le cs de l'ADE
#define LOCK_D          3       //  Pin pour controler l'adc Temperature
#define TYPE_E_F_ON     2       
#define TYPE_2_L2L3_ON  1      
#define TYPE_2_NL1_ON   0      

//l'expander 0x27

#define PM0             7       //  Pin pour controler le mode de calcul de l'ADE
#define PM1             6       //  Pin pour controler le mode de calcul de l'ADE
#define PM_CS           5       //  Pin pour controler le cs de l'ADE
#define T_CS            4       //  Pin pour controler l'adc Temperature
#define CP_CS           3       
#define PP_CS           2  
#define CP_DIS          1 
#define LED_DIS         0  


// registers
#define MCP23008_IODIR 0x00   //!< I/O direction register
#define MCP23008_IPOL 0x01    //!< Input polarity register
#define MCP23008_GPINTEN 0x02 //!< Interrupt-on-change control register
#define MCP23008_DEFVAL  0x03 //!< Default compare register for interrupt-on-change
#define REG_INTCON 0x04 //!< Interrupt control register
#define REG_IOCON 0x05  //!< Configuration register
#define REG_GPPU 0x06   //!< Pull-up resistor configuration register
#define REG_INTF 0x07   //!< Interrupt flag register
#define REG_INTCAP 0x08 //!< Interrupt capture register
#define REG_GPIO 0x09   //!< Port register
#define REG_OLAT 0x0A   //!< Output latch register

/*
 LES LABELS SONT A CHANGER DANS LA FONCTION expanderlabelize()
*/
typedef struct expander
{
    /* data */
    int fd;                     // descripeur du fichier /dev/i2c-dev
    uint8_t buff[4];            // buffer contenant la derniere valeur ecrite ou lue
    char label[8][MAX_STRING];  // label des port GPIO pour l'affichage dans console
    uint8_t addr;
    int8_t erreur;              // TODO :mettre en place un systeme d'erreur pour le debug

}expander_t;

expander_t* expander_init(uint8_t);

void expander_labelize(expander_t*);

void expander_openI2C(expander_t*);

void expander_closeI2C(expander_t*);

void expander_setI2C(expander_t*);

void expander_setPullup(expander_t * exp, uint8_t val);

uint8_t expander_getAllPinsGPIO(expander_t*);
uint8_t expander_getPinGPIO(expander_t*, uint8_t);

void expander_setPinGPIO(expander_t*, uint8_t);
void expander_resetPinGPIO(expander_t*, uint8_t);

void expander_setOnlyPinResetOthersGPIO(expander_t*, uint8_t);
void expander_resetOnlyPinSetOthersGPIO(expander_t*, uint8_t);

void expander_togglePinGPIO(expander_t*, uint8_t);

void expander_setAllPinsGPIO(expander_t*);
void expander_resetAllPinsGPIO(expander_t*);

void expander_setAndResetSomePinsGPIO(expander_t*, uint8_t);

void expander_polGPIO(expander_t *exp, uint8_t val);

void expander_printGPIO(expander_t*);

void expander_closeAndFree(expander_t*);

#endif