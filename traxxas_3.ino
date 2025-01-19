#include "WiFiWebSocket.h"
#include <ArduinoJson.h>
#include "Simulation.h"
#include "Ampoules.h"
#include "config.h"
#include "gyro.h"
#include "in_servo.h"
#include "out_servo.h"
#include <queue>
#include <ESP32Servo.h>  // Bibliothèque spécifique à l'ESP32

bool blink = false;                       // Variable globale pour gérer la synchronisation
unsigned long lastBlinkChange = 0;        // Chronomètre pour l'état de blink
const unsigned long blinkInterval = 500;  // Intervalle de changement (ms)

bool hasBraked = false;        // Indique si un freinage a été effectué (Traxxas seulement)
bool possibleReverse = false;  // Indique si la marche arrière est possible après un freinage


//mod de transmition au démarrage 0-pour silence  1-pour le systéme 2-pour le gyro / 3-pour les servos / 4-pour les sortie
int transmit_mod = 1;


// Initialisation des clignotants
Ampoule clignotantGauche(PIN_CLIGNOTANT_GAUCHE, ETAT_BAS_CLIGNOTANT_GAUCHE, ETAT_HAUT_CLIGNOTANT_GAUCHE,
                         "Clignotant Gauche", "CLI", 0, VITESSE_CLIGNOTANT_GAUCHE);
Ampoule clignotantDroit(PIN_CLIGNOTANT_DROIT, ETAT_BAS_CLIGNOTANT_DROIT, ETAT_HAUT_CLIGNOTANT_DROIT,
                        "Clignotant Droit", "CLI", 0, VITESSE_CLIGNOTANT_DROIT);

// Initialisation de l'angel eyes
Ampoule angelEyes(PIN_ANGEL_EYES, ETAT_BAS_ANGEL_EYES, ETAT_HAUT_ANGEL_EYES,
                  "Angel Eyes", "PWM", VITESSE_ANGEL_EYES);

// Initialisation du troisième feu stop
Ampoule third_brake(PIN_THIRD_BRAKE, ETAT_BAS_THIRD_BRAKE, ETAT_HAUT_THIRD_BRAKE,
                    "Troisième Feu Stop", "PWM", VITESSE_THIRD_BRAKE);

// Initialisation du feu stop
Ampoule brakes(PIN_BRAKES, ETAT_BAS_BRAKES, ETAT_HAUT_BRAKES,
               "Feu Stop", "PWM", VITESSE_BRAKES);

// Initialisation des feux avant
Ampoule HEADLIGHTS(PIN_HEADLIGHTS, ETAT_BAS_HEADLIGHTS, ETAT_HAUT_HEADLIGHTS,
                   "Feux Avant", "PWM", VITESSE_HEADLIGHTS);

// Initialisation du buzzer
Ampoule BUZZER_WARNING(PIN_BUZZER, ETAT_BAS_BUZZER, ETAT_HAUT_BUZZER, "Buzzer", "BUZ", 0, VITESSE_BUZZER);

// Initialisation du feu de recul
Ampoule BACKWARD(PIN_BACKWARD, ETAT_BAS_BACKWARD, ETAT_HAUT_BACKWARD, "Feu de Recul", "PWM", 0, VITESSE_BACKWARD);




// Objet WiFiWebSocket avec les constantes définies dans config.h
WiFiWebSocket wifiWebSocket(WIFI_SSID, WIFI_PASSWORD);

bool tilted = false;  //si la voiture est en trop penchée

unsigned long previousMillis = 0;  // Gestion du temps pour le `loop`
const unsigned long interval = 250;

unsigned long previousMillis_5s = 0;  // Gestion du temps pour le `loop`
const unsigned long interval_5s = 2500;

int angle = 0;       // Angle actuel des servos
int increment = 45;  // Incrément d'angle (1 pour avancer, -1 pour reculer)

// Déclaration des servos
CustomServo* servo1;
CustomServo* servo2;
CustomServo* servo3;

