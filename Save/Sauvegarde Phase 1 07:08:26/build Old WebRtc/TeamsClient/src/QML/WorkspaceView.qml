import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    anchors.fill: parent
    anchors.margins: 8

    // ─── État global de navigation ───────────────────────────────────────────
    property int currentIndex: 0

    Timer {
        id: searchDebounce
        interval: 300
        repeat: false
        onTriggered: {
            const q = searchField.text.trim();
            if (q.length > 0) {
                chatVM.searchUsers(q);
            }
        }
    }

    // ─── Fond noir général ───────────────────────────────────────────────────
    background: Rectangle {
        color: "#000000"
    }

    // ─── Header ──────────────────────────────────────────────────────────────
    header: Rectangle {
        height: 66
        width: parent.width
        color: "transparent"

        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0.11, 0.11, 0.12, 0.92)
            radius: 16
            layer.enabled: true

            // Séparateur bas
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                height: 1
                color: Qt.rgba(1, 1, 1, 0.08)
            }
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 50
            anchors.rightMargin: 14
            spacing: 12

            Item {
                Layout.fillWidth: true
            }

            // Barre de recherche
            Rectangle {
                id: searchBarRect
                Layout.preferredWidth: 380
                Layout.preferredHeight: 34
                radius: 10
                color: Qt.rgba(1, 1, 1, 0.08)

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 6

                    Text {
                        text: "⌕"
                        font.pixelSize: 15
                        color: "#636366"
                        Layout.alignment: Qt.AlignVCenter
                    }

                    TextField {
                        id: searchField
                        Layout.fillWidth: true
                        placeholderText: "Rechercher"
                        color: "#ffffff"
                        font.pixelSize: 15
                        font.family: "SF Pro Text"
                        verticalAlignment: TextInput.AlignVCenter
                        background: Item {}
                        placeholderTextColor: "#636366"

                        onTextChanged: searchDebounce.restart()
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            // Bouton Nouveau
            Rectangle {
                Layout.preferredWidth: 80
                Layout.preferredHeight: 32
                radius: 8
                color: btnHover.containsMouse ? Qt.rgba(0.04, 0.52, 1.0, 0.85) : "#0A84FF"

                Behavior on color {
                    ColorAnimation {
                        duration: 120
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: "Nouveau"
                    color: "#ffffff"
                    font.pixelSize: 13
                    font.weight: Font.Medium
                    font.family: "SF Pro Text"
                }

                MouseArea {
                    id: btnHover
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }
    }

    // ─── Corps principal ──────────────────────────────────────────────────────
    RowLayout {
        anchors.fill: parent
        anchors.topMargin: 10
        spacing: 8

        // ── Sidebar ──────────────────────────────────────────────────────────
        Rectangle {
            Layout.preferredWidth: 76
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
            }

            Column {
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8

                Repeater {
                    model: [
                        {
                            glyph: "🏠",
                            label: "Accueil"
                        },
                        {
                            glyph: "🧟",
                            label: "Messages"
                        },
                        {
                            glyph: "🥵",
                            label: "Réglages"
                        }
                    ]

                    delegate: Item {
                        width: 54
                        height: 54

                        property bool hovered: sideHover.containsMouse
                        property bool active: index === root.currentIndex

                        // Fond actif / survol
                        Rectangle {
                            anchors.fill: parent
                            radius: 8
                            color: active ? Qt.rgba(0.04, 0.52, 1.0, 0.18) : (hovered ? Qt.rgba(1, 1, 1, 0.07) : "transparent")
                            Behavior on color {
                                ColorAnimation {
                                    duration: 100
                                }
                            }
                        }

                        // Icône + label
                        Column {
                            anchors.centerIn: parent
                            spacing: 2

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: modelData.glyph
                                font.pixelSize: 17
                                color: active ? "#0A84FF" : "#ebebf5"
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: modelData.label
                                font.pixelSize: 9
                                font.family: "SF Pro Text"
                                color: active ? "#0A84FF" : "#636366"
                            }
                        }

                        MouseArea {
                            id: sideHover
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                if (index === root.currentIndex)
                                    return;
                                root.currentIndex = index;
                                switch (index) {
                                case 0:
                                    stackView.replace(homeView);
                                    break;
                                case 1:
                                    stackView.replace(messageView);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        // ── Zone de contenu principale ───────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 16
            color: "#000000"
            layer.enabled: true

            StackView {
                id: stackView
                anchors.fill: parent
                initialItem: homeView

                replaceEnter: Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 400
                        easing.type: Easing.InOutQuad
                    }
                }
                replaceExit: Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 400
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
    }
    // Popup d'appel
    Popup {
        id: incomingCallPopup

        modal: true
        focus: true
        width: 320
        height: 180

        property var contact

        anchors.centerIn: Overlay.overlay

        background: Rectangle {
            radius: 12
            color: "#1C1C1E"
        }

        Column {
            anchors.centerIn: parent
            spacing: 24

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: incomingCallPopup.contact ? incomingCallPopup.contact.username : ""
                color: "white"
                font.pixelSize: 20
                font.bold: true
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 40

                Rectangle {
                    width: 60
                    height: 60
                    radius: 30
                    color: "#30D158"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            incomingCallPopup.close();
                            chatVM.acceptIncomingCall(incomingCallPopup.contact.username);
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "✓"
                        color: "white"
                        font.pixelSize: 28
                    }
                }

                Rectangle {
                    width: 60
                    height: 60
                    radius: 30
                    color: "#FF3B30"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            incomingCallPopup.close();
                            chatVM.rejectIncomingCall();
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        color: "white"
                        font.pixelSize: 28
                    }
                }
            }
        }
    }

    // ─── Dropdown résultats de recherche ────────────────────────────────────
    Popup {
        id: searchDropdown
        parent: searchBarRect
        x: 0
        y: searchBarRect.height + 6
        width: searchBarRect.width
        height: Math.min(280, Math.max(60, searchResultsList.contentHeight + 16))
        padding: 8
        modal: false
        focus: false
        closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
        visible: searchField.text.trim().length > 0

        background: Rectangle {
            radius: 12
            color: Qt.rgba(0.13, 0.13, 0.14, 0.98)
            border.color: Qt.rgba(1, 1, 1, 0.08)
            border.width: 1
        }

        contentItem: ScrollView {
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ListView {
                id: searchResultsList
                model: chatVM.searchResults
                spacing: 4
                boundsBehavior: Flickable.StopAtBounds

                Text {
                    anchors.centerIn: parent
                    visible: searchResultsList.count === 0
                    text: "Aucun résultat"
                    font.pixelSize: 13
                    font.family: "SF Pro Text"
                    color: "#636366"
                }

                delegate: Rectangle {
                    width: searchResultsList.width
                    height: 54
                    radius: 8
                    color: resultHover.containsMouse ? Qt.rgba(1, 1, 1, 0.07) : "transparent"
                    Behavior on color {
                        ColorAnimation {
                            duration: 100
                        }
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 10

                        Rectangle {
                            width: 32
                            height: 32
                            radius: 16
                            color: model.avatarColor

                            Text {
                                anchors.centerIn: parent
                                text: model.initials
                                font.pixelSize: 12
                                font.weight: Font.Bold
                                font.family: "SF Pro Text"
                                color: "#ffffff"
                            }

                            Rectangle {
                                width: 9
                                height: 9
                                radius: 4.5
                                color: model.online ? "#30D158" : "#636366"
                                border.color: Qt.rgba(0.13, 0.13, 0.14, 0.98)
                                border.width: 1.5
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                            }
                        }

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

                    MouseArea {
                        id: resultHover
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            chatVM.selectUser(model.uuid);
                            searchField.text = "";
                            root.currentIndex = 1;
                            stackView.replace(messageView);
                        }
                    }
                }
            }
        }
    }

    // ─── Composants des vues ─────────────────────────────────────────────────
    Component {
        id: homeView
        HomeView {}
    }
    Component {
        id: messageView
        MessageView {}
    }
    Connections {
        target: chatVM

        function onIncomingCall(contact) {
            console.log("QML: incomingCall reçu pour", contact.username);
            incomingCallPopup.contact = contact;
            incomingCallPopup.open();
        }
        function onCancelIncomingCall(contact) {
            console.log("QML: cancelIncomingCall reçu, popup visible avant close:", incomingCallPopup.visible);
            incomingCallPopup.contact = null;
            incomingCallPopup.close();
        }
    }
}
