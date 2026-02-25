#include "Sources.h"
#include "LocalVideoSource.h"
#include <scoped_refptr.h>

Sources& Sources::instance() {
    static Sources instance;
    return instance;
}

Sources::Sources() {
    // Création propre, ref-countée
    local_video_ =
        webrtc::scoped_refptr<LocalVideoSource>(new LocalVideoSource());
}

webrtc::scoped_refptr<LocalVideoSource> Sources::localVideo() {
    return local_video_;
}