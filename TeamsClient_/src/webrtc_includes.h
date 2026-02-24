// webrtc_includes.h — inclus UNIQUEMENT dans les .cpp qui en ont besoin
#pragma once

#undef emit
#include "api/media_stream_interface.h"
#include "rtc_base/ref_counted_object.h"
#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/media_stream_interface.h"
#define emit 