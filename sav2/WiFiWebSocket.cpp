// WiFiWebSocket.cpp
#include "WiFiWebSocket.h"
#include "config.h"
#include <queue>

std::queue<String> messageQueue;

WiFiWebSocket::WiFiWebSocket()
    : ssid(WIFI_SSID), password(WIFI_PASSWORD), server(80), ws("/ws") {}

void WiFiWebSocket::begin() {
    Serial.println("Connexion au Wi-Fi...");

    WiFi.begin(ssid, password);
    wsConnectStartTime = millis();

    wifiInitialized = false;
    reconnectAttempts = 5;

    checkWiFiConnection();
}

void WiFiWebSocket::checkWiFiConnection() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!wifiInitialized) {
            Serial.println("\nWi-Fi connecté !");
            Serial.print("Adresse IP : ");
            Serial.println(WiFi.localIP());

            wifiInitialized = true;
            configureStaticIP();
            initWebSocket();
            server.begin();
        }
    } else if (millis() - wsConnectStartTime >= 15000 && reconnectAttempts > 0) {
        Serial.println("Tentative de reconnexion au Wi-Fi...");
        WiFi.begin(ssid, password);
        wsConnectStartTime = millis();
        reconnectAttempts--;
    } else if (reconnectAttempts == 0) {
        handleFailedConnection();
    }
}

void WiFiWebSocket::handle() {
    if (WiFi.status() != WL_CONNECTED) {
        wifiConnected = false;
        Serial.println("Wi-Fi déconnecté. Tentative de reconnexion...");
        checkWiFiConnection();
    } else if (!wifiConnected) {
        wifiConnected = true;
        Serial.println("Wi-Fi reconnecté !");
        configureStaticIP();
        initWebSocket();
    }

    ws.cleanupClients();
}

void WiFiWebSocket::send(const String &message) {
    if (ws.count() > 0) {
        ws.textAll(message);
    }
}

void WiFiWebSocket::configureStaticIP() {
    IPAddress gateway = WiFi.gatewayIP();
    IPAddress staticIP(gateway[0], gateway[1], gateway[2], 100);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.config(staticIP, gateway, subnet);
}

void WiFiWebSocket::initWebSocket() {
    ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->onWebSocketEvent(server, client, type, arg, data, len);
    });
    server.addHandler(&ws);
}

void WiFiWebSocket::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("Client WebSocket connecté !");
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.println("Client WebSocket déconnecté !");
    } else if (type == WS_EVT_DATA) {
        String message;
        for (size_t i = 0; i < len; i++) {
            message += (char)data[i];
        }
        messageQueue.push(message);
    }
}

void WiFiWebSocket::handleFailedConnection() {
    Serial.println("Impossible de se connecter au Wi-Fi après plusieurs tentatives. Redémarrage...");
    ESP.restart();
}

String formatTime(unsigned long milliseconds) {
    unsigned long seconds = milliseconds / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    seconds = seconds % 60;
    minutes = minutes % 60;

    char buffer[9];
    snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, seconds);
    return String(buffer);
}