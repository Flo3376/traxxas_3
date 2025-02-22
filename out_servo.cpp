#include "out_servo.h"

// Constructeur : Initialise le servo sur une broche spécifique avec des limites min/max et une position initiale
CustomServo::CustomServo(int pin, int min, int max, int initialPos) {
    this->pin = pin;
    this->currentAngle = initialPos;
    this->servo.attach(pin, min, max);      // Attache le servo à la broche avec les limites
    this->servo.write(initialPos);          // Définit la position initiale
}

// Déplacement progressif vers un angle cible en une durée donnée
void CustomServo::goTo(int targetAngle, unsigned long duration) {
    this->startTime = millis();            // Enregistre le temps de début du mouvement
    this->duration = duration;             // Définit la durée du mouvement
    this->startAngle = currentAngle;       // Sauvegarde l'angle de départ
    this->targetAngle = targetAngle;       // Définit l'angle cible
    this->active = true;                   // Active le mouvement progressif
}

// Récupère la broche associée au servo
int CustomServo::getPin() const {
    return pin;
}

// Mise à jour du mouvement progressif (à appeler régulièrement dans loop())
void CustomServo::update() {
    if (active) {
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - startTime;

        if (elapsedTime >= duration) {
            // Si la durée est écoulée, on met le servo à sa position finale
            servo.write(targetAngle);
            currentAngle = targetAngle;
            active = false; // Désactive le mouvement
        } else {
            // Calcul de l'interpolation linéaire entre l'angle de départ et l'angle cible
            float progress = (float)elapsedTime / (float)duration;
            int newAngle = startAngle + (targetAngle - startAngle) * progress;
            servo.write(newAngle);
            currentAngle = newAngle;
        }
    }
}

// Déplacement immédiat du servo à un angle donné
void CustomServo::jumpTo(int targetAngle) {
    servo.write(targetAngle);
    currentAngle = targetAngle;
    active = false; // Désactive tout mouvement progressif en cours
}

// Récupérer l'angle actuel du servo
int CustomServo::getCurrentAngle() {
    return currentAngle;
}