void setup() {
  delay(2500);
  // Initialisation des communications
  Serial.begin(115200);
  Serial.println("demarrage");

  //initialisation des servos (sorties)
  servo1 = new CustomServo(aux_serv_1, 500, 2400, 0);  // Pin 14, position initiale 0
  servo2 = new CustomServo(aux_serv_2, 500, 2400, 0);  // Pin 15, position initiale 0
  servo3 = new CustomServo(aux_serv_3, 500, 2400, 0);  // Pin 16, position initiale 0

  wifiWebSocket.start();  // Démarre Wi-Fi et WebSocket
  setupGyro();            // Initialisation du gyroscope

  pinMode(channel_1, INPUT);
  pinMode(channel_2, INPUT);
  pinMode(channel_3, INPUT);
  pinMode(channel_4, INPUT);
  pinMode(channel_5, INPUT);
  pinMode(channel_6, INPUT);
  pinMode(S_9_PWM, OUTPUT);

  if (hp_sound) {
    servo1->jumpTo(180);
    delay(250);
    servo1->jumpTo(180);
    delay(100);
    servo1->jumpTo(180);
    delay(250);
    servo1->jumpTo(180);
    delay(100);
    servo1->jumpTo(180);
    delay(250);
    servo1->jumpTo(180);
  }




  /*=====================*/
  /*test des sortie pwm  */
  /*=====================*/
  const int S_PWM[] = {
    S_1_PWM,
    S_2_PWM,
    S_3_PWM,
    S_4_PWM,
    S_5_PWM,
    S_6_PWM,
    S_7_PWM,
    S_8_PWM,
  };                                                       // Tableau des pins PWM
  const int PWM_COUNT = sizeof(S_PWM) / sizeof(S_PWM[0]);  // Nombre total de pins PWM
  for (int i = 0; i < PWM_COUNT; i++) {
    analogWrite(S_PWM[i], 255);  // Allumer la PWM au maximum
    delay(500);                  // Pause de 500 ms
    analogWrite(S_PWM[i], 0);    // Éteindre la PWM
    delay(500);                  // Pause de 500 ms
  }
  digitalWrite(S_9_PWM, HIGH);  // Éteindre la PWM
  delay(250);
  digitalWrite(S_9_PWM, LOW);
}

unsigned long lastReconnectCheck = 0;          // Dernière vérification de la connexion Wi-Fi
const unsigned long reconnectInterval = 5000;  // Intervalle de 5 secondes pour tenter une reconnexion

