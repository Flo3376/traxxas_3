#ifndef AMPOULES_H
#define AMPOULES_H

#include <Arduino.h>

class Clignotant {
private:
    int pin;                      // Numéro de la broche
    unsigned long vitesse;        // Intervalle entre ON et OFF
    unsigned long dernierChangement; // Temps du dernier changement d'état
    bool etat;                    // État actuel (ON ou OFF)
    int etatBas;                  // Valeur de l'état bas (PWM 0-255)
    int etatHaut;                 // Valeur de l'état haut (PWM 0-255)
    bool actif;                   // Indique si le clignotant est actif
    bool forceActif;              // Indique si l'état forcé est actif

public:
    Clignotant(int pinAssociee, unsigned long intervalle, int bas = 0, int haut = 255)
        : pin(pinAssociee), vitesse(intervalle), dernierChangement(0), etat(false),
          etatBas(bas), etatHaut(haut), actif(true), forceActif(false) {
        pinMode(pin, OUTPUT);
        analogWrite(pin, etatBas); // Initialiser avec l'état bas
    }

    void forceOutput(int valeur) {
        if(valeur<0)
        {
          forceActif = false;
        }
        else{
          forceActif = true;
          analogWrite(pin, valeur);
        }
    }

    void disableForce() {
        forceActif = false;
    }

    void attacherPin(int pinAssociee) {
        pin = pinAssociee;
        pinMode(pin, OUTPUT);
        analogWrite(pin, etatBas);
    }

    void setVitesse(unsigned long intervalle) {
        vitesse = intervalle;
    }

    void setEtatBas(int eB) {
        etatBas = eB;
    }

    void setEtatHaut(int eH) {
        etatHaut = eH;
    }

    void stop() {
        if (forceActif) return;
        actif = false;
        etat = false;
        analogWrite(pin, etatBas);
    }

    void run() {
        if (pin == -1) {
                //Serial.println("Erreur : Pin non configurée");
                return;
            }
        if (forceActif) return;
        if (!actif) {
              // Réactiver automatiquement le clignotant si inactif
              actif = true;
              dernierChangement = millis(); // Réinitialiser le timer
              etat = false;                 // Repartir avec l'état bas
              analogWrite(pin, etatBas);
              //Serial.println("Clignotant réactivé");
              return;
          }

          unsigned long currentMillis = millis();

          // Vérifier si l'intervalle est écoulé
          if (currentMillis - dernierChangement >= vitesse) {
              dernierChangement = currentMillis; // Mettre à jour le timer
              etat = !etat;                      // Basculer l'état ON/OFF
              analogWrite(pin, etat ? etatHaut : etatBas);

              // Log de débogage
              //Serial.print("Etat changé : ");
              //Serial.println(etat ? "HAUT" : "BAS");
          } else {
              // Log pour suivre l'absence de changement
              //Serial.println("Aucun changement d'état");
          }
    }

    void update() {
        if (forceActif || !actif) return;

        unsigned long currentMillis = millis();

        if (currentMillis - dernierChangement >= vitesse) {
            dernierChangement = currentMillis;
            etat = !etat;
            analogWrite(pin, etat ? etatHaut : etatBas);
        }
    }

    String getStatus() {
        String status = "Pin: " + String(pin);
        status += ", Etat: " + String(etat ? "ON" : "OFF");
        status += ", Actif: " + String(actif ? "Oui" : "Non");
        status += ", Force Actif: " + String(forceActif ? "Oui" : "Non");
        status += ", Etat Bas: " + String(etatBas);
        status += ", Etat Haut: " + String(etatHaut);
        return status;
    }
};

class PWM_Light {
private:
    int pin;
    unsigned long vitesse;
    unsigned long dernierChangement;
    int etatBas;
    int etatHaut;
    int luminositeActuelle;
    bool actif;
    bool montante;
    int pas;
    bool forceActif;

public:
    PWM_Light(int pinAssociee, unsigned long dureeTransition, int bas = 0, int haut = 255)
        : pin(pinAssociee), vitesse(dureeTransition), dernierChangement(0),
          etatBas(bas), etatHaut(haut), luminositeActuelle(bas),
          actif(false), montante(false), pas(1), forceActif(false) {
        pinMode(pin, OUTPUT);
        analogWrite(pin, etatBas);
        calculerPas();
    }

    void forceOutput(int valeur) {
        if(valeur<0)
        {
          forceActif = false;
        }
        else{
          forceActif = true;
          analogWrite(pin, valeur);
        }
    }

    void disableForce() {
        forceActif = false;
    }

    void calculerPas() {
        int delta = abs(etatHaut - etatBas);
        pas = delta / (vitesse / 10);
        if (pas < 1) pas = 1;
    }

    void setVitesse(unsigned long dureeTransition) {
        vitesse = dureeTransition;
        calculerPas();
    }

    void setEtatBas(int bas) {
        etatBas = bas;
        calculerPas();
    }

    void setEtatHaut(int haut) {
        etatHaut = haut;
        calculerPas();
    }

    void run() {
        if (forceActif) return;
        actif = true;
        montante = true;
    }

    void stop() {
        if (forceActif) return;
        actif = true;
        montante = false;
    }

    void update() {
        if (forceActif || !actif) return;

        unsigned long currentMillis = millis();

        if (currentMillis - dernierChangement >= 10) {
            dernierChangement = currentMillis;

            if (montante && luminositeActuelle < etatHaut) {
                luminositeActuelle += pas;
                if (luminositeActuelle > etatHaut) luminositeActuelle = etatHaut;
            } else if (!montante && luminositeActuelle > etatBas) {
                luminositeActuelle -= pas;
                if (luminositeActuelle < etatBas) luminositeActuelle = etatBas;
            } else {
                actif = false;
            }

            analogWrite(pin, luminositeActuelle);
        }
    }

    String getStatus() {
        String status = "Pin: " + String(pin);
        status += ", Luminosité Actuelle: " + String(luminositeActuelle);
        status += ", Actif: " + String(actif ? "Oui" : "Non");
        status += ", Force Actif: " + String(forceActif ? "Oui" : "Non");
        return status;
    }
};

class TOR_Light {
private:
    int pin;
    int etatBas;
    int etatHaut;
    bool actif;
    bool forceActif;

public:
    TOR_Light(int pinAssociee, int bas = 0, int haut = 255)
        : pin(pinAssociee), etatBas(bas), etatHaut(haut), actif(false), forceActif(false) {
        pinMode(pin, OUTPUT);
        analogWrite(pin, etatBas);
    }

    void forceOutput(int valeur) {
        if(valeur<0)
        {
          forceActif = false;
        }
        else{
          forceActif = true;
          analogWrite(pin, valeur);
        }
    }

    void disableForce() {
        forceActif = false;
    }

    void run() {
        if (forceActif) return;
        actif = true;
        analogWrite(pin, etatHaut);
    }

    void stop() {
        if (forceActif) return;
        actif = false;
        analogWrite(pin, etatBas);
    }

    String getStatus() {
        String status = "Pin: " + String(pin);
        status += ", État: " + String(actif ? "Allumé" : "Éteint");
        status += ", Force Actif: " + String(forceActif ? "Oui" : "Non");
        return status;
    }
};
#endif
