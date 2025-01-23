#include "Ampoules.h"



// Constructeur
Ampoule::Ampoule(int pinAssociee, int bas, int haut, String nomAmpoule,
                 String mode, unsigned long transition, unsigned long interval) {
  pin = pinAssociee;
  etatBas = bas < 0 ? 0 : bas;
  etatHaut = haut > 255 ? 255 : haut;
  nom = nomAmpoule;
  methode = mode;

  if (methode != "TOR" && methode != "PWM" && methode != "CLI" && methode != "BUZ") {
    Serial.println("Mode invalide. Réinitialisé à TOR.");
    methode = "TOR";
  }

  if (methode == "TOR" || methode == "CLI") {
    tempsTransition = 0;
  } else {
    if (transition <= 0) {
      Serial.println("tempsTransition invalide. Valeur par défaut appliquée : 100 ms.");
      tempsTransition = 100;
    } else {
      tempsTransition = transition;
    }
  }

  if (methode == "PWM") {
    intervalCommutation = 0;
  } else {
    if (interval < 10) {
      Serial.println("intervalCommutation trop court. Valeur par défaut appliquée : 500 ms.");
      intervalCommutation = 500;
    } else {
      intervalCommutation = interval;
    }
  }

  if (methode == "BUZ") {
    tempsTransition = 0;  // Pas de transition pour un buzzer
    if (interval < 10) {
      Serial.println("intervalCommutation trop court pour le buzzer. Valeur par défaut : 500 ms.");
      intervalCommutation = 500;  // Valeur par défaut pour BUZ
    } else {
      intervalCommutation = interval;
    }
  }

  etat = false;
  actif = false;
  forcerActif = false;
  valeurForcee = -1;
   // Initialisation du timer pour cet objet spécifique
  //dernierChangement = millis();  // On enregistre le moment de création
}

// Définir etatBas
void Ampoule::setEtatBas(int bas) {
  if (bas < 0) bas = 0;  // Garde-fou
  if (bas >= etatHaut) {
    Serial.println("Erreur : etatBas ne peut pas être supérieur ou égal à etatHaut");
    return;
  }
  etatBas = bas;
  if (!actif) {
    analogWrite(pin, etatBas);  // Appliquer immédiatement si l'ampoule est inactive
  }
}

// Définir etatHaut
void Ampoule::setEtatHaut(int haut) {
  if (haut > 255) haut = 255;  // Garde-fou
  if (haut <= etatBas) {
    Serial.println("Erreur : etatHaut ne peut pas être inférieur ou égal à etatBas");
    return;
  }
  etatHaut = haut;
  if (actif && methode == "TOR") {
    analogWrite(pin, etatHaut);  // Appliquer immédiatement si l'ampoule est active en mode TOR
  }
}

// Méthode run
void Ampoule::run() {
  if (forcerActif) return;
  actif = true;

  if (methode == "TOR") {
    analogWrite(pin, etatHaut);
    etat = true;
  } else if (methode == "PWM") {
    etat = true;
  } else if (methode == "CLI") {
    //etat = true;
    // Gestion CLI via update()
  } else if (methode == "BUZ") {
    //etat = true;
    // Gestion BUZ via update()
  }
}

// Méthode stop
void Ampoule::stop() {
  if (forcerActif) return;
  actif = false;

  if (methode == "TOR" || methode == "CLI") {
    analogWrite(pin, etatBas);
    etat = false;
  } else if (methode == "PWM") {
    etat = false;
  } else if (methode == "BUZ") {
    etat = false;
    digitalWrite(pin, LOW);
  }
}

bool  Ampoule::get_actif()
{
  return actif;
  }
// Méthode forceOutput
void Ampoule::forceOutput(int valeur) {
  if (valeur < 0) {
    forcerActif = false;
    return;
  }

  forcerActif = true;
  valeurForcee = valeur;
  analogWrite(pin, valeur);
}

// Méthode update
void Ampoule::update(bool blinkState) {
  if (forcerActif) return;
  if (!actif) return;

  unsigned long currentMillis = millis();

  if (methode == "CLI") {
    if (etat != blinkState) {  // Vérifiez si l'état doit changer
        etat = blinkState;
        analogWrite(pin, etat ? etatHaut : etatBas);
        //Serial.println("CLI Update : " + etat);
      }
  }else if (methode == "BUZ") {
    if (etat != blinkState) {  // Vérifiez si l'état doit changer
        etat = blinkState;
        digitalWrite(pin, etat ? HIGH : LOW);
        Serial.println("BUZ Update : " + etat);
      }
  } else if (methode == "PWM") {
    if (tempsTransition <= 0) {
      tempsTransition = 100;
    }

    if (etat) {
      analogWrite(pin, etatHaut);
    } else {
      analogWrite(pin, etatBas);
    }
  } 
  else{Serial.println(nom + " mod inconnu " + methode);}
}

// Méthode getStatus
String Ampoule::getStatus() {
  String status = "Nom: " + nom + ", Pin: " + String(pin);
  status += ", Methode: " + methode + ", Etat: ";
  status += etat ? "ON" : "OFF";
  status += ", Actif: " + String(actif ? "Oui" : "Non");
  status += ", Forcer: " + String(forcerActif ? "Oui" : "Non");
  return status;
}
