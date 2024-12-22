#ifndef WIFIWEBSOCKET_H
#define WIFIWEBSOCKET_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <queue>

extern bool Debug_socket;

extern std::queue<String> messageQueue; // File d’attente pour les messages WebSocket
class WiFiWebSocket {
public:
    WiFiWebSocket(const char* ssid, const char* password); // Constructeur
    void start(); // Démarre Wi-Fi et WebSocket
    void stop(); // Arrête Wi-Fi et WebSocket
    void handle(); // Vérifie les connexions et restaure si nécessaire
    void sendData(const String& text); // Pour texte brut
    void handleMessage(); // Ajoute la méthode pour traiter la file d'attente

private:
    const char* ssid;
    const char* password;
    AsyncWebServer server;
    AsyncWebSocket ws;
    bool wifiConnected;
    static WiFiWebSocket* instance;

    

    static void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len); // Gestion des événements WebSocket
};

#endif
