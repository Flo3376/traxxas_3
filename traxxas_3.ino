/*======================*/
//     Lib Externe      //
/*======================*/
#include <ESP32Servo.h>   // Lib spécifique à l'ESP32
#include <queue>          // Lib pour les message en file d'attende
#include <ArduinoJson.h>  // LiB pour l'utilisation du format JSON

/*======================*/
//  Dépendance interne  //
/*======================*/
#include "Ampoules.h"       // Contrôle des ampoules (phares, clignotants, etc.)
#include "config.h"         // Fichier de configuration pour définir les paramètres globaux du projet
#include "gyro.h"           // Gestion du gyroscope, pour détecter l'inclinaison ou la stabilité
#include "in_servo.h"       // Gestion des servos utilisés comme entrées (par exemple, récupération des commandes utilisateur)
#include "out_servo.h"      // Gestion des servos utilisés comme sorties (par exemple, contrôle des moteurs ou des roues)
#include "WiFiWebSocket.h"  // Gestion de la communication via WebSocket pour le Wi-Fi

/*======================*/
//      Variables       //
/*======================*/
bool hasBraked = false;        	// Indique si un freinage a été effectué (Traxxas seulement)
bool possibleReverse = false;  	// Indique si la marche arrière est possible après un freinage (Traxxas seulement)
bool blink = false;            	// Variable globale pour gérer la synchronisation
bool blink_hs = false;   		    // Variable globale pour gérer la synchronisation
bool blink_degraded = false;   	// Variable globale pour gérer la synchronisation
bool tilted = false;          	// si la voiture est en trop penchée
bool use_wifi = false;        	// activation ou non du wifi dés de démarrage de l'esp
int transmit_mod = 1;          	// mod de transmition au démarrage 0-pour silence  1-pour le systéme 2-pour le gyro / 3-pour les servos / 4-pour les sortie
bool initialized_expo = false;	// Assure que les bips de démarrage ne se jouent qu'une seule fois dans le mod EXPO
int step_expo = 0;              // Étape actuelle dans le scénario EXPO
bool isInMode3 = false;         // Indique si on est activement dans le mode 3

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
/*=====   Objet WiFiWebSocket avec les constantes définies dans config.h   =======*/
WiFiWebSocket wifiWebSocket(WIFI_SSID, WIFI_PASSWORD);

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
    wifiWebSocket.start();
  }

  /*=============================*/
  /* Intialisation des ampoules  */
  /*=============================*/
  /*
	   Ces fonctions initialisent des ampoules avec les paramètres suivants :
	  - pin : Numéro de la broche où l'ampoule est connectée. Les valeurs sont définies dans le fichier config.h.
	  - etat_bas : Puissance lumineuse au repos (0 = éteint). Cette valeur peut être modifiée dynamiquement.
	  - etat_haut : Puissance lumineuse lorsque l'ampoule est activée. Cette valeur peut également être modifiée dynamiquement.
	  - nom : Nom descriptif de l'ampoule (ex. "Feu de Recul"), utilisé pour les diagnostics avancés.
	  - type : Type de fonctionnement de la sortie :
	           - "TOR" : Tout ou rien (marche/arrêt uniquement).
	           - "PWM" : Variation progressive (non testé).
	           - "CLI" : Clignotement régulier.
	  - transition : Temps de transition (en millisecondes) entre deux états (utilisé uniquement pour PWM).
	  - vitesse : Vitesse de clignotement (utilisé uniquement si le type est "CLI").
	*/
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
  /*
	  Cette section configure les servos connectés aux broches spécifiées.
	  Les servos sont réglés pour fonctionner sur une plage de fréquences comprise entre 500 et 2400 µs,
	  ce qui correspond à un signal PWM standard élargi. Chaque servo est initialisé avec une position
	  centrale (0 degré) par défaut.

	  Paramètres de l'initialisation :
	  - aux_serv_X : Broche à laquelle le servo est connecté (définie dans config.h).
	  - 500 : Durée d'impulsion minimale (en microsecondes), correspond au mouvement minimum du servo.
	  - 2400 : Durée d'impulsion maximale (en microsecondes), correspond au mouvement maximum du servo.
	  - 0 : Position initiale (centrale) du servo.
  */
  servo1 = new CustomServo(aux_serv_1, 500, 2400, 0);
  servo2 = new CustomServo(aux_serv_2, 500, 2400, 0);
  servo3 = new CustomServo(aux_serv_3, 500, 2400, 0);


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
  /*  test des sortie    */
  /*=====================*/
  const int S_PWM[] = { S_1_PWM, S_2_PWM, S_3_PWM, S_4_PWM, S_5_PWM, S_6_PWM, S_7_PWM, S_8_PWM };  // Tableau des pins sortie

  const int PWM_COUNT = sizeof(S_PWM) / sizeof(S_PWM[0]);  // Nombre total de pins sortie
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
  static unsigned long last_50_Loop = 0;     // La dernière fois l'on a mesuré les 50ms
  static unsigned long last_125_Loop = 0;    // La dernière fois l'on a mesuré les 125ms
  static unsigned long last_250_Loop = 0;    // La dernière fois l'on a mesuré les 250ms
  static unsigned long last_500_Loop = 0;    // La dernière fois l'on a mesuré les 500ms
  static unsigned long last_1000_Loop = 0;   // La dernière fois l'on a mesuré les 1s
  static unsigned long last_5000_Loop = 0;   // La dernière fois l'on a mesuré les 5
  static unsigned long last_10000_Loop = 0;  // La dernière fois l'on a mesuré les 10s
  

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
    backward.update(false);
    b_led.update(false);
    angelEyes.update(false);

    /*=====   reception des message depuis le smartphone si le wifi est activé   =======*/
    if (use_wifi) {
      wifiWebSocket.handle();

      if (!messageQueue.empty()) {
        Serial.println("message dans la queue");
        handleMessage();
      }
    }

    /*=====   appelé les scénario en fonction du mod d'énergie   =======*/
    /*
	  Cette section gère les différents modes d'énergie du véhicule. 
	  Le passage entre les modes est basé sur des durées d'inactivité ou des événements spécifiques.
	  Une accélération ou une commande utilisateur fait immédiatement repasser le véhicule en mode NORMAL.

	  Description des modes :
	  - **NORMAL** :
	    - Mode de fonctionnement complet.
	    - Tous les composants sont actifs : servos, ampoules, Wi-Fi, etc.
	    - Ce mode est utilisé lorsque le véhicule est en activité normale.

	  - **WAIT** :
	    - Simule une attente où le pilote aurait serré le frein à main et activé les warnings.
	    - Utilisé après une période d'inactivité modérée.

	  - **FORGET** :
	    - Suppose que l'utilisateur a oublié son 4x4 sous tension.
	    - Le véhicule émet des bips réguliers pour rappeler à l'utilisateur qu'il doit intervenir.
	    - Peut inclure des alertes visuelles et sonores.

	  - **EXPO** (non encore implémenté) :
	    - Présente le 4x4 sur un présentoir d'exposition.
	    - Active un clignotement attractif (phares, warning, etc.) tout en désactivant des fonctionnalités inutiles comme le Wi-Fi.

	  Logique de transition entre les modes :
	  - Le système passe automatiquement d'un mode à l'autre dans l'ordre suivant après une durée d'inactivité définie :
	    NORMAL → WAIT → FORGET → EXPO.
	  - Toute action utilisateur, comme une accélération, ramène immédiatement au mode NORMAL.
	*/
    switch (vehicule_mode) {
      case NORMAL:
        scenarioNormal();  // Fonctionnement complet
        break;
      case WAIT:
        scenarioWait();  // Simulation d'attente avec warnings et frein à main
        break;
      case FORGET:
        scenarioForget();  // Rappel sonore à intervalle régulier et désactivation de certaines lumiére pour économiser la batterie
        break;
      case EXPO:
        scenarioExpo();  // Non implémenté, mais destiné à l'exposition (Wi-Fi désactivé, clignotements attractifs)
        break;
    }
  }

  /*=====   Toutes les 125 ms   =======*/
  if (currentMillis - last_125_Loop >= Interval_125) {
    last_125_Loop = currentMillis;
    blink_hs = !blink_hs;  // Alterner entre true et false pour les clignotans en fonctionnement normal
  }

  /*=====   Toutes les 250 ms   =======*/
  if (currentMillis - last_250_Loop >= Interval_250) {
    last_250_Loop = currentMillis;

    /*=====   Alterner entre true et false pour les clignotans en fonctionnement dégradé   =======*/
    blink_degraded = !blink_degraded;

    /*=====   transmition vers le smartphone si le wifi est activé   =======*/
    /*
	 Modes de transmission (`transmit_mod`) :
	  - **Mode 1 (géré ailleurs)** :
	    - Ce mode s'exécute toutes les 5 secondes (géré dans une autre partie du code).
	    - Transmet des informations peu importantes, comme un état général ou un message "keep-alive".
	    
	  - **Mode 2 : Gyroscope** :
	    - Envoie régulièrement les données du gyroscope au smartphone.
	    - Les données comprennent l'angle d'inclinaison, l'accélération ou d'autres paramètres liés à la stabilité.

	  - **Mode 3 : Récepteur radio** :
	    - Transmet les positions théoriques des commandes demandées par la télécommande.
	    - Ces données reflètent l'état souhaité des actionneurs (ex. servos) tel que défini par l'utilisateur.

	  - **Mode 4 ( partiellement implémenté ou non géré)** :
	    - Ce mode est prévu pour forcer les sorties dans des états spécifiques, indépendamment des commandes normales.
	    - Il surcharge le fonctionnement standard du véhicule pour imposer un contrôle manuel ou des tests.
	*/
    if (use_wifi) {
      if (transmit_mod == 2) {
        // Envoie les données du gyroscope au smartphone
        String data_to_send = generateGyroJson();  // Génère les données JSON du gyroscope
        wifiWebSocket.sendData(data_to_send);      // Envoie via WebSocket
      }
      if (transmit_mod == 4) {
        // Envoie les données des servos au smartphone
        String jsonData = generateServoJson();  // Génère les données JSON des servos
        wifiWebSocket.sendData(jsonData);       // Envoie via WebSocket
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
      doc["type"] = "info";                        // Type de message (info général)
      doc["name"] = car_name;                      // Nom du véhicule (défini dans config.h)
      doc["version"] = version_soft;               // Version logicielle
      doc["localip"] = WiFi.localIP().toString();  // Adresse IP locale
      doc["uptime"] = millis() / 1000;             // Temps de fonctionnement en secondes
      doc["ssid"] = WIFI_SSID;                     // Nom du réseau Wi-Fi
      doc["rssi"] = WiFi.RSSI();                   // Puissance du signal Wi-Fi (dBm)
      doc["output"] = output_u;                    // Nombre de sorties
      doc["input"] = input_u;                      // Nombre de entrées
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
  //*=====   bridageg du système pour ne pas faire tourner le processeur à fond pour rien   =======*/
  delay(20);
}



void scenarioNormal() {
  /*
  Fonction : scenarioNormal()
  ---------------------------
  Cette fonction gère le comportement du véhicule en mode NORMAL, où toutes les fonctionnalités sont actives.
  Elle surveille et contrôle plusieurs aspects du véhicule en temps réel, notamment :
  - Gyroscope pour détecter un angle critique.
  - Commandes d'éclairage selon le commodo (off, veilleuse, phares, plein phares).
  - Clignotants selon la direction (gauche/droite).
  - Gestion des freins et de l'accélérateur.
  - Différenciation entre les modes Axial et Traxxas pour la gestion du frein et de la marche arrière.
*/
  BUZZER_WARNING.update(blink);
  clignotantGauche.update(blink);
  clignotantDroit.update(blink);

  static unsigned long last_7500_Loop = 0;  // La dernière fois l'on a mesuré les 10s

  /*=====   on remet le mod EXPO à 0    =======*/
  initialized_expo=false;
  step_expo=0;

  /*=====   surveillance gyro :  si le 4x4 atteint un angle trop élevé    =======*/

  // Si l'angle est critique, activer les clignotants et émettre une alarme sonore/visuelle
  if (tilted) {
    if (debug_output) {
      Serial.println("tilted");
    }
    clignotantGauche.run();
    clignotantDroit.run();
    if (!hp_sound) {
      BUZZER_WARNING.run();  // Active le buzzer d'alarme
    } else {
      servo1->jumpTo(180);
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
      b_led.stop();

      break;

    case 1:  // Mode 1 : Veilleuses
      angelEyes.run();
      brakes.setEtatBas(25);      // Faible luminosité pour feu stop
      headlights.setEtatBas(25);  // Faible luminosité pour les phares
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
      brakes.setEtatBas(25);      // Faible luminosité pour feu stop
      headlights.setEtatBas(80);  // Luminosité moyenne pour les phares
      b_led.stop();
      headlights.stop();
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
      isInMode3 = false;

    // Vérifier si on doit passer en mode 3
    if (throttle_data > 50) {
      light_mod_mode = 3;
      last_7500_Loop = millis(); // Initialiser le timer lors du passage au mode 3
      isInMode3 = true;
      Serial.println("Passage au mode 3");
    }
      break;

    case 3:  // Mode 3 : Plein phares

    if (!isInMode3) {
      // Passage initial au mode 3
      isInMode3 = true;
      last_7500_Loop = millis(); // Initialiser le timer
    }
      angelEyes.run();
      brakes.setEtatBas(25);  // Faible luminosité pour feu stop
      headlights.run();       // Phares à pleine puissance
      b_led.run();
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
      // Gestion du timer pour revenir au mode 2
      if (throttle_data > 50) {
        last_7500_Loop = millis(); // Réinitialiser le timer si l'accélération est au-dessus de 50 %
        Serial.println("Maintien du mode 3");
      } else if (millis() - last_7500_Loop >= Interval_7500) { 
        light_mod_mode = 2;    // Retour au mode 2 après temporisation
        isInMode3 = false;
        Serial.println("Retour automatique au mode 2");
      }
      break;
  }

  /*=====   surveillance cligno : tourne à gauche ou a droite    =======*/
  // Active les clignotants en fonction des commandes de direction
  if (!tilted) {  // Priorité au gyroscope
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
  if (!traxxas) {
    /*=====   Mode Axial =======*/
    if (throttle_data > DEAD_ZONE) {  //si la position de l'accellérateur est au dessus de la zone morte (on avance)
      third_brake.stop();             // Pas de frein
      brakes.stop();                  // Pas de frein
      backward.stop();                // Pas de marche arrière
      if (debug_output) { Serial.println("Axial : Avancer"); }
      if (!tilted) { BUZZER_WARNING.stop(); }
    } else if (throttle_data >= -DEAD_ZONE && throttle_data <= DEAD_ZONE) {  //si la position de l'accellérateur est dans la zone morte (accellérateur relaché)
      third_brake.run();
      brakes.run();
      backward.stop();
      if (debug_output) { Serial.println("Axial : Frein"); }
      if (!tilted) { BUZZER_WARNING.stop(); }
    } else {  //si la position de l'accellérateur est au dessous de la zone morte (on recule)
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
}

/*
  Fonction : scenarioWait()
  --------------------------
  Ce scénario représente une phase d'attente où :
  - Les warnings (clignotants) clignotent en mode dégradé.
  - Les feux de position et freins sont activés.
  - L'état des autres dispositifs laissés actifs par NORMAL est préservé.
*/
void scenarioWait() {
  clignotantGauche.update(blink);
  clignotantDroit.update(blink);
  //BUZZER_WARNING.update(blink_degraded);

  clignotantGauche.setEtatBas(0);  // Feu de position uniquement si américain
  clignotantDroit.setEtatBas(0);
  clignotantGauche.run();
  clignotantDroit.run();
  third_brake.run();
  brakes.run();
  if (!tilted) { BUZZER_WARNING.stop(); }
}
/*
  Fonction : scenarioForget()
  ----------------------------
  Ce scénario simule une alerte pour rappeler à l'utilisateur que le véhicule est sous tension.
  - Le buzzer clignote en mode dégradé.
  - La marche arrière est explicitement désactivée.
  - Les dispositifs laissés actifs par WAIT (warnings, freins) restent allumés si non désactivés ici.
*/

void scenarioForget() {
  clignotantGauche.update(blink_hs);
  clignotantDroit.update(blink_hs);
  backward.stop();

  static unsigned long lastBipTime = 0;    // Moment du dernier bip
  unsigned long currentMillis = millis();  // Temps actuel en millisecondes

  // Vérifie si 30 secondes (30 000 ms) se sont écoulées depuis le dernier bip
  if (currentMillis - lastBipTime >= 30000) {
    digitalWrite(S_9_PWM, HIGH);  // Éteindre la PWM
    delay(250);
    digitalWrite(S_9_PWM, LOW);
    delay(50);
    digitalWrite(S_9_PWM, HIGH);  // Éteindre la PWM
    delay(250);
    digitalWrite(S_9_PWM, LOW);
    lastBipTime = currentMillis;  // Met à jour le moment du dernier bip
  }
}

void scenarioExpo() {

  clignotantGauche.update(blink);
  clignotantDroit.update(blink);

  static unsigned long lastActionTime = 0;    // Temps de la dernière action
  unsigned long currentMillis = millis();

  // Étape 1 : Signaler l'entrée en EXPO par 3 bips courts
  if (!initialized_expo) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(S_9_PWM, HIGH);  // Active la sortie PWM
      delay(125);
      digitalWrite(S_9_PWM, LOW);
      delay(125);
    }
    initialized_expo = true; // Les bips sont joués une seule fois
    lastActionTime = currentMillis; // Synchronisation pour la séquence lumineuse

    // Extinction complète avant de commencer
    brakes.stop();
    backward.stop();
    b_led.stop();
    clignotantGauche.stop();
    clignotantDroit.stop();
    angelEyes.stop();
    headlights.stop();
  }

  // Étape 2 : Gestion des lumières (allumage progressif avec clignotements)
  if (currentMillis - lastActionTime >= 500) { // Exécuter toutes les 500 ms
    lastActionTime = currentMillis;

    // Gestion des étapes avec un switch
    switch (step_expo) {
      // Allumage progressif
      case 0:
        Serial.println("Étape 0 : Allumage du clignotant gauche");
        clignotantGauche.run();
        break;

      case 5:
        Serial.println("Étape 5 : Fixation du clignotant gauche à intensité maximale");
        clignotantGauche.stop();
        clignotantGauche.setEtatBas(254);
        break;

      case 10:
        Serial.println("Étape 10 : Allumage du clignotant droit");
        clignotantDroit.run();
        break;

      case 15:
        Serial.println("Étape 15 : Fixation du clignotant droit à intensité maximale");
        clignotantDroit.stop();
        clignotantDroit.setEtatBas(254);
        break;

      case 20:
        Serial.println("Étape 20 : Allumage des Angel Eyes");
        angelEyes.run();
        break;

      case 25:
        Serial.println("Étape 25 : Allumage des Angel Eyes (fixé)");
        angelEyes.run();
        break;

      case 30:
        Serial.println("Étape 30 : Allumage des phares à faible intensité");
        headlights.setEtatBas(25);
        break;

      case 35:
        Serial.println("Étape 35 : Allumage des phares à intensité moyenne");
        headlights.setEtatBas(80);
        break;

      case 40:
        Serial.println("Étape 40 : Activation complète des phares");
        headlights.run();
        break;

      case 45:
        Serial.println("Étape 45 : Réduction de l’intensité des clignotants");
        clignotantDroit.setEtatBas(25);
        clignotantGauche.setEtatBas(25);
        break;

      case 50:
        Serial.println("Étape 50 : Allumage du clignotant droit");
        clignotantDroit.setEtatBas(0);
        clignotantDroit.run();
        break;

      case 55:
        Serial.println("Étape 55 : Allumage du clignotant gauche");
        clignotantGauche.setEtatBas(0);
        clignotantGauche.run();
        break;

      case 60:
        Serial.println("Étape 60 : Activation des LED arrière");
        b_led.run();
        break;

      case 65:
        Serial.println("Étape 65 : Activation des freins");
        brakes.run();
        break;

      case 70:
        Serial.println("Étape 70 : Activation de la marche arrière");
        backward.run();
        break;

      case 75:
        Serial.println("Étape 75 : Activation du troisième feu stop");
        third_brake.run();
        break;

      // Extinction progressive (ordre inverse)
      case 80:
        Serial.println("Étape 80 : Extinction du troisième feu stop");
        third_brake.stop();
        break;

      case 85:
        Serial.println("Étape 85 : Extinction de la marche arrière");
        backward.stop();
        break;

      case 90:
        Serial.println("Étape 90 : Extinction des freins");
        brakes.stop();
        break;

      case 95:
        Serial.println("Étape 95 : Extinction des LED arrière");
        b_led.stop();
        break;

      case 100:
        Serial.println("Étape 100 : Extinction des clignotants gauche et droit");
        clignotantGauche.stop();
        clignotantDroit.stop();
        break;

      case 105:
        Serial.println("Étape 105 : Extinction des Angel Eyes");
        angelEyes.stop();
        break;

      case 110:
        Serial.println("Étape 110 : Extinction des phares");
        headlights.stop();
        headlights.setEtatBas(0);
        break;

      case 115:
        Serial.println("Redémarrage du scénario EXPO");
        step_expo = -1; // Réinitialisation (passera à 0 au prochain incrément)
        break;

      default:
        //Serial.print("Étape inconnue ou sans action : ");
        //Serial.println(step);
        break;
    }

    step_expo++; // Incrémente l'étape
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
