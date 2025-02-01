const GaugeManager = (function () {
    let initialized = false;
    let servos = [];

    function initializeGauges(servoData) {
        if (initialized) return;

        const gaugeContainer = document.getElementById("servo-gauges");
        gaugeContainer.innerHTML = ""; // Nettoyage du conteneur

        servos = servoData.map(servo => {
            const isSingle = (servo.name === "throttle" || servo.name === "brake");

            const gaugeBox = document.createElement("div");
            gaugeBox.classList.add("gauge-box");

            gaugeBox.innerHTML = `
                <div class="gauge ${isSingle ? 'single' : 'dual'}" id="gauge-${servo.name}">
                    <div class="${isSingle ? 'fill' : 'fill-positive'}" id="fill-positive-${servo.name}"></div>
                    ${isSingle ? '' : '<div class="fill-negative" id="fill-negative-' + servo.name + '"></div>'}
                    ${isSingle ? '' : '<div class="zero-line"></div>'}
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

            if (!valueElement || !fillPositive) return;

            let displayValue = servo.value;

            if (servo.name === "throttle") {
                // Si la valeur est négative, on affiche 0 mais on garde le remplissage correct
                displayValue = Math.max(0, servo.value);
                valueElement.textContent = displayValue;
                fillPositive.style.height = `${servo.value}%`; // Pas de changement ici, on garde le bon remplissage
            } 
            else if (servo.name === "brake") {
                // On inverse la valeur et on bloque à 0 si négatif
                displayValue = Math.max(0, -servo.value);
                valueElement.textContent = displayValue;
                fillPositive.style.height = `${displayValue}%`;
            } 
            else {
                // Jauge 2 sens (-100 à 100) (NE PAS TOUCHER)
                valueElement.textContent = servo.value;
                if (servo.value >= 0) {
                    fillPositive.style.height = `${servo.value}%`;
                    fillPositive.style.bottom = "100px";
                    fillNegative.style.height = "0";
                } else {
                    fillNegative.style.height = `${Math.abs(servo.value)}%`;
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
