

/**
 * @file expander-i2c.c
 * @author Hamza RAHAL
 * @brief 
 * @version 0.3
 * @date 2022-05-19
 * 
 * @copyright Saemload (c) 2022
 * 
 */


#include "expander_i2c.h"

#define Er_Ecriture -1
#define Er_Lecture -2
#define Er_Ouverture -3
#define Er_Fermeture -4
#define Er_I2C  -5
#define Er_Expander_Ecriture -10
#define VERSION_EXPANDER_I2C "1.0"




/**
 ** 
 * @brief   ouvre et configure l'interface i2c de la RP, instancie une variable de type expander_t et initialise ses champs dont l'adresse esclave du MCP
 * 
 * @param   addr adresse en HEXA du MCP23008 (0x__)
 * 
 * 
 * @return  renvoi un pointeur sur la variable instanciée
 *  
 **/
expander_t* expander_init(uint8_t addr){
    if(addr > 0x27 || addr < 0x20 )
    {
        printf("ERREUR %s : vous avez saisie 0x%02x\nOr addr doit etre entre 0x20 et 0x27 pour l'expander\n",__func__, addr);
        //exit(EXIT_FAILURE);
        return NULL;
    }
    expander_t* exp = malloc(sizeof(expander_t));
    if (exp == NULL){
        printf("ERREUR %s : allocation echouee\n", __func__);
        //exit(EXIT_FAILURE);
        return NULL;
    }


    exp->addr = addr;
    exp->erreur = 0;
    expander_labelize(exp);
    expander_openI2C(exp);
    expander_setI2C(exp);

    usleep(1);
    return exp;
}





/**
 ** 
 * @brief   definit le champs label qui associera un nom a chaque pin GPIO pour la console
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  
 **/
void expander_labelize(expander_t* exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        //exit(EXIT_FAILURE);
        return;
    }
    if(exp->addr == 0x26){

        strcpy(exp->label[0],"TYPE-2_NL1_ON*---->");
        strcpy(exp->label[1],"TYPE-2_L2L3_ON*--->");
        strcpy(exp->label[2],"TYPE-E/F_ON*------>");
        strcpy(exp->label[3],"LOCK_D*----------->");
        strcpy(exp->label[4],"RCD_DIS#*--------->");
        strcpy(exp->label[5],"RCD_TST#*--------->");
        strcpy(exp->label[6],"RCD_RESET#*------->");
        strcpy(exp->label[7],"------------------>");

    }
    else if(exp->addr == 0x27){

        strcpy(exp->label[0],"LED_DIS#*--------->");
        strcpy(exp->label[1],"CP_DIS#*---------->");
        strcpy(exp->label[2],"PP_CS*------------>");
        strcpy(exp->label[3],"CP_CS*------------>");
        strcpy(exp->label[4],"T_CS*------------->");
        strcpy(exp->label[5],"PM_CS*------------>");
        strcpy(exp->label[6],"PM1*-------------->");
        strcpy(exp->label[7],"PM0*-------------->");
    }
    else{

        strcpy(exp->label[0],"");
        strcpy(exp->label[1],"");
        strcpy(exp->label[2],"");
        strcpy(exp->label[3],"");
        strcpy(exp->label[4],"");
        strcpy(exp->label[5],"");
        strcpy(exp->label[6],"");
        strcpy(exp->label[7],"");
    }


}



/**
 ** 
 * @brief   ouvre l'interface i2c de la RP
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  
 **/
void expander_openI2C(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        return;
        //exit(EXIT_FAILURE);
    }
    exp->fd = open(I2C_DEVICE, O_RDWR);
    if(exp->fd < 0) {

        printf("Warning fonction %s : pas pu ouvrir l'i2c on retente apres 1sec\n", __func__);
        sleep(1);

        exp->fd = open(I2C_DEVICE, O_RDWR);
        if(exp->fd < 0) {
        
            fprintf(stderr, "fonction %s: Unable to open i2c device: %s\n", __func__, strerror(errno));
            exp->erreur = Er_Ouverture;

            //exit(EXIT_FAILURE);
            return;
        }
        
    }
}



/**
 ** 
 * @brief   ferme l'interface i2c de la RP
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  
 **/
void expander_closeI2C(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
    return;
    }
    if(close(exp->fd) < 0) {

        fprintf(stderr, "fonction %s: Unable to close i2c device: %s\n", __func__, strerror(errno));
        exp->erreur = Er_Fermeture;
        //exit(EXIT_FAILURE);
        return;
    }
}



/**
 ** 
 * @brief   configure l'interface i2c, et lui fait connaitre l'adresse de l'expander
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  
 **/
