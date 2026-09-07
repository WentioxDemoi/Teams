import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: messageRoot
    width: parent ? parent.width : 0
    height: parent ? parent.height : 0

    Connections {
        target: chatVM
    }
    Connections {
        target: webRTCVM
    }

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
                Layout.preferredWidth: messageRoot.width > 0 ? Math.round(messageRoot.width * 0.25) : 200
                Layout.preferredHeight: 40
                radius: 12
                color: Qt.rgba(0.11, 0.11, 0.12, 0.92)

                Text {
                    anchors.centerIn: parent
                    text: "Messages"
                    font.pixelSize: 17
                    font.weight: Font.DemiBold
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
                        color: chatVM.selectedContact && chatVM.selectedContact.uuid ? chatVM.selectedContact.avatarColor : "#636366"

                        Text {
                            anchors.centerIn: parent
                            text: chatVM.selectedContact && chatVM.selectedContact.uuid ? chatVM.selectedContact.initials : ""
                            font.pixelSize: 11
                            font.weight: Font.Bold
                            font.family: "SF Pro Text"
                            color: "#ffffff"
                        }

                        Rectangle {
                            width: 8
                            height: 8
                            radius: 4
                            color: chatVM.selectedContact && chatVM.selectedContact.online ? "#30D158" : "#636366"
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
                            text: chatVM.selectedContact && chatVM.selectedContact.uuid ? chatVM.selectedContact.username : ""
                            font.pixelSize: 14
                            font.weight: Font.DemiBold
                            font.family: "SF Pro Text"
                            color: "#ffffff"
                        }

                        Text {
                            text: chatVM.selectedContact && chatVM.selectedContact.online ? "En ligne" : "Hors ligne"
                            font.pixelSize: 11
                            font.family: "SF Pro Text"
                            color: chatVM.selectedContact && chatVM.selectedContact.online ? "#30D158" : "#636366"
                        }
                    }

                    // Bouton téléphone
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 8
                        color: phoneHover.containsMouse ? Qt.rgba(1, 1, 1, 0.12) : Qt.rgba(1, 1, 1, 0.07)
                        Behavior on color {
                            ColorAnimation {
                                duration: 100
                            }
                        }
                        enabled: chatVM.selectedContact && chatVM.selectedContact.uuid
                        opacity: enabled ? 1.0 : 0.4

                        Text {
                            anchors.centerIn: parent
                            text: "📞"
                            font.pixelSize: 15
                        }

                        MouseArea {
                            id: phoneHover
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (chatVM.selectedContact && chatVM.selectedContact.uuid) {
                                    chatVM.startCall(chatVM.selectedContact.uuid, chatVM.selectedContact.username);
                                }
                            }
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
                Layout.preferredWidth: messageRoot.width > 0 ? Math.round(messageRoot.width * 0.25) : 200
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
                        model: chatVM.contactList
                        spacing: 4
                        boundsBehavior: Flickable.StopAtBounds

                        delegate: Rectangle {
                            width: contactList.width
                            height: 62
                            radius: 10
                            color: mouseArea.containsMouse ? Qt.rgba(1, 1, 1, 0.07) : (model.uuid === ((chatVM.selectedContact && chatVM.selectedContact.uuid) ? chatVM.selectedContact.uuid : "") ? Qt.rgba(0.04, 0.52, 1.0, 0.18) : "transparent")
                            Behavior on color {
                                ColorAnimation {
                                    duration: 100
                                }
                            }

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
                                        color: model.online ? "#30D158" : "#636366"
                                        border.color: model.online ? Qt.rgba(0.11, 0.11, 0.12, 0.92) : Qt.rgba(1, 1, 1, 0.30)
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
                                            text: model.username
                                            font.pixelSize: 13
                                            font.weight: Font.DemiBold
                                            font.family: "SF Pro Text"
                                            color: "#ffffff"
                                            elide: Text.ElideRight
                                        }
                                    }

                                    RowLayout {
                                        width: parent.width
                                        spacing: 6

                                        Text {
                                            Layout.fillWidth: true
                                            Layout.alignment: Qt.AlignVCenter
                                            text: model.lastMessage
                                            font.pixelSize: 12
                                            color: "#636366"
                                            elide: Text.ElideRight
                                        }

                                        Rectangle {
                                            visible: model.unreadCount > 0
                                            Layout.preferredWidth: 16
                                            Layout.preferredHeight: 16
                                            radius: 8
                                            color: "#FF3B30"

                                            Text {
                                                anchors.centerIn: parent
                                                text: model.unreadCount > 9 ? "9+" : model.unreadCount
                                                font.pixelSize: 9
                                                font.bold: true
                                                color: "white"
                                            }
                                        }
                                    }
                                }
                            }

                            MouseArea {
                                id: mouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: chatVM.selectContact(model.uuid)
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
                            model: chatVM.currentMessageList
                            topMargin: 16
                            bottomMargin: 16
                            spacing: 10
                            boundsBehavior: Flickable.StopAtBounds

                            onCountChanged: Qt.callLater(() => positionViewAtEnd())
                            Component.onCompleted: positionViewAtEnd()

                            delegate: Item {
                                width: messageList.width
                                height: bubble.height + 18

                                property bool isMe: model.fromMe

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
                                        text: model.content
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
                                    text: Qt.formatTime(model.timestamp, "hh:mm")
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

                            // Inputfield message
                            Rectangle {
                                Layout.fillWidth: true
                                height: 34
                                radius: 17
                                color: Qt.rgba(1, 1, 1, 0.07)

                                TextField {
                                    id: messageInput
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
                                    Keys.onPressed: event => {
                                        if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                                            if (messageInput.text.trim() !== "") {
                                                chatVM.sendMessage(messageInput.text);
                                                messageInput.clear();
                                            }
                                            event.accepted = true;
                                        }
                                    }
                                }
                            }

                            // Bouton envoyer
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
                                    onClicked: {
                                        if (messageInput.text.trim() !== "") {
                                            chatVM.sendMessage(messageInput.text);
                                            messageInput.clear();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // ─── État vide "Faites coucou" ──────────────────────────────────
                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 14
                    z: 2
                    visible: messageList.count === 0 && chatVM.selectedContact && chatVM.selectedContact.uuid

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "👋"
                        font.pixelSize: 52
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: chatVM.selectedContact && chatVM.selectedContact.uuid ? "Faites coucou à " + chatVM.selectedContact.username + " !" : ""
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                        font.family: "SF Pro Text"
                        color: "#ffffff"
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "Aucun message pour le moment.\nEnvoyez le premier message !"
                        font.pixelSize: 13
                        font.family: "SF Pro Text"
                        color: "#636366"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
