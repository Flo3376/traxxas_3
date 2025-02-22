#include "gyro.h"
#include "config.h"

// Instance du capteur GY-521 (MPU6050)
Adafruit_MPU6050 mpu;

// Définition des variables globales, initialisées avec les valeurs par défaut de config.h
float gyroOffsetX = DEFAULT_GYRO_OFFSET_X;
float gyroOffsetY = DEFAULT_GYRO_OFFSET_Y;
float gyroOffsetZ = DEFAULT_GYRO_OFFSET_Z;

float accelOffsetX = DEFAULT_ACCEL_OFFSET_X;
float accelOffsetY = DEFAULT_ACCEL_OFFSET_Y;
float accelOffsetZ = DEFAULT_ACCEL_OFFSET_Z;

// Valeurs par défaut des angles d'inclinaison
// Ces valeurs sont intentionnellement erronées au démarrage pour éviter les fausses lectures
// Comme une aiguille de jauge d'essence qui ne bouge que lorsque le contact est mis
float roll = 5;
float pitch = 5;

float ax = DEFAULT_AX;
float ay = DEFAULT_AY;
float az = DEFAULT_AZ;

int limit_g_x = DEFAULT_LIMIT_G_X;
int limit_g_y = DEFAULT_LIMIT_G_Y;

int orientation_gyro = DEFAULT_ORIENTATION_GYRO;

// Variable de debug
bool debug_gyro = false;

// Initialisation du gyroscope
void setupGyro() {
    Wire.begin(I2C_SDA, I2C_SCL);
    unsigned long startTime = millis();
    bool gyroDetected = false;

    // Tentative de détection du capteur sur 5 secondes
    while (millis() - startTime < 5000) {
        if (mpu.begin()) {
            gyroDetected = true;
            break;
        }
        delay(500);
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

// Lecture des données du gyroscope et correction en fonction de l'orientation
void readGyro() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Lecture des accélérations brutes
    float raw_ax = a.acceleration.x;
    float raw_ay = a.acceleration.y;
    float raw_az = a.acceleration.z;

    // Ajustement des axes en fonction de l'orientation du capteur dans le boîtier
    switch (orientation_gyro) {
        case 1: // Rotation 90°
            ax = -raw_ay;
            ay = raw_ax;
            az = raw_az;
            break;
        case 2: // Rotation 180°
            ax = -raw_ax;
            ay = -raw_ay;
            az = raw_az;
            break;
        case 3: // Rotation 270°
            ax = raw_ay;
            ay = -raw_ax;
            az = raw_az;
            break;
        default: // Orientation par défaut
            ax = raw_ax;
            ay = raw_ay;
            az = raw_az;
            break;
    }

    // Calcul des angles Roll et Pitch à partir des accélérations mesurées
    roll = gyroOffsetX + atan2(ay, sqrt(ax * ax + az * az)) * 180 / PI;
    pitch = gyroOffsetY + atan2(-ax, sqrt(ay * ay + az * az)) * 180 / PI;

    // Affichage des données si le mode debug est activé
    if (debug_gyro) {
        Serial.println("=== Gyroscope Data ===");
        Serial.print("Orientation: "); Serial.println(orientation_gyro);
        Serial.print("Accélération X: "); Serial.println(ax);
        Serial.print("Accélération Y: "); Serial.println(ay);
        Serial.print("Accélération Z: "); Serial.println(az);
        Serial.print("Angle Roll: "); Serial.println(roll);
        Serial.print("Angle Pitch: "); Serial.println(pitch);
        Serial.print("Limite G X: "); Serial.println(limit_g_x);
        Serial.print("Limite G Y: "); Serial.println(limit_g_y);
    }
}

// Réinitialisation des offsets du gyroscope pour recalibrer les valeurs mesurées
void resetGyro() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Recalcul des valeurs actuelles de roll et pitch comme nouveaux offsets
    float currentRoll = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180 / PI;
    float currentPitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;

    gyroOffsetX = currentRoll;
    gyroOffsetY = -currentPitch;

    // Affichage des nouvelles valeurs si le mode debug est activé
    if (debug_gyro) {
        Serial.println("=== Gyroscope Reset ===");
        Serial.print("Nouvel Offset X: "); Serial.println(gyroOffsetX);
        Serial.print("Nouvel Offset Y: "); Serial.println(gyroOffsetY);
    }
}
