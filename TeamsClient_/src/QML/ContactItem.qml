import QtQuick
import QtQuick.Layouts

// ─── Item contact ─────────────────────────────────────────────────────────────
// Propriétés exposées
// ─────────────────────────────────────────────────────────────────────────────
Item {
    id: contactRoot

    height: 62

    // Props publiques
    property string contactName:    "Nom Prénom"
    property string contactInitials:"NP"
    property string lastMsg:        "Dernier message…"
    property string msgTime:        "00:00"
    property int    unreadCount:    0
    property bool   isOnline:       false
    property color  avatarBg:       "#0A84FF"
    property bool   isSelected:     false

    signal clicked()

    // ── Fond (sélectionné / survol) ───────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        radius: 10
        color: isSelected
               ? Qt.rgba(0.04, 0.52, 1.0, 0.18)
               : (hoverArea.containsMouse ? Qt.rgba(1, 1, 1, 0.06) : "transparent")
        Behavior on color { ColorAnimation { duration: 100 } }
    }

    // ── Contenu ───────────────────────────────────────────────────────────────
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 8
        anchors.bottomMargin: 8
        spacing: 10

        // Avatar avec indicateur en ligne
        Item {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignVCenter

            // Cercle avatar
            Rectangle {
                id: avatarCircle
                anchors.fill: parent
                radius: 20
                color: avatarBg

                Text {
                    anchors.centerIn: parent
                    text: contactInitials
                    font.pixelSize: 14
                    font.weight: Font.SemiBold
                    font.family: "SF Pro Text"
                    color: "#ffffff"
                }
            }

            // Point "en ligne"
            Rectangle {
                visible: isOnline
                width: 11
                height: 11
                radius: 6
                color: "#30D158"
                anchors.right: avatarCircle.right
                anchors.bottom: avatarCircle.bottom
                anchors.rightMargin: -1
                anchors.bottomMargin: -1

                // Bordure pour détacher du fond
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width + 2
                    height: parent.height + 2
                    radius: (parent.width + 2) / 2
                    color: "transparent"
                    border.color: Qt.rgba(0.11, 0.11, 0.12, 1.0)
                    border.width: 2
                }
            }
        }

        // Nom + dernier message
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 3

            RowLayout {
                Layout.fillWidth: true
                spacing: 4

                Text {
                    Layout.fillWidth: true
                    text: contactName
                    font.pixelSize: 14
                    font.weight: unreadCount > 0 ? Font.SemiBold : Font.Medium
                    font.family: "SF Pro Text"
                    color: "#ffffff"
                    elide: Text.ElideRight
                }

                Text {
                    text: msgTime
                    font.pixelSize: 11
                    font.family: "SF Pro Text"
                    color: unreadCount > 0 ? "#0A84FF" : "#636366"
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 4

                Text {
                    Layout.fillWidth: true
                    text: lastMsg
                    font.pixelSize: 12
                    font.family: "SF Pro Text"
                    color: unreadCount > 0 ? "#ebebf5" : "#636366"
                    elide: Text.ElideRight
                    font.weight: unreadCount > 0 ? Font.Medium : Font.Normal
                }

                // Badge non-lu
                Rectangle {
                    visible: unreadCount > 0
                    Layout.preferredWidth: unreadCount > 9 ? 22 : 18
                    Layout.preferredHeight: 18
                    radius: 9
                    color: "#0A84FF"

                    Text {
                        anchors.centerIn: parent
                        text: unreadCount > 9 ? "9+" : unreadCount.toString()
                        font.pixelSize: 11
                        font.weight: Font.Bold
                        font.family: "SF Pro Text"
                        color: "#ffffff"
                    }
                }
            }
        }
    }

    // ── Zone cliquable ────────────────────────────────────────────────────────
    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: contactRoot.clicked()
    }
}
