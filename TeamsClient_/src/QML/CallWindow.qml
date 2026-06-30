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

    onClosing: function(close) {
        // Empêche la destruction "silencieuse" par la croix native : on repasse
        // toujours par endCall() côté C++ pour que callWindow_ soit correctement
        // remis à nullptr et que deleteLater() soit appelé une seule fois, au bon endroit.
        if (webRTCVM) webRTCVM.endCall();
    }

    // ─── Vidéo distante : plein écran ──────────────────────────────────────
    VideoOutput {
        id: remoteVideoOutput
        anchors.fill: parent
        fillMode: VideoOutput.PreserveAspectCrop

        Component.onCompleted: if (webRTCVM) webRTCVM.setRemoteVideoSink(videoSink)
    }

    // Placeholder quand le flux distant n'a pas encore démarré
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12
        visible: !hasRemoteFrame.active

        // Petit hack pour détecter "pas encore de frame" sans dépendre d'une API spécifique :
        // on se contente d'afficher tant que rien ne semble avoir été peint.
        // Si tu exposes plus tard un état explicite (ex: webRTCVM.remoteConnected),
        // remplace cette condition par lui — ce sera plus fiable.
        Timer {
            id: hasRemoteFrame
            property bool active: false
            interval: 1500
            running: true
            onTriggered: active = true
        }

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
            text: "En cours de connexion…"
            font.pixelSize: 13
            font.family: "SF Pro Text"
            color: "#9A9A9E"
        }
    }

    // ─── Vidéo locale : PiP en bas à gauche ────────────────────────────────
    Rectangle {
        id: localVideoFrame
        width: 200
        height: 150
        radius: 12
        color: '#9c9cc2'
        clip: true
        anchors.left: parent.left
        anchors.bottom: controlsBar.top
        anchors.leftMargin: 16
        anchors.bottomMargin: 16
        border.color: Qt.rgba(1, 1, 1, 0.12)
        border.width: 1

        VideoOutput {
            id: localVideoOutput
            anchors.fill: parent
            fillMode: VideoOutput.PreserveAspectCrop
            visible: webRTCVM && webRTCVM.cameraEnabled

            Component.onCompleted: if (webRTCVM) webRTCVM.setLocalVideoSink(videoSink)
        }

        // Placeholder quand la caméra locale est désactivée
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 6
            visible: !(webRTCVM && webRTCVM.cameraEnabled)

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: "📷"
                font.pixelSize: 28
                opacity: 0.5
            }

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: "Caméra désactivée"
                font.pixelSize: 11
                font.family: "SF Pro Text"
                color: "#8E8E93"
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

            Behavior on color { ColorAnimation { duration: 120 } }

            Text {
                anchors.centerIn: parent
                text: webRTCVM && webRTCVM.cameraEnabled ? "📷" : "📷"
                font.pixelSize: 16
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: if (webRTCVM) webRTCVM.toggleCamera()
            }
        }

        // Bouton micro (désactivé par défaut)
        Rectangle {
            width: 44
            height: 44
            radius: 22
            color: webRTCVM && webRTCVM.micEnabled ? Qt.rgba(1, 1, 1, 0.14) : "#FF3B30"

            Behavior on color { ColorAnimation { duration: 120 } }

            Text {
                anchors.centerIn: parent
                text: webRTCVM && webRTCVM.micEnabled ? "🎙️" : "🔇"
                font.pixelSize: 16
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: if (webRTCVM) webRTCVM.toggleMic()
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
                onClicked: if (webRTCVM) webRTCVM.endCall()
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
