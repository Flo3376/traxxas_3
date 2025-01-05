#include "out_servo.h"
#include <Arduino.h>
#include "config.h"


void ServoController::initializeServos() {
    for (int i = 0; i < 3; i++) {
        servos[i].attach(servoPins[i], minPulse, maxPulse);
        servos[i].write(servoAngles[i]);

      }
}

void ServoController::jumpTo(int servoIndex, int targetAngle) {
    if (servoIndex >= 0 && servoIndex < 3) {
        targetAngle = constrain(targetAngle, 0, 180);
        servos[servoIndex].write(targetAngle);
        servoStates[servoIndex].currentAngle = targetAngle;
    }
}

void ServoController::goToAsync(int servoIndex, int targetAngle, unsigned long duration) {
    if (servoIndex >= 0 && servoIndex < 3) {
        ServoState &state = servoStates[servoIndex];
        
        state.active = true;
        state.returningToCenter = false;
        state.originalAngle = state.currentAngle; // Angle de départ
        state.targetAngle = constrain(targetAngle, 0, 180); // Angle cible
        state.startTime = millis();
        state.duration = duration;
    }
}
void ServoController::pulseToAsync(int servoIndex, int angle, unsigned long duration) {
    if (servoIndex >= 0 && servoIndex < 3) {
        ServoState &state = servoStates[servoIndex];

        state.active = true;
        state.returningToCenter = true;
        state.originalAngle = 90; // Centre
        state.targetAngle = constrain(angle, 0, 180);
        state.startTime = millis();
        state.duration = duration;

        // Déplacer immédiatement vers l'angle cible
        servos[servoIndex].write(state.targetAngle);
        state.currentAngle = state.targetAngle;
    }
}

void ServoController::writeAngle(int servoIndex, int angle) {
    if (servoIndex >= 0 && servoIndex < 3) { // Vérifie que l'index du servo est valide
        angle = constrain(angle, 0, 180);   // Limite l'angle à la plage acceptable
        servos[servoIndex].write(angle);    // Commande directe au servo
        servoStates[servoIndex].currentAngle = angle; // Met à jour l'état actuel

        Serial.print("servo : ");
        Serial.print(servoIndex);
        Serial.print("     angle : ");
        Serial.println(angle);
    } else {
        Serial.print("Erreur: servoIndex ");
        Serial.print(servoIndex);
        Serial.println(" est invalide.");
    }
}


void ServoController::update() {
    unsigned long currentTime = millis();

    for (int i = 0; i < 3; i++) {
        ServoState &state = servoStates[i];

        if (state.active) {
            unsigned long elapsedTime = currentTime - state.startTime;

            Serial.print("Servo "); Serial.print(i);
            Serial.print(" actif. TargetAngle: "); Serial.print(state.targetAngle);
            Serial.print(" CurrentAngle: "); Serial.print(state.currentAngle);
            Serial.print(" ElapsedTime: "); Serial.println(elapsedTime);

            if (elapsedTime >= state.duration) {
                Serial.println("Temps écoulé. Action finale en cours...");
            } else {
                Serial.println("Transition en cours...");
            }

            if (elapsedTime >= state.duration) {
                // Si c'est "pulse_to", retour au centre après la durée
                if (state.returningToCenter) {
                    servos[i].write(state.originalAngle);
                    state.currentAngle = state.originalAngle;
                    state.active = false;
                } else {
                    state.active = false; // Fin de "go_to"
                }
            } else if (!state.returningToCenter) {
                // Transition pour "go_to"
                float progress = (float)elapsedTime / (float)state.duration;
                int newAngle = state.originalAngle + (state.targetAngle - state.originalAngle) * progress;
                servos[i].write(newAngle);
                state.currentAngle = newAngle;
            }
        }
    }
}
