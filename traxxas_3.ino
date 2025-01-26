/*======================*/
//     Lib Externe      //
/*======================*/
#include <ESP32Servo.h>   // Lib spécifique à l'ESP32
#include <queue>          // Lib pour les message en file d'attende
#include <ArduinoJson.h>  // LiB pour l'utilisation du format JSON

/*======================*/
//  Dépendance interne  //
/*======================*/
#include "Simulation.h"
#include "Ampoules.h"
#include "config.h"
#include "gyro.h"
#include "in_servo.h"
#include "out_servo.h"
#include "WiFiWebSocket.h"


bool hasBraked = false;        // Indique si un freinage a été effectué (Traxxas seulement)
bool possibleReverse = false;  // Indique si la marche arrière est possible après un freinage
bool blink = false;            // Variable globale pour gérer la synchronisation
bool blink_degraded = false;   // Variable globale pour gérer la synchronisation
bool tilted = false;           // si la voiture est en trop penchée
bool use_wifi = false;         // activation ou non du wifi dés de démarrage de l'esp
int transmit_mod = 1;          // mod de transmition au démarrage 0-pour silence  1-pour le systéme 2-pour le gyro / 3-pour les servos / 4-pour les sortie

unsigned long previousMillis = 0;      // Gestion du temps pour le `loop`
unsigned long previousMillis_5s = 0;   // Gestion du temps pour le `loop`
unsigned long lastBlinkChange = 0;     // Chronomètre pour l'état de blink
unsigned long lastReconnectCheck = 0;  // Dernière vérification de la connexion Wi-Fi

/*=====================================*/
/*  Déclaration des servos (sorties)   */
/*=====================================*/
CustomServo* servo1;
CustomServo* servo2;
CustomServo* servo3;

/*=============================*/
/*  Déclaration des ampoules   */
/*=============================*/
Ampoule clignotantGauche;
Ampoule clignotantDroit;
Ampoule angelEyes;
Ampoule third_brake;
Ampoule brakes;
Ampoule headlights;
Ampoule BUZZER_WARNING;
Ampoule backward;
Ampoule b_led;

/*=============================*/
/*     Déclaration u wifi      */
/*=============================*/
WiFiWebSocket wifiWebSocket(WIFI_SSID, WIFI_PASSWORD);  // Objet WiFiWebSocket avec les constantes définies dans config.h

