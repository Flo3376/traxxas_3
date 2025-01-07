#ifndef CONFIG_H
#define CONFIG_H

/*======================*/
//       Wifi           //
/*======================*/
#define WIFI_SSID "Redmi Note 13 Pro 5G"
#define WIFI_PASSWORD "uheku675"
//#define WIFI_SSID "NG_XR500_5GZ"
//#define WIFI_PASSWORD "YsfW6wB89j6pkAB9C3jSef749H8cUV"


/*======================*/
//  Gestion des Sorties //
/*======================*/
// sortie leds et buzzers !!ne pas modifier!!
const int S_1_PWM = 32;
const int S_2_PWM = 33;
const int S_3_PWM = 25;
const int S_4_PWM = 26;
const int S_5_PWM = 27;
const int S_6_PWM = 14;
const int S_7_PWM = 12;
const int S_8_PWM = 13;

//config voiture principale
const bool american = true; 

// Variables globales
const bool debug_output = false;

/*=======================================*/
//  Configuration des roles des ampoules //
/*=======================================*/
// Configuration du clignotant gauche
const int PIN_CLIGNOTANT_GAUCHE = S_8_PWM;  // Broche associée
const unsigned long VITESSE_CLIGNOTANT_GAUCHE = 500; // Intervalle en ms
const int ETAT_BAS_CLIGNOTANT_GAUCHE = 10;  // État bas (PWM)
const int ETAT_HAUT_CLIGNOTANT_GAUCHE = 255; // État haut (PWM)
const int LVL_CLIGNOTANT_GAUCHE = -45; // Seuil de déclenchement du clinotant (supérieur à 100 = jamais)


// Configuration du clignotant droit
const int PIN_CLIGNOTANT_DROIT = S_7_PWM;
const unsigned long VITESSE_CLIGNOTANT_DROIT = 500;
const int ETAT_BAS_CLIGNOTANT_DROIT = 10;
const int ETAT_HAUT_CLIGNOTANT_DROIT = 255;
const int LVL_CLIGNOTANT_DROIT = 45; // Seuil de déclenchement du clinotant (supérieur à 100 = jamais)


// Configuration des Angel_eyes
const int PIN_ANGEL_EYES = S_2_PWM;
const unsigned long VITESSE_ANGEL_EYES = 500;
const int ETAT_BAS_ANGEL_EYES = 5;
const int ETAT_HAUT_ANGEL_EYES = 128;

// Configuration du troisiéme feu stop
const int PIN_THIRD_BRAKE = S_6_PWM;
const int ETAT_BAS_THIRD_BRAKE = 5;
const int ETAT_HAUT_THIRD_BRAKE = 255;

// Configuration des stops
const int PIN_BRAKES = S_5_PWM;
const int ETAT_BAS_BRAKES = 5;
const int ETAT_HAUT_BRAKES = 255;
const int LVL_BRAKES = -10;

// Configuration des feu avants
const int PIN_HEADLIGHTS = S_1_PWM;
const int ETAT_BAS_HEADLIGHTS = 5;
const int ETAT_HAUT_HEADLIGHTS = 255;



/*======================*/
//  Info  pour le mod 1 //
/*======================*/
constexpr const char* car_name = "Hummer_Bob";
constexpr const char* version_soft = "0.7.0";
constexpr const int input_u = 4;
constexpr const int output_u = 8;


/*======================*/
//    Entrées servo     //
/*======================*/
// Entée servo pour écouter la télécommande !!ne pas modifier!!
const int channel_1 = 36;
const int channel_2 = 39;
const int channel_3 = 34;
const int channel_4 = 17;
const int channel_5 = 16;
const int channel_6 = 4;

/*======================*/
//    Sortie servo     //
/*======================*/
// Sortie servo pour piloter des servos aux !!ne pas modifier!!
const int aux_serv_1 = 19;
const int aux_serv_2 = 18;
const int aux_serv_3 = 5;

//info sur la largeur d'impulsion utilisé par la télécommande pour utiliser un servo
const int minPulse = 1000;
const int maxPulse = 2000;


/*======================*/
//  Gestion des entrées //
/*======================*/
// Chaque entrée peu servir à surveiller quelque chose
// mettre -1 pour non attribué, sinon mettre channel_1 ou channel_2 ou channel_3 ou ......
const int steer = channel_3;
const int throttle = channel_1;
const int brake = channel_1;
const int gear_box = -1;
const int front_diff = -1;
const int rear_diff = -1;
const int light = -1;
const int winch = -1;
const int horn = -1;
const int light_mod= channel_2;

#endif
