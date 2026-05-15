import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: messageRoot
    anchors.fill: parent

    required property int selectedContact

    // ─── Données de contacts ─────────────────────────────────────────────────
    ListModel {
        id: contactsModel

        ListElement {
            name: "Alice Martin"
            initials: "AM"
            lastMessage: "OK je regarde ça ce soir 👍"
            time: "14:32"
            unread: 2
            online: true
            avatarColor: "#0A84FF"
        }
        ListElement {
            name: "Bob Dupont"
            initials: "BD"
            lastMessage: "Tu peux m'envoyer le fichier ?"
            time: "13:10"
            unread: 0
            online: false
            avatarColor: "#30D158"
        }
        ListElement {
            name: "Clara Roux"
            initials: "CR"
            lastMessage: "Parfait, à demain alors !"
            time: "11:58"
            unread: 0
            online: true
            avatarColor: "#FF375F"
        }
        ListElement {
            name: "David Leroy"
            initials: "DL"
            lastMessage: "Merci pour tout 🙏"
            time: "Hier"
            unread: 1
            online: false
            avatarColor: "#FF9F0A"
        }
        ListElement {
            name: "Emma Bernard"
            initials: "EB"
            lastMessage: "C'est noté, je te tiens au courant."
            time: "Hier"
            unread: 0
            online: true
            avatarColor: "#BF5AF2"
        }
        ListElement {
            name: "François Petit"
            initials: "FP"
            lastMessage: "Rappelle-moi quand tu peux !"
            time: "Lun"
            unread: 0
            online: false
            avatarColor: "#5AC8FA"
        }
        ListElement {
            name: "Gaëlle Simon"
            initials: "GS"
            lastMessage: "Reçu 👌"
            time: "Lun"
            unread: 3
            online: false
            avatarColor: "#FF6961"
        }
        ListElement {
            name: "Hugo Moreau"
            initials: "HM"
            lastMessage: "On se voit vendredi ?"
            time: "Dim"
            unread: 0
            online: true
            avatarColor: "#34C759"
        }
    }

    // ─── Fausses conversations par contact ────────────────────────────────────
    property var conversations: [
        // Alice
        [
            { text: "Salut ! T'as regardé le doc que j'ai partagé ?", fromMe: false, time: "14:20" },
            { text: "Pas encore, j'ai été prise dans les réunions toute la matinée", fromMe: true, time: "14:25" },
            { text: "OK je regarde ça ce soir 👍", fromMe: false, time: "14:32" }
        ],
        // Bob
        [
            { text: "Hey, t'as avancé sur le projet ?", fromMe: true, time: "12:50" },
            { text: "Ouais, presque fini. Il me manque juste un fichier", fromMe: false, time: "13:05" },
            { text: "Tu peux m'envoyer le fichier ?", fromMe: false, time: "13:10" }
        ],
        // Clara
        [
            { text: "On se retrouve à quelle heure demain ?", fromMe: true, time: "11:40" },
            { text: "10h ça te va ?", fromMe: false, time: "11:45" },
            { text: "Nickel !", fromMe: true, time: "11:55" },
            { text: "Parfait, à demain alors !", fromMe: false, time: "11:58" }
        ],
        // David
        [
            { text: "J'ai déposé le colis chez toi", fromMe: true, time: "Hier" },
            { text: "Merci pour tout 🙏", fromMe: false, time: "Hier" }
        ],
        // Emma
        [
            { text: "Tu peux me tenir au courant de l'avancement ?", fromMe: true, time: "Hier" },
            { text: "C'est noté, je te tiens au courant.", fromMe: false, time: "Hier" }
        ],
        // François
        [
            { text: "J'ai essayé de t'appeler", fromMe: true, time: "Lun" },
            { text: "Rappelle-moi quand tu peux !", fromMe: false, time: "Lun" }
        ],
        // Gaëlle
        [
            { text: "C'est bon, j'ai bien reçu tout ce que tu m'as envoyé", fromMe: true, time: "Lun" },
            { text: "Reçu 👌", fromMe: false, time: "Lun" }
        ],
        // Hugo
        [
            { text: "Dispo vendredi ?", fromMe: false, time: "Dim" },
            { text: "On se voit vendredi ?", fromMe: false, time: "Dim" }
        ]
    ]

   ColumnLayout {
    anchors.fill: parent
    spacing: 8

    // ─── Barre haute ─────────────────────────────────────────────────────
    RowLayout {
        Layout.fillWidth: true
        Layout.preferredHeight: 40
        spacing: 8

        // Titre "Messages"
        Rectangle {
            Layout.preferredWidth: Math.round(messageRoot.width * 0.25)
            Layout.preferredHeight: 40
            radius: 12
            color: Qt.rgba(0.11, 0.11, 0.12, 0.92)

            Text {
                anchors.centerIn: parent
                text: "Messages"
                font.pixelSize: 17
                font.weight: Font.SemiBold
                font.family: "SF Pro Text"
                color: "#ffffff"
            }
        }

        // Barre Contacts Infos
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            radius: 12
            color: Qt.rgba(0.11, 0.11, 0.12, 0.92)

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                spacing: 10

                // Avatar mini
                Rectangle {
                    width: 28
                    height: 28
                    radius: 14
                    color: contactsModel.get(messageRoot.selectedContact).avatarColor

                    Text {
                        anchors.centerIn: parent
                        text: contactsModel.get(messageRoot.selectedContact).initials
                        font.pixelSize: 11
                        font.weight: Font.Bold
                        font.family: "SF Pro Text"
                        color: "#ffffff"
                    }

                    Rectangle {
                        width: 8
                        height: 8
                        radius: 4
                        color: contactsModel.get(messageRoot.selectedContact).online ? "#30D158" : "#636366"
                        border.color: Qt.rgba(0.11, 0.11, 0.12, 0.92)
                        border.width: 1.5
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                    }
                }

                // Nom + statut
                Column {
                    spacing: 1
                    Layout.fillWidth: true

                    Text {
                        text: contactsModel.get(messageRoot.selectedContact).name
                        font.pixelSize: 14
                        font.weight: Font.SemiBold
                        font.family: "SF Pro Text"
                        color: "#ffffff"
                    }

                    Text {
                        text: contactsModel.get(messageRoot.selectedContact).online ? "En ligne" : "Hors ligne"
                        font.pixelSize: 11
                        font.family: "SF Pro Text"
                        color: contactsModel.get(messageRoot.selectedContact).online ? "#30D158" : "#636366"
                    }
                }

                // Bouton téléphone
                Rectangle {
                    width: 32
                    height: 32
                    radius: 8
                    color: phoneHover.containsMouse ? Qt.rgba(1,1,1,0.12) : Qt.rgba(1,1,1,0.07)
                    Behavior on color { ColorAnimation { duration: 100 } }

                    Text {
                        anchors.centerIn: parent
                        text: "📞"
                        font.pixelSize: 15
                    }

                    MouseArea {
                        id: phoneHover
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }

                // Bouton caméra
                Rectangle {
                    width: 32
                    height: 32
                    radius: 8
                    color: camHover.containsMouse ? Qt.rgba(1,1,1,0.12) : Qt.rgba(1,1,1,0.07)
                    Behavior on color { ColorAnimation { duration: 100 } }

                    Text {
                        anchors.centerIn: parent
                        text: "📷"
                        font.pixelSize: 15
                    }

                    MouseArea {
                        id: camHover
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
            }
        }
    }

    // ─── Layout bas ───────────────────────────────────────────────────────
    RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: 8

        // ── Liste des contacts ────────────────────────────────────────────
        Rectangle {
            Layout.preferredWidth: Math.round(messageRoot.width * 0.25)
            Layout.fillHeight: true
            radius: 12
            color: Qt.rgba(0.11, 0.11, 0.12, 0.92)
            clip: true

            ScrollView {
                anchors.fill: parent
                anchors.margins: 6
                clip: true
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff

                ListView {
                    id: contactList
                    model: contactsModel
                    spacing: 4
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: Rectangle {
                        width: contactList.width
                        height: 62
                        radius: 10
                        color: mouseArea.containsMouse
                               ? Qt.rgba(1, 1, 1, 0.07)
                               : (index === messageRoot.selectedContact
                                  ? Qt.rgba(0.04, 0.52, 1.0, 0.18)
                                  : "transparent")
                        Behavior on color { ColorAnimation { duration: 100 } }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            spacing: 10

                            Rectangle {
                                width: 38
                                height: 38
                                radius: 19
                                color: model.avatarColor

                                Text {
                                    anchors.centerIn: parent
                                    text: model.initials
                                    font.pixelSize: 13
                                    font.weight: Font.Bold
                                    font.family: "SF Pro Text"
                                    color: "#ffffff"
                                }

                                Rectangle {
                                    width: 10
                                    height: 10
                                    radius: 5
                                    color: model.online ? "#30D158" : "transparent"
                                    border.color: model.online ? Qt.rgba(0.11,0.11,0.12,0.92) : "transparent"
                                    border.width: 1.5
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                }
                            }

                            Column {
                                Layout.fillWidth: true
                                spacing: 3

                                RowLayout {
                                    width: parent.width
                                    spacing: 4

                                    Text {
                                        Layout.fillWidth: true
                                        text: model.name
                                        font.pixelSize: 13
                                        font.weight: Font.SemiBold
                                        font.family: "SF Pro Text"
                                        color: "#ffffff"
                                        elide: Text.ElideRight
                                    }

                                    Text {
                                        text: model.time
                                        font.pixelSize: 11
                                        font.family: "SF Pro Text"
                                        color: "#636366"
                                    }
                                }

                                RowLayout {
                                    width: parent.width
                                    spacing: 4

                                    Text {
                                        Layout.fillWidth: true
                                        text: model.lastMessage
                                        font.pixelSize: 12
                                        font.family: "SF Pro Text"
                                        color: "#636366"
                                        elide: Text.ElideRight
                                    }

                                    Rectangle {
                                        visible: model.unread > 0
                                        width: 18
                                        height: 18
                                        radius: 9
                                        color: "#0A84FF"

                                        Text {
                                            anchors.centerIn: parent
                                            text: model.unread
                                            font.pixelSize: 10
                                            font.weight: Font.Bold
                                            color: "#ffffff"
                                        }
                                    }
                                }
                            }
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: messageRoot.selectedContact = index
                        }
                    }
                }
            }
        }

        // ── Zone de messages ──────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 12
            color: Qt.rgba(0.11, 0.11, 0.12, 0.92)
            clip: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    ListView {
                        id: messageList
                        model: messageRoot.conversations[messageRoot.selectedContact]
                        topMargin: 16
                        bottomMargin: 16
                        spacing: 10
                        boundsBehavior: Flickable.StopAtBounds

                        onCountChanged: Qt.callLater(() => positionViewAtEnd())
                        Component.onCompleted: positionViewAtEnd()

                        delegate: Item {
                            width: messageList.width
                            height: bubble.height + 18

                            property bool isMe: modelData.fromMe

                            Rectangle {
                                id: bubble
                                x: isMe ? parent.width - width - 16 : 16
                                width: Math.min(bubbleText.implicitWidth + 24, messageList.width * 0.65)
                                height: bubbleText.implicitHeight + 18
                                radius: 16
                                color: isMe ? "#0A84FF" : Qt.rgba(1, 1, 1, 0.1)

                                Text {
                                    id: bubbleText
                                    anchors.centerIn: parent
                                    width: parent.width - 24
                                    text: modelData.text
                                    font.pixelSize: 14
                                    font.family: "SF Pro Text"
                                    color: "#ffffff"
                                    wrapMode: Text.Wrap
                                }
                            }

                            Text {
                                anchors.top: bubble.bottom
                                anchors.topMargin: 3
                                x: bubble.x
                                text: modelData.time
                                font.pixelSize: 10
                                font.family: "SF Pro Text"
                                color: "#636366"
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: Qt.rgba(1, 1, 1, 0.06)
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 50
                    color: "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 10

                        Rectangle {
                            Layout.fillWidth: true
                            height: 34
                            radius: 17
                            color: Qt.rgba(1, 1, 1, 0.07)

                            TextField {
                                anchors.fill: parent
                                anchors.leftMargin: 14
                                anchors.rightMargin: 14
                                placeholderText: "Message…"
                                placeholderTextColor: "#636366"
                                color: "#ffffff"
                                font.pixelSize: 14
                                font.family: "SF Pro Text"
                                verticalAlignment: TextInput.AlignVCenter
                                background: Item {}
                            }
                        }

                        Rectangle {
                            width: 34
                            height: 34
                            radius: 17
                            color: "#0A84FF"

                            Text {
                                anchors.centerIn: parent
                                text: "↑"
                                font.pixelSize: 18
                                font.weight: Font.Bold
                                color: "#ffffff"
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                            }
                        }
                    }
                }
            }
        }
    }
   }
}