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

            // Vérifie si c'est une jauge à 1 sens ou 2 sens
            const isOneWay = ["throttle", "brake"].includes(servo.name.toLowerCase());
            const gaugeType = isOneWay ? "one-way-gauge" : "two-way-gauge";

            gaugeBox.innerHTML = `
                <div class="gauge ${gaugeType}" id="gauge-${servo.name}">
                    ${isOneWay 
                        ? '<div class="fill" id="fill-' + servo.name + '"></div>' 
                        : '<div class="fill fill-positive" id="fill-positive-' + servo.name + '"></div>' +
                          '<div class="fill fill-negative" id="fill-negative-' + servo.name + '"></div>' +
                          '<div class="zero-line"></div>'}
                </div>
                <div class="gauge-value" id="value-${servo.name}">0</div>
                <div>${servo.name}</div>
            `;

            gaugeContainer.appendChild(gaugeBox);
            return servo.name;
        });

        initialized = true;
    }

    function updateGauges(servoData) {
        servoData.forEach(servo => {
            const valueElement = document.getElementById(`value-${servo.name}`);
            const fillPositive = document.getElementById(`fill-positive-${servo.name}`);
            const fillNegative = document.getElementById(`fill-negative-${servo.name}`);
            const fillElement = document.getElementById(`fill-${servo.name}`);

            let displayValue = servo.value; // Valeur brute
            let height = 0;

            if (["throttle", "brake"].includes(servo.name.toLowerCase())) {
                // 🎯 Gestion des jauges 1 sens (Throttle & Brake)
                displayValue = servo.name.toLowerCase() === "throttle"
                    ? Math.max(0, displayValue) // Throttle → Ignore négatif
                    : Math.abs(Math.min(0, displayValue)); // Brake → Ignore positif et rend négatif en positif

                height = (displayValue / 100) * 200; // x2 pour occuper toute la jauge
                fillElement.style.height = `${height}px`;
                fillElement.style.bottom = "0"; // Toujours du bas
            } 
            else {
                // 🎯 Gestion des jauges 2 sens (Steer, etc.)
                height = (Math.abs(displayValue) / 100) * 100; // Échelle normale pour 2 sens

                if (displayValue >= 0) {
                    fillPositive.style.height = `${height}px`; // Monte vers le haut
                    fillPositive.style.bottom = "100px"; // Centre
                    fillNegative.style.height = "0"; // Vide la partie négative
                } else {
                    fillNegative.style.height = `${height}px`; // Descend vers le bas
                    fillNegative.style.bottom = "0"; // Base de la jauge
                    fillPositive.style.height = "0"; // Vide la partie positive
                }
            }

            // Mise à jour de l'affichage numérique
            if (valueElement) {
                valueElement.textContent = displayValue;
            }
        });
    }

    return {
        initializeGauges,
        updateGauges,
    };
})();
