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
                <button class="ampoule-btn" disabled>ON / OFF</button>
            `;

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
                <button class="ampoule-btn" disabled>ON / OFF</button>
            `;

            container.appendChild(ampouleBox);
        });
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
