# Teams
Le but de ce projet est de recoder une application similaire à Teams

Requirements :
    - Cmake >= 3.16
    - Boost >= 1.89.0
    - QT6 (Idéalement QT 6.9.3)
    - Docker >= 28.4

Pour la compilation du client :
    # Le cmake sera à perfectionner pour ajouter plus d'outils au fur et a mesure.
    - Aller dans le dossier build de TeamsClient_
    - cmake ../ -DCMAKE_PREFIX_PATH=$HOME/Qt/6.9.3/macos/lib/cmake/Qt6 # Attention à bien spécifier le path vers QT6
    - make
    - ./TeamsClient_.app/Contents/MacOS/TeamsClient_ (ou double cliquer sur l'app dans build/)
    Pour créer un .dmg :
        - ~/Qt/6.9.3/macos/bin/macdeployqt TeamsClient_.app/
        - hdiutil create -volname "TeamsClient_" -srcfolder TeamsClient_.app -ov -format UDZO TeamsClient_.dmg
    
Pour la compilation du server :
    # Docker permet de faire tourner le server boost dans les mêmes conditions que sur # l'EC2 sur lequel il sera déployé.
    # Installation manuelle de boost 1.89.0 un peu déroutante, elle n'installe pas
    # les .cmake requis par défaut par Cmake. Link vers les libs dynamiques .so.
    # Il faudra ajouter d'autres dépendances à l'avenir.
    - docker build -t server .
    - docker run -it server
