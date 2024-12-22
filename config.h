#ifndef CONFIG_H
#define CONFIG_H

// Configuration Wi-Fi : Définir le SSID et le mot de passe
#define WIFI_SSID "Redmi Note 13 Pro 5G"
#define WIFI_PASSWORD "uheku675"
//#define WIFI_SSID "NG_XR500_5GZ"
//#define WIFI_PASSWORD "YsfW6wB89j6pkAB9C3jSef749H8cUV"
//#define MDNS_NAME "bobinette_the_esp_killeuse"
// Configuration IP statique (si nécessaire)
//#define WIFI_STATIC_IP IPAddress(192, 168, 127, 100)
//#define WIFI_GATEWAY IPAddress(192, 168, 127, 1)
//#define WIFI_SUBNET IPAddress(255, 255, 255, 0)






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



// Configuration du clignotant gauche
const int PIN_CLIGNOTANT_GAUCHE = S_2_PWM;  // Broche associée
const unsigned long VITESSE_CLIGNOTANT_GAUCHE = 500; // Intervalle en ms
const int ETAT_BAS_CLIGNOTANT_GAUCHE = 10;  // État bas (PWM)
const int ETAT_HAUT_CLIGNOTANT_GAUCHE = 255; // État haut (PWM)

// Configuration du clignotant droit
const int PIN_CLIGNOTANT_DROIT = S_3_PWM;
const unsigned long VITESSE_CLIGNOTANT_DROIT = 500;
const int ETAT_BAS_CLIGNOTANT_DROIT = 10;
const int ETAT_HAUT_CLIGNOTANT_DROIT = 255;

// Configuration des Angel_eyes
const int PIN_ANGEL_EYES = S_8_PWM;
const unsigned long VITESSE_ANGEL_EYES = 500;
const int ETAT_BAS_ANGEL_EYES = 5;
const int ETAT_HAUT_ANGEL_EYES = 128;

// Configuration du troisiéme feu stop
const int PIN_THIRD_BRAKE = S_4_PWM;
const int ETAT_BAS_THIRD_BRAKE = 5;
const int ETAT_HAUT_THIRD_BRAKE = 128;

// Configuration des stops
const int PIN_BRAKES = S_1_PWM;
const int ETAT_BAS_BRAKES = 5;
const int ETAT_HAUT_BRAKES = 255;

// Configuration des feu avants
const int PIN_HEADLIGHTS = S_6_PWM;
const int ETAT_BAS_HEADLIGHTS = 5;
const int ETAT_HAUT_HEADLIGHTS = 255;



/*======================*/
//  Info  pour le mod 1 //
/*======================*/
constexpr const char* car_name = "Hummer_Bob";
constexpr const char* version_soft = "0.4.75";
constexpr const int input_u = 4;
constexpr const int output_u = 8;


/*======================*/
//    Entrées servo     //
/*======================*/
// Entée servo pour écouter la télécommande !!ne pas modifier!!
/*const int channel_1 = 19;
const int channel_2 = 18;
const int channel_3 = 5;*/
const int channel_1 = 36;
const int channel_2 = 39;
const int channel_3 = 34;
const int channel_4 = 17;
const int channel_5 = 16;
const int channel_6 = 4;

/*======================*/
//  Gestion des entrées //
/*======================*/
// Chaque entrée peu servir à surveiller quelque chose
// mettre -1 pour non attribué, sinon mettre channel_1 ou channel_2 ou channel_3 ou ......
const int steer = channel_6;
const int throttle = channel_5;
const int brake = channel_5;
const int gear_box = channel_2;
const int front_diff = -1;
const int rear_diff = -1;
const int light = -1;
const int winch = -1;
const int horn = -1;
const int light_mod= channel_3;


//info sur la largeur d'impulsion utilisé par la télécommande pour utiliser un servo
const int minPulse = 1000;
const int maxPulse = 2000;




#endif
