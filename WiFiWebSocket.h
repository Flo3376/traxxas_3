#ifndef WIFIWEBSOCKET_H
#define WIFIWEBSOCKET_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <queue>

// Variable globale pour activer/désactiver les logs des WebSockets
extern bool Debug_socket;

// File d'attente pour stocker les messages WebSocket reçus et les traiter en différé
extern std::queue<String> messageQueue;

/**
 * Classe WiFiWebSocket
 * Permet la gestion du Wi-Fi et de la communication via WebSocket
 */
class WiFiWebSocket {
public:
    WiFiWebSocket(const char* ssid, const char* password); // Constructeur
    void start(); // Démarre Wi-Fi et WebSocket
    void stop(); // Arrête Wi-Fi et WebSocket
    void handle(); // Vérifie les connexions et restaure si nécessaire
    void sendData(const String& text); // Pour texte brut
    void handleMessage(); // Ajoute la méthode pour traiter la file d'attente

private:
    const char* ssid;       ///< Nom du réseau Wi-Fi
    const char* password;   ///< Mot de passe du réseau Wi-Fi
    AsyncWebServer server;  ///< Serveur HTTP asynchrone
    AsyncWebSocket ws;      ///< Gestion des WebSockets
    bool wifiConnected;     ///< État de la connexion Wi-Fi
    static WiFiWebSocket* instance; ///< Pointeur vers l'instance unique de la classe

    

    static void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len); // Gestion des événements WebSocket
};

#endif
