#ifndef CONFIG_H
#define CONFIG_H

// Configuration Wi-Fi : Définir le SSID et le mot de passe
#define WIFI_SSID "Redmi Note 13 Pro 5G"
#define WIFI_PASSWORD "uheku675"
//#define MDNS_NAME "bobinette_the_esp_killeuse"
// Configuration IP statique (si nécessaire)
//#define WIFI_STATIC_IP IPAddress(192, 168, 127, 100)
//#define WIFI_GATEWAY IPAddress(192, 168, 127, 1)
//#define WIFI_SUBNET IPAddress(255, 255, 255, 0)




/*======================*/
//  Gestion des Sorties //
/*======================*/
// sortie leds et buzzers !!ne pas modifier!!
const int S_1_PWM = 19;
const int S_2_PWM = 18;
const int S_3_PWM = 5;
const int S_4_PWM = 17;
const int S_5_PWM = 16;
const int S_6_PWM = 4;
const int S_7_PWM = 2;
const int S_8_PWM = 15;



// Configuration du clignotant gauche
const int PIN_CLIGNOTANT_GAUCHE = S_2_PWM;  // Broche associée
const unsigned long VITESSE_CLIGNOTANT_GAUCHE = 500; // Intervalle en ms
const int ETAT_BAS_CLIGNOTANT_GAUCHE = 25;  // État bas (PWM)
const int ETAT_HAUT_CLIGNOTANT_GAUCHE = 255; // État haut (PWM)

// Configuration du clignotant droit
const int PIN_CLIGNOTANT_DROIT = S_3_PWM;
const unsigned long VITESSE_CLIGNOTANT_DROIT = 500;
const int ETAT_BAS_CLIGNOTANT_DROIT = 25;
const int ETAT_HAUT_CLIGNOTANT_DROIT = 255;

// Configuration des Angel_eyes
const int PIN_ANGEL_EYES = S_8_PWM;
const unsigned long VITESSE_ANGEL_EYES = 500;
const int ETAT_BAS_ANGEL_EYES = 0;
const int ETAT_HAUT_ANGEL_EYES = 128;




#endif
