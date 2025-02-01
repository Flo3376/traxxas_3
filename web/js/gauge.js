const GaugeManager = (function () {
    let initialized = false;
    let servos = [];

    function initializeGauges(servoData) {
        if (initialized) return; // On ne crée les jauges qu'une seule fois

        const gaugeContainer = document.getElementById("servo-gauges");
        gaugeContainer.innerHTML = ""; // Nettoyage du conteneur

        servos = servoData.map((servo) => {
            const gaugeBox = document.createElement("div");
            gaugeBox.classList.add("gauge-box");

            gaugeBox.innerHTML = `
                <div class="gauge" id="gauge-${servo.name}">
                    <div class="fill" id="fill-${servo.name}"></div>
                </div>
                <div class="gauge-value" id="value-${servo.name}">0</div>
                <div>${servo.name}</div>
            `;

            gaugeContainer.appendChild(gaugeBox);
            return servo.name; // Stocke les noms pour les mises à jour futures
        });

        initialized = true;
    }

    function updateGauges(servoData) {
        servoData.forEach(servo => {
            const valueElement = document.getElementById(`value-${servo.name}`);
            const fillElement = document.getElementById(`fill-${servo.name}`);

            if (valueElement && fillElement) {
                let displayValue = servo.value; // Valeur brute

                // Gestion spéciale pour "throttle" et "brake"
                if (servo.name.toLowerCase() === "throttle") {
                    displayValue = Math.max(0, servo.value); // Garde uniquement la partie positive
                } else if (servo.name.toLowerCase() === "brake") {
                    displayValue = Math.abs(Math.min(0, servo.value)); // Garde uniquement la partie négative, mais l'affiche en positif
                }

                valueElement.textContent = displayValue; // Affichage numérique

                // Hauteur proportionnelle (100% = 100px)
                const height = (displayValue / 100) * 100;
                fillElement.style.height = `${height}px`; 
            }
        });
    }

    return {
        initializeGauges,
        updateGauges,
    };
})();
