# rpi-MCP23008
bibliothèque pour driver I2C GPIO expander(MCP23008) sur Linux Raspberry Pi

# Utilisation
Il faut instancier une variable de type expander_t en utilisant la fonction 
```
 expander_init(uint8_t adresse_hexa);
```
qui renvoie la structure créee

Puis utiliser les fonctions pour changer ou lire l'état des GPIO

et ne pas oublier de libérer la mémoire en appelant
```
expander_closeAndFree(expander_t e)
```
# contact
n'hésitez pas à me faire savoir d'éventuels bugs ou idée pour améliorer cette librairie