void expander_setI2C(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
        return;
    }   

    if(ioctl(exp->fd,I2C_SLAVE,exp->addr) < 0) {
        printf("ERREUR de setting de l'address l'interface I2C de la RPZ ...\n");
        close(exp->fd);
        exp->erreur = Er_I2C;
        //exit(EXIT_FAILURE);
        return;
    }

}

/**
 ** 
 * @brief   configure les pullUp des gpio
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   val valeur des pull up en HEXA
 *
 * 
 *  **/
void expander_setPullup(expander_t * exp, uint8_t val){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

       // exit(EXIT_FAILURE);
        return;
    }
    
        // pull up activé
    exp->buff[0] = REG_GPPU;
    exp->buff[1] = val;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur GPPU\r\n");
        exp->erreur = Er_Ecriture;
        //exit(EXIT_FAILURE);
         return; 
    }
    usleep(100);
}

/**
 ** 
 * @brief   Renvoi l'état des pins GPIO (0-7)
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 *  @return l'état des pins sous forme d'un octet où chaque bit correspond a un pin
 * 
 *  **/
uint8_t expander_getAllPinsGPIO(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

       // exit(EXIT_FAILURE);
           return 0;
    }

    /**
 * polarité du registre POL de l'expander
 **/
    // exp->buff[0] = MCP23008_IPOL;
    // exp->buff[1] = 0x00;
    // if(write(exp->fd,exp->buff,2) != 2){
        
    //     printf("ERREUR d'écriture du registre IPOL (branché sur i2c?)\n");
    //     close(exp->fd);
    //     exit(EXIT_FAILURE);
    //     return;
    //     exp->erreur = -1;

    // }

/**
 * Selection du registre GPIO de l'expander
 **/
    exp->buff[0] = REG_GPIO; 
    if(write(exp->fd,exp->buff,1) != 1){
        
        printf("ERREUR d'écriture du registre GPIO (branché sur i2c?)\n");
        close(exp->fd);
        exp->erreur = Er_Ecriture;
        //exit(EXIT_FAILURE);
        return 0;
    }
/**
 * Lecture du registre GPIO de l'expander
 **/
    exp->buff[0] = REG_GPIO;
    if(read(exp->fd,exp->buff,1) != 1) {
        printf("ERREUR de de lecture sur GPIO\n");
        close(exp->fd);
        exp->erreur = Er_Lecture;
        //exit(EXIT_FAILURE);
        return 0;
    }
    usleep(100);
    
    return exp->buff[0];

}



/**
 * 
 * @brief   Renvoi l'état du pin
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 *  @return 0x00 ou 0x01 en fonction de l'état du pin
 * 
 *  **/
uint8_t expander_getPinGPIO(expander_t *exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
            return 0;
    }
        if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin selectionne non compris entre 0 et 7\n", __func__);
        exp->erreur = Er_Expander_Ecriture;
        //exit(EXIT_FAILURE);
        return 0;
    }

    uint8_t ret = expander_getAllPinsGPIO(exp);
    usleep(100);

    return (ret >> pin) & 0x01;
}



/**
 * 
 * @brief   mets un pin a 1
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_setPinGPIO(expander_t *exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

       // exit(EXIT_FAILURE);
        return;
    }
    
    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exp->erreur = Er_Expander_Ecriture;
        //exit(EXIT_FAILURE);
        return;

    }
    uint8_t ancienGPIO = expander_getAllPinsGPIO(exp);
    uint8_t nouveauGPIO = ancienGPIO | (0x01 << pin);


    int cpt = 0;
    while(expander_getAllPinsGPIO(exp) != nouveauGPIO && cpt < 5){
    /* Ecriture des gpio de l'expander
    **/
        cpt++;
        exp->buff[0] = MCP23008_IODIR;
        exp->buff[1] = 0x00;

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur IODIR\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }


        exp->buff[0] = REG_OLAT;
        exp->buff[1] = nouveauGPIO;

    #ifdef DEBUG
        printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
    #endif

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur OLAT\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }
    #ifdef DEBUG
        printf("mise a 1 de GPIO[%d] %s\n", pin, exp->label[pin]);
    #endif
    }
    usleep(100);

}



/**
 * 
 * @brief   mets un pin a 0
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_resetPinGPIO(expander_t *exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
    
       // exit(EXIT_FAILURE);
        return;
    }

    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exp->erreur = Er_Expander_Ecriture;
        //exit(EXIT_FAILURE);
        return;
    }

    uint8_t ancienGPIO = expander_getAllPinsGPIO(exp);
    uint8_t nouveauGPIO = ancienGPIO & ~(0x01 << pin);

    int cpt = 0;
    while(expander_getAllPinsGPIO(exp) != nouveauGPIO && cpt < 5){

    /* Ecriture des gpio de l'expander
    **/         
        cpt++;
        exp->buff[0] = MCP23008_IODIR;
        exp->buff[1] = 0x00;

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur IODIR\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;    
        }

        exp->buff[0] = REG_OLAT;

        exp->buff[1] = nouveauGPIO;

    #ifdef DEBUG
        printf("__Ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
    #endif

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur OLAT\r\n");
            exp->erreur = Er_Ecriture;
           // exit(EXIT_FAILURE);
            return;
        }
        #ifdef DEBUG
        printf("mise a 0 de GPIO[%d] %s\n", pin , exp->label[pin]);
    #endif
    }

}



