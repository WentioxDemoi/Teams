## Working Version Backups

Known working versions of the project are preserved as binaries to avoid having to rebuild everything later and to prevent having to resolve dependency-related issues again, especially those involving WebRTC.

Two versions are currently archived:
- **Previous WebRTC version**
- **New WebRTC version**

Each version contains the artifacts required for its use:

- **Linux**: binary to run inside a Docker container
- **macOS**: `.dmg` file to install

The goal is to keep a directly usable version available, regardless of the future state of the dependencies and build environment.