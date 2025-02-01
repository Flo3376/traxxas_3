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

            valueElement.textContent = servo.value;

            if (servo.name === "throttle" || servo.name === "brake") {
                // Jauge 1 sens (0 à 100)
                fillPositive.style.height = `${servo.value}%`;
            } else {
                // Jauge 2 sens (-100 à 100)
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
