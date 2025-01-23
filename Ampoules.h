#ifndef AMPOULES_H
#define AMPOULES_H

#include <Arduino.h>

class Ampoule {
private:
    int pin;
    int etatBas;
    int etatHaut;
    String nom;
    String methode;
    unsigned long tempsTransition;
    unsigned long intervalCommutation;
    bool etat;
    bool actif;
    bool forcerActif;
    int valeurForcee;
    unsigned long dernierChangement;

public:
    Ampoule(int pinAssociee, int bas, int haut, String nomAmpoule,
            String mode, unsigned long transition = 0, unsigned long interval = 0);

    void run();
    void stop();
    bool  get_actif();
    void forceOutput(int valeur);
    void update(bool blinkState);
    void setEtatBas(int bas);
    void setEtatHaut(int haut);
    String getStatus();
};

#endif