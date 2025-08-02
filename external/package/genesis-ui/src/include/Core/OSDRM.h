#pragma once

#include "Config.h"

class OSDRM{
 public:
  bool Init(){
    // Open DRM
    drm_fd = open("/dev/dri/card1", O_RDWR | O_CLOEXEC);
    if (drm_fd < 0) {
      std::cerr << "Unable to open render node: " << strerror(errno) << std::endl;
      return -1;
    }
    
    m_Resources = drmModeGetResources(drm_fd);
    /* CHECK(resources, "drmModeGetResources failed"); */
    
    for (int i = 0; i < m_Resources->count_connectors; ++i) {
      m_Connector = drmModeGetConnector(drm_fd, m_Resources->connectors[i]);
      if (m_Connector->connection == DRM_MODE_CONNECTED) {
	conn_id = m_Connector->connector_id;
	break;
      }
      drmModeFreeConnector(m_Connector);
      m_Connector = nullptr;
    }
    /* CHECK(conn, "No connected connector found"); */
    
    /* CHECK(conn->count_modes > 0, "No valid modes"); */
    m_Mode = m_Connector->modes[0];
    
    // drmModeEncoder *enc = drmModeGetEncoder(drm_fd, conn->encoder_id);
    // CHECK(enc, "No encoder");
    // crtc_id = enc->crtc_id;
    
    for (int i = 0; i < m_Connector->count_encoders; ++i) {
      m_Encoder = drmModeGetEncoder(drm_fd, m_Connector->encoders[i]);
      if (m_Encoder && m_Encoder->crtc_id) {
        crtc_id = m_Encoder->crtc_id;
        break;
      }
      if (m_Encoder) {
        drmModeFreeEncoder(m_Encoder);
      }
    }
    
    if (crtc_id == 0) {
      fprintf(stderr, "No CRTC found for connector %u\n", m_Connector->connector_id);
      // Handle gracefully...
    }
    
    /* CHECK(enc, "No encoder with CRTC found"); */
    
    m_Crtc = drmModeGetCrtc(drm_fd, crtc_id);

    return 0;
    
  }
  void render(){
    
  }
  void Cleanup(){
    
  }

  int GetDrmFd() {
    return drm_fd;
  }

  drmModeModeInfo& GetMode() { return m_Mode;}

  int GetCrtcID() { return crtc_id;}
 private:
  drmModeModeInfo m_Mode;
  drmModeCrtc *m_Crtc = nullptr;
  drmModeRes *m_Resources = nullptr;
  drmModeConnector *m_Connector = nullptr;
  drmModeEncoder *m_Encoder = nullptr;
  int drm_fd = -1;
  uint32_t crtc_id;
  uint32_t conn_id;
};
