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
const int S_1_PWM = 32;		//blanc AR Feu de recul, ne supporte pas le pwm
const int S_2_PWM = 33;		//bleu AR Stop
const int S_3_PWM = 25;		//vert AR Troisiéme feu stop
const int S_4_PWM = 26; 	//vert AV barre led, ne supporte pas le pwm
const int S_5_PWM = 27; 	//blanc AV  Phare
const int S_6_PWM = 14; 	//bleu AV Angel Eye
const int S_7_PWM = 12; 	//jaune AV AR CLIGNOTANT_DROIT
const int S_8_PWM = 13; 	//jaune AV AR CLIGNOTANT_GAUCHE
const int S_9_PWM = 2; 		//réservé buzzer

//config voiture principale
const bool american = true; 

// Variables globales
const bool debug_output = false;

/*=======================================*/
//  Configuration des roles des ampoules //
/*=======================================*/

// Configuration du buzzer
const int PIN_BUZZER = S_9_PWM;  // Broche associée
const unsigned long VITESSE_BUZZER = 500; // Intervalle en ms
const int ETAT_BAS_BUZZER = 0;  // État bas (PWM)
const int ETAT_HAUT_BUZZER = 255; // État haut (PWM)



// Configuration du clignotant gauche
const int PIN_CLIGNOTANT_GAUCHE = S_8_PWM;  // Broche associée
const unsigned long VITESSE_CLIGNOTANT_GAUCHE = 500; // Intervalle en ms
const int ETAT_BAS_CLIGNOTANT_GAUCHE = 0;  // État bas (PWM)
const int ETAT_HAUT_CLIGNOTANT_GAUCHE = 255; // État haut (PWM)
const int LVL_CLIGNOTANT_GAUCHE = -45; // Seuil de déclenchement du clinotant (supérieur à 100 = jamais)


// Configuration du clignotant droit
const int PIN_CLIGNOTANT_DROIT = S_7_PWM;
const unsigned long VITESSE_CLIGNOTANT_DROIT = 500;
const int ETAT_BAS_CLIGNOTANT_DROIT = 0;
const int ETAT_HAUT_CLIGNOTANT_DROIT = 255;
const int LVL_CLIGNOTANT_DROIT = 45; // Seuil de déclenchement du clinotant (supérieur à 100 = jamais)


// Configuration des Angel_eyes
const int PIN_ANGEL_EYES = S_6_PWM;
const unsigned long VITESSE_ANGEL_EYES = 500;
const int ETAT_BAS_ANGEL_EYES = 0;
const int ETAT_HAUT_ANGEL_EYES = 255;

// Configuration des feu avants
const int PIN_headlights = S_5_PWM;
const unsigned long VITESSE_headlights = 500;
const int ETAT_BAS_headlights = 0;
const int ETAT_HAUT_headlights = 255;

// Configuration du troisiéme feu stop
const int PIN_THIRD_BRAKE = S_3_PWM;
const unsigned long VITESSE_THIRD_BRAKE = 250;
const int ETAT_BAS_THIRD_BRAKE = 0;
const int ETAT_HAUT_THIRD_BRAKE = 255;

// Configuration des stops
const int PIN_BRAKES = S_2_PWM;
const unsigned long VITESSE_BRAKES = 250;
const int ETAT_BAS_BRAKES = 0;
const int ETAT_HAUT_BRAKES = 255;
const int LVL_BRAKES = -10;

// Configuration des feux de recul
const int PIN_BACKWARD = S_1_PWM;
const unsigned long VITESSE_BACKWARD = 250;
const int ETAT_BAS_BACKWARD = 0;
const int ETAT_HAUT_BACKWARD = 50;
const int LVL_BACKWARD = -10;

// Configuration de la barre à led
const int PIN_BAL = S_4_PWM;
const unsigned long VITESSE_BAL = 250;
const int ETAT_BAS_BAL = 0;
const int ETAT_HAUT_BAL = 255;


// Configuration des zones mortes de l'accellérateur
const bool traxxas = true; // Mode par défaut pour la marche arrière (Traxxas = true ou Axial = false)
const int DEAD_ZONE = 3; // Plage de tolérance autour de zéro




/*======================*/
//  Info  pour le mod 1 //
/*======================*/
constexpr const char* car_name = "Hummer_Bob";
constexpr const char* version_soft = "A1.5.0";
constexpr const int input_u = 6;
constexpr const int output_u = 9;


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
//    Sortie alarm     //
/*======================*/
const bool hp_sound = false;
const int hp_output = aux_serv_1;

/*======================*/
//  Gestion des entrées //
/*======================*/
// Chaque entrée peu servir à surveiller quelque chose
// mettre -1 pour non attribué, sinon mettre channel_1 ou channel_2 ou channel_3 ou ......
const int steer = channel_3;
const int throttle = channel_2;
const int brake = channel_2;
const int gear_box = -1;
const int front_diff = -1;
const int rear_diff = -1;
const int light = -1;
const int winch = -1;
const int horn = -1;
const int light_mod= channel_1;

/*======================*/
//     Mods veilles     //
/*======================*/
// Délais pour les modes veilles
const unsigned long WAIT_TIMEOUT = 10000;  // 10 secondes
const unsigned long FORGET_TIMEOUT = 120000; // 2 minutes
const unsigned long EXPO_TIMEOUT = 300000; // 5 minutes

// États possibles de la voiture
enum VehiculeMode { NORMAL, WAIT, FORGET, EXPO };
extern VehiculeMode vehicule_mode;
extern VehiculeMode last_vehicule_mode;

/*=======================================*/
//    différent timer (ne pas toucher)   //
/*=======================================*/
//le programme a des taches qui s'execute à différente cadence (boucle lente/rapide/moyenne)
const unsigned long Interval_50= 50;          //50 ms
const unsigned long Interval_125= 125;        //125 ms
const unsigned long Interval_250= 250;        //250 ms
const unsigned long Interval_500= 500;        //500 ms
const unsigned long Interval_1000= 1000;      // 1 s
const unsigned long Interval_5000= 1000;      // 5 s
const unsigned long Interval_10000= 10000;    //10 s

#endif
