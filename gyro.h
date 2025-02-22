#ifndef GYRO_H
#define GYRO_H

#include <Adafruit_MPU6050.h>
#include "config.h"

// Déclaration des variables globales définies dans config.h
extern float gyroOffsetX, gyroOffsetY, gyroOffsetZ;
extern float accelOffsetX, accelOffsetY, accelOffsetZ;
extern float roll, pitch;
extern float ax, ay, az;
extern int limit_g_x, limit_g_y;
extern int orientation_gyro;

// Déclaration d'une variable globale pour activer/désactiver le debug du gyroscope
extern bool debug_gyro;

// Fonction d'initialisation du gyroscope (configuration et vérification du capteur)
void setupGyro();

// Fonction de lecture des données du gyroscope et mise à jour des variables globales
void readGyro();

// Fonction pour réinitialiser les offsets du gyroscope afin de calibrer les mesures
void resetGyro();

#endif