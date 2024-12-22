const WebSocketManager = (function () {
    let ws = null; // WebSocket actuel
    let targetIP = ""; // IP de l'ESP détectée
    const connectionTimeout = 5000; // Timeout pour les connexions
    const maxScanAttempts = 5; // Nombre d'essais de ping

    // Met à jour l'interface utilisateur pour afficher le statut
    function updateStatus(message) {
        const status = document.getElementById("status");
        const loader = document.getElementById("loader");
        status.textContent = message;
        loader.style.display = message.includes("Tentative") ? "grid" : "none";
    }

    // Tente de pinger un serveur à une IP donnée
    async function pingServer(ip) {
        const url = `http://${ip}/`;
        try {
            const controller = new AbortController();
            const timeoutId = setTimeout(() => controller.abort(), 250);

            await fetch(url, { method: 'HEAD', signal: controller.signal, mode: 'no-cors' });
            clearTimeout(timeoutId);
            console.log(`Ping réussi pour ${ip}`);
            return true;
        } catch (error) {
            if (error.name === "AbortError") {
                console.log(`Timeout pour le ping à ${ip}`);
            } else {
                console.warn(`Erreur lors du ping à ${ip}`);
            }
            return false;
        }
    }

    // Tente de se connecter à un WebSocket
    async function tryConnectWebSocket(ip) {
        const wsURL = `ws://${ip}/ws`;
        return new Promise((resolve, reject) => {
            console.log(`Tentative de connexion WebSocket : ${wsURL}`);
            const testWs = new WebSocket(wsURL);
            let timeout;

            timeout = setTimeout(() => {
                console.warn(`Timeout pour la connexion à ${ip}`);
                testWs.close();
                reject(new Error(`Timeout pour ${ip}`));
            }, connectionTimeout);

            testWs.onopen = () => {
                clearTimeout(timeout);
                console.log(`WebSocket connecté à ${ip}`);
                resolve(testWs);
            };

            testWs.onerror = () => {
                clearTimeout(timeout);
                console.warn(`Erreur lors de la connexion à ${ip}`);
                reject(new Error(`Erreur WebSocket pour ${ip}`));
            };

            testWs.onclose = () => {
                clearTimeout(timeout);
                console.warn(`WebSocket fermé pour ${ip}`);
                reject(new Error(`Connexion fermée pour ${ip}`));
            };
        });
    }

    // Scanne les IP pour trouver l'ESP
    async function scanAndConnect(baseIP = "192.168") {
        for (let x = 2; x <= 255; x++) {
            const ip = `${baseIP}.${x}.100`;
            updateStatus(`Tentative de connexion à ${ip}`);

            try {
                // Vérifier si l'IP répond au ping
                const isReachable = await pingServer(ip);
                if (!isReachable) continue;

                // Tenter de se connecter au WebSocket
                ws = await tryConnectWebSocket(ip);
                targetIP = ip;

                console.log(`Connexion WebSocket réussie à ${ip}`);
                updateStatus(`Connecté à ${ip}`);
                initializeWebSocketHandlers();
                return;
            } catch (error) {
                console.log(`Erreur pour ${ip}: ${error.message}`);
            }
        }

        console.error("Aucune passerelle trouvée.");
        updateStatus("Aucune passerelle trouvée.");
    }

    // Initialise les gestionnaires d'événements WebSocket
    function initializeWebSocketHandlers() {
        if (!ws) return;

        ws.onmessage = (event) => {
            try {
                const data = JSON.parse(event.data);
                console.log("Message reçu :", data);

                if (data.type === "pong") {
                    console.log("Pong reçu !");
                } else if (data.type === "simulation") {
                    // Exemple : gérer les données de simulation
                    setX(data.roll);
                    setY(data.pitch);
                } else if (data.type === "info") {
                    InfoManager.displayInfo(data);
                } 
                else if (data.type === "gyro") {
                        GyroManager.setX(data.roll);
                        GyroManager.setY(data.pitch);
                        Offset_X=data.Offset_X;
                        Offset_Y=data.Offset_Y;
                        limit_g_x = data.Limit_g_x;
                        limit_g_y = data.Limit_g_y;
                        //setX(data.roll);
                        //setY(data.pitch);
                    }
                else if (data.type === "channels") {
                        updateGauges(data);
                    }
                else {
                    console.warn("Message inconnu ou inattendu :", data);
                }
            } catch (error) {
                console.error("Erreur lors du parsing du message WebSocket :", error);
            }
        };

        ws.onclose = () => {
            updateStatus("WebSocket fermé. Tentative de reconnexion...");
            console.error("WebSocket fermé. Tentative de reconnexion...");
            reconnectWebSocket();
        };

        ws.onerror = (error) => {
            console.error("Erreur WebSocket :", error);
            ws.close();
        };
    }

    // Fonction pour reconnecter ou relancer un scan
    function reconnectWebSocket() {
        console.log("Tentative de reconnexion...");
        scanAndConnect();
    }

    function sendTabRequest(tabIndex) {
        const tabRequest = { type: "mod", mod_req: tabIndex };
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(tabRequest));
            console.log("Tab request sent:", tabRequest);
        } else {
            console.warn("WebSocket not connected");
        }
    }

    // Expose les méthodes publiques
    return {
        scanAndConnect,
        reconnectWebSocket,
        sendTabRequest,
    };
})();

// Exportation en tant que module ES6 (si nécessaire)
//export default WebSocketManager;
