#include "gyro.h"
#include <ArduinoJson.h>

// Instance du capteur GY-521
Adafruit_MPU6050 mpu;

// Variables globales
bool debug_gyro = false;

float gyroOffsetX = -3.3, gyroOffsetY = 6.7, gyroOffsetZ = 0.0;
float accelOffsetX = 0, accelOffsetY = -0, accelOffsetZ = -9.47;
float roll = 5.0, pitch = 5.0;
float ax = -0.66, ay = -0.66, az = 9.41;

int limit_g_x=10;
int limit_g_y=15;

int orientation_gyro=1;

//unsigned long previousGyroMillis = 0;         // Dernière lecture du gyroscope
//const unsigned long gyroInterval = 5000;      // Intervalle en millisecondes (2 secondes)

// Initialisation du gyroscope
void setupGyro() {
    // Initialisation du bus I2C
    Wire.begin(I2C_SDA, I2C_SCL);

    unsigned long startTime = millis();
    bool gyroDetected = false;

    // Tenter de détecter le gyroscope pendant 5 secondes
    while (millis() - startTime < 5000) {
        if (mpu.begin()) {
            gyroDetected = true;
            break;
        }
        delay(500); // Attendre un peu avant de réessayer
    }

    if (gyroDetected) {
        Serial.println("MPU6050 prêt!");
        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);
    } else {
        Serial.println("Impossible de détecter le MPU6050 après 5 secondes. La partie gyro sera ignorée.");
    }
}

// Lecture des données du gyroscope
void readGyro() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Lire les données des accélérations
    float raw_ax = a.acceleration.x;
    float raw_ay = a.acceleration.y;
    float raw_az = a.acceleration.z;

    // Ajuster les axes selon l'orientation du boîtier
    switch (orientation_gyro) {
        case 1: // 1/4 de tour
            ax = -raw_ay;
            ay = raw_ax;
            az = raw_az;
            break;
        case 2: // 1/2 tour
            ax = -raw_ax;
            ay = -raw_ay;
            az = raw_az;
            break;
        case 3: // 3/4 de tour
            ax = raw_ay;
            ay = -raw_ax;
            az = raw_az;
            break;
        default: // Orientation normale (0 tour)
            ax = raw_ax;
            ay = raw_ay;
            az = raw_az;
            break;
    }

    // Calculer roll et pitch en tenant compte des axes ajustés
    roll = gyroOffsetX + atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI;
    pitch = gyroOffsetY + atan2(-ax, sqrt(ay * ay + az * az)) * 180 / PI;


    if (debug_gyro) {
        Serial.println("=== Gyroscope Data ===");
        Serial.print("Orientation: "); Serial.println(orientation_gyro);
        Serial.print("ax: "); Serial.println(ax);
        Serial.print("ay: "); Serial.println(ay);
        Serial.print("az: "); Serial.println(az);
        Serial.print("Roll: "); Serial.println(roll);
        Serial.print("Pitch: "); Serial.println(pitch);
        Serial.print("Limit X: "); Serial.println(limit_g_x);
        Serial.print("Limit Y: "); Serial.println(limit_g_y);
    }
}

// Génération des données JSON
String generateGyroJson() {
    JsonDocument doc;

    // Ajouter les valeurs au document JSON
    doc["type"] = "gyro";
    doc["Orientation"] = orientation_gyro;
    doc["roll"] = roll;
    doc["pitch"] = pitch;
    doc["Offset_X"] = gyroOffsetX;
    doc["Offset_Y"] = gyroOffsetY;
    doc["ay"] = ay;
    doc["az"] = az;
    doc["Limit_g_x"] = limit_g_x;
    doc["Limit_g_y"] = limit_g_y;

    // Convertir le document en une chaîne JSON
    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

void resetGyro() {
    // Lire les données actuelles du gyroscope
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Calculer les valeurs actuelles de roll et pitch
    float currentRoll = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180 / PI;
    float currentPitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;

    // Définir les offsets basés sur les valeurs actuelles
    gyroOffsetX = currentRoll;
    gyroOffsetY = -currentPitch;

    // Optionnel: Afficher les nouvelles valeurs d'offset pour vérification
    if (debug_gyro) {
        Serial.println("=== Gyroscope Reset ===");
        Serial.print("New Gyro Offset X: "); Serial.println(gyroOffsetX);
        Serial.print("New Gyro Offset Y: "); Serial.println(gyroOffsetY);
    }
}
