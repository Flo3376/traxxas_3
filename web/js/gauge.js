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

            // Vérifier si c'est une jauge spéciale (throttle ou brake)
            const isSpecialGauge = ["throttle", "brake"].includes(servo.name.toLowerCase());

            gaugeBox.innerHTML = `
                <div class="gauge ${isSpecialGauge ? 'special-gauge' : ''}" id="gauge-${servo.name}">
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
            const gaugeElement = document.getElementById(`gauge-${servo.name}`);

            if (valueElement && fillElement && gaugeElement) {
                let displayValue = servo.value; // Valeur brute
                let height = 0; // Hauteur de remplissage

                if (servo.name.toLowerCase() === "throttle") {
                    displayValue = Math.max(0, servo.value); // Ignore négatif
                    height = (displayValue / 100) * 200; // x2 pour occuper toute la jauge
                } 
                else if (servo.name.toLowerCase() === "brake") {
                    displayValue = Math.abs(Math.min(0, servo.value)); // Ignore positif, affiche positif
                    height = (displayValue / 100) * 200; // x2 pour occuper toute la jauge
                } 
                else {
                    height = (Math.abs(displayValue) / 100) * 100; // Normal pour les autres
                }

                valueElement.textContent = displayValue; // Affichage numérique
                fillElement.style.height = `${height}px`; 

                // Supprimer la ligne blanche du zéro pour Throttle & Brake
                if (["throttle", "brake"].includes(servo.name.toLowerCase())) {
                    gaugeElement.classList.add("no-zero-line");
                }
            }
        });
    }

    return {
        initializeGauges,
        updateGauges,
    };
})();
