#ifndef In_SERVO_H
#define In_SERVO_H

#include <ArduinoJson.h>

// Définition de constantes et déclarations externes
extern bool Debug_Input_Servo;
extern int data_channel_1;
extern int data_channel_2;
extern int data_channel_3;
extern int data_channel_4;
extern int data_channel_5;
extern int data_channel_6;

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

extern int light_mod_mode;       // Variable pour le mode (0 à 3)

/*======================*/
//  Gestion des entrées //
/*======================*/
// Chaque entrée peu servir à surveiller quelque chose
// mettre -1 pour non attribué, sinon mettre channel_1 ou channel_2 ou channel_3 ou ......
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

// Prototypes des fonctions
int readPWM(int pin);
String generateServoJson();
void updateChannels();
int getChannelData(int channel);

#endif
