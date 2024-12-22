let billeLayer = null; // Accessible globalement

const GyroManager = (function () {
    let svg = null;
    
    let rotation = 0; // Angle de rotation actuel
    let yDegrees = 0; // Position Y actuel
    let Offset_X = 0;
    let Offset_Y = 0;
    let limit_g_x = 0;
    let limit_g_y = 0;

    let targetRotation = 0; // Angle cible
    let targetYDegrees = 0; // Position Y cible

    const billeCenterX = 140.5; // Coordonnée X du centre de rotation
    const billeCenterY = 140.5; // Coordonnée Y du centre de rotation
    const maxY = 125; // Déplacement maximum en pixels
    const maxAngle = 90; // Angle maximum en degrés
    const yRatio = maxY / maxAngle; // Ratio d'étalonnage

    let progress = 0; // Progression pour l'interpolation

    let blinkIntervalTopBottom = null;
    let blinkIntervalLeftRight = null;

    const svgContainer = document.getElementById("svg-container");
    const alertSound = new Audio("https://lasonotheque.org/UPLOAD/mp3/1920.mp3");

    // Initialisation
    function initialize(svgElementId, billeLayerId) {
        svg = document.querySelector(svgElementId);
        billeLayer = svg?.getElementById(billeLayerId);

        // Écouteur pour réinitialiser le gyroscope
        document.getElementById("reset-gyro").addEventListener("click", async function () {
            const tabRequest = { type: "command", command: "reset_gyro" };
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify(tabRequest));
            }
        });

        // Démarre les mises à jour périodiques des alertes
        setInterval(updateBlinks, 250);
    }

    // Fonction pour convertir les degrés en pixels
    function degreesToPixels(degrees) {
        return degrees * yRatio;
    }

    // Fonction easing pour un effet fluide
    function easing(t) {
        return t * t * (3 - 2 * t); // Ease-in-out
    }

    // Animation fluide avec easing
    function smoothUpdate() {
        if (progress < 1) {
            progress += 0.02; // Ajuste la vitesse

            // Interpolation avec easing
            rotation += (targetRotation - rotation) * easing(progress);
            yDegrees += (targetYDegrees - yDegrees) * easing(progress);

            updateBilleTransform();
            requestAnimationFrame(smoothUpdate); // Continue l'animation
        } else {
            rotation = targetRotation;
            yDegrees = targetYDegrees;
            updateBilleTransform();
        }
    }

    // Fonction pour appliquer les transformations
    function updateBilleTransform() {
        const radianRotation = (rotation * Math.PI) / 180;

        const height = degreesToPixels(yDegrees);
        const dynamicDrift = -2.85 * yDegrees * Math.sin(radianRotation);

        const translateX = height * Math.sin(radianRotation) + dynamicDrift;
        const translateY = height * Math.cos(radianRotation);

        const transformString = `
            translate(${translateX}, ${translateY})
            rotate(${rotation}, ${billeCenterX}, ${billeCenterY})`;
        const billeLayer = GyroManager.getBilleLayer();
        if (billeLayer) {
            billeLayer.setAttribute("transform", transformString);
        } else {
            console.warn("billeLayer est null, vérifiez l'ID ou le chargement du SVG.");
        }

        updateValuesDisplay();
    }

    // Met à jour les valeurs affichées
    function updateValuesDisplay() {
        document.getElementById("roll-value").textContent = rotation.toFixed(1);
        document.getElementById("pitch-value").textContent = yDegrees.toFixed(1);

        document.getElementById("correction-roll").textContent = Offset_X.toFixed(1);
        document.getElementById("correction-pitch").textContent = Offset_Y.toFixed(1);

        document.getElementById("limit-roll").textContent = limit_g_x.toFixed(1);
        document.getElementById("limit-pitch").textContent = limit_g_y.toFixed(1);
    }

    // Commandes pour manipuler manuellement les cibles
    function setX(newRotation) {
        targetRotation = newRotation;
        progress = 0; // Réinitialise la progression
        requestAnimationFrame(smoothUpdate);
    }

    function setY(degrees) {
        targetYDegrees = degrees;
        progress = 0; // Réinitialise la progression
        requestAnimationFrame(smoothUpdate);
    }

    // Met à jour les alertes de clignotement
    function updateBlinks() {
        if (Math.abs(yDegrees) > limit_g_x) {
            if (!blinkIntervalTopBottom) {
                blinkIntervalTopBottom = setInterval(() => {
                    svgContainer.classList.toggle("blink-top-bottom");
                }, 500);
            }
        } else {
            clearInterval(blinkIntervalTopBottom);
            blinkIntervalTopBottom = null;
            svgContainer.classList.remove("blink-top-bottom");
        }

        if (Math.abs(rotation) > limit_g_y) {
            if (!blinkIntervalLeftRight) {
                blinkIntervalLeftRight = setInterval(() => {
                    svgContainer.classList.toggle("blink-left-right");
                }, 500);
            }
        } else {
            clearInterval(blinkIntervalLeftRight);
            blinkIntervalLeftRight = null;
            svgContainer.classList.remove("blink-left-right");
        }

        if (Math.abs(rotation) > limit_g_y || Math.abs(yDegrees) > limit_g_x) {
            alertSound.play();
        } else {
            alertSound.pause();
        }
    }

    // Expose les fonctions publiques
    return {
        initialize,
        getBilleLayer: () => billeLayer, // Méthode pour accéder à billeLayer
        setX,
        setY,
    };
})();