void setup() {
  /*=============================*/
  /*  Intialisation du port com  */
  /*=============================*/
  Serial.begin(115200);
  Serial.println("demarrage");

  /*=============================*/
  /*    Intialisation du wifi    */
  /*=============================*/
  if (use_wifi) {
    wifiWebSocket.start();  // Démarre Wi-Fi et WebSocket
  }

  /*=============================*/
  /* Intialisation des ampoules  */
  /*=============================*/
  clignotantGauche.init(PIN_CLIGNOTANT_GAUCHE, ETAT_BAS_CLIGNOTANT_GAUCHE, ETAT_HAUT_CLIGNOTANT_GAUCHE, "Clignotant Gauche", "CLI", 0, VITESSE_CLIGNOTANT_GAUCHE);  // Initialisation du clignotant gauche
  clignotantDroit.init(PIN_CLIGNOTANT_DROIT, ETAT_BAS_CLIGNOTANT_DROIT, ETAT_HAUT_CLIGNOTANT_DROIT, "Clignotant Droit", "CLI", 0, VITESSE_CLIGNOTANT_DROIT);        // Initialisation du clignotants droit
  angelEyes.init(PIN_ANGEL_EYES, ETAT_BAS_ANGEL_EYES, ETAT_HAUT_ANGEL_EYES, "Angel Eyes", "PWM", VITESSE_ANGEL_EYES);                                               // Initialisation de l'angel eyes
  third_brake.init(PIN_THIRD_BRAKE, ETAT_BAS_THIRD_BRAKE, ETAT_HAUT_THIRD_BRAKE, "Troisième Feu Stop", "TOR", VITESSE_THIRD_BRAKE);                                 // Initialisation du troisième feu stop
  brakes.init(PIN_BRAKES, ETAT_BAS_BRAKES, ETAT_HAUT_BRAKES, "Feu Stop", "PWM", VITESSE_BRAKES);                                                                    // Initialisation des feux stop
  headlights.init(PIN_headlights, ETAT_BAS_headlights, ETAT_HAUT_headlights, "Feux Avant", "PWM", VITESSE_headlights);                                              // Initialisation des feux avant
  BUZZER_WARNING.init(PIN_BUZZER, ETAT_BAS_BUZZER, ETAT_HAUT_BUZZER, "Buzzer", "BUZ", 0, VITESSE_BUZZER);                                                           // Initialisation du buzzer
  backward.init(PIN_BACKWARD, ETAT_BAS_BACKWARD, ETAT_HAUT_BACKWARD, "Feu de Recul", "TOR", 0, VITESSE_BACKWARD);                                                   // Initialisation du feu de recul
  b_led.init(PIN_BAL, ETAT_BAS_BAL, ETAT_HAUT_BAL, "Feu de Recul", "TOR", 0, VITESSE_BAL);                                                                          // Initialisation du feu de recul

  /*=====================================*/
  /* Intialisation des servos (sorties)  */
  /*=====================================*/
  servo1 = new CustomServo(aux_serv_1, 500, 2400, 0);  // Pin 14, position initiale 0
  servo2 = new CustomServo(aux_serv_2, 500, 2400, 0);  // Pin 15, position initiale 0
  servo3 = new CustomServo(aux_serv_3, 500, 2400, 0);  // Pin 16, position initiale 0


  setupGyro();  // Initialisation du gyroscope

  /*=====================================*/
  /* Intialisation des pins (sorties)  */
  /*=====================================*/
  pinMode(channel_1, INPUT);
  pinMode(channel_2, INPUT);
  pinMode(channel_3, INPUT);
  pinMode(channel_4, INPUT);
  pinMode(channel_5, INPUT);
  pinMode(channel_6, INPUT);
  pinMode(S_9_PWM, OUTPUT);

  /*=====================*/
  /*test des sortie pwm  */
  /*=====================*/
  const int S_PWM[] = { S_1_PWM, S_2_PWM, S_3_PWM, S_4_PWM, S_5_PWM, S_6_PWM, S_7_PWM, S_8_PWM };  // Tableau des pins PWM
  const int PWM_COUNT = sizeof(S_PWM) / sizeof(S_PWM[0]);                                          // Nombre total de pins PWM
  for (int i = 0; i < PWM_COUNT; i++) {
    analogWrite(S_PWM[i], 255);  // Allumer la PWM au maximum
    delay(250);                  // Pause de 500 ms
    analogWrite(S_PWM[i], 0);    // Éteindre la PWM
    delay(100);                  // Pause de 500 ms
  }
  digitalWrite(S_9_PWM, HIGH);  // Éteindre la PWM
  delay(250);
  digitalWrite(S_9_PWM, LOW);
}

void handleInputs() {
  updateChannels();   //Lecture de cannaux partagé avec le recepteur
  monitorThrottle();  // Gestion de l'activité sur throttle
  readGyro();         // Lecture des données du gyro
}

