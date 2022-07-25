#include <stdio.h>
#include <stdlib.h>
#include "MCP23017.h"

int main(){
  // on déclare une instance qui pilotera l'expander d'adresse 0x20
  expander_t* expander = expander_init(0x20);
  //on met a 1 le pin 0 de l'expander 0x20
  expander_setPinGPIO(expander, 0);
  
  // on libère la mémoire allouée
  expander_closeAndFree(expander);
  
  return EXIT_SUCCESS;
}
