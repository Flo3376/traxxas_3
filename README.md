# Traxxas_3 - Contrôle ESP32 pour 4x4 Télécommandé 🚗📡

Ce projet permet de contrôler un **4x4 télécommandé** à l’aide d’un **ESP32** via **Wi-Fi et WebSocket**.  
Il inclut la gestion des **servos, moteurs et capteurs**, et permet d'interagir en temps réel via WebSocket.

## 📌 Fonctionnalités actuellement implémentées
✅ **Contrôle des servos et moteurs** via ESP32  
✅ **WebSocket pour une communication rapide**  
✅ **Gestion du gyroscope (MPU6050) pour la détection d’inclinaison**  
✅ **Sauvegarde persistante de `use_limit` dans la mémoire Flash**  
✅ **Personnalisation de certains paramètres via WebSocket**  

---

## 📥 Installation
### **1️⃣ Cloner le dépôt**
```sh
git clone https://github.com/Flo3376/traxxas_3.git
cd traxxas_3
```

### **2️⃣ Installer les bibliothèques nécessaires**
Les bibliothèques suivantes doivent être installées dans l’IDE Arduino ou PlatformIO :

| 📚 Bibliothèque | 📦 Version Recommandée |
|--------------|------------------|
| **Async TCP** | `3.3.5` |
| **ESP Async WebServer** | `3.7.1` |
| **ESP32Servo** | `3.0.6` |
| **Adafruit MPU6050** | `2.2.6` |
| **Adafruit Unified Sensor** | `1.1.15` |
| **Adafruit SSD1306** | `2.5.13` |
| **Adafruit GFX Library** | `1.11.11` |
| **Adafruit BusIO** | `1.17.0` |
| **ArduinoJson** | `7.3.0` |

📌 **Astuce :** Pour installer toutes ces bibliothèques en une seule commande avec PlatformIO :
```sh
pio lib install "AsyncTCP" "ESP Async WebServer" "ESP32Servo" "Adafruit MPU6050" "ArduinoJson"
```

---

## ⚙️ Gestion de `use_limit`

- `use_limit` est un paramètre permettant d’activer ou désactiver une limite d’inclinaison basée sur le MPU6050.
- Il peut être modifié via WebSocket ou un bouton physique sur l'ESP32.
- La valeur est sauvegardée **de manière persistante** dans la mémoire Flash, et sera restaurée après un redémarrage.

### **📌 Modifier `use_limit` via WebSocket**
Une fois connecté à WebSocket, envoyer :  
- **Pour activer** : `"use_limit_on"`  
- **Pour désactiver** : `"use_limit_off"`  

---

## 🚀 Utilisation
1. **Allumer l’ESP32.**
2. **Se connecter à WebSocket pour modifier `use_limit` en temps réel.**
3. **Le changement est sauvegardé et sera restauré après un redémarrage.**

---

## 🛠 Développement & Personnalisation

### **🔧 Ajout de nouvelles fonctionnalités**
Ce projet est en cours d’évolution. Les fonctionnalités suivantes sont prévues :  
- **Mode AP pour configurer le Wi-Fi sans PC** 🔜  
- **Personnalisation des canaux `IN/OUT` via WebSocket** 🔜  
- **Sauvegarde/restauration des paramètres via JSON** 🔜  
- **Reboot et mise à jour à distance via WebSocket** 🔜  

---

## 📖 Licence
Ce projet est sous **licence MIT**.  
Vous êtes libre de l’utiliser, modifier et partager. 🎯

---

## 🤝 Contribuer
Les contributions sont les bienvenues ! 🛠  
- **Signalez un bug** en ouvrant une *Issue*.  
- **Proposez des améliorations** via une *Pull Request*.  

---

## 📞 Contact
Si vous avez des questions, vous pouvez me contacter via **GitHub** ou **[Flo3376](https://github.com/Flo3376)**.  

🚀 **Bon développement et amuse-toi avec ton 4x4 télécommandé !** 🛻😃
