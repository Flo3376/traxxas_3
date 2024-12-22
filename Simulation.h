#ifndef SIMULATION_H
#define SIMULATION_H

#include <Arduino.h> // Nécessaire pour utiliser le type String et d'autres fonctionnalités Arduino


class Simulation {
private:
    float roll;                  // X
    float pitch;                 // Y
    bool active;                 // Indicateur de simulation en cours
    unsigned long lastUpdate;    // Dernière mise à jour
    unsigned long startTime;     // Heure de début de simulation
    const unsigned long interval; // Intervalle entre deux envois (ms)
    const unsigned long duration; // Durée totale de la simulation (ms)
    float amplitude;             // Amplitude des sinusoïdes
    float frequency;             // Fréquence des sinusoïdes
    float phaseShift;            // Déphasage des sinusoïdes
    int count;                   // Compteur de messages envoyés
    void generateData();         // Génère les données à envoyer
    String formatData() const;      // Formate les données en JSON
    

public:
    Simulation(unsigned long duration = 30000, float amplitude = 35.0, float frequency = 0.1);

    void start();                   // Démarre la simulation
    void stop();                    // Arrête la simulation
    String update();                // Met à jour la simulation et retourne les données
    bool isActive() const;          // Vérifie si la simulation est active
};

#endif // SIMULATION_H
