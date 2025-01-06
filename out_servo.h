#ifndef OUT_SERVO_H
#define OUT_SERVO_H

#include <ESP32Servo.h> // Bibliothèque spécifique à l'ESP32

class CustomServo {
public:
    // Constructeur
    CustomServo(int pin, int min, int max,int initialPos = 0);

    // Méthodes publiques
    void goTo(int targetAngle, unsigned long duration); // Déplacement asynchrone
    void update();                                      // Mise à jour à appeler régulièrement
    void jumpTo(int targetAngle);                      // Déplacement immédiat
    int getCurrentAngle();                             // Récupérer l'angle actuel

private:
    // Variables internes
    Servo servo; // Utilisation de la classe ESP32Servo
    int pin;
    int currentAngle;
    int startAngle;
    int targetAngle;
    unsigned long startTime;
    unsigned long duration;
    bool active = false;
};

#endif // OUT_SERVO_H
