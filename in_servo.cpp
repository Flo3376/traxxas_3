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
bool wifiSwitchLocked = false; // Verrouillage pour éviter les bascules multiples

// Tableau global pour stocker les données des canaux
int data_channels[6] = { 0, 0, 0, 0, 0, 0 };


// Variables globales pour surveiller le throttle
unsigned long last_activity_time = millis();
VehiculeMode vehicule_mode = WAIT; // Définition de la variable globale
VehiculeMode last_vehicule_mode = WAIT; // Initialisé à NORMAL par défaut


unsigned long lightModPressStart = 0;
bool lightModLongPressDetected = false;

// Surveille l'activité de l'accélérateur pour ajuster l'état du véhicule.
void monitorThrottle() {
    int current_throttle = throttle_data; // Valeur actuelle du throttle
    unsigned long current_time = millis();

    // Vérifier si la variation est significative
    if (abs(current_throttle) > DEAD_ZONE) {
        //last_significant_throttle = current_throttle; // Mise à jour de la valeur significative
        last_activity_time = current_time;           // Réinitialiser le compteur
    }


    // Calculer le temps écoulé depuis la dernière activité
    unsigned long inactivity_duration = current_time - last_activity_time;

    // Mettre à jour le statut en fonction de l'inactivité
    if (inactivity_duration > EXPO_TIMEOUT) {
      if(last_vehicule_mode!=EXPO)
       {
        vehicule_mode = EXPO;
        Serial.println("Passage en mod EXPO");
       }
        
    }
    else if (inactivity_duration > FORGET_TIMEOUT) {
       if(last_vehicule_mode!=FORGET)
       {
        vehicule_mode = FORGET;
        Serial.println("Passage en mod FORGET");
       }
        
    } else if (inactivity_duration > WAIT_TIMEOUT) {
      if(last_vehicule_mode!=WAIT)
       {
        vehicule_mode = WAIT;
        Serial.println("Passage en mod WAIT");
       }
        
    }
    else {
      if(last_vehicule_mode!=NORMAL)
       {
        vehicule_mode = NORMAL;
        Serial.println("Passage en mod NORMAL");
       }
    }
    // Mettre à jour le dernier mode connu
    last_vehicule_mode = vehicule_mode;
}

// Lit une impulsion PWM sur une broche donnée et la convertit en une valeur exploitable.
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

// Gère le mode d'éclairage en fonction des interactions.
void handleLightMod() {
    if (light_mod_data != prev_light_mod_data) {
        if (light_mod_data == 100) { // Bouton pressé
            lightModPressStart = millis();
            wifiSwitchLocked = false; // Déverrouiller au nouvel appui
            Serial.println("Appui détecté");
        } else if (light_mod_data == -100) { // Bouton relâché
            if (!lightModLongPressDetected) { // Vérifier si on n'a pas déjà détecté l’appui long
                // Changement normal du mode d'éclairage
                light_mod_mode = (light_mod_mode + 1) % 4;
                if (Debug_Input_Servo) {
                    Serial.print("Light Mod mode changé : ");
                    Serial.println(light_mod_mode);
                }
            }
            // Réinitialiser les états
            lightModLongPressDetected = false;
            wifiSwitchLocked = false; // Déverrouiller après relâchement
        }

        prev_light_mod_data = light_mod_data;
    }

    // Vérifier en continu si l'appui long est dépassé
    if (light_mod_data == 100 && millis() - lightModPressStart >= 5000 && !lightModLongPressDetected && !wifiSwitchLocked) {
        lightModLongPressDetected = true; // Marquer comme détecté
        wifiSwitchLocked = true; // Verrouiller pour éviter les boucles
        Serial.println("Appui long détecté : demande de changement Wi-Fi");

        // Basculer l'état du Wi-Fi (à gérer dans le main)
    }
}

// Met à jour les valeurs des canaux en lisant les signaux PWM.
void updateChannels() {
    data_channels[0] = readPWM(channel_1);
    data_channels[1] = readPWM(channel_2);
    data_channels[2] = readPWM(channel_3);
    data_channels[3] = readPWM(channel_4);
    data_channels[4] = readPWM(channel_5);
    data_channels[5] = readPWM(channel_6);

    steer_data = data_channels[steer];
    throttle_data = data_channels[throttle];
    brake_data = data_channels[brake];
    gear_box_data = data_channels[gear_box];
    front_diff_data = data_channels[front_diff];
    rear_diff_data = data_channels[rear_diff];
    light_data = data_channels[light];
    winch_data = data_channels[winch];
    horn_data = data_channels[horn];
    light_mod_data = data_channels[light_mod];
}

/*
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

void updateChannels() {
  data_channels[0] = readPWM(channel_1);
  data_channels[1] = readPWM(channel_2);
  data_channels[2] = readPWM(channel_3);
  data_channels[3] = readPWM(channel_4);
  data_channels[4] = readPWM(channel_5);
  data_channels[5] = readPWM(channel_6);

  // Mise à jour des données spécifiques
  updateInputData();
}*/


// Retourne la valeur actuelle d'un canal donné
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

// Convertit le mode du véhicule en une chaîne de caractères.
const char* vehiculeModeToString(VehiculeMode mode) {
    switch (mode) {
        case NORMAL: return "NORMAL";
        case WAIT: return "WAIT";
        case FORGET: return "FORGET";
        case EXPO: return "EXPO";
        default: return "UNKNOWN";
    }
}