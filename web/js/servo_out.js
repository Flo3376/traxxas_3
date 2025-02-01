const ServoOutManager = (function () {
    let initialized = false;

    function initializeServoOut(servos) {
        if (initialized) return;
        
        const container = document.getElementById("servo-out-controls");
        container.innerHTML = ""; // Nettoyage

        Object.keys(servos).forEach(servo => {
            const controlBox = document.createElement("div");
            controlBox.classList.add("servo-box");

            controlBox.innerHTML = `
                <div class="servo-label">${servo.toUpperCase()}</div>
                <input type="range" min="-100" max="100" value="${servos[servo]}" id="slider-${servo}" class="servo-slider">
                <div class="servo-value" id="value-${servo}">${servos[servo]}</div>
            `;

            container.appendChild(controlBox);

            // Écouteur pour mise à jour en temps réel
            document.getElementById(`slider-${servo}`).addEventListener("input", (event) => {
                const newValue = event.target.value;
                document.getElementById(`value-${servo}`).textContent = newValue;
                sendServoOutUpdate(servo, newValue);
            });
        });

        initialized = true;
    }

    function sendServoOutUpdate(servo, value) {
        if (ws && ws.readyState === WebSocket.OPEN) {
            const message = {
                type: "out_servo",
                servo: servo,
                value: parseInt(value)
            };
            ws.send(JSON.stringify(message));
            console.log("Commande Servo Out envoyée :", message);
        }
    }

    function updateServoOutValues(servos) {
        Object.keys(servos).forEach(servo => {
            const slider = document.getElementById(`slider-${servo}`);
            const valueDisplay = document.getElementById(`value-${servo}`);
            if (slider && valueDisplay) {
                slider.value = servos[servo];
                valueDisplay.textContent = servos[servo];
            }
        });
    }

    return {
        initializeServoOut,
        updateServoOutValues
    };
})();
