#pragma once
#include "Config.h"

// Thin POD to carry display/scanout state built by your existing init code.
struct DisplayState {
  int drm_fd = -1;
  uint32_t crtc_id = 0;
  uint32_t conn_id = 0;
  drmModeModeInfo mode{}; // non-const because drmModeSetCrtc wants a mutable ptr
  
  gbm_device*  gbm_dev  = nullptr;
  gbm_surface* gbm_surf = nullptr;
  
  EGLDisplay egl_display = EGL_NO_DISPLAY;
  EGLSurface egl_surf    = EGL_NO_SURFACE;
};

// ------------------------------------------------------------
// PageFlipper: owns gbm front buffers, FB cache, flip/wait
// ------------------------------------------------------------
class PageFlipper {
public:
    explicit PageFlipper(const DisplayState& s) : s_(s) {
        std::memset(&ev_, 0, sizeof(ev_));
        ev_.version = DRM_EVENT_CONTEXT_VERSION;
        ev_.page_flip_handler = &PageFlipper::on_page_flip_static;
    }

    // Present current EGL front buffer. On the first call, does a modeset.
    // On subsequent calls, queues a page flip and waits for completion,
    // then releases the previous BO.
    bool present() {
        // Lock GBM front buffer produced by eglSwapBuffers
        gbm_bo* bo = gbm_surface_lock_front_buffer(s_.gbm_surf);
        if (!bo) return false;

        uint32_t fb = get_or_create_fb(bo);
        if (!fb) {
          gbm_surface_release_buffer(s_.gbm_surf, bo);
          return false;
        }


	uint32_t conn_mut = s_.conn_id;
	drmModeModeInfo mode_mut = s_.mode;
        if (first_) {
	  if (drmModeSetCrtc(s_.drm_fd, s_.crtc_id, fb, 0, 0, &conn_mut, 1, &mode_mut) != 0) {
	    return false;
	  }
	  first_ = false;
	  if (prev_bo_) gbm_surface_release_buffer(s_.gbm_surf, prev_bo_);
        } else {
	  waiting_ = true;
	  if (drmModePageFlip(s_.drm_fd, s_.crtc_id, fb, DRM_MODE_PAGE_FLIP_EVENT, this) != 0) {
	    // Fallback: blocking modeset if flip not supported
	    if (drmModeSetCrtc(s_.drm_fd, s_.crtc_id, fb, 0, 0, &conn_mut, 1, &mode_mut) != 0) {
	      waiting_ = false;
	      gbm_surface_release_buffer(s_.gbm_surf, bo);
	      return false;
	    }
	    waiting_ = false;
            }
	  while (waiting_) wait_for_event();
	  if (prev_bo_) gbm_surface_release_buffer(s_.gbm_surf, prev_bo_);
        }
        prev_bo_ = bo;
        prev_fb_ = fb;
        return true;
    }

    void cleanup() {
        for (auto& kv : fb_cache_) if (kv.second) drmModeRmFB(s_.drm_fd, kv.second);
        fb_cache_.clear();
        if (prev_bo_) { gbm_surface_release_buffer(s_.gbm_surf, prev_bo_); prev_bo_ = nullptr; }
    }

private:
    uint32_t get_or_create_fb(gbm_bo* bo) {
        auto it = fb_cache_.find(bo);
        if (it != fb_cache_.end()) return it->second;
        uint32_t handle = gbm_bo_get_handle(bo).u32;
        uint32_t stride = gbm_bo_get_stride(bo);
        uint32_t fb = 0;
        if (drmModeAddFB(s_.drm_fd, s_.mode.hdisplay, s_.mode.vdisplay,
                         24, 32, stride, handle, &fb) != 0) return 0;
        fb_cache_[bo] = fb;
        return fb;
    }

    static void on_page_flip_static(int fd, unsigned int, unsigned int, unsigned int, void* data) {
        reinterpret_cast<PageFlipper*>(data)->on_page_flip(fd);
    }
    void on_page_flip(int) { waiting_ = false; }

    void wait_for_event() {
        fd_set fds; FD_ZERO(&fds); FD_SET(s_.drm_fd, &fds);
        if (select(s_.drm_fd + 1, &fds, nullptr, nullptr, nullptr) > 0) {
            drmHandleEvent(s_.drm_fd, &ev_);
        }
    }

    const DisplayState& s_;
    drmEventContext ev_{};
    std::unordered_map<gbm_bo*, uint32_t> fb_cache_;
    gbm_bo*  prev_bo_ = nullptr;
    uint32_t prev_fb_ = 0;
    bool first_ = true;
    volatile bool waiting_ = false;
};
