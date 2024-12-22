#include "WiFiWebSocket.h"
#include "esp_log.h"
#include <ArduinoJson.h>
#include "Simulation.h" // Inclusion du simulateur
#include "Ampoules.h"
#include "config.h"

// Initialisation des clignotants
Clignotant clignotantGauche(PIN_CLIGNOTANT_GAUCHE, VITESSE_CLIGNOTANT_GAUCHE,ETAT_BAS_CLIGNOTANT_GAUCHE, ETAT_HAUT_CLIGNOTANT_GAUCHE);
Clignotant clignotantDroit(PIN_CLIGNOTANT_DROIT, VITESSE_CLIGNOTANT_DROIT,ETAT_BAS_CLIGNOTANT_DROIT, ETAT_HAUT_CLIGNOTANT_DROIT);
// Initialisation de l'angel eyes
Angel_Eyes angelEyes(PIN_ANGEL_EYES, VITESSE_ANGEL_EYES, ETAT_BAS_ANGEL_EYES, ETAT_HAUT_ANGEL_EYES); // Broche 5, durée 2 secondes, bas=0, haut=255


// Instance utilisant les valeurs de config.h
WiFiWebSocket wifiManager;

WiFiWebSocket wifiWebSocket;
Simulation simulation(wifiWebSocket.getWebSocket()); // Utilisation de la méthode `getWebSocket`

void setup() {
    Serial.begin(115200);
    // Réduire les logs Wi-Fi au minimum
    esp_log_level_set("wifi", ESP_LOG_NONE); // Options : ESP_LOG_NONE, ESP_LOG_ERROR, ESP_LOG_WARN, etc.

    wifiWebSocket.begin(); // Initialisation du Wi-Fi et des WebSockets

    // Initialisation du Wi-Fi et WebSocket
    wifiManager.begin();
  }

void loop() {
    // Gestion Wi-Fi et WebSocket
    wifiManager.handle();
    simulation.update(); // Mise à jour de la simulation
    
    // Si un message est présent dans la file d'attente, on le traite
    if (!messageQueue.empty()) {
        handleMessage(); // Appel à une fonction pour gérer un seul message
    }


}

void handleMessage() {
    // Vérifie si la file d'attente contient un message
    if (messageQueue.empty()) {
        return;
    }

    // Récupérer le premier message de la file d'attente
    String message = messageQueue.front();

    // Affiche le message pour le débogage
    Serial.println("Traitement du message : " + message);

    // Parse le message en JSON
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (!error) {
        // Vérifie si le message contient un champ "type"
        if (doc.containsKey("type")) {
            String type = doc["type"];

            if (type == "ping") {
                Serial.println("Ping reçu !");
                wifiWebSocket.send("{\"type\":\"pong\"}");
                }
            else if (type == "traxxas") {
                Serial.println("Identification demandé!");
                wifiWebSocket.send("{\"type\":\"Axial\"}");
            } else if (type == "command") {
                // Gérer une commande spécifique
                String command = doc["command"];
                Serial.println("Commande reçue, enfin je crois : " + command);

                if (command == "start") {
                    Serial.println("Démarrage de la simulation !");
                    simulation.start(); // Appel à la méthode pour démarrer la simulation
                } else if (command == "stop") {
                    Serial.println("Arrêt de la simulation !");
                    simulation.stop(); // Appel à la méthode pour arrêter la simulation
                } else {
                    Serial.println("Commande inconnue : " + command);
                }
            }
        } else {
            Serial.println("Type de message non reconnu.");
        }
    } else {
        Serial.println("Erreur lors de la lecture du JSON.");
    }

    // Supprimer le message de la file d'attente après traitement
    messageQueue.pop();
}

