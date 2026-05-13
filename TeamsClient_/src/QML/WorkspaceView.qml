import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    anchors.fill: parent
    anchors.margins: 6

    // ─────────────────────────────
    // HEADER (style macOS léger)
    // ─────────────────────────────
    header: Rectangle {
        height: 52
        width: parent.width

        color: "#f5f5f7"
        radius: 14

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10

            Item { Layout.fillWidth: true }

            TextField {
                Layout.preferredWidth: 420
                placeholderText: "Search..."

                background: Rectangle {
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e5e5ea"
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "TMP"

                background: Rectangle {
                    radius: 8
                    color: "#ffffff"
                    border.color: "#e5e5ea"
                }
            }
        }
    }

    // ─────────────────────────────
    // BODY (sidebar + content)
    // ─────────────────────────────
    RowLayout {
        anchors.fill: parent
        anchors.topMargin: 10
        spacing: 10

        // ───────── SIDEBAR (Finder style)
        Rectangle {
            Layout.preferredWidth: 56
            Layout.fillHeight: true

            color: "#f5f5f7"
            radius: 16
            clip: true

            Column {
                anchors.centerIn: parent
                spacing: 14

                Repeater {
                    model: ["🏠", "📁", "⚙️"]

                    delegate: Rectangle {
                        width: 42
                        height: 42
                        radius: 10

                        color: "transparent"

                        property bool hovered: false

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                        }

                        Rectangle {
                            anchors.fill: parent
                            radius: 10
                            color: parent.hovered ? "#e8e8ed" : "transparent"
                        }

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            font.pixelSize: 18
                        }
                    }
                }
            }
        }

        // ───────── CONTENU PRINCIPAL
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            color: "#ffffff"
            radius: 16

            // zone vide pour l'instant
            Item {
                anchors.fill: parent
            }
        }
    }
}