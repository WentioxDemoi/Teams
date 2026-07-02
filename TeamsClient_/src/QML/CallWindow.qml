import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

ApplicationWindow {
    id: callWindow
    width: 960
    height: 640
    minimumWidth: 480
    minimumHeight: 320
    visible: true
    title: "Appel — " + (webRTCVM ? webRTCVM.remoteUsername : "")
    color: "#000000"

    flags: Qt.Window

    onClosing: function (close) {
        if (webRTCVM)
            webRTCVM.hangup();
    }

    // ─── Vidéo distante : plein écran ──────────────────────────────────────
    VideoOutput {
        id: remoteVideoOutput
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectCrop

        Component.onCompleted: if (webRTCVM)
            webRTCVM.setRemoteVideoSink(videoSink)
    }

    // ─── Overlay : en attente de connexion OU caméra distante coupée ──────
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12
        visible: webRTCVM && (!webRTCVM.isContactConnected || !webRTCVM.isRemoteCameraEnabled)

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 96
            height: 96
            radius: 48
            color: "#3A3A3C"

            Text {
                anchors.centerIn: parent
                text: webRTCVM && webRTCVM.remoteUsername.length > 0 ? webRTCVM.remoteUsername.charAt(0).toUpperCase() : "?"
                font.pixelSize: 36
                font.weight: Font.Bold
                color: "#ffffff"
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: webRTCVM ? webRTCVM.remoteUsername : ""
            font.pixelSize: 18
            font.weight: Font.DemiBold
            font.family: "SF Pro Text"
            color: "#ffffff"
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: {
                if (!webRTCVM)
                    return "";
                if (!webRTCVM.isContactConnected)
                    return "En attente de la connexion de " + webRTCVM.remoteUsername;
                if (!webRTCVM.isRemoteCameraEnabled)
                    return webRTCVM.remoteUsername + " a désactivé sa caméra";
                return "";
            }
            font.pixelSize: 13
            font.family: "SF Pro Text"
            color: "#9A9A9E"
        }
    }

    // ─── Vidéo locale : PiP en bas à gauche ────────────────────────────────
    Rectangle {
        id: localVideoFrame

        width: 320
        height: 180      // 16:9

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        anchors.bottomMargin: 20

        radius: 14
        clip: true
        color: "#202020"
        
        layer.enabled: true

        layer.smooth: true

        border.color: Qt.rgba(1, 1, 1, 0.15)
        border.width: 1

        VideoOutput {
            id: localVideoOutput
            anchors.fill: parent
            fillMode: VideoOutput.PreserveAspectFit

            Component.onCompleted: {
                if (webRTCVM)
                    webRTCVM.setLocalVideoSink(videoSink);
            }
        }

        // Placeholder lorsque la caméra est coupée
        Rectangle {
            anchors.fill: parent
            color: "#303030"
            visible: !(webRTCVM && webRTCVM.cameraEnabled)

            Column {
                anchors.centerIn: parent
                spacing: 10

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "📷"
                    font.pixelSize: 36
                    color: "white"
                    opacity: 0.6
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Caméra désactivée"
                    font.pixelSize: 15
                    color: "white"
                    opacity: 0.8
                }
            }
        }
    }

    // ─── Barre de contrôle haut-droite : caméra / micro / quitter ──────────
    RowLayout {
        id: controlsBar
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 16
        anchors.rightMargin: 16
        spacing: 10
        z: 10

        // Bouton caméra (désactivée par défaut)
        Rectangle {
            width: 44
            height: 44
            radius: 22
            color: webRTCVM && webRTCVM.cameraEnabled ? Qt.rgba(1, 1, 1, 0.14) : "#FF3B30"

            Behavior on color {
                ColorAnimation {
                    duration: 120
                }
            }

            Text {
                anchors.centerIn: parent
                text: webRTCVM && webRTCVM.cameraEnabled ? "📷" : "📷"
                font.pixelSize: 16
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: if (webRTCVM)
                    webRTCVM.toggleCamera()
            }
        }

        // Bouton micro (désactivé par défaut)
        Rectangle {
            width: 44
            height: 44
            radius: 22
            color: webRTCVM && webRTCVM.micEnabled ? Qt.rgba(1, 1, 1, 0.14) : "#FF3B30"

            Behavior on color {
                ColorAnimation {
                    duration: 120
                }
            }

            Text {
                anchors.centerIn: parent
                text: webRTCVM && webRTCVM.micEnabled ? "🎙️" : "🔇"
                font.pixelSize: 16
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: if (webRTCVM)
                    webRTCVM.toggleMic()
            }
        }

        // Bouton quitter l'appel
        Rectangle {
            width: 44
            height: 44
            radius: 22
            color: "#FF3B30"

            Text {
                anchors.centerIn: parent
                text: "✕"
                font.pixelSize: 18
                font.weight: Font.Bold
                color: "#ffffff"
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: if (webRTCVM)
                    webRTCVM.hangup()
            }
        }
    }

    Connections {
        target: webRTCVM
        function onCallEnded() {
            callWindow.close();
        }
    }
}
