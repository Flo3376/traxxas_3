const AmpoulesManager = (function () {
    let initialized = false;

    function initializeAmpoules(ampoules) {
        if (initialized) return;
        
        const container = document.getElementById("ampoules-list");
        container.innerHTML = ""; // Nettoyage

        ampoules.forEach(ampoule => {
            const ampouleBox = document.createElement("div");
            ampouleBox.classList.add("ampoule-box");

            // Vérification si actif ou inactif
            const isActive = ampoule.actif ? "Actif" : "Inactif";
            const activeClass = ampoule.actif ? "active" : "inactive";

            ampouleBox.innerHTML = `
                <div class="ampoule-name">${ampoule.name}</div>
                <div class="ampoule-status ${activeClass}">${isActive}</div>
                <div class="ampoule-info">
                    <span>Pin: ${ampoule.pin}</span> | 
                    <span>Mode: ${ampoule.mode}</span> | 
                    <span>État: ${ampoule.etat_bas} → ${ampoule.etat_haut}</span>
                </div>
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

            const isActive = ampoule.actif ? "Actif" : "Inactif";
            const activeClass = ampoule.actif ? "active" : "inactive";

            ampouleBox.innerHTML = `
                <div class="ampoule-name">${ampoule.name}</div>
                <div class="ampoule-status ${activeClass}">${isActive}</div>
                <div class="ampoule-info">
                    <span>Pin: ${ampoule.pin}</span> | 
                    <span>Mode: ${ampoule.mode}</span> | 
                    <span>État: ${ampoule.etat_bas} → ${ampoule.etat_haut}</span>
                </div>
            `;

            container.appendChild(ampouleBox);
        });
    }

    return {
        initializeAmpoules,
        updateAmpoules
    };
})();
