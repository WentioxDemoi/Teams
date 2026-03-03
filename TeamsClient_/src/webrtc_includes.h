#include "api/ref_count.h"
#include "rtc_base/ref_counter.h"

// Macro pour les overrides indispensables de tous les scoped_refptr (Code legacy qui fait à peu près la même chose que les shared_ptr mtn)
#define WEBRTC_REF_COUNT_IMPL \
    void AddRef() const override { ref_count_.IncRef(); } \
    webrtc::RefCountReleaseStatus Release() const override { \
        auto status = ref_count_.DecRef(); \
        if (status == webrtc::RefCountReleaseStatus::kDroppedLastRef) delete this; \
        return status; \
    } \
    mutable webrtc::webrtc_impl::RefCounter ref_count_{0};