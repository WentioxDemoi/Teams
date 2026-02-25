#ifndef SOURCES_H
#define SOURCES_H

#include "../includes.h"
#include "../webrtc_includes.h"
#include "LocalVideoSource.h"

class Sources {
public:
    // Accès global
    static Sources& instance();

    // Accès aux sources
    webrtc::scoped_refptr<LocalVideoSource> localVideo();

    // Interdire copie / move
    Sources(const Sources&) = delete;
    Sources& operator=(const Sources&) = delete;

private:
    Sources();
    ~Sources() = default;

    webrtc::scoped_refptr<LocalVideoSource> local_video_;
};

#endif