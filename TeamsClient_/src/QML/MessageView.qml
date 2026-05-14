import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: messageRoot
    anchors.fill: parent

    // ─── Données de contacts ─────────────────────────────────────────────────
    required property int selectedContact
    
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

    // ─── Layout principal ─────────────────────────────────────────────────────
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ── Panneau contacts (≈ 2.5/10 de la zone) ───────────────────────────
        Rectangle {
            Layout.preferredWidth: Math.round(messageRoot.width * 0.26)
            Layout.fillHeight: true
            radius: 16
            color: Qt.rgba(0.11, 0.11, 0.12, 1.0)
            clip: true

            // Séparateur droit
            Rectangle {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 16
                anchors.bottomMargin: 16
                width: 1
                color: Qt.rgba(1, 1, 1, 0.06)
                z: 2
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // En-tête "Messages"
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        text: "Messages"
                        font.pixelSize: 17
                        font.weight: Font.SemiBold
                        font.family: "SF Pro Text"
                        color: "#ffffff"
                    }

                    // Séparateur bas du header
                    Rectangle {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        height: 1
                        color: Qt.rgba(1, 1, 1, 0.07)
                    }
                }

                // Liste scrollable des contacts
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    ListView {
                        id: contactList
                        width: parent.width
                        model: contactsModel
                        spacing: 2
                        topMargin: 8
                        bottomMargin: 8
                        boundsBehavior: Flickable.StopAtBounds

                        ScrollBar.vertical: ScrollBar {
                            policy: ScrollBar.AsNeeded
                            width: 4
                            contentItem: Rectangle {
                                radius: 2
                                color: Qt.rgba(1, 1, 1, 0.18)
                            }
                            background: Rectangle { color: "transparent" }
                        }

                        delegate: ContactItem {
                            width: contactList.width - 16
                            x: 8
                            contactName: model.name
                            contactInitials: model.initials
                            lastMsg: model.lastMessage
                            msgTime: model.time
                            unreadCount: model.unread
                            isOnline: model.online
                            avatarBg: model.avatarColor
                            isSelected: index === messageRoot.selectedContact

                            onClicked: messageRoot.selectedContact = index
                        }
                    }
                }
            }
        }

        // ── Zone de conversation (reste de l'espace) ──────────────────────────
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Placeholder : affiche le nom du contact sélectionné
            Column {
                anchors.centerIn: parent
                spacing: 12

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: contactsModel.get(messageRoot.selectedContact).initials
                    font.pixelSize: 42
                    font.weight: Font.Bold
                    color: contactsModel.get(messageRoot.selectedContact).avatarColor
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: contactsModel.get(messageRoot.selectedContact).name
                    font.pixelSize: 18
                    font.weight: Font.SemiBold
                    font.family: "SF Pro Text"
                    color: "#ffffff"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Commencez une conversation…"
                    font.pixelSize: 13
                    font.family: "SF Pro Text"
                    color: "#636366"
                }
            }
        }
    }
}
