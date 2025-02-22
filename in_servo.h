#ifndef IN_SERVO_H
#define IN_SERVO_H

#include "config.h"

// Déclaration des variables de débogage
extern bool Debug_Input_Servo;

// Variables pour stocker les valeurs des canaux de la télécommande
extern int data_channel_1;
extern int data_channel_2;
extern int data_channel_3;
extern int data_channel_4;
extern int data_channel_5;
extern int data_channel_6;

// Variables associées aux différentes commandes du véhicule
extern int steer_data;
extern int throttle_data;
extern int gear_box_data;
extern int front_diff_data;
extern int rear_diff_data;
extern int light_data;
extern int winch_data;
extern int horn_data;
extern int brake_data;
extern int light_mod_data;

// Mode d'éclairage (ex: feux de croisement, route, clignotants...)
extern int light_mod_mode;
extern bool lightModLongPressDetected;

// Tableau pour stocker les valeurs des canaux de télécommande
extern int data_channels[6];

extern const int steer;
extern const int throttle;
extern const int brake;
extern const int gear_box;
extern const int front_diff;
extern const int rear_diff;
extern const int light;
extern const int winch;
extern const int horn;
extern const int light_mod;


// Lit une impulsion PWM sur une broche donnée et la convertit en une valeur exploitable.
int readPWM(int pin);
// Met à jour les valeurs des canaux en lisant les signaux PWM.
void updateChannels();
// Retourne la valeur actuelle d'un canal donné.
int getChannelData(int channel);
// Surveille l'activité de l'accélérateur pour ajuster l'état du véhicule.
void monitorThrottle() ;

// Variables pour le mode de fonctionnement du véhicule
extern VehiculeMode last_vehicule_mode; // Dernier mode connu
const char* vehiculeModeToString(VehiculeMode mode);

#endif
