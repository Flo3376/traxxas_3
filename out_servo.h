#ifndef OUT_SERVO_H
#define OUT_SERVO_H

#include <ESP32Servo.h> // Bibliothèque pour gérer les servos sur ESP32

// Définition de la classe CustomServo pour gérer un servo-moteur
class CustomServo {
public:
    // Constructeur : initialise un servo avec une broche, une plage min/max et une position initiale
    CustomServo(int pin, int min, int max, int initialPos = 0);

    // Déplacement progressif du servo vers un angle donné en un certain temps
    void goTo(int targetAngle, unsigned long duration);
    
    // Mise à jour du servo (à appeler régulièrement dans la boucle principale)
    void update();
    
    // Déplacement immédiat à un angle donné
    void jumpTo(int targetAngle);
    
    // Récupérer l'angle actuel du servo
    int getCurrentAngle();
    
    // Récupérer la broche associée au servo
    int getPin() const;

private:
    Servo servo;              // Instance de la classe Servo pour contrôler le moteur
    int pin;                  // Broche associée au servo
    int currentAngle;         // Angle actuel du servo
    int startAngle;           // Angle de départ pour les mouvements progressifs
    int targetAngle;          // Angle cible à atteindre
    unsigned long startTime;  // Temps de départ du mouvement progressif
    unsigned long duration;   // Durée totale du mouvement progressif
    bool active = false;      // Indique si un mouvement progressif est en cours
};

#endif // OUT_SERVO_H