void loop() {
  unsigned long currentMillis = millis();    // Récupération du temps passé depuis le démarrage (chrono)
  static unsigned long last_50_Loop = 0;     // il y a combien de temps que l'on a mesuré les 50ms
  static unsigned long last_250_Loop = 0;    // il y a combien de temps que l'on a mesuré les 250ms
  static unsigned long last_500_Loop = 0;    // il y a combien de temps que l'on a mesuré les 500ms
  static unsigned long last_1000_Loop = 0;   // il y a combien de temps que l'on a mesuré les 1s
  static unsigned long last_5000_Loop = 0;   // il y a combien de temps que l'on a mesuré les 5
  static unsigned long last_10000_Loop = 0;  // il y a combien de temps que l'on a mesuré les 10s

  /*=====   Toutes les 50 ms   =======*/
  if (currentMillis - last_50_Loop >= Interval_50) {
    last_50_Loop = currentMillis;  // On mets à jours le dernier passage des 50ms

    handleInputs();  // On interroge les inputs

    /*=====   surveillance du gyro : si le 4x4 atteint un angle trop élevé  =======*/
    tilted = (abs(roll) > abs(limit_g_x) || abs(pitch) > abs(limit_g_y));

    /*=====    Mise à jours des sorties    =======*/
    headlights.update(false);
    brakes.update(false);
    third_brake.update(false);

    clignotantGauche.update(blink);
    clignotantDroit.update(blink);
    backward.update(false);
    b_led.update(false);
    angelEyes.update(false);

    /*=====   transmition vers le smartphone si le wifi est activé   =======*/
    if (use_wifi) {
      wifiWebSocket.handle();

      if (!messageQueue.empty()) {
        Serial.println("message dans la queue");
        handleMessage();
      }
    }

    /*=====   appelé les scénario en fonction du mod d'énergie   =======*/
    switch (vehicule_mode) {
      case NORMAL:
        scenarioNormal();
        break;
      case WAIT:
        scenarioWait();
        break;
      case FORGET:
        scenarioForget();
        break;
    }
  }

  /*=====   Toutes les 250 ms   =======*/
  if (currentMillis - last_250_Loop >= Interval_250) {
    last_250_Loop = currentMillis;

    blink_degraded = !blink_degraded;  // Alterner entre true et false pour les clignotans en fonctionnement dégradé

    /*=====   transmition vers le smartphone si le wifi est activé   =======*/
    if (use_wifi) {
      if (transmit_mod == 2) {
        String data_to_send = generateGyroJson();
        wifiWebSocket.sendData(data_to_send);
      }
      if (transmit_mod == 4) {
        String jsonData = generateServoJson();
        wifiWebSocket.sendData(jsonData);
      }
    }
  }

  /*=====   Toutes les 500 ms   =======*/
  if (currentMillis - last_500_Loop >= Interval_500) {
    last_500_Loop = currentMillis;
    blink = !blink;  // Alterner entre true et false pour les clignotans en fonctionnement normal
  }

  /*=====   Toutes les 1 s   =======*/
  if (currentMillis - last_1000_Loop >= Interval_1000) {
    last_1000_Loop = currentMillis;
  }

  /*=====   Toutes les 5 s   =======*/
  if (currentMillis - last_5000_Loop >= Interval_5000) {
    last_5000_Loop = currentMillis;

    /*=====   transmition vers le smartphone si le wifi est activé   =======*/
    if (transmit_mod == 1 && use_wifi) {

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
    }
  }


  //*=====   Toutes les 10 s   =======*/
  if (currentMillis - last_10000_Loop >= Interval_10000) {
    last_10000_Loop = currentMillis;
    Serial.print("Mode actuel : ");
    Serial.println(vehiculeModeToString(vehicule_mode));
  }


  delay(20);
}



