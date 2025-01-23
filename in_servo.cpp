#include "in_servo.h"
#include <Arduino.h>
#include "config.h"  // Ajout pour accéder aux constantes channel_1, channel_2, etc.


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
int brake_data = -1;
int light_mod_data = -1;
int prev_light_mod_data = 0;
int light_mod_mode = 0; // Définition de la variable

// Variables globales pour surveiller le throttle
int last_significant_throttle = 0;
unsigned long last_activity_time = millis();
VehicleMode vehicle_mode = WAIT; // Définition de la variable globale

void monitorThrottle() {
    int current_throttle = throttle_data; // Valeur actuelle du throttle
    unsigned long current_time = millis();

    // Vérifier si la variation est significative
    if (abs(current_throttle - last_significant_throttle) > 5) {
        last_significant_throttle = current_throttle; // Mise à jour de la valeur significative
        last_activity_time = current_time;           // Réinitialiser le compteur
    }

    // Calculer le temps écoulé depuis la dernière activité
    unsigned long inactivity_duration = current_time - last_activity_time;

    // Mettre à jour le statut en fonction de l'inactivité
    if (inactivity_duration > FORGET_TIMEOUT) {
        vehicle_mode = FORGET;
    } else if (inactivity_duration > WAIT_TIMEOUT) {
        vehicle_mode = WAIT;
    } else {
        vehicle_mode = NORMAL;
    }
}


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
  return 0;  // Retourner 0 si aucune valeur n'a été lue
}

void handleLightMod() {
    // Si l'état actuel est différent de l'état précédent
    if (light_mod_data != prev_light_mod_data) {
        // Vérifier que le bouton est en position "appuyé" (100)
        if (light_mod_data == 100) {
            // Incrémenter le mode avec retour à 0 après 3
            light_mod_mode = (light_mod_mode + 1) % 4;

            // Afficher le mode actuel pour debug
            if (Debug_Input_Servo) {
                Serial.print("Light Mod mode changé : ");
                Serial.println(light_mod_mode);
            }
        }

        // Mettre à jour la valeur précédente
        prev_light_mod_data = light_mod_data;
    }
}


void updateInputData() {
  // Associer les données des canaux aux rôles définis dans config.h
  steer_data = (steer != -1) ? getChannelData(steer) : 0;
  throttle_data = (throttle != -1) ? getChannelData(throttle) : 0;
  brake_data = (brake != -1) ? getChannelData(brake) : 0;
  gear_box_data = (gear_box != -1) ? getChannelData(gear_box) : 0;
  front_diff_data = (front_diff != -1) ? getChannelData(front_diff) : 0;
  rear_diff_data = (rear_diff != -1) ? getChannelData(rear_diff) : 0;
  light_data = (light != -1) ? getChannelData(light) : 0;
  winch_data = (winch != -1) ? getChannelData(winch) : 0;
  horn_data = (horn != -1) ? getChannelData(horn) : 0;
  light_mod_data = (light_mod != -1) ? getChannelData(light_mod) : 0;

  // Surveiller le changement de mode light_mod
  handleLightMod();
}

// Tableau global pour stocker les données des canaux
int data_channels[6] = { 0, 0, 0, 0, 0, 0 };

void updateChannels() {
  data_channels[0] = readPWM(channel_1);
  data_channels[1] = readPWM(channel_2);
  data_channels[2] = readPWM(channel_3);
  data_channels[3] = readPWM(channel_4);
  data_channels[4] = readPWM(channel_5);
  data_channels[5] = readPWM(channel_6);

  // Mise à jour des données spécifiques
  updateInputData();
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
