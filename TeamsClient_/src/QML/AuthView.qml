import QtQuick
import QtQuick.Controls

Page {

    property bool showLogin: true

    Column {
        anchors.centerIn: parent
        spacing: 16
        width: 300

        Loader {
            id: formLoader
            width: parent.width
            sourceComponent: showLogin ? loginForm : registerForm
            // La hauteur suit l'item chargé
            height: item ? item.implicitHeight : 0
        }

        Button {
            width: parent.width
            text: showLogin ? "Switch to Register" : "Switch to Login"
            onClicked: showLogin = !showLogin
        }
    }

    Component {
        id: loginForm
        Column {
            spacing: 12
            TextField {
                width: 300
                placeholderText: "Email"
                onTextChanged: formLoader.item._email = text
            }
            TextField {
                width: 300
                placeholderText: "Password"
                echoMode: TextInput.Password
                onTextChanged: formLoader.item._password = text
            }
            Button {
                width: 300
                text: "Login"
                onClicked: authVM.loginUser(
                    formLoader.item._email,
                    formLoader.item._password
                )
            }
            // Propriétés internes pour récupérer les valeurs
            property string _email: ""
            property string _password: ""
        }
    }

    Component {
        id: registerForm
        Column {
            spacing: 12
            TextField { width: 300; placeholderText: "Username";  onTextChanged: formLoader.item._username = text }
            TextField { width: 300; placeholderText: "Email";     onTextChanged: formLoader.item._email    = text }
            TextField { width: 300; placeholderText: "Password";  echoMode: TextInput.Password; onTextChanged: formLoader.item._password = text }
            Button {
                width: 300
                text: "Register"
                onClicked: authVM.registerUser(
                    formLoader.item._email,
                    formLoader.item._username,
                    formLoader.item._password
                )
            }
            property string _username: ""
            property string _email: ""
            property string _password: ""
        }
    }
}