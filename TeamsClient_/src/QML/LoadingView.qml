import QtQuick
import QtQuick.Controls

Page {
    anchors.fill: parent

    background: Rectangle {
        color: "#000000"
    }

    Column {
        anchors.centerIn: parent
        spacing: 16

        Item {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 40
            height: 40

            Repeater {
                model: 12

                Rectangle {
                    width: 3
                    height: 9
                    radius: 2
                    color: "#ffffff"
                    opacity: (index / 12) * 0.85 + 0.08
                    x: 20 - width / 2
                    y: 20 - height / 2
                    transformOrigin: Item.Bottom
                    rotation: index * 30

                    SequentialAnimation on opacity {
                        loops: Animation.Infinite
                        running: true
                        PauseAnimation { duration: (11 - index) * 83 }
                        NumberAnimation {
                            from: 0.08
                            to: 0.92
                            duration: 100
                        }
                        NumberAnimation {
                            from: 0.92
                            to: 0.08
                            duration: 900
                        }
                    }
                }
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Chargement de votre espace…"
            font.pixelSize: 15
            font.family: "SF Pro Text"
            color: "#636366"
            topPadding: 4
        }
    }
}