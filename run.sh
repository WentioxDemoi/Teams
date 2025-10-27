#!/bin/bash

# === CONFIGURATION ===
SERVER_DIR="TeamsServer_"
CLIENT_DIR="TeamsClient_"
CLIENT_BIN="./build/TeamsClient_.app/Contents/MacOS/TeamsClient_"
CLIENT_TEST_BIN="./build/TeamsClient_Tests"   # nom à adapter si différent
DOCKER_IMAGE="teams_server"
DOCKER_CONTAINER="teams_server_container"

# === COULEURS ===
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
RESET="\033[0m"

# === FONCTIONS ===

build_client() {
    echo -e "${YELLOW}🔧 Compilation du client...${RESET}"
    cd "$CLIENT_DIR" || exit 1
    cmake -S . -B build > /dev/null 2>&1
    cmake --build build > /dev/null 2>&1
    cd ..
    echo -e "${GREEN}✅ Client compilé avec succès.${RESET}"
}

build_client_tests() {
    echo -e "${YELLOW}🔧 Compilation des tests du client...${RESET}"
    cd "$CLIENT_DIR" || exit 1
    cmake -S . -B build > /dev/null 2>&1
    cmake --build build --target TeamsClient_Tests > /dev/null 2>&1 || {
        echo -e "${RED}❌ Échec de la compilation des tests.${RESET}"
        cd ..
        exit 1
    }
    cd ..
    echo -e "${GREEN}✅ Tests du client compilés avec succès.${RESET}"
}

run_server() {
    echo -e "${GREEN}🚀 Lancement du serveur via Docker...${RESET}"
    if [[ "$(docker images -q $DOCKER_IMAGE 2> /dev/null)" == "" ]]; then
        echo -e "${YELLOW}🔧 Construction de l'image Docker du serveur...${RESET}"
        docker build -t $DOCKER_IMAGE "$SERVER_DIR"
    fi
    docker run --name $DOCKER_CONTAINER -d -p 8080:8080 $DOCKER_IMAGE
    SERVER_PID=$(docker ps -qf "name=$DOCKER_CONTAINER")
    echo -e "${GREEN}✅ Serveur Docker lancé avec succès. Container ID: $SERVER_PID${RESET}"
}

run_client() {
    echo -e "${GREEN}🚀 Lancement du client...${RESET}"
    cd "$CLIENT_DIR" || exit 1
    $CLIENT_BIN &
    CLIENT_PID=$!
    cd - > /dev/null
}

run_client_tests() {
    echo -e "${GREEN}🧪 Exécution des tests du client...${RESET}"
    cd "$CLIENT_DIR/build" || { echo -e "${RED}❌ Dossier build introuvable.${RESET}"; exit 1; }

    if [ -f "CTestTestfile.cmake" ]; then
        ctest --output-on-failure
    elif [ -f "$CLIENT_TEST_BIN" ]; then
        $CLIENT_TEST_BIN
    else
        echo -e "${RED}❌ Aucun test détecté (ni CTest, ni binaire de test trouvé).${RESET}"
    fi
    cd - > /dev/null
}

run_server_tests() {
    echo -e "${YELLOW}🧪 Tests du serveur non encore implémentés.${RESET}"
    # TODO: Implémenter les tests du serveur
}

create_dmg() {
    echo -e "${YELLOW}📦 Création du fichier .dmg...${RESET}"
    cd "$CLIENT_DIR/build" || { echo -e "${RED}❌ Dossier build introuvable.${RESET}"; exit 1; }
    if [ ! -d "TeamsClient_.app" ]; then
        echo -e "${RED}❌ Application TeamsClient_.app introuvable.${RESET}"
        cd - > /dev/null
        return
    fi
    ~/Qt/6.9.3/macos/bin/macdeployqt TeamsClient_.app/
    hdiutil create -volname "TeamsClient_" -srcfolder TeamsClient_.app -ov -format UDZO TeamsClient_.dmg
    echo -e "${GREEN}✅ .dmg créé avec succès.${RESET}"
    cd - > /dev/null
}

stop_all() {
    echo -e "${YELLOW}🛑 Arrêt des processus...${RESET}"
    kill $CLIENT_PID 2>/dev/null
    if [[ -n $SERVER_PID ]]; then
        docker stop $DOCKER_CONTAINER > /dev/null
        docker rm $DOCKER_CONTAINER > /dev/null
        echo -e "${GREEN}✅ Serveur Docker arrêté.${RESET}"
    fi
    echo -e "${GREEN}✅ Tout est arrêté.${RESET}"
}

# === MENU ===

echo -e "${YELLOW}=========================="
echo -e "  Gestion du projet Teams"
echo -e "==========================${RESET}"
echo ""
echo "1) Lancer le serveur (Docker)"
echo "2) Lancer le client"
echo "3) Lancer serveur + client"
echo "4) Quitter"
echo "5) Lancer les tests du client"
echo "6) Lancer les tests du serveur"
echo "7) Créer un .dmg du client"
echo ""
read -rp "👉 Choisis une option [1-7] : " choice

case $choice in
    1)
        run_server
        echo -e "${YELLOW}🟢 Serveur Docker en cours. Ctrl+C pour arrêter.${RESET}"
        trap stop_all SIGINT
        wait
        ;;
    2)
        build_client
        run_client
        wait $CLIENT_PID
        ;;
    3)
        run_server
        sleep 1
        build_client
        run_client
        echo -e "${YELLOW}🟢 Serveur Docker et client en cours. Ctrl+C pour quitter.${RESET}"
        trap stop_all SIGINT
        wait
        ;;
    4)
        echo "Bye 👋"
        exit 0
        ;;
    5)
        build_client_tests
        run_client_tests
        ;;
    6)
        run_server_tests
        ;;
    7)
        create_dmg
        ;;
    *)
        echo -e "${RED}❌ Option invalide.${RESET}"
        exit 1
        ;;
esac