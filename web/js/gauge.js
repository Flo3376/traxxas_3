// Gestion des jauges dans gauge.js

// Initialisation des éléments HTML des jauges
const gaugesPositive = Array.from({ length: 6 }, (_, i) =>
    document.getElementById(`fill-positive${i + 1}`)
);

const gaugesNegative = Array.from({ length: 6 }, (_, i) =>
    document.getElementById(`fill-negative${i + 1}`)
);

const gaugeValues = Array.from({ length: 6 }, (_, i) =>
    document.getElementById(`value${i + 1}`)
);

// Fonction pour mettre à jour les jauges avec de nouvelles données
function updateGauges(data) {
    for (let i = 1; i <= 6; i++) {
        const channelValue = data[`Ch${i}`];
        if (channelValue !== undefined) {
            gaugeValues[i - 1].textContent = channelValue;
            if (channelValue >= 0) {
                gaugesPositive[i - 1].style.height = `${channelValue}px`;
                gaugesPositive[i - 1].style.bottom = "100px"; // Position à partir de 0
                gaugesNegative[i - 1].style.height = "0";
            } else {
                gaugesNegative[i - 1].style.height = `${Math.abs(channelValue)}px`;
                gaugesNegative[i - 1].style.top = "100px"; // Position à partir de 0
                gaugesPositive[i - 1].style.height = "0";
            }
        }
    }
}

// Export de la fonction si vous utilisez un module ES6
//export { updateGauges };
