const GaugeManager = (function () {
    let initialized = false;
    let servos = [];

    function initializeGauges(servoData) {
        if (initialized) return; // On ne crée les jauges qu'une seule fois

        const gaugeContainer = document.getElementById("servo-gauges");
        gaugeContainer.innerHTML = ""; // Nettoyage du conteneur

        servos = servoData.map((servo, index) => {
            const gaugeBox = document.createElement("div");
            gaugeBox.classList.add("gauge-box");

            gaugeBox.innerHTML = `
                <div class="gauge" id="gauge-${servo.name}">
                    <div class="fill-positive" id="fill-positive-${servo.name}"></div>
                    <div class="fill-negative" id="fill-negative-${servo.name}"></div>
                    <div class="zero-line"></div>
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
                valueElement.textContent = servo.value;
                if (servo.value >= 0) {
                    fillPositive.style.height = `${servo.value}px`;
                    fillPositive.style.bottom = "100px";
                    fillNegative.style.height = "0";
                } else {
                    fillNegative.style.height = `${Math.abs(servo.value)}px`;
                    fillNegative.style.top = "100px";
                    fillPositive.style.height = "0";
                }
            }
        });
    }

    return {
        initializeGauges,
        updateGauges,
    };
})();
