#include "WiFiWebSocket.h"
WiFiWebSocket* WiFiWebSocket::instance = nullptr;

std::queue<String> messageQueue; // Initialisation de la file d’attente globale

bool Debug_socket = false;


// Constructeur
WiFiWebSocket::WiFiWebSocket(const char* ssid, const char* password)
    : ssid(ssid), password(password), server(80), ws("/ws"), wifiConnected(false) {
      instance = this;
      Serial.println("WiFiWebSocket instance initialisée.");
    }

// Méthode start : démarre Wi-Fi et WebSocket
void WiFiWebSocket::start() {
    Serial.println("Démarrage du Wi-Fi et du WebSocket...");
    WiFi.begin(ssid, password);

    unsigned long startTime = millis();
    while (millis() - startTime < 5000) { // Timeout de 5 secondes
        if (WiFi.status() == WL_CONNECTED) {
            wifiConnected = true;
            Serial.println("Wi-Fi connecté !");
            Serial.print("Adresse IP initiale attribuée : ");
            Serial.println(WiFi.localIP());

            // Configuration IP statique
            IPAddress currentIP = WiFi.localIP(); // Récupère l'IP attribuée
            IPAddress gateway = WiFi.gatewayIP(); // Récupère la passerelle

            if (currentIP[0] == 0 || gateway[0] == 0) {
                Serial.println("Impossible de récupérer l'IP ou la passerelle. Configuration IP statique annulée.");
                return;
            }

            // Construit l'IP statique dans le même sous-réseau avec .100
            IPAddress staticIP(currentIP[0], currentIP[1], currentIP[2], 100);
            IPAddress subnet(255, 255, 255, 0);

            if (WiFi.config(staticIP, gateway, subnet)) {
                Serial.println("Configuration IP statique réussie !");
                Serial.print("Nouvelle adresse IP : ");
                Serial.println(staticIP);
            } else {
                Serial.println("Échec de la configuration de l'IP statique.");
            }

            // Logs de la force du signal
            Serial.print("Force du signal (RSSI) : ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");

            // Initialisation WebSocket
            ws.onEvent(onWebSocketEvent);
            server.addHandler(&ws);
            server.begin();
            Serial.println("WebSocket initialisé.");
            Serial.println("Serveur Web démarré.");
            return;
        }
        delay(500);
    }

    Serial.println("Échec de la connexion au Wi-Fi. Mode sans connexion activé.");
}

// Méthode stop : arrête Wi-Fi et WebSocket
void WiFiWebSocket::stop() {
    Serial.println("Arrêt du Wi-Fi et du WebSocket...");
    if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
    }
    ws.closeAll(); // Ferme toutes les connexions WebSocket
    Serial.println("Wi-Fi et WebSocket arrêtés.");
    wifiConnected = false;
}

// Méthode handle : vérifie les connexions et restaure si nécessaire
void WiFiWebSocket::handle() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi déconnecté. Tentative de reconnexion...");
        stop();
        start();
    } else {
        ws.cleanupClients(); // Nettoie les clients WebSocket inactifs
    }
}


// Méthode onWebSocketEvent : inchangée
void WiFiWebSocket::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("Client connecté. ID : %u\n", client->id());
        //client->text("Connexion établie !");
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("Client déconnecté. ID : %u\n", client->id());
    } else if (type == WS_EVT_DATA) {
        String message;
        for (size_t i = 0; i < len; i++) {
            message += (char)data[i];
        }
        Serial.printf("Message reçu : %s\n", message.c_str());
        //client->text("Message reçu : " + message); // Echo
        messageQueue.push(message);
        
    }
}

// Envoie du texte brut via WebSocket
void WiFiWebSocket::sendData(const String& text) {
    if (ws.count() > 0) { // Vérifie si des clients sont connectés
        ws.textAll(text);

        if(Debug_socket){
          Serial.print("Message envoyé : ");
          Serial.println(text);
        }
        
    } else {
        Serial.println("Aucun client connecté au WebSocket. Envoi ignoré.");
    }
}
