const JaugeManager = (function () {
    const gauges = {}; // Stocke les jauges créées dynamiquement

    // Fonction pour créer une jauge
    function createGauge(id, label, maxValue) {
        const opts = {
            angle: 0, // Angle de la jauge (demi-cercle)
            lineWidth: 0.2, // Largeur de l'arc
            radiusScale: 1,
            pointer: {
                length: 0.6, // Longueur de l'aiguille
                strokeWidth: 0.035, // Largeur de l'aiguille
                color: '#000000', // Couleur de l'aiguille
            },
            staticZones: [
                { strokeStyle: "#30B32D", min: 0, max: maxValue / 3 }, // Vert
                { strokeStyle: "#FFDD00", min: maxValue / 3, max: (2 * maxValue) / 3 }, // Jaune
                { strokeStyle: "#F03E3E", min: (2 * maxValue) / 3, max: maxValue }, // Rouge
            ],
            staticLabels: {
                font: "10px sans-serif",
                labels: [0, maxValue / 3, (2 * maxValue) / 3, maxValue],
                color: "#000000",
                fractionDigits: 0,
            },
            renderTicks: {
                divisions: 5,
                divWidth: 1.1,
                divLength: 0.7,
                divColor: "#333333",
                subDivisions: 3,
                subLength: 0.5,
                subWidth: 0.6,
                subColor: "#666666",
            },
            limitMax: false,
            limitMin: false,
            highDpiSupport: true,
        };

        const container = document.getElementById("gauge-container");
        const gaugeBox = document.createElement("div");
        gaugeBox.classList.add("gauge-box");
        gaugeBox.innerHTML = `
            <canvas id="${id}" width="200" height="200"></canvas>
            <div class="gauge-label">${label}</div>
        `;
        container.appendChild(gaugeBox);

        const target = document.getElementById(id);
        const gauge = new Gauge(target).setOptions(opts);
        gauge.maxValue = maxValue;
        gauge.setMinValue(0);
        gauge.set(0);

        gauges[id] = gauge; // Stocke la jauge pour la mise à jour
    }

    // Fonction pour mettre à jour une jauge
    function updateGauge(id, value) {
        if (gauges[id]) {
            gauges[id].set(value); // Met à jour la jauge existante
        }
    }

    // Gère les jauges dynamiquement selon les données reçues
    function handleData(data) {
        data.in_servo.forEach((servo) => {
            const id = `gauge-${servo.name}`;
            if (!gauges[id]) {
                // Crée la jauge si elle n'existe pas encore
                createGauge(id, servo.name, 100);
            }

            // Met à jour la jauge avec la valeur reçue
            const value = servo.name.toLowerCase() === "brake"
                ? Math.abs(servo.value) // Convertir en positif pour Brake
                : Math.max(0, servo.value); // Throttle ou autres
            updateGauge(id, value);
        });
    }

    return {
        handleData,
    };
})();