// Exemple d'implémentation pour un scénario
void scenarioNormal() {
  BUZZER_WARNING.update(blink);

  /*=====   surveillance gyro :  si le 4x4 atteint un angle trop élevé    =======*/
  if (tilted) {
    if (debug_output) {
      Serial.println("tilted");
    }
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
    if (!hp_sound && !possibleReverse) {
      BUZZER_WARNING.stop();
    } else {
      servo1->jumpTo(0);
    }
  }

  /*=====   surveillance commodo éclairage : off/veilleuse/pahre/plein phare    =======*/
  if (debug_output) {
    Serial.print("Mode actuel de Light Mod : ");
    Serial.println(light_mod_mode);
  }
  switch (light_mod_mode) {
    case 0:  // Mode 0 : Tout éteint
      angelEyes.stop();
      brakes.setEtatBas(0);
      clignotantGauche.setEtatBas(0);
      clignotantDroit.setEtatBas(0);
      headlights.setEtatBas(0);
      headlights.stop();

      break;

    case 1:  // Mode 1 : Veilleuses
      angelEyes.run();
      brakes.setEtatBas(25);     // 50% pour feu stop
      headlights.setEtatBas(25);  // 30% pour les phares
      if (clignotantGauche.get_actif()) {
        clignotantGauche.setEtatBas(0);
      } else {
        clignotantGauche.setEtatBas(american ? 40 : 0);  // Feu de position uniquement si américain
      }
      if (clignotantDroit.get_actif()) {
        clignotantDroit.setEtatBas(0);
      } else {
        clignotantDroit.setEtatBas(american ? 40 : 0);
      }
      break;

    case 2:  // Mode 2 : Phares
      angelEyes.run();
      brakes.setEtatBas(25);  // 50% pour feu stop
      headlights.setEtatBas(80);
      if (clignotantGauche.get_actif()) {
        clignotantGauche.setEtatBas(0);

      } else {
        clignotantGauche.setEtatBas(american ? 40 : 0);  // Feu de position uniquement si américain
      }
      if (clignotantDroit.get_actif()) {
        clignotantDroit.setEtatBas(0);
      } else {
        clignotantDroit.setEtatBas(american ? 40 : 0);
      }
      break;

    case 3:  // Mode 3 : Plein phares
      angelEyes.run();
      brakes.setEtatBas(25);  // 50% pour feu stop
      headlights.run();        // 100% pour les phares
      if (clignotantGauche.get_actif()) {
        clignotantGauche.setEtatBas(0);
      } else {
        clignotantGauche.setEtatBas(american ? 40 : 0);  // Feu de position uniquement si américain
      }
      if (clignotantDroit.get_actif()) {
        clignotantDroit.setEtatBas(0);
      } else {
        clignotantDroit.setEtatBas(american ? 40 : 0);
      }
      break;
  }

  /*=====   surveillance cligno : tourne à gauche ou a droite    =======*/
  //si on a pas d'alarme en dévers (prioritaire)
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

  /*=====   surveillance frein et accélérrateur   =======*/
  if (throttle_data > 50 && (light_mod_mode==2 || light_mod_mode==3)){
    b_led.run();
  }
  else{
    b_led.stop();
  }
  if (!traxxas) {
    /*=====   Mode Axial =======*/
    if (throttle_data > DEAD_ZONE) {
      third_brake.stop();
      brakes.stop();
      backward.stop();  // Pas de marche arrière
      if (debug_output) { Serial.println("Axial : Avancer"); }
      if (!tilted) { BUZZER_WARNING.stop(); }
    } else if (throttle_data >= -DEAD_ZONE && throttle_data <= DEAD_ZONE) {
      third_brake.run();
      brakes.run();
      backward.stop();
      if (debug_output) { Serial.println("Axial : Frein"); }
      if (!tilted) { BUZZER_WARNING.stop(); }
    } else {
      third_brake.stop();
      brakes.stop();
      backward.run();  // Marche arrière directe
      BUZZER_WARNING.run();
      if (debug_output) { Serial.println("Axial : Reculer"); }
    }
  } else {

    /*=====   Mode Traxxas =======*/
    if (throttle_data > DEAD_ZONE) {
      // Avancer, réinitialiser tous les états
      hasBraked = false;
      possibleReverse = false;
      third_brake.stop();
      brakes.stop();
      backward.stop();
      if (!tilted) { BUZZER_WARNING.stop(); }
      if (debug_output) { Serial.println("Traxxas : Avancer"); }
    } else if (throttle_data >= -DEAD_ZONE && throttle_data <= DEAD_ZONE) {
      // Décélération (pas de frein)
      if (hasBraked) {
        possibleReverse = true;  // Autorise la marche arrière après relâchement du frein
        hasBraked = false;       // Réinitialise le freinage pour attendre une nouvelle impulsion
        Serial.println("Traxxas : Frein relâché, marche arrière possible");
      }
      third_brake.stop();
      brakes.stop();
      backward.stop();
      if (!tilted) {
          BUZZER_WARNING.stop();
      }

      
      if (debug_output) { Serial.println("Traxxas : Décélérer"); }
    } else if (throttle_data < -DEAD_ZONE) {
      if (!hasBraked && !possibleReverse) {
        // Première impulsion : Frein
        hasBraked = true;
        third_brake.run();
        brakes.run();
        //backward.stop();
        

        if (debug_output) { Serial.println("Traxxas : Freiner"); }
      } else if (possibleReverse) {
        // Seconde impulsion : Marche arrière
        third_brake.stop();
        brakes.stop();
        backward.run();
        BUZZER_WARNING.run();
        if (debug_output) { Serial.println("Traxxas : Reculer"); }
      }
    }
  }
  Serial.print("test : ");
  Serial.print( String(tilted ? "  ON  " : "  OFF  "));
  Serial.println( String(possibleReverse ? "  ON  " : "  OFF  "));
  if (!tilted && !possibleReverse) {
    //BUZZER_WARNING.stop();
  }
}

void scenarioWait() {
  BUZZER_WARNING.update(blink_degraded);

  clignotantGauche.setEtatBas(0);  // Feu de position uniquement si américain
  clignotantDroit.setEtatBas(0);
  clignotantGauche.run();
  clignotantDroit.run();
  third_brake.run();
  brakes.run();
  if (!tilted) { BUZZER_WARNING.stop(); }
}

void scenarioForget() {
  BUZZER_WARNING.update(blink_degraded);
  backward.stop();


  // Implémentation pour le mode FORGET
  //if (!tilted) { BUZZER_WARNING.stop(); }
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
      } else if (output == "headlights") {
        headlights.forceOutput(value);
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
