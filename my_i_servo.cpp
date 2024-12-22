#include "my_i_servo.h"
#include <Arduino.h>
#include "config.h" // Ajout pour accéder aux constantes channel_1, channel_2, etc.


// Initialisation des variables globales
bool Debug_Input_Servo = false;

// Donnée des channels entrées !!ne pas modifier!!
int steer_data = -1;
int throttle_data = -1;
int gear_box_data = -1;
int front_diff_data = -1;
int rear_diff_data = -1;
int light_data = -1;
int winch_data = -1;
int horn_data = -1;
int brake_data= -1;
int light_mod_data = -1;

// Implémentation des fonctions
int readPWM(int pin) {
    
    unsigned long pulseWidth = pulseIn(pin, HIGH, 20000);

    if (pulseWidth > 0) {
        int scaledValue = map(pulseWidth, 1000, 2000, -100, 100);
        scaledValue = constrain(scaledValue, -100, 100);

        if (Debug_Input_Servo) {
            Serial.print("Lecture PWM sur la broche : ");
            Serial.println(pin);
            Serial.print("Largeur d'impulsion : ");
            Serial.print(pulseWidth);
            Serial.print(" µs -> Valeur convertie : ");
            Serial.println(scaledValue);
        }
        return scaledValue;
    }
    return 0; // Retourner 0 si aucune valeur n'a été lue
}

// Tableau global pour stocker les données des canaux
int data_channels[6] = {0, 0, 0, 0, 0, 0};

void updateChannels() {
    data_channels[0] = readPWM(channel_1);
    data_channels[1] = readPWM(channel_2);
    data_channels[2] = readPWM(channel_3);
    data_channels[3] = readPWM(channel_4);
    data_channels[4] = readPWM(channel_5);
    data_channels[5] = readPWM(channel_6);
}

int getChannelData(int channel) {
    switch (channel) {
        case channel_1: return data_channels[0];
        case channel_2: return data_channels[1];
        case channel_3: return data_channels[2];
        case channel_4: return data_channels[3];
        case channel_5: return data_channels[4];
        case channel_6: return data_channels[5];
        default: return 0;
    }
}

String generateServoJson() {
    JsonDocument doc;
    doc["type"] = "channels";
    doc["Ch1"] = data_channels[0];
    doc["Ch2"] = data_channels[1];
    doc["Ch3"] = data_channels[2];
    doc["Ch4"] = data_channels[3];
    doc["Ch5"] = data_channels[4];
    doc["Ch6"] = data_channels[5];

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}
