#include "Ampoules.h"

// Constructeur par défaut
Ampoule::Ampoule() : pin(-1), etatBas(0), etatHaut(0), nom(""), methode(""),
                     tempsTransition(0), intervalCommutation(0), etat(false),
                     actif(false), forcerActif(false), valeurForcee(-1) {}

void Ampoule::init(int pinAssociee, int bas, int haut, String nomAmpoule,
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

    tempsTransition = (methode == "PWM" || transition > 0) ? transition : 100;
    intervalCommutation = (methode == "BUZ" && interval < 10) ? 500 : interval;

    etat = false;
    actif = false;
    forcerActif = false;
    valeurForcee = -1;
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
// Retourne l'état actif
bool Ampoule::get_actif() const {return actif;}

// Force une valeur spécifique pour la sortie
void Ampoule::forceOutput(int valeur) {
  if (valeur < 0) {
    forcerActif = false;
    return;
  }

  forcerActif = true;
  valeurForcee = valeur;
  analogWrite(pin, valeur);
}

// Mise à jour en fonction du mode et de l'état de clignotement
void Ampoule::update(bool blinkState) {
  if (forcerActif || !actif) return;

  unsigned long currentMillis = millis();

  if (methode == "CLI") {
    if (etat != blinkState) {  // Vérifiez si l'état doit changer
        etat = blinkState;
        analogWrite(pin, etat ? etatHaut : etatBas);
      }
  }else if (methode == "BUZ") {
    if (etat != blinkState) {  // Vérifiez si l'état doit changer
        etat = blinkState;
        digitalWrite(pin, etat ? HIGH : LOW);
        Serial.println("BUZ Update : " + String(etat ? "ON" : "OFF"));
      }
  } else if (methode == "PWM") {
    if (tempsTransition <= 0) {
      tempsTransition = 100;
    }
    analogWrite(pin, etat ? etatHaut : etatBas);
  } 
  else if (methode == "TOR"){}
  else{Serial.println(nom + " mod inconnu " + methode);}
}

// Retourne les informations sous forme de texte
String Ampoule::getStatus() {
    String status = "Nom: " + nom + ", Pin: " + String(pin);
    status += ", Methode: " + methode + ", Etat: ";
    status += etat ? "ON" : "OFF";
    status += ", Actif: " + String(actif ? "Oui" : "Non");
    status += ", Forcer: " + String(forcerActif ? "Oui" : "Non");
    return status;
}

// Génère un JSON avec les infos de l'ampoule
String Ampoule::getInfo() const {
    JsonDocument doc;
    doc["name"] = nom;
    doc["pin"] = pin;
    doc["etat_bas"] = etatBas;
    doc["etat_haut"] = etatHaut;
    doc["mode"] = methode;
    doc["actif"] = get_actif();

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}


