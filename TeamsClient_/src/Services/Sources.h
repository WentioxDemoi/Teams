#ifndef SOURCES_H
#define SOURCES_H

#include "../includes.h"
#include "../webrtc_includes.h"
#include "LocalVideoSource.h"
#include "RemoteVideoSource.h"

class Sources {
public:
    // Accès global
    static Sources& instance();

    // Accès aux sources
    webrtc::scoped_refptr<LocalVideoSource> localVideo();
    webrtc::scoped_refptr<RemoteVideoSource> remoteVideo();

    // Interdire copie / move
    Sources(const Sources&) = delete;
    Sources& operator=(const Sources&) = delete;

private:
    Sources();
    ~Sources() = default;

    webrtc::scoped_refptr<LocalVideoSource> local_video_;
    webrtc::scoped_refptr<RemoteVideoSource> remote_video_;
    
};

#endif