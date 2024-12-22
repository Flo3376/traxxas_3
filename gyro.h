#ifndef GYRO_H
#define GYRO_H

#include <Adafruit_MPU6050.h>
// Variables globales externes (si nécessaires pour d'autres modules)
extern bool debug_gyro;

// Constantes et broches pour le gyroscope
#define I2C_SDA 21
#define I2C_SCL 22

// Variables pour les offsets et données du gyroscope
extern float gyroOffsetX, gyroOffsetY, gyroOffsetZ;
extern float accelOffsetX, accelOffsetY, accelOffsetZ;
extern float roll, pitch, ax, ay, az;
extern int orientation_gyro;
extern int limit_g_x, limit_g_y;

// Initialisation du gyroscope
void setupGyro();

// Lecture des données du gyroscope
void readGyro();

// Lecture des données du gyroscope
void resetGyro();

// Génération des données JSON
String generateGyroJson();

#endif
