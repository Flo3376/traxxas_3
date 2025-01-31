#ifndef AMPOULES_H
#define AMPOULES_H

#include <ArduinoJson.h>  // Lib pour l'utilisation du format JSON
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
    Ampoule(); // Constructeur par défaut
    Ampoule(int pinAssociee, int bas, int haut, String nomAmpoule,
            String mode, unsigned long transition = 0, unsigned long interval = 0);

    void init(int pinAssociee, int bas, int haut, String nomAmpoule,
              String mode, unsigned long transition = 0, unsigned long interval = 0);

    

    void run();
    void stop();
    bool get_actif() const;  // Ajout de 'const' ici
    void forceOutput(int valeur);
    void update(bool blinkState);
    void setEtatBas(int bas);
    void setEtatHaut(int haut);
    String getStatus();
    String getInfo() const;
};

#endif