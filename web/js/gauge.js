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

            // Vérifie si c'est une jauge spéciale (Throttle ou Brake)
            const isSpecialGauge = ["throttle", "brake"].includes(servo.name.toLowerCase());

            gaugeBox.innerHTML = `
                <div class="gauge ${isSpecialGauge ? 'special-gauge' : ''}" id="gauge-${servo.name}">
                    <div class="fill ${isSpecialGauge ? 'fill-special' : 'fill-normal'}" id="fill-${servo.name}"></div>
                    ${isSpecialGauge ? '' : '<div class="zero-line"></div>'} <!-- Garde le zéro sauf pour Throttle & Brake -->
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
            const gaugeElement = document.getElementById(`gauge-${servo.name}`);

            if (valueElement && fillElement && gaugeElement) {
                let displayValue = servo.value;
                let height = 0;

                if (servo.name.toLowerCase() === "throttle") {
                    displayValue = Math.max(0, servo.value); // Ignore les valeurs négatives
                    height = (displayValue / 100) * 200; // x2 pour occuper toute la jauge
                    fillElement.style.bottom = "0"; // Throttle commence du bas
                } 
                else if (servo.name.toLowerCase() === "brake") {
                    displayValue = Math.abs(Math.min(0, servo.value)); // Ignore les valeurs positives
                    height = (displayValue / 100) * 200; // x2 pour occuper toute la jauge
                    fillElement.style.bottom = "0"; // Brake commence du bas
                } 
                else {
                    height = (Math.abs(displayValue) / 100) * 100; // Jauges normales
                    if (displayValue >= 0) {
                        fillElement.style.bottom = "100px"; // Les valeurs positives montent
                    } else {
                        fillElement.style.bottom = `${100 - height}px`; // Les valeurs négatives descendent
                    }
                }

                valueElement.textContent = displayValue; // Affichage numérique
                fillElement.style.height = `${height}px`; 
            }
        });
    }

    return {
        initializeGauges,
        updateGauges,
    };
})();
