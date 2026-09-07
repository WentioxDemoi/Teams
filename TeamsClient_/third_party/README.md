# WebRTC & libyuv (C++ Static Build)

This directory contains **locally built, statically linked versions of WebRTC and libyuv**, intended for use in a C++ project with **CMake**.

The full WebRTC and libyuv source trees are not included in this repository. They are downloaded and built through scripts in order to maintain a controlled build environment.

> ⚠️ WebRTC and libyuv versions may evolve over time. The commits used for known working builds are documented below so that the corresponding versions can be restored if necessary.

---

## Prerequisites (macOS)

Make sure the following tools are installed:

```bash
brew install git python ninja cmake
```

---

## Dependency: WebRTC

WebRTC is a real-time communication library providing, among other things:

- real-time audio / video
- secure network transport
- NAT traversal (STUN / TURN)
- low-level P2P primitives

This repository does **not** provide a signaling server.

### WebRTC Version Used

The current known working build was created using the following commit:

```text
WebRTC commit:
c4f5b6ef7e5711428a6e433eb7c8d9d3593e7e1a
```

> ⚠️ WebRTC is retrieved using `fetch` / `gclient sync`. If compilation issues or behavioral changes occur, it may therefore be necessary to revert to the known working commit listed above.

If a new WebRTC version is used, the build script may need to be adapted, especially if there are changes to the API, `BUILD.gn` files, dependencies, or GN options.

---

## Dependency: depot_tools

`depot_tools` is the set of tools used by Chromium/WebRTC to retrieve and manage project sources and dependencies, notably through `fetch` and `gclient`.

The build script uses an **explicitly pinned version of `depot_tools`** to prevent future tool updates from changing the build behavior.

### Version Used

```text
depot_tools commit:
6fbb6012d6138844379a2df23eea5e86a2ea6696
```

The script automatically clones `depot_tools` if it is not already present, then checks out the specified commit.

It is therefore normally **not necessary to manually select a `depot_tools` version**.

---

## Building WebRTC

From the `third_party` directory:

```bash
./build_webrtc.sh
```

The script performs the following steps:

1. Creates the WebRTC working directory.
2. Downloads `depot_tools` if it is not already present.
3. Automatically checks out the pinned `depot_tools` version.
4. Configures `gclient`.
5. Downloads and synchronizes the WebRTC sources and dependencies.
6. Applies the required `BUILD.gn` patch.
7. Generates the GN build configuration.
8. Builds WebRTC as a static library (`libwebrtc.a`).
9. Copies the library into `lib/`.

### WebRTC Compatibility

The current build has been tested with:

```text
WebRTC:
c4f5b6ef7e5711428a6e433eb7c8d9d3593e7e1a

depot_tools:
6fbb6012d6138844379a2df23eea5e86a2ea6696
```

If the build stops working after a new `gclient sync`, first check which WebRTC revision is currently being used.

To restore the known working version, it may be necessary to manually check out the WebRTC commit listed above before running the synchronization/build again.

> ⚠️ The script is currently configured for **macOS arm64**.
>
> ⚠️ The GN options and the `BUILD.gn` patch depend on the WebRTC version. A newer WebRTC version may therefore require changes to the build script.

---

## Dependency: libyuv

libyuv is a low-level library used by WebRTC for:

- YUV / RGB format conversions
- image scaling and rotation
- SIMD optimizations (ARM / x86)

It is built **locally and independently** in order to avoid implicit system dependencies.

### libyuv Version Used

The current known working build was created using the following commit:

```text
libyuv commit:
f489037bfc93bd5338974b00b5765efb7b3afa4c
```

The build script checks out this version in order to maintain a known working version.

Depending on the project, libyuv may be:

- linked directly by the application;
- or used indirectly through WebRTC.

---

## Building libyuv

From the `third_party` directory:

```bash
./build_libyuv.sh
```

The script performs the following steps:

1. Clones libyuv from the official Chromium repository.
2. Checks out the pinned libyuv version.
3. Configures a Release build for macOS arm64.
4. Builds the library.
5. Installs the headers and static library locally.

> ⚠️ The script is currently configured for **macOS arm64**.
>
> For `x86_64` or other platforms, update `CMAKE_OSX_ARCHITECTURES` accordingly.

> ⚠️ If libyuv is already statically integrated into WebRTC, it does not need to be linked separately.

---

## Using WebRTC and libyuv with CMake

### WebRTC

See the project's current `CMakeLists.txt` for the WebRTC integration.

### libyuv

See the project's current `CMakeLists.txt` for the libyuv integration.

---

## Directory Structure

```text
third_party/
├── build_libyuv.sh
├── build_webrtc.sh
│
├── webrtc/
│   ├── include/
│   ├── lib/
│   ├── src/           # do not commit
│   └── depot_tools/   # do not commit
│
├── libyuv/
│   ├── libyuv/
│   │   ├── include/
│   │   └── lib/
│   └── libyuv_src/    # do not commit
```

---

## Important Notes

Never commit the locally downloaded sources and tools to Git:

- `webrtc/src/`
- `webrtc/depot_tools/`
- `libyuv/libyuv_src/`

Locally generated libraries and headers are also not intended to be committed to this repository unless explicitly stated otherwise.

The builds are currently **specific to macOS arm64**.

For Linux or Windows, the GN / CMake build scripts will need to be adapted.

No network infrastructure (signaling server, TURN server) is provided here.

---

## Supported Platforms

- macOS
  - arm64 ✅
  - x86_64 ⚠️ (script requires adaptation)
- Linux ❌
- Windows ❌

---

## Reproducibility and Known Working Versions

Known working versions of WebRTC and libyuv are kept as a reference in order to avoid having to rediscover a compatible combination of versions later.

The versions used for the current known working build are:

```text
WebRTC:
c4f5b6ef7e5711428a6e433eb7c8d9d3593e7e1a

depot_tools:
6fbb6012d6138844379a2df23eea5e86a2ea6696

libyuv:
f489037bfc93bd5338974b00b5765efb7b3afa4c
```

The `depot_tools` and libyuv commits are **pinned directly in the build scripts**.

The known working WebRTC commit is also used by the build script to select the corresponding version.

This configuration preserves a known working combination of dependencies.

Updating WebRTC or its dependencies may require changes to the `BUILD.gn` patch, GN options, or the C++ code using WebRTC.

If a newer version causes issues, reverting to the commits listed above should restore the previously used build environment.

---

## License

- WebRTC is distributed under the BSD 3-Clause License.
- libyuv is distributed under the BSD 3-Clause License.
- Third-party licenses are included in the respective official repositories.