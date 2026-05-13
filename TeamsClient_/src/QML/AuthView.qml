import QtQuick
import QtQuick.Controls

Page {
    property bool showLogin: true

    background: Rectangle {
        color: "#000000"
    }

    // Carte centrale
    Rectangle {
        anchors.centerIn: parent
        width: 340
        height: formCol.implicitHeight + 48
        radius: 20
        color: "#1c1c1e"

        // Liseré subtil
        Rectangle {
            anchors.fill: parent
            radius: 20
            color: "transparent"
            border.color: Qt.rgba(1, 1, 1, 0.08)
            border.width: 1
        }

        Column {
            id: formCol
            anchors.centerIn: parent
            width: parent.width - 48
            spacing: 0

            // ── Logo / Titre ──────────────────────────────────────────────
            Column {
                width: parent.width
                spacing: 6
                bottomPadding: 28

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "⬡"
                    font.pixelSize: 36
                    color: "#0A84FF"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: showLogin ? "Connexion" : "Créer un compte"
                    font.pixelSize: 22
                    font.weight: Font.SemiBold
                    font.family: "SF Pro Display"
                    color: "#ffffff"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: showLogin
                          ? "Bienvenue, entrez vos identifiants."
                          : "Remplissez les champs pour continuer."
                    font.pixelSize: 13
                    font.family: "SF Pro Text"
                    color: "#636366"
                }
            }

            // ── Formulaire ────────────────────────────────────────────────
            Loader {
                id: formLoader
                width: parent.width
                sourceComponent: showLogin ? loginForm : registerForm
                height: item ? item.implicitHeight : 0
            }

            // ── Séparateur + switch ───────────────────────────────────────
            Column {
                width: parent.width
                spacing: 0
                topPadding: 20

                // Ligne séparatrice "ou"
                Row {
                    width: parent.width
                    spacing: 10

                    Rectangle {
                        width: (parent.width - orLabel.width - 20) / 2
                        height: 1
                        anchors.verticalCenter: parent.verticalCenter
                        color: Qt.rgba(1, 1, 1, 0.08)
                    }

                    Text {
                        id: orLabel
                        text: "ou"
                        font.pixelSize: 12
                        font.family: "SF Pro Text"
                        color: "#48484a"
                    }

                    Rectangle {
                        width: (parent.width - orLabel.width - 20) / 2
                        height: 1
                        anchors.verticalCenter: parent.verticalCenter
                        color: Qt.rgba(1, 1, 1, 0.08)
                    }
                }

                // Lien switch
                Item {
                    width: parent.width
                    height: 44

                    Text {
                        anchors.centerIn: parent
                        text: showLogin
                              ? "Pas encore de compte ?  <font color='#0A84FF'>S'inscrire</font>"
                              : "Déjà un compte ?  <font color='#0A84FF'>Se connecter</font>"
                        font.pixelSize: 13
                        font.family: "SF Pro Text"
                        color: "#636366"
                        textFormat: Text.RichText

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: showLogin = !showLogin
                        }
                    }
                }
            }
        }
    }

    // ── Composants de formulaire ──────────────────────────────────────────

    Component {
        id: loginForm
        Column {
            spacing: 10

            property string _email: ""
            property string _password: ""

            // Champ Email
            Rectangle {
                width: parent.width
                height: 44
                radius: 10
                color: Qt.rgba(1, 1, 1, 0.06)
                border.color: emailField.activeFocus
                              ? Qt.rgba(0.04, 0.52, 1.0, 0.6)
                              : Qt.rgba(1, 1, 1, 0.08)
                border.width: 1
                Behavior on border.color { ColorAnimation { duration: 150 } }

                TextField {
                    id: emailField
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    placeholderText: "Adresse e-mail"
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.family: "SF Pro Text"
                    placeholderTextColor: "#48484a"
                    verticalAlignment: TextInput.AlignVCenter
                    background: Item {}
                    onTextChanged: formLoader.item._email = text
                }
            }

            // Champ Password
            Rectangle {
                width: parent.width
                height: 44
                radius: 10
                color: Qt.rgba(1, 1, 1, 0.06)
                border.color: passField.activeFocus
                              ? Qt.rgba(0.04, 0.52, 1.0, 0.6)
                              : Qt.rgba(1, 1, 1, 0.08)
                border.width: 1
                Behavior on border.color { ColorAnimation { duration: 150 } }

                TextField {
                    id: passField
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    placeholderText: "Mot de passe"
                    echoMode: TextInput.Password
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.family: "SF Pro Text"
                    placeholderTextColor: "#48484a"
                    verticalAlignment: TextInput.AlignVCenter
                    background: Item {}
                    onTextChanged: formLoader.item._password = text
                }
            }

            // Mot de passe oublié
            Text {
                anchors.right: parent.right
                text: "Mot de passe oublié ?"
                font.pixelSize: 12
                font.family: "SF Pro Text"
                color: "#0A84FF"
                topPadding: 2
                bottomPadding: 6

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }
            }

            // Bouton Login
            Rectangle {
                width: parent.width
                height: 44
                radius: 10
                color: loginHover.containsMouse
                       ? Qt.rgba(0.04, 0.52, 1.0, 0.85)
                       : "#0A84FF"
                Behavior on color { ColorAnimation { duration: 120 } }

                Text {
                    anchors.centerIn: parent
                    text: "Se connecter"
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.weight: Font.SemiBold
                    font.family: "SF Pro Text"
                }

                MouseArea {
                    id: loginHover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: authVM.loginUser(
                        formLoader.item._email,
                        formLoader.item._password
                    )
                }
            }
        }
    }

    Component {
        id: registerForm
        Column {
            spacing: 10

            property string _username: ""
            property string _email: ""
            property string _password: ""

            // Champ Username
            Rectangle {
                width: parent.width
                height: 44
                radius: 10
                color: Qt.rgba(1, 1, 1, 0.06)
                border.color: unameField.activeFocus
                              ? Qt.rgba(0.04, 0.52, 1.0, 0.6)
                              : Qt.rgba(1, 1, 1, 0.08)
                border.width: 1
                Behavior on border.color { ColorAnimation { duration: 150 } }

                TextField {
                    id: unameField
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    placeholderText: "Nom d'utilisateur"
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.family: "SF Pro Text"
                    placeholderTextColor: "#48484a"
                    verticalAlignment: TextInput.AlignVCenter
                    background: Item {}
                    onTextChanged: formLoader.item._username = text
                }
            }

            // Champ Email
            Rectangle {
                width: parent.width
                height: 44
                radius: 10
                color: Qt.rgba(1, 1, 1, 0.06)
                border.color: regEmailField.activeFocus
                              ? Qt.rgba(0.04, 0.52, 1.0, 0.6)
                              : Qt.rgba(1, 1, 1, 0.08)
                border.width: 1
                Behavior on border.color { ColorAnimation { duration: 150 } }

                TextField {
                    id: regEmailField
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    placeholderText: "Adresse e-mail"
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.family: "SF Pro Text"
                    placeholderTextColor: "#48484a"
                    verticalAlignment: TextInput.AlignVCenter
                    background: Item {}
                    onTextChanged: formLoader.item._email = text
                }
            }

            // Champ Password
            Rectangle {
                width: parent.width
                height: 44
                radius: 10
                color: Qt.rgba(1, 1, 1, 0.06)
                border.color: regPassField.activeFocus
                              ? Qt.rgba(0.04, 0.52, 1.0, 0.6)
                              : Qt.rgba(1, 1, 1, 0.08)
                border.width: 1
                Behavior on border.color { ColorAnimation { duration: 150 } }

                TextField {
                    id: regPassField
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    placeholderText: "Mot de passe"
                    echoMode: TextInput.Password
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.family: "SF Pro Text"
                    placeholderTextColor: "#48484a"
                    verticalAlignment: TextInput.AlignVCenter
                    background: Item {}
                    onTextChanged: formLoader.item._password = text
                }
            }

            // Bouton Register
            Rectangle {
                width: parent.width
                height: 44
                radius: 10
                color: regHover.containsMouse
                       ? Qt.rgba(0.04, 0.52, 1.0, 0.85)
                       : "#0A84FF"
                Behavior on color { ColorAnimation { duration: 120 } }

                Text {
                    anchors.centerIn: parent
                    text: "Créer un compte"
                    color: "#ffffff"
                    font.pixelSize: 15
                    font.weight: Font.SemiBold
                    font.family: "SF Pro Text"
                }

                MouseArea {
                    id: regHover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: authVM.registerUser(
                        formLoader.item._email,
                        formLoader.item._username,
                        formLoader.item._password
                    )
                }
            }
        }
    }
}