/**
 * 
 * @brief   inverse l'état d'un pin
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_togglePinGPIO(expander_t* exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
        //exit(EXIT_FAILURE);
        return;
    }

    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exp->erreur = Er_Expander_Ecriture;
        //exit(EXIT_FAILURE);
        return;
    }

    if(expander_getPinGPIO(exp, pin)){

        expander_resetPinGPIO(exp,pin);
    }
    else{
        expander_setPinGPIO(exp,pin);
    }
}

/**
 ** 
 * @brief   mets tout les pins a 1
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 * 
 *  **/
void expander_setAllPinsGPIO(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
            return;
    }

    int cpt = 0;
    while(expander_getAllPinsGPIO(exp) != 0xFF && cpt < 5){
    /* Ecriture des gpio de l'expander
    **/

        cpt++;
        exp->buff[0] = MCP23008_IODIR;
        exp->buff[1] = 0x00;

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur IODIR\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }

        exp->buff[0] = REG_OLAT;
        exp->buff[1] = 0xFF;
    #ifdef DEBUG
        printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
    #endif

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur OLAT\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }
    #ifdef DEBUG
        printf("mise a 1 de tous les GPIO\n");
    #endif
    }
    usleep(100);

}



/**
 * 
 * @brief   mets tout les pin a 0
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 * 
 *  **/
void expander_resetAllPinsGPIO(expander_t *exp){


    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
        return;    
    }
    int cpt = 0;
    while(expander_getAllPinsGPIO(exp) != 0x00 && cpt < 5){
    /* Ecriture des gpio de l'expander
    **/
        cpt++;
        exp->buff[0] = MCP23008_IODIR;
        exp->buff[1] = 0x00;

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur IODIR\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }

        exp->buff[0] = REG_OLAT;
    #ifdef DEBUG
        printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
    #endif
        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur OLAT\r\n");
            exp->erreur = Er_Ecriture;
           // exit(EXIT_FAILURE);
            return;    
        }
    #ifdef DEBUG
        printf("mise a 0 de tous les GPIO\n");
    #endif
    }
    usleep(100);

}



/**
 * 
 * @brief   mets le pin a 1 et tout les autres a 0
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_setOnlyPinResetOthersGPIO(expander_t* exp, uint8_t pin){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
        return;
    }
    
    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exp->erreur = Er_Expander_Ecriture;
        //exit(EXIT_FAILURE);
        return;
    

    }
    int cpt = 0;
    while(expander_getAllPinsGPIO(exp) != (0x01 << pin) && cpt < 5){
        
        cpt++;
        exp->buff[0] = MCP23008_IODIR;
        exp->buff[1] = 0x00;

        if(write(exp->fd,exp->buff,2) != 2) {
            
            printf("ERREUR d'ecriture sur IODIR\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }
        exp->buff[0] = REG_OLAT;
        exp->buff[1] = 0x01 << pin;
        #ifdef DEBUG
        printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
        #endif
        if(write(exp->fd,exp->buff,2) != 2) {
            
            printf("ERREUR d'ecriture sur OLAT\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }
        #ifdef DEBUG
        printf("mise a 1 du seul GPIO[%d] %s\n", pin, exp->label[pin]);
        #endif
    }
    usleep(100);

}



/**
 * 
 * @brief    mets le pin a 0 et tout les autres a 1
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   pin le pin en question (entre 0 et 7)
 * 
 * 
 *  **/
void expander_resetOnlyPinSetOthersGPIO(expander_t* exp, uint8_t pin){
    
    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
        return;    
    }

    if(pin > 7 || pin < 0)
    {
        printf("ERREUR fonction %s : parametre pin doit etre compris entre 0 et 7\n", __func__);
        exp->erreur = Er_Expander_Ecriture;
       // exit(EXIT_FAILURE);
        return;
    }
    int cpt = 0;
    while(expander_getAllPinsGPIO(exp) != (0x01 << pin) && cpt < 5){
        
        cpt++;
        exp->buff[0] = MCP23008_IODIR;
        exp->buff[1] = 0x00;

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur IODIR\r\n");
            exp->erreur = Er_Ecriture;
           // exit(EXIT_FAILURE);
            return;
        }
        
        exp->buff[0] = REG_OLAT;
        exp->buff[1] = ~(0x01 << pin);
    #ifdef DEBUG
        printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
    #endif
        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur OLAT\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }
        #ifdef DEBUG
        printf("mise a 1 du seul GPIO[%d]\n", pin);
    #endif
    }
    usleep(100);
    
}

