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

            const isOneWay = ["throttle", "brake"].includes(servo.name.toLowerCase());
            const gaugeType = isOneWay ? "one-way-gauge" : "two-way-gauge";

            gaugeBox.innerHTML = `
                <div class="gauge ${gaugeType}" id="gauge-${servo.name}">
                    ${isOneWay ? '<div class="fill" id="fill-' + servo.name + '"></div>' :
                        '<div class="fill fill-positive" id="fill-positive-' + servo.name + '"></div>' +
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

            let displayValue = servo.value;
            let height = 0;

            if (["throttle", "brake"].includes(servo.name.toLowerCase())) {
                // 🎯 Gestion des jauges 1 sens (Throttle & Brake)
                displayValue = servo.name.toLowerCase() === "throttle"
                    ? Math.max(0, displayValue)
                    : Math.abs(Math.min(0, displayValue));

                height = (displayValue / 100) * 200; // x2 pour occuper toute la jauge
                fillElement.style.height = `${height}px`;
            } 
            else {
                // 🎯 Gestion des jauges 2 sens (Steer, etc.)
                height = (Math.abs(displayValue) / 100) * 100;

                if (displayValue >= 0) {
                    fillPositive.style.height = `${height}px`;
                    fillNegative.style.height = "0";
                } else {
                    fillNegative.style.height = `${height}px`;
                    fillPositive.style.height = "0";
                }
            }

            valueElement.textContent = displayValue;
        });
    }

    return {
        initializeGauges,
        updateGauges,
    };
})();
