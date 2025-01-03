#include "WiFiWebSocket.h"
#include <ArduinoJson.h>
#include "Simulation.h" // Inclusion du simulateur
#include "Ampoules.h"
#include "config.h"
#include "gyro.h"
#include "my_i_servo.h"
#include <queue>


#include <ESP32Servo.h>
Servo servo1; // Servo sur la broche 19
Servo servo2; // Servo sur la broche 18
Servo servo3; // Servo sur la broche 15
//std::queue<String> messageQueue; // File d’attente pour les messages WebSocket

// Configuration Wi-Fi
//const char* ssid = "Redmi Note 13 Pro 5G";
//const char* password = "uheku675";


//mod de transmition au démarrage 0-pour silence  1-pour le systéme 2-pour le gyro / 3-pour les servos / 4-pour les sortie 
int transmit_mod=1;


// Initialisation des clignotants
Clignotant clignotantGauche(PIN_CLIGNOTANT_GAUCHE, VITESSE_CLIGNOTANT_GAUCHE,ETAT_BAS_CLIGNOTANT_GAUCHE, ETAT_HAUT_CLIGNOTANT_GAUCHE);
Clignotant clignotantDroit(PIN_CLIGNOTANT_DROIT, VITESSE_CLIGNOTANT_DROIT,ETAT_BAS_CLIGNOTANT_DROIT, ETAT_HAUT_CLIGNOTANT_DROIT);
// Initialisation de l'angel eyes
PWM_Light angelEyes(PIN_ANGEL_EYES, VITESSE_ANGEL_EYES, ETAT_BAS_ANGEL_EYES, ETAT_HAUT_ANGEL_EYES);

//Initialisation Troisiéme feu stop
TOR_Light third_brake(PIN_THIRD_BRAKE, ETAT_BAS_THIRD_BRAKE, ETAT_HAUT_THIRD_BRAKE);
//Initialisation feu stop
TOR_Light brakes(PIN_BRAKES, ETAT_BAS_BRAKES, ETAT_HAUT_BRAKES); 

//Initialisation des feu avants
TOR_Light HEADLIGHTS(PIN_HEADLIGHTS, ETAT_BAS_HEADLIGHTS, ETAT_HAUT_HEADLIGHTS);




// Objet WiFiWebSocket avec les constantes définies dans config.h
WiFiWebSocket wifiWebSocket(WIFI_SSID, WIFI_PASSWORD);

unsigned long previousMillis = 0; // Gestion du temps pour le `loop`
const unsigned long interval = 250;

unsigned long previousMillis_5s = 0; // Gestion du temps pour le `loop`
const unsigned long interval_5s = 2500;

int angle = 0;                  // Angle actuel des servos
int increment = 45;              // Incrément d'angle (1 pour avancer, -1 pour reculer)




void setup() {
    delay(5000);
    Serial.begin(115200);
    
    wifiWebSocket.start(); // Démarre Wi-Fi et WebSocket
    setupGyro(); // Initialisation du gyroscope
    pinMode(channel_1, INPUT);
    pinMode(channel_2, INPUT);
    pinMode(channel_3, INPUT);
    pinMode(channel_4, INPUT);
    pinMode(channel_5, INPUT);
    pinMode(channel_6, INPUT);

    servo1.attach(19);
    servo2.attach(18);
    servo3.attach(5);

  }

unsigned long lastReconnectCheck = 0; // Dernière vérification de la connexion Wi-Fi
const unsigned long reconnectInterval = 5000; // Intervalle de 5 secondes pour tenter une reconnexion
//18 19 15 aux

void loop() {
    unsigned long currentMillis = millis();

    // Boucle lente
    static unsigned long lastSlowLoop = 0;
    const unsigned long slowInterval = 10000; // 10 secondes
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
        //wifiWebSocket.printWebSocketStatistics();
    }

    // Boucle moyenne
    static unsigned long lastMediumLoop = 0;
    const unsigned long mediumInterval = 250; // 250 ms
    if (currentMillis - lastMediumLoop >= mediumInterval) {
        lastMediumLoop = currentMillis;
        updateChannels();
        if (transmit_mod == 2) {
            String data_to_send = generateGyroJson();
            wifiWebSocket.sendData(data_to_send);
        }
        if (transmit_mod == 4) {
            String jsonData = generateServoJson();
            wifiWebSocket.sendData(jsonData);
        }

        // Mouvement d'essuie-glace
        angle += increment; // Ajuste l'angle des servos
        if (angle >= 180 || angle <= 0) {
            increment = -increment; // Inverse la direction au bout des limites
        }

        // Commande des servos
        servo1.write(angle);
        servo2.write(angle);
        servo3.write(angle);
    }

    // Boucle rapide
    static unsigned long lastFastLoop = 0;
    const unsigned long fastInterval = 50; // 50 ms
    if (currentMillis - lastFastLoop >= fastInterval) {

        
        lastFastLoop = currentMillis;
        wifiWebSocket.handle();

         if (!messageQueue.empty()) {
            Serial.println("message dans la queue");
            handleMessage();
        }

        clignotantGauche.run();
        clignotantDroit.run();
        third_brake.stop();
        brakes.stop();
        HEADLIGHTS.stop();

      }
}


void handleMessage() {
    // Vérifie si la file d'attente contient un message
    if (messageQueue.empty()) {
      
        return;
    }

    // Récupérer le premier message de la file d'attente
    String message = messageQueue.front();
    messageQueue.pop(); // Retire immédiatement le message pour éviter des doublons

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
            }else if (command == "reset_gyro")
            {
              resetGyro();
            }
             else {
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
                transmit_mod = mod_req.toInt(); // Convertir String en int avant assignation
                Serial.println("Mod reçu et valide : " + String(transmit_mod)); // Conversion explicite en String pour l'affichage
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



