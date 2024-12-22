#include "Simulation.h"

// Constructeur
Simulation::Simulation(unsigned long duration, float amplitude, float frequency)
    : roll(0), pitch(0), active(false), lastUpdate(0), startTime(0),
      interval(250), duration(duration), amplitude(amplitude),
      frequency(frequency), phaseShift(M_PI / 2), count(0) {}

// Génère les données à partir des sinusoïdes
void Simulation::generateData() {
    unsigned long elapsedTime = millis() - startTime;
    float timeInSeconds = elapsedTime / 1000.0;
    roll = amplitude * sin(2 * M_PI * frequency * timeInSeconds);
    pitch = amplitude * sin(2 * M_PI * frequency * timeInSeconds + phaseShift);
}

// Formate les données en JSON
String Simulation::formatData() const {
    return "{\"type\":\"simulation\",\"count\":" + String(count) +
           ",\"roll\":" + String(roll, 1) +
           ",\"pitch\":" + String(pitch, 1) + "}";
}

// Démarre une simulation
void Simulation::start() {
    if (!active) {
        active = true;
        startTime = millis();
        lastUpdate = millis();
        count = 0;
        Serial.println("Simulation démarrée.");
    }
}

// Arrête une simulation
void Simulation::stop() {
    if (active) {
        active = false;
        Serial.println("Simulation arrêtée.");
    }
}

// Met à jour la simulation et retourne les données si une mise à jour est nécessaire
String Simulation::update() {
    if (active) {
        unsigned long now = millis();

        // Arrête la simulation si elle dépasse la durée prévue
        if (now - startTime > duration) {
            stop();
            return ""; // Pas de données à envoyer
        }

        // Génère des données si l'intervalle est atteint
        if (now - lastUpdate >= interval) {
            lastUpdate = now;
            generateData();
            count++;
            return formatData();
        }
    }
    return ""; // Pas de nouvelles données
}

// Vérifie si la simulation est active
bool Simulation::isActive() const {
    return active;
}
