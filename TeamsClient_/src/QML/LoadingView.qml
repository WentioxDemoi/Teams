import QtQuick
import QtQuick.Controls

Page {
    anchors.fill: parent

    Column {
        anchors.centerIn: parent
        spacing: 20

        BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            running: true
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Loading your ecosystem"
            font.pixelSize: 16
            color: "#666666"
        }
    }
}