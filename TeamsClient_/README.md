# Qt Chat Client - MVVM Architecture

## Project Structure

```text
src/
├── Models/          # Data models
├── ViewModels/      # Presentation logic
├── Views/           # User interfaces
├── Services/        # Network and business services
├── Components/      # Reusable components
├── Utils/           # Utilities
└── Core/            # Base classes
```

## Build

1. Navigate to the build directory:
```bash
cd build/
```

2. Generate the build files with CMake (adjust the Qt6 path according to your configuration):
```bash
cmake ../ # Or to include tests: cmake ../ -DBUILD_TESTS=ON
```

3. Build the project:
```bash
make
```

4. Launch the application:
```bash
./TeamsClient_.app/Contents/MacOS/TeamsClient_
```

Alternatively, you can double-click the application in the `build/` directory.

## Preview

1. Navigate to the build directory:
```bash
cd build/
```

2. Run the following command on the target QML file:
```bash
~/Qt/6.9.3/macos/bin/qml ../src/QML/Main.qml
```

## File Formatting

1. Navigate to the build directory:
```bash
cd build/
```

2. Run the following command on the target QML file:
```bash
~/Qt/6.9.3/macos/bin/qmlformat ../src/QML/Main.qml
```

### Camera Access

- Reset camera permissions on macOS:
```bash
tccutil reset Camera
```

### Microphone Access

- Reset microphone permissions on macOS:
```bash
tccutil reset Microphone
```

### Creating a `.dmg` File

1. Run `macdeployqt` to prepare the application:
```bash
~/Qt/6.9.3/macos/bin/macdeployqt TeamsClient_.app/ -qmldir=/Users/remidesbordes/Documents/Teams/TeamsClient_/src/QML
```

2. Remove unused SQL plugins:
```bash
find TeamsClient_.app -name "libqsqlodbc.dylib" -delete
find TeamsClient_.app -name "libqsqlpsql.dylib" -delete
```

3. Re-sign the application after any modifications to the bundle:
```bash
codesign --force --deep --sign - TeamsClient_.app
```

4. Create the `.dmg` file:
```bash
hdiutil create -volname "TeamsClient_" -srcfolder TeamsClient_.app -ov -format UDZO TeamsClient_.dmg
```

## Running Tests

1. Run the test binary:
```bash
ctest -V # Or ./TeamsClient_Tests
```

2. Run the command to generate the coverage report:
```bash
gcovr \
    --root .. \
    --filter '../src/' \
    --html-details coverage.html
```

3. Open `coverage.html` in a browser.

## Dependencies

- Qt 6.9.3 (Core, Widgets, Network, Test)
- Qt Keychain 0.15.0 (manually installed in the Qt repository)
  - https://github.com/frankosterfeld/qtkeychain/tree/0.15
  - `mkdir build && cd build`
  - `cmake .. -DCMAKE_PREFIX_PATH=$HOME/Qt/6.9.3/macos -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/Qt/6.9.3/macos -DBUILD_WITH_QT6=ON`
  - `make -j$(sysctl -n hw.ncpu)`
  - `make install`
- CMake 3.16+
- C++17 compiler

## MVVM Architecture

- **Model**: Pure data without business logic
- **View**: User interface (Qt Widgets/QML)
- **ViewModel**: Presentation logic and data binding
- **Services**: Business logic and network communication

## TODO

- Add a service to check the locally stored token by sending a request to the server
- Create the database and associated services
- Add unit tests
- When logging into the same account from two instances at the same time, the session is broken and the software needs to be restarted
- Issue when closing the software after a call: the application remains active and does not shut down