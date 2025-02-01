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
                    <div class="fill fill-positive" id="fill-positive-${servo.name}"></div>
                    <div class="fill fill-negative" id="fill-negative-${servo.name}"></div>
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
            const fillPositive = document.getElementById(`fill-positive-${servo.name}`);
            const fillNegative = document.getElementById(`fill-negative-${servo.name}`);

            if (valueElement && fillPositive && fillNegative) {
                let displayValue = servo.value;
                let height = 0;

                if (servo.name.toLowerCase() === "throttle") {
                    displayValue = Math.max(0, servo.value);
                    height = (displayValue / 100) * 200;
                    fillPositive.style.height = `${height}px`;
                    fillPositive.style.bottom = "0"; // Commence du bas
                    fillNegative.style.height = "0";
                } 
                else if (servo.name.toLowerCase() === "brake") {
                    displayValue = Math.abs(Math.min(0, servo.value));
                    height = (displayValue / 100) * 200;
                    fillNegative.style.height = `${height}px`;
                    fillNegative.style.bottom = "0"; // Commence du bas
                    fillPositive.style.height = "0";
                } 
                else {
                    height = (Math.abs(displayValue) / 100) * 100;
                    if (displayValue >= 0) {
                        fillPositive.style.height = `${height}px`;
                        fillPositive.style.bottom = "100px"; // Monte normalement
                        fillNegative.style.height = "0";
                    } else {
                        fillNegative.style.height = `${height}px`;
                        fillNegative.style.bottom = "0"; // Descend correctement
                        fillPositive.style.height = "0";
                    }
                }

                valueElement.textContent = displayValue; // Mise à jour de l'affichage
            }
        });
    }

    return {
        initializeGauges,
        updateGauges,
    };
})();
