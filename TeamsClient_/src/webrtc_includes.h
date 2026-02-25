// webrtc_includes.h — inclus UNIQUEMENT dans les .cpp qui en ont besoin
#pragma once

// #undef emit
#include "api/media_stream_interface.h"
#include "rtc_base/ref_counted_object.h"
#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/media_stream_interface.h"
#include <rtc_base/thread.h>
#include <create_peerconnection_factory.h>
#include <rtc_base/ref_counted_object.h>
#include "api/video/i420_buffer.h"  // WebRTC I420BufferInterface
#include "api/scoped_refptr.h"
#include "media/base/adapted_video_track_source.h"
#include "absl/types/optional.h"
// #define emit 

// Macro pour les overrides indispensables de tous les scoped_refptr (Code legacy qui fait à peu près la même chose que les shared_ptr mtn)
#define WEBRTC_REF_COUNT_IMPL \
    void AddRef() const override { ref_count_.IncRef(); } \
    webrtc::RefCountReleaseStatus Release() const override { \
        auto status = ref_count_.DecRef(); \
        if (status == webrtc::RefCountReleaseStatus::kDroppedLastRef) delete this; \
        return status; \
    } \
    mutable webrtc::webrtc_impl::RefCounter ref_count_{0};