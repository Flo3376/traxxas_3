#include "out_servo.h"

// Constructeur
CustomServo::CustomServo(int pin,int min, int max, int initialPos) {
    this->pin = pin;
    this->currentAngle = initialPos;
    this->servo.attach(pin,min,max);        // Attache le servo à la broche spécifiée
    this->servo.write(initialPos); // Définit la position initiale
}

// Déplacement asynchrone
void CustomServo::goTo(int targetAngle, unsigned long duration) {
    this->startTime = millis();
    this->duration = duration;
    this->startAngle = currentAngle;
    this->targetAngle = targetAngle;
    this->active = true;
}

int CustomServo::getPin() const {
    return pin;
}

// Mise à jour à appeler dans loop()
void CustomServo::update() {
    if (active) {
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - startTime;

        if (elapsedTime >= duration) {
            // Terminer le mouvement
            servo.write(targetAngle);
            currentAngle = targetAngle;
            active = false;
        } else {
            // Interpolation linéaire
            float progress = (float)elapsedTime / (float)duration;
            int newAngle = startAngle + (targetAngle - startAngle) * progress;
            servo.write(newAngle);
            currentAngle = newAngle;
        }
    }
}

// Déplacement immédiat
void CustomServo::jumpTo(int targetAngle) {
    servo.write(targetAngle);
    currentAngle = targetAngle;
    active = false;
}

// Récupérer l'angle actuel
int CustomServo::getCurrentAngle() {
    return currentAngle;
}
