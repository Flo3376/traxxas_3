const AmpoulesManager = (function () {
    let initialized = false;
    const buttonStates = {}; // Stocke l'état des boutons (indépendant du JSON)

    function initializeAmpoules(ampoules) {
        if (initialized) return;

        const container = document.getElementById("ampoules-list");
        container.innerHTML = ""; // Nettoyage

        ampoules.forEach(ampoule => {
            const ampouleBox = document.createElement("div");
            ampouleBox.classList.add("ampoule-box");

            // Déterminer la couleur de l'ampoule
            const iconClass = getAmpouleClass(ampoule);

            // Initialiser l'état du bouton (par défaut : auto)
            if (!buttonStates[ampoule.pin]) {
                buttonStates[ampoule.pin] = "auto";
            }
            const buttonState = buttonStates[ampoule.pin];
            const buttonClass = getButtonClass(buttonState);

            ampouleBox.innerHTML = `
                <div class="ampoule-name">${ampoule.name}</div>
                <i class="fas fa-lightbulb ${iconClass}" id="icon-${ampoule.pin}"></i>
                <div class="ampoule-info">
                    <span class="titre">Pin:</span> ${ampoule.pin} | 
                    <span class="titre">Mode:</span> ${ampoule.mode} | 
                    <span class="titre">État:</span> ${ampoule.etat_bas} → ${ampoule.etat_haut}
                </div>
                <button class="ampoule-btn ${buttonClass}" data-state="${buttonState}" id="btn-${ampoule.pin}">${capitalize(buttonState)}</button>
            `;

            // Ajoute l'événement pour le bouton ON/OFF/AUTO
            ampouleBox.querySelector(".ampoule-btn").addEventListener("click", (event) => {
                toggleAmpouleState(event.target, ampoule.pin);
            });

            container.appendChild(ampouleBox);
        });

        initialized = true;
    }

    function updateAmpoules(ampoules) {
        ampoules.forEach(ampoule => {
            const icon = document.getElementById(`icon-${ampoule.pin}`);
            if (icon) {
                // Met à jour la classe de l'icône en fonction de l'état
                const iconClass = getAmpouleClass(ampoule);
                icon.className = `fas fa-lightbulb ${iconClass}`;
            }
        });
    }

    function toggleAmpouleState(button, pin) {
        const currentState = buttonStates[pin];
        let newState;

        // Alterne entre les trois états
        if (currentState === "auto") {
            newState = "on";
        } else if (currentState === "on") {
            newState = "off";
        } else {
            newState = "auto";
        }

        // Met à jour l'état du bouton
        buttonStates[pin] = newState;
        button.textContent = capitalize(newState);
        button.className = `ampoule-btn ${getButtonClass(newState)}`;
    }

    function getAmpouleClass(ampoule) {
    if (ampoule.actif || ampoule.etat_bas > 0) {
        // Si l'ampoule est active ou a un état bas > 0
        if (ampoule.etat_bas > 0) {
            return "ampoule-medium"; // Orange pour état intermédiaire
        } else {
            return "ampoule-on"; // Jaune si pleine puissance
        }
    }
    // Si l'ampoule est inactive et a un état bas de 0
    return "ampoule-off"; // Grise si éteinte
}

    function getButtonClass(state) {
        if (state === "on") return "on";
        if (state === "off") return "off";
        return "auto"; // Par défaut : auto
    }

    function capitalize(str) {
        return str.charAt(0).toUpperCase() + str.slice(1);
    }

    return {
        initializeAmpoules,
        updateAmpoules
    };
})();