void loop() {
  unsigned long currentMillis = millis();
  // Mise à jour de la variable blink toutes les 500 ms
  if (currentMillis - lastBlinkChange >= blinkInterval) {
    lastBlinkChange = currentMillis;
    blink = !blink;  // Alterner entre true et false
  }

  third_brake.update(false);
  delay(20);
  brakes.update(false);
  delay(20);
  BUZZER_WARNING.update(blink);
  delay(20);
  clignotantGauche.update(blink);
  delay(20);
  clignotantDroit.update(blink);
  delay(20);
  BACKWARD.update(false);
  delay(20);


  // Boucle lente
  static unsigned long lastSlowLoop = 0;
  const unsigned long slowInterval = 10000;  // 10 secondes
  if (currentMillis - lastSlowLoop >= slowInterval) {
    lastSlowLoop = currentMillis;


    if (transmit_mod == 1) {

      JsonDocument doc;
      doc["type"] = "info";
      doc["name"] = car_name;
      doc["version"] = version_soft;
      doc["localip"] = WiFi.localIP().toString();
      doc["uptime"] = millis() / 1000;
      doc["ssid"] = WIFI_SSID;
      doc["rssi"] = WiFi.RSSI();
      doc["output"] = output_u;
      doc["input"] = input_u;


      String jsonString;
      serializeJson(doc, jsonString);
      wifiWebSocket.sendData(jsonString);


      //String data_to_send = generateGyroJson();
      //wifiWebSocket.sendData(data_to_send);
    }
  }

  // Boucle moyenne
  static unsigned long lastMediumLoop = 0;
  const unsigned long mediumInterval = 250;  // 250 ms
  if (currentMillis - lastMediumLoop >= mediumInterval) {
    lastMediumLoop = currentMillis;
    updateChannels();
    readGyro();
    if (transmit_mod == 2) {
      String data_to_send = generateGyroJson();
      wifiWebSocket.sendData(data_to_send);
    }
    if (transmit_mod == 4) {
      String jsonData = generateServoJson();
      wifiWebSocket.sendData(jsonData);
    }

    /*=======================================*/
    /*          surveillance gyro            */
    /* si le 4x4 atteint un angle trop élevé */
    /*=======================================*/

    if (abs(roll) > abs(limit_g_x) || abs(pitch) > abs(limit_g_y)) {
      tilted = true;

      //Serial.println("tilted");


      clignotantGauche.run();
      clignotantDroit.run();
      if (!hp_sound) {
        BUZZER_WARNING.run();
        //Serial.println("BUZZER");
      } else {
        servo1->jumpTo(180);
        delay(250);
        servo1->jumpTo(0);
        delay(100);
      }
    } else {
      tilted = false;
      if (!hp_sound) {
        BUZZER_WARNING.stop();
      } else {
        servo1->jumpTo(0);
      }
    }

    /*=======================================*/
    /*          surveillance cligno          */
    /*   tourne à gauche ou a droite         */
    /*=======================================*/

    if (!tilted) {
      if (steer_data > LVL_CLIGNOTANT_DROIT) {
        clignotantDroit.run();
        if (debug_output) {
          Serial.print(steer_data);
          Serial.print(" >  ");
          Serial.print(LVL_CLIGNOTANT_DROIT);
          Serial.println("    DROIT");
        }
      } else {
        clignotantDroit.stop();
      }
      if (steer_data < LVL_CLIGNOTANT_GAUCHE) {
        clignotantGauche.run();
        if (debug_output) {
          Serial.print(steer_data);
          Serial.print(" <  ");
          Serial.print(LVL_CLIGNOTANT_GAUCHE);
          Serial.println("    GAUCHE");
        }
      } else {
        clignotantGauche.stop();
      }
    }

    /*=======================================*/
    /*          surveillance frein           */
    /*            et accélérrateur           */
    /*=======================================*/
    /*if (throttle_data < LVL_BRAKES) {
      third_brake.run();
      brakes.run();
      if (debug_output) {
        Serial.print(throttle_data);
        Serial.print(" <  ");
        Serial.print(LVL_BRAKES);
        Serial.println("    stop on");
      }
    } else {
      third_brake.stop();
      brakes.stop();

      if (debug_output) {
        Serial.print(throttle_data);
        Serial.print(" >  ");
        Serial.print(LVL_BRAKES);
        Serial.println("    stop off");
      }
    }*/

    if (!traxxas) {
      // Mode Axial
      if (throttle_data > DEAD_ZONE) {
        third_brake.stop();
        brakes.stop();
        BACKWARD.stop();  // Pas de marche arrière
        Serial.println("Axial : Avancer");
        if(!tilted){BUZZER_WARNING.stop();}
      } else if (throttle_data >= -DEAD_ZONE && throttle_data <= DEAD_ZONE) {
        third_brake.run();
        brakes.run();
        BACKWARD.stop();
        Serial.println("Axial : Frein");
        if(!tilted){BUZZER_WARNING.stop();}
      } else {  // throttle_data < -DEAD_ZONE
        third_brake.stop();
        brakes.stop();
        BACKWARD.run();  // Marche arrière directe
        BUZZER_WARNING.run();
        Serial.println("Axial : Reculer");
      }
    } else {

      // Mode Traxxas
      if (throttle_data > DEAD_ZONE) {
        // Avancer, réinitialiser tous les états
        hasBraked = false;
        possibleReverse = false;
        third_brake.stop();
        brakes.stop();
        BACKWARD.stop();
        if(!tilted){BUZZER_WARNING.stop();}
        Serial.println("Traxxas : Avancer");
      } else if (throttle_data >= -DEAD_ZONE && throttle_data <= DEAD_ZONE) {
        // Décélération (pas de frein)
        if (hasBraked) {
          possibleReverse = true;  // Autorise la marche arrière après relâchement du frein
          hasBraked = false;       // Réinitialise le freinage pour attendre une nouvelle impulsion
          Serial.println("Traxxas : Frein relâché, marche arrière possible");
        }
        third_brake.stop();
        brakes.stop();
        BACKWARD.stop();
        if(!tilted){BUZZER_WARNING.stop();}
        Serial.println("Traxxas : Décélérer");
      } else if(throttle_data < -DEAD_ZONE){  // throttle_data < -DEAD_ZONE
        if (!hasBraked && !possibleReverse) {
          // Première impulsion : Frein
          hasBraked = true;
          third_brake.run();
          brakes.run();
          BACKWARD.stop();
          if(!tilted){BUZZER_WARNING.stop();}
          
          Serial.println("Traxxas : Freiner");
        } else if (possibleReverse) {
          // Seconde impulsion : Marche arrière
          //hasBraked = false;
          //possibleReverse = false;  // Marche arrière effectuée, réinitialiser les états
          third_brake.stop();
          brakes.stop();
          BACKWARD.run();
          BUZZER_WARNING.run();
          Serial.println("Traxxas : Reculer");
          
        }
      }
    }

    /*=======================================*/
    /*    surveillance commodo éclairage     */
    /*  off/veilleuse/pahre/plein phare      */
    /*=======================================*/
    if (debug_output) {
      Serial.print("Mode actuel de Light Mod : ");
      Serial.println(light_mod_mode);
    }
    switch (light_mod_mode) {
      case 0:  // Mode 0 : Tout éteint
        HEADLIGHTS.setEtatBas(0);
        HEADLIGHTS.stop();
        angelEyes.stop();
        brakes.setEtatBas(0);
        clignotantGauche.setEtatBas(0);  // Feu de position uniquement si américain
        clignotantDroit.setEtatBas(0);

        break;

      case 1:  // Mode 1 : Veilleuses
        angelEyes.run();
        brakes.setEtatBas(128);                           // 50% pour feu stop
        HEADLIGHTS.setEtatBas(77);                        // 30% pour les phares
        clignotantGauche.setEtatBas(american ? 128 : 0);  // Feu de position uniquement si américain
        clignotantDroit.setEtatBas(american ? 128 : 0);
        break;

      case 2:  // Mode 2 : Phares
        angelEyes.run();
        brakes.setEtatBas(128);                           // 50% pour feu stop
        HEADLIGHTS.setEtatBas(153);                       // 60% pour les phares
        clignotantGauche.setEtatBas(american ? 128 : 0);  // Feu de position uniquement si américain
        clignotantDroit.setEtatBas(american ? 128 : 0);
        break;

      case 3:  // Mode 3 : Plein phares
        angelEyes.run();
        brakes.setEtatBas(128);                           // 50% pour feu stop
        HEADLIGHTS.run();                                 // 100% pour les phares
        clignotantGauche.setEtatBas(american ? 128 : 0);  // Feu de position uniquement si américain
        clignotantDroit.setEtatBas(american ? 128 : 0);
        break;
    }
  }


  // Boucle rapide
  static unsigned long lastFastLoop = 0;
  const unsigned long fastInterval = 50;  // 50 ms
  if (currentMillis - lastFastLoop >= fastInterval) {


    lastFastLoop = currentMillis;


    wifiWebSocket.handle();

    if (!messageQueue.empty()) {
      Serial.println("message dans la queue");
      handleMessage();
    }
  }
}


