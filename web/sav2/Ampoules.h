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

  public:
      // Constructeur avec paramètres
      Clignotant(int pinAssociee, unsigned long intervalle, int bas = 0, int haut = 255)
          : pin(pinAssociee), vitesse(intervalle), dernierChangement(0), etat(false),
            etatBas(bas), etatHaut(haut), actif(true) {
          pinMode(pin, OUTPUT);
          analogWrite(pin, etatBas); // Initialiser avec l'état bas
      }

      // Modifier la broche associée
      void attacherPin(int pinAssociee) {
          pin = pinAssociee;
          pinMode(pin, OUTPUT);
          analogWrite(pin, etatBas); // Réinitialiser avec l'état bas
      }

      // Modifier la vitesse (intervalle ON/OFF)
      void setVitesse(unsigned long intervalle) {
          vitesse = intervalle;
      }

      // Modifier la vitesse (intervalle ON/OFF)
      void setEtatBas(int eB) {
          etatBas = eB;
      }

      // Modifier la vitesse (intervalle ON/OFF)
      void setEtatHaut(int eH) {
          etatHaut = eH;
      }

      // Arrêter le clignotant et appliquer l'état bas
      void stop() {
          actif = false;
          etat = false;
          analogWrite(pin, etatBas); // Appliquer l'état bas
      }

      void run() {
          if (pin == -1) {
              //Serial.println("Erreur : Pin non configurée");
              return;
          }

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

      // Obtenir l'état du clignotant (pour debug)
      String getStatus() {
          String status = "Pin: " + String(pin);
          status += ", Etat: " + String(etat ? "ON" : "OFF");
          status += ", Actif: " + String(actif ? "Oui" : "Non");
          status += ", Etat Bas: " + String(etatBas);
          status += ", Etat Haut: " + String(etatHaut);
          return status;
      }
};

class Angel_Eyes {
    private:
        int pin;                      // Numéro de la broche
        unsigned long vitesse;        // Durée totale de la montée/descente (en ms)
        unsigned long dernierChangement; // Temps du dernier changement de luminosité
        int etatBas;                  // Valeur de l'état bas (PWM 0-255)
        int etatHaut;                 // Valeur de l'état haut (PWM 0-255)
        int luminositeActuelle;       // Luminosité actuelle (PWM 0-255)
        bool actif;                   // Indique si l'angel eyes est actif
        bool montante;                // Indique si on est en montée ou descente
        int pas;                      // Valeur de l'incrément/décrément de luminosité

    public:
        // Constructeur avec paramètres
        Angel_Eyes(int pinAssociee, unsigned long dureeTransition, int bas = 0, int haut = 255)
            : pin(pinAssociee), vitesse(dureeTransition), dernierChangement(0),
              etatBas(bas), etatHaut(haut), luminositeActuelle(bas),
              actif(false), montante(false), pas(1) {
            pinMode(pin, OUTPUT);
            analogWrite(pin, etatBas); // Initialiser avec l'état bas
            calculerPas();            // Calculer le pas initial pour la transition
        }

        // Méthode pour recalculer le pas de transition en fonction de la durée totale
        void calculerPas() {
            int delta = abs(etatHaut - etatBas); // Différence entre état bas et haut
            pas = delta / (vitesse / 10);        // Pas calculé pour un changement toutes les 10 ms
            if (pas < 1) pas = 1;               // Assurer un minimum de pas
        }

        // Modifier la vitesse (durée de la montée/descente)
        void setVitesse(unsigned long dureeTransition) {
            vitesse = dureeTransition;
            calculerPas();
        }

        // Modifier les états bas et haut
        void setEtatBas(int bas) {
            etatBas = bas;
            calculerPas();
        }

        void setEtatHaut(int haut) {
            etatHaut = haut;
            calculerPas();
        }

        // Allumer progressivement
        void run() {
            actif = true;
            montante = true;  // Passer en mode montée
        }

        // Éteindre progressivement
        void stop() {
            actif = true;
            montante = false; // Passer en mode descente
        }

        // Gestion progressive de la luminosité
        void update() {
            if (!actif) return; // Si pas actif, ne rien faire

            unsigned long currentMillis = millis();

            if (currentMillis - dernierChangement >= 10) { // Transition toutes les 10 ms
                dernierChangement = currentMillis;

                if (montante && luminositeActuelle < etatHaut) {
                    luminositeActuelle += pas;
                    if (luminositeActuelle > etatHaut) luminositeActuelle = etatHaut; // Limiter à l'état haut
                } else if (!montante && luminositeActuelle > etatBas) {
                    luminositeActuelle -= pas;
                    if (luminositeActuelle < etatBas) luminositeActuelle = etatBas; // Limiter à l'état bas
                } else {
                    actif = false; // Arrêter lorsque la transition est terminée
                }

                // Appliquer la luminosité actuelle
                analogWrite(pin, luminositeActuelle);
            }
        }

        // Obtenir l'état de l'angel eyes (pour debug)
        String getStatus() {
            String status = "Pin: " + String(pin);
            status += ", Luminosité Actuelle: " + String(luminositeActuelle);
            status += ", Actif: " + String(actif ? "Oui" : "Non");
            status += ", Montée: " + String(montante ? "Oui" : "Non");
            return status;
        }
};

#endif
