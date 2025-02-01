const AmpoulesManager = (function () {
    let initialized = false;

    function initializeAmpoules(ampoules) {
        if (initialized) return;

        const container = document.getElementById("ampoules-list");
        container.innerHTML = ""; // Nettoyage

        ampoules.forEach(ampoule => {
            const ampouleBox = document.createElement("div");
            ampouleBox.classList.add("ampoule-box");

            // Déterminer la couleur de l'ampoule
            const iconClass = getAmpouleClass(ampoule);

            ampouleBox.innerHTML = `
                <div class="ampoule-name">${ampoule.name}</div>
                <i class="fas fa-lightbulb ${iconClass}"></i>
                <div class="ampoule-info">
                    <span class="titre">Pin:</span> ${ampoule.pin} | 
                    <span class="titre">Mode:</span> ${ampoule.mode} | 
                    <span class="titre">État:</span> ${ampoule.etat_bas} → ${ampoule.etat_haut}
                </div>
                <button class="ampoule-btn auto" data-state="auto" id="btn-${ampoule.pin}">Auto</button>
            `;

            // Ajoute l'événement pour le bouton ON/OFF/AUTO
            ampouleBox.querySelector(".ampoule-btn").addEventListener("click", (event) => {
                toggleAmpouleState(event.target);
            });

            container.appendChild(ampouleBox);
        });

        initialized = true;
    }

    function updateAmpoules(ampoules) {
        const container = document.getElementById("ampoules-list");
        container.innerHTML = ""; // Rafraîchissement

        ampoules.forEach(ampoule => {
            const ampouleBox = document.createElement("div");
            ampouleBox.classList.add("ampoule-box");

            const iconClass = getAmpouleClass(ampoule);

            ampouleBox.innerHTML = `
                <div class="ampoule-name">${ampoule.name}</div>
                <i class="fas fa-lightbulb ${iconClass}"></i>
                <div class="ampoule-info">
                    <span class="titre">Pin:</span> ${ampoule.pin} | 
                    <span class="titre">Mode:</span> ${ampoule.mode} | 
                    <span class="titre">État:</span> ${ampoule.etat_bas} → ${ampoule.etat_haut}
                </div>
                <button class="ampoule-btn auto" data-state="auto" id="btn-${ampoule.pin}">Auto</button>
            `;

            ampouleBox.querySelector(".ampoule-btn").addEventListener("click", (event) => {
                toggleAmpouleState(event.target);
            });

            container.appendChild(ampouleBox);
        });
    }

    function toggleAmpouleState(button) {
        const currentState = button.getAttribute("data-state");
        let newState;

        // Alterne entre les trois états
        if (currentState === "auto") {
            newState = "on";
            button.classList.remove("auto");
            button.classList.add("on");
            button.textContent = "On";
        } else if (currentState === "on") {
            newState = "off";
            button.classList.remove("on");
            button.classList.add("off");
            button.textContent = "Off";
        } else {
            newState = "auto";
            button.classList.remove("off");
            button.classList.add("auto");
            button.textContent = "Auto";
        }

        button.setAttribute("data-state", newState);
    }

    function getAmpouleClass(ampoule) {
        if (ampoule.actif) {
            if (ampoule.etat_bas > 0) {
                return "ampoule-medium"; // Orange pour état intermédiaire
            } else {
                return "ampoule-on"; // Jaune si pleine puissance
            }
        }
        return "ampoule-off"; // Grise si éteinte
    }

    return {
        initializeAmpoules,
        updateAmpoules
    };
})();
