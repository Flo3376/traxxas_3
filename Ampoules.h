#ifndef AMPOULES_H
#define AMPOULES_H

#include <ArduinoJson.h>                   // Bibliothèque pour la gestion du format JSON
#include <Arduino.h>

// Définition de la classe Ampoule pour gérer des ampoules LED ou autres sorties
class Ampoule {
private:
    int pin;                              // Broche associée à l'ampoule
    int etatBas;                          // Niveau bas (ex: 0 pour éteint)
    int etatHaut;                         // Niveau haut (ex: 255 pour allumé en PWM)
    String nom;                           // Nom de l'ampoule
    String methode;                       // Méthode de contrôle (TOR, PWM, CLI, BUZ)
    unsigned long tempsTransition;        // Durée de transition en mode PWM
    unsigned long intervalCommutation;    // Intervalle pour les clignotements
    bool etat;                            // État actuel (allumé/éteint)
    bool actif;                           // Indique si l'ampoule est active
    bool forcerActif;                     // Force un état spécifique
    int valeurForcee;                     // Valeur forcée pour l'ampoule
    unsigned long dernierChangement;      // Dernière mise à jour

public:
    Ampoule(); // Constructeur par défaut
    Ampoule(int pinAssociee, int bas, int haut, String nomAmpoule,
            String mode, unsigned long transition = 0, unsigned long interval = 0);

    void init(int pinAssociee, int bas, int haut, String nomAmpoule,
              String mode, unsigned long transition = 0, unsigned long interval = 0);

    void run();                         // Active l'ampoule
    void stop();                        // Désactive l'ampoule
    bool get_actif() const;             // Vérifie si l'ampoule est active
    void forceOutput(int valeur);       // Force une valeur donnée
    void update(bool blinkState);       // Met à jour l'état en fonction du mode (clignotement, PWM...)
    void setEtatBas(int bas);           // Définit le niveau bas
    void setEtatHaut(int haut);         // Définit le niveau haut
    String getStatus();                 // Retourne l'état sous forme de texte
    String getInfo() const;             // Retourne les informations sous forme JSON
};

#endif