void handleMessage() {
  // Vérifie si la file d'attente contient un message
  if (messageQueue.empty()) {

    return;
  }

  // Récupérer le premier message de la file d'attente
  String message = messageQueue.front();
  messageQueue.pop();  // Retire immédiatement le message pour éviter des doublons

  // Affiche le message pour le débogage
  Serial.println("Traitement du message : " + message);

  // Parse le message en JSON
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.println("Erreur lors de la lecture du JSON : " + String(error.c_str()));
    return;
  }

  // Vérifie si le message contient un champ "type"
  if (doc["type"].isNull()) {
    Serial.println("Type de message non spécifié.");
    return;
  }

  String type = doc["type"];

  if (type == "ping") {
    Serial.println("Ping reçu !");
    wifiWebSocket.sendData("{\"type\":\"pong\"}");

  } else if (type == "traxxas") {
    Serial.println("Identification demandée !");
    wifiWebSocket.sendData("{\"type\":\"Axial\"}");

  } else if (type == "command") {
    if (!doc["output"].isNull() && !doc["value"].isNull()) {
      String output = doc["output"];
      int value = doc["value"];

      Serial.println("Commande de forçage reçue : " + output + " -> " + String(value));

      // Identifier la sortie et appliquer le forçage
      if (output == "clignotantGauche") {
        clignotantGauche.forceOutput(value);
      } else if (output == "clignotantDroit") {
        clignotantDroit.forceOutput(value);
      } else if (output == "angelEyes") {
        angelEyes.forceOutput(value);
      } else if (output == "third_brake") {
        third_brake.forceOutput(value);
      } else if (output == "brakes") {
        brakes.forceOutput(value);
      } else if (output == "HEADLIGHTS") {
        HEADLIGHTS.forceOutput(value);
      } else {
        Serial.println("Sortie inconnue : " + output);
      }
    } else if (!doc["command"].isNull()) {
      String command = doc["command"];
      Serial.println("Commande standard reçue : " + command);

      if (command == "start") {
        Serial.println("Démarrage de la simulation !");
        //simulation.start();
      } else if (command == "stop") {
        Serial.println("Arrêt de la simulation !");
        //simulation.stop();
      } else if (command == "reset_gyro") {
        resetGyro();
      } else {
        Serial.println("Commande inconnue : " + command);
      }
    } else {
      Serial.println("Commande mal formatée.");
    }

  } else if (type == "mod") {

    if (!doc["mod_req"].isNull()) {
      String mod_req = doc["mod_req"];
      // Vérifie que mod_req contient uniquement des chiffres
      bool isNumeric = true;
      for (int i = 0; i < mod_req.length(); i++) {
        if (!isdigit(mod_req[i])) {
          isNumeric = false;
          break;
        }
      }

      if (isNumeric) {
        transmit_mod = mod_req.toInt();                                  // Convertir String en int avant assignation
        Serial.println("Mod reçu et valide : " + String(transmit_mod));  // Conversion explicite en String pour l'affichage
      } else {
        Serial.println("Erreur : 'mod' contient des caractères non numériques !");
      }
    } else {
      Serial.println("Clé 'mod_req' manquante !");
    }

  } else {
    Serial.println("Type de message non reconnu : " + type);
  }
}
