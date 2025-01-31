// Gestion des informations de statut et de connexion
const InfoManager = (function () {
    const status = document.getElementById("status");
    const loader = document.getElementById("loader");
    const infoDiv = document.getElementById("info");

    // Met à jour les informations dans le panneau
    function displayInfo(data) {
        const uptimeFormatted = formatUptime(data.uptime);
        const rssiColor = getRssiColor(systemData.rssi);
         const content = `
            <p><strong style="color:orange">Nom:</strong> ${systemData.name}</p>
            <hr>
            <p><strong style="color:orange">Version:</strong> ${systemData.version}</p>
            <p><strong style="color:orange">Adresse IP locale:</strong> ${systemData.localip}</p>
            <p><strong style="color:orange">SSID:</strong> ${systemData.ssid}</p>
            <p><strong style="color:orange">RSSI:</strong> <span style="color:${rssiColor}">${systemData.rssi} dBm</span></p>
            <hr>
            <p><strong style="color:orange">Entrées:</strong> ${systemData.input}</p>
            <p><strong style="color:orange">Sorties:</strong> ${systemData.output}</p>
            <hr>
            <p><strong style="color:orange">Uptime:</strong> ${systemData.uptime}s</p>
        `;
        infoDiv.innerHTML = content;
        status.style.display = "none"; // Cache le loader
    }

    // Détermine la couleur en fonction de la puissance du signal RSSI
    function getRssiColor(rssi) {
        if (rssi >= -50) {
            return 'green'; // Excellent signal
        } else if (rssi >= -70) {
            return 'yellow'; // Good signal
        } else if (rssi >= -85) {
            return 'orange'; // Medium signal
        } else {
            return 'red'; // Poor signal
        }
    }

    // Formatte le temps de fonctionnement (uptime) en format lisible
    function formatUptime(seconds) {
        const hours = Math.floor(seconds / 3600);
        const minutes = Math.floor((seconds % 3600) / 60);
        const secs = seconds % 60;
        if (hours > 0) {
            return `${hours}h ${minutes}m ${secs}s`;
        } else if (minutes > 0) {
            return `${minutes}m ${secs}s`;
        } else {
            return `${secs}s`;
        }
    }

    // Expose les fonctions publiques
    return {
        displayInfo,
    };
})();
