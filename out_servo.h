#ifndef Out_SERVO_H
#define Out_SERVO_H


#include "config.h"
#include <ESP32Servo.h>

class ServoController {
public:
    void initializeServos();
    void pulseToAsync(int servoIndex, int angle, unsigned long duration);
    void goToAsync(int servoIndex, int targetAngle, unsigned long duration);
    void jumpTo(int servoIndex, int targetAngle);
    void update(); // À appeler régulièrement dans `loop`.
    void writeAngle(int servoIndex, int angle);

private:
    Servo servos[3]; // 3 servos configurés
    int servoPins[3] = {aux_serv_1, aux_serv_2, aux_serv_3}; // Correspond aux constantes aux_serv_1, aux_serv_2, etc.
    int servoAngles[3] = {90, 90, 90}; // Angles initiaux
    struct ServoState {
        bool active = false;           // Suivi de l'état actif pour "go_to" et "pulse_to"
        bool returningToCenter = false; // Pour gérer le retour au centre après "pulse_to"
        int originalAngle = 90;
        int currentAngle = 90;         // Angle actuel suivi
        int targetAngle = 90;          // Angle cible
        unsigned long startTime = 0;
        unsigned long duration = 0;
    };

    ServoState servoStates[3]; // Gestion des états pour chaque servo
};

#endif