/**
 * 
 * @brief    mets une config precise des pins
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   config la config sur un octet
 * 
 * 
 *  **/
void expander_setAndResetSomePinsGPIO(expander_t* exp, uint8_t config){

        
    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
        return;    
    }
        exp->buff[0] = MCP23008_IODIR;
        exp->buff[1] = 0;

    if(write(exp->fd,exp->buff,2) != 2) {
        printf("ERREUR d'ecriture sur IODIR\r\n");
        exp->erreur = Er_Ecriture;
       // exit(EXIT_FAILURE);
        return;
    }
    int cpt = 0;
    while(expander_getAllPinsGPIO(exp) != config && cpt < 5){

        cpt++;
        exp->buff[0] = REG_OLAT;
        exp->buff[1] = config;
    #ifdef DEBUG
        printf("ecriture sur OLAT de 0x%02x...\n",exp->buff[1]);
    #endif

        if(write(exp->fd,exp->buff,2) != 2) {
            printf("ERREUR d'ecriture sur OLAT\r\n");
            exp->erreur = Er_Ecriture;
            //exit(EXIT_FAILURE);
            return;
        }
        #ifdef DEBUG
        printf("mise a %02x du GPIO\n", config);
    #endif
    }
    usleep(100);

}

/**
 * 
 * @brief    Affiche l'etat des pin sur la console
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 * 
 *  **/
void expander_printGPIO(expander_t *exp){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

       // exit(EXIT_FAILURE);
            return;
    }

/**
 * polarité du registre POL de l'expander
 **/
    // exp->buff[0] = MCP23008_IPOL;
    // exp->buff[1] = 0x00;
    // if(write(exp->fd,exp->buff,2) != 2){
        
    //     printf("ERREUR d'écriture du registre IPOL (branché sur i2c?)\n");
    //     close(exp->fd);
    //     exit(EXIT_FAILURE);
    //     exp->erreur = -1;
    // }


/**
 * Lecture du registre GPIO de l'expander
 **/
    exp->buff[0] = REG_GPIO;
    if(write(exp->fd,exp->buff,1) != 1) {
        printf("ERREUR de selection du registre GPIO pour lecture\n");
        close(exp->fd);
        exp->erreur = Er_Lecture;
       // exit(EXIT_FAILURE);
        return;
    }

    exp->buff[0] = REG_GPIO;
    if(read(exp->fd,exp->buff,1) != 1) {
        printf("ERREUR de de lecture sur GPIO\n");
        close(exp->fd);
        exp->erreur = Er_Lecture;
       // exit(EXIT_FAILURE);
        return;
    }

    usleep(1);
/**
 * Affichage des ports GPIO de l'expander
 **/
    printf("___");
    printf("Expander 0x%02x", exp->addr);
    printf("_______________\n");
    for (int i = 0; i < 8; i++)
    {
        
        printf("%s GPIO[%d] : %d\r\n",exp->label[i], i, (exp->buff[0] >> i ) & 0x01);
    }
    printf("_______________________________\n");
    putchar('\n');
    usleep(100);

}

/**
 * 
 * @brief   configure la polarité des gpio( inverser les read)
 * @param   exp pointeur sur variable structuré de l'expander
 * @param   val valeur des polarité, Si un bit est est à 1, le bit du registre GPIO correspondant reflétera la valeur inversée sur la broche.
 * 
 * 
 *  **/
void expander_polGPIO(expander_t *exp, uint8_t val){

    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);

        //exit(EXIT_FAILURE);
        return;
    }


    exp->buff[0] = MCP23008_IPOL;
    exp->buff[1] = val;
    if(write(exp->fd,exp->buff,2) != 2){
        
        printf("ERREUR d'écriture du registre IPOL (branché sur i2c?)\n");
        close(exp->fd);
        exp->erreur = Er_Ecriture;
        //exit(EXIT_FAILURE);
        return;
    }

}

/**
 * 
 * @brief   ferme l'interface i2C et libère la memoire utilisé pour exp
 * 
 * @param   exp pointeur sur variable structuré de l'expander
 * 
 * 
 *  **/
void expander_closeAndFree(expander_t *exp)
{
    if(exp == NULL || exp == 0)
    {
        printf("ERREUR fonction %s : parametre exp NULL (utiliser: expander_init())\n", __func__);
    
        return;
    }
    expander_closeI2C(exp);
    free(exp);
}
