import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    width: 970
    height: 600
    visible: true
    title: "Teams"
    color: "#000000"


    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: loadingView

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

    Component {
        id: loadingView
        LoadingView {}
    }
    Component {
        id: authView
        AuthView {}
    }
    Component {
        id: workspaceView
        WorkspaceView {}
    }

    Connections {
        target: authVM

        function onAuthSuccess() {
            console.log("Auth success, switching to workspace");
            stackView.replace(workspaceView);
        }
        function onNoTokenFound() {
            console.log("Error NoTokenFound:");
            stackView.replace(authView);
        }
        function onAuthError(error) {
            console.log("Error login:", error);
        }
    }

    Component.onCompleted: authVM.start()
}
