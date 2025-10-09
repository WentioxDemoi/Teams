# Teams
Le but de ce projet est de recoder une application similaire à Teams

Requirements :
    - Cmake > 3.16
    - QT6 (Idéalement QT 6.9.3)

Pour la compilation du client :
    - Aller dans le dossier build de TeamsClient_
    - cmake ../ -DCMAKE_PREFIX_PATH=$HOME/Qt/6.9.3/macos/lib/cmake/Qt6 # Attention à bien spécifier le path vers QT6
    - make
    - ./TeamsClient_.app/Contents/MacOS/TeamsClient_ (ou double cliquer sur l'app dans build/)
    Pour créer un .dmg :
        - ~/Qt/6.9.3/macos/bin/macdeployqt TeamsClient_.app/
        - hdiutil create -volname "TeamsClient_" -srcfolder TeamsClient_.app -ov -format UDZO TeamsClient_.dmg