import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    anchors.fill: parent
    anchors.margins: 8

    background: Rectangle {
        color: "#000000"
    }

    header: Rectangle {
        height: 66
        width: parent.width
        color: "transparent"

        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0.11, 0.11, 0.12, 0.92)
            radius: 16
            layer.enabled: true

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

            Item { Layout.fillWidth: true }

            Rectangle {
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
                        Layout.fillWidth: true
                        placeholderText: "Rechercher"
                        color: "#ffffff"
                        font.pixelSize: 15
                        font.family: "SF Pro Text"
                        verticalAlignment: TextInput.AlignVCenter

                        background: Item {}

                        placeholderTextColor: "#636366"
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                Layout.preferredWidth: 80
                Layout.preferredHeight: 32
                radius: 8
                color: btnHover.containsMouse
                       ? Qt.rgba(0.04, 0.52, 1.0, 0.85)
                       : "#0A84FF"

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

                Behavior on color { ColorAnimation { duration: 120 } }
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.topMargin: 10
        spacing: 8

        Rectangle {
            Layout.preferredWidth: 76
            Layout.fillHeight: true
            radius: 16
            color: Qt.rgba(0.11, 0.11, 0.12, 1.0)
            clip: true

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
                        { glyph: "🏠", label: "Accueil" },
                        { glyph: "🧟", label: "Fichiers" },
                        { glyph: "🥵", label: "Réglages" }
                    ]

                    delegate: Item {
                        width: 54
                        height: 54

                        property bool hovered: sideHover.containsMouse
                        property bool active: index === 0

                        Rectangle {
                            anchors.fill: parent
                            radius: 8
                            color: active
                                   ? Qt.rgba(0.04, 0.52, 1.0, 0.18)
                                   : (hovered ? Qt.rgba(1, 1, 1, 0.07) : "transparent")
                            Behavior on color { ColorAnimation { duration: 100 } }
                        }

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
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 16
            color: "#1c1c1e"
            layer.enabled: true

            Column {
                anchors.centerIn: parent
                spacing: 10

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "⊡"
                    font.pixelSize: 44
                    color: "#3a3a3c"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Aucun élément"
                    font.pixelSize: 17
                    font.weight: Font.Medium
                    font.family: "SF Pro Display"
                    color: "#ffffff"
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Commencez par créer ou importer du contenu."
                    font.pixelSize: 13
                    font.family: "SF Pro Text"
                    color: "#636366"
                }
            }
        }
    }
}