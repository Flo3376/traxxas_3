const ServoOutManager = (function () {
    let initialized = false;
    let ws = null; // WebSocket local

    function initializeServoOut(servos, webSocket) {
        if (initialized) return;
        ws = webSocket; // Stocker l'instance WebSocket

        const container = document.getElementById("servo-out-controls");
        container.innerHTML = ""; // Nettoyage

        servos.forEach(servo => {
            const controlBox = document.createElement("div");
            controlBox.classList.add("servo-box");

            controlBox.innerHTML = `
                <div class="servo-label">Servo Pin ${servo.pin}</div>
                <input type="range" min="-100" max="100" value="${servo.position}" id="slider-${servo.pin}" class="servo-slider">
                <div class="servo-value" id="value-${servo.pin}">${servo.position}</div>
            `;

            container.appendChild(controlBox);

            // Écouteur pour mise à jour en temps réel
            document.getElementById(`slider-${servo.pin}`).addEventListener("input", (event) => {
                const newValue = event.target.value;
                document.getElementById(`value-${servo.pin}`).textContent = newValue;
                sendServoOutUpdate(servo.pin, newValue);
            });
        });

        initialized = true;
    }

    function sendServoOutUpdate(pin, position) {
        if (ws && ws.readyState === WebSocket.OPEN) {
            const message = {
                type: "servo",
                out_servo: [
                    {
                        pin: pin,
                        position: parseInt(position)
                    }
                ]
            };
            ws.send(JSON.stringify(message));
            console.log("Commande Servo Out envoyée :", message);
        }
    }

    function updateServoOutValues(servos) {
        servos.forEach(servo => {
            const slider = document.getElementById(`slider-${servo.pin}`);
            const valueDisplay = document.getElementById(`value-${servo.pin}`);
            if (slider && valueDisplay) {
                slider.value = servo.position;
                valueDisplay.textContent = servo.position;
            }
        });
    }

    return {
        initializeServoOut,
        updateServoOutValues
    };
})();
