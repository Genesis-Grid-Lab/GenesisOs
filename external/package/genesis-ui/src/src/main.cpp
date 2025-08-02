#define GL_GLES2
#include "Config.h"
#include "Screen/PageFlipper.h"

#define CHECK(x, msg) if (!(x)) { std::cerr << msg << std::endl; cleanup(); return -1; }

using namespace std;

// Vertex Shader Source
const char* vertSource = R"(
attribute vec3 aPosition;

uniform mat4 uModelViewProjection;

void main()
{
    gl_Position = uModelViewProjection * vec4(aPosition, 1.0);
}
)";

// Fragment Shader Source
const char* fragSource = R"(
precision mediump float;

void main()
{
    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
)";

struct Vertex {
    GLfloat pos[3];
};

// Global for cleanup
int drm_fd = -1;
drmModeModeInfo mode;
uint32_t crtc_id;
drmModeCrtc *saved_crtc = nullptr;
gbm_device *gbm = nullptr;
gbm_surface *gbm_surf = nullptr;
EGLDisplay egl_display = EGL_NO_DISPLAY;
EGLSurface egl_surf = EGL_NO_SURFACE;
EGLContext egl_ctx = EGL_NO_CONTEXT;
uint32_t conn_id;
DisplayState dState;
PageFlipper flipper{dState};

void cleanup() {
    if (egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl_ctx != EGL_NO_CONTEXT) eglDestroyContext(egl_display, egl_ctx);
        if (egl_surf != EGL_NO_SURFACE) eglDestroySurface(egl_display, egl_surf);
        eglTerminate(egl_display);
    }
    if (gbm_surf) gbm_surface_destroy(gbm_surf);
    if (gbm) gbm_device_destroy(gbm);
    if (saved_crtc) {
        drmModeSetCrtc(drm_fd, saved_crtc->crtc_id, saved_crtc->buffer_id,
                       saved_crtc->x, saved_crtc->y, &conn_id, 1, &saved_crtc->mode);
        drmModeFreeCrtc(saved_crtc);
    }
    if (drm_fd >= 0) close(drm_fd);
}

int main()
{
    // Open DRM
    drm_fd = open("/dev/dri/card1", O_RDWR | O_CLOEXEC);
    if (drm_fd < 0) {
        cerr << "1 Unable to open render node: " << strerror(errno) << endl;
        return -1;
    }

    drmModeRes *resources = drmModeGetResources(drm_fd);
    CHECK(resources, "drmModeGetResources failed");

    drmModeConnector *conn = nullptr;
    for (int i = 0; i < resources->count_connectors; ++i) {
        conn = drmModeGetConnector(drm_fd, resources->connectors[i]);
        if (conn->connection == DRM_MODE_CONNECTED) {
            conn_id = conn->connector_id;
            break;
        }
        drmModeFreeConnector(conn);
        conn = nullptr;
    }
    CHECK(conn, "No connected connector found");

    CHECK(conn->count_modes > 0, "No valid modes");
    mode = conn->modes[0];

    // drmModeEncoder *enc = drmModeGetEncoder(drm_fd, conn->encoder_id);
    // CHECK(enc, "No encoder");
    // crtc_id = enc->crtc_id;
    drmModeEncoder *enc = NULL;
    for (int i = 0; i < conn->count_encoders; ++i) {
      enc = drmModeGetEncoder(drm_fd, conn->encoders[i]);
      if (enc && enc->crtc_id) {
        crtc_id = enc->crtc_id;
        break;
      }
      if (enc) {
        drmModeFreeEncoder(enc);
      }
    }

    if (crtc_id == 0) {
      fprintf(stderr, "No CRTC found for connector %u\n", conn->connector_id);
      // Handle gracefully...
    }
    
    CHECK(enc, "No encoder with CRTC found");

    saved_crtc = drmModeGetCrtc(drm_fd, crtc_id);

    // Create GBM device
    gbm = gbm_create_device(drm_fd);
    if (!gbm) {
        cerr << "Unable to create GBM device" << endl;
        close(drm_fd);
        return -1;
    }

    // Create GBM surface
    gbm_surf = gbm_surface_create(gbm, mode.hdisplay, mode.vdisplay, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);
    if (!gbm_surf) {
        cerr << "Unable to create GBM surface" << endl;
        gbm_device_destroy(gbm);
        close(drm_fd);
        return -1;
    }

    // Initialize EGL
    egl_display = eglGetPlatformDisplay(EGL_PLATFORM_GBM_KHR, gbm, NULL);
    if (egl_display == EGL_NO_DISPLAY) {
        cerr << "Unable to get EGL platform display" << endl;
        gbm_surface_destroy(gbm_surf);
        gbm_device_destroy(gbm);
        close(drm_fd);
        return -1;
    }

    if (!eglInitialize(egl_display, NULL, NULL)) {
        cerr << "Unable to initialize EGL" << endl;
        eglTerminate(egl_display);
        gbm_surface_destroy(gbm_surf);
        gbm_device_destroy(gbm);
        close(drm_fd);
        return -1;
    }

    std::cout << "EGL Version = " << eglQueryString(egl_display, EGL_VERSION) << "\n";
    std::cout << "EGL Vendor = " << eglQueryString(egl_display, EGL_VENDOR) << "\n";

    eglBindAPI(EGL_OPENGL_ES_API);

    // Choose config
    EGLint attr[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
	EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    EGLConfig config = NULL;
    EGLint num_configs = 0;

    if (!eglChooseConfig(egl_display, attr, &config, 1, &num_configs) ||
        num_configs == 0) {
        cerr << "Unable to choose EGL config" << endl;
        eglTerminate(egl_display);
        gbm_surface_destroy(gbm_surf);
        gbm_device_destroy(gbm);
        close(drm_fd);
        return -1;
    }

    // Create context
    EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext egl_ctx = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, contextAttributes);
    if (egl_ctx == EGL_NO_CONTEXT) {
        cerr << "Unable to create EGL context" << endl;
        eglTerminate(egl_display);
        gbm_surface_destroy(gbm_surf);
        gbm_device_destroy(gbm);
        close(drm_fd);
        return -1;
    }

    // Create surface
    egl_surf = eglCreateWindowSurface(egl_display, config, gbm_surf, NULL);
    if (egl_surf == EGL_NO_SURFACE) {
        cerr << "Unable to create EGL surface : " << eglGetError() << endl;
        eglTerminate(egl_display);
        gbm_surface_destroy(gbm_surf);
        gbm_device_destroy(gbm);
        close(drm_fd);
        return -1;
    }

    if (!eglMakeCurrent(egl_display, egl_surf, egl_surf, egl_ctx)) {
        cerr << "Unable to make context current" << endl;
        eglDestroySurface(egl_display, egl_surf);
        eglTerminate(egl_display);
        gbm_surface_destroy(gbm_surf);
        gbm_device_destroy(gbm);
        close(drm_fd);
        return -1;
    }

    // Compile shaders
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertSource, NULL);
    glCompileShader(vert);
    GLint compiled = 0;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        cerr << "Vertex shader failed to compile" << endl;
    }

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSource, NULL);
    glCompileShader(frag);
    glGetShaderiv(frag, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        cerr << "Fragment shader failed to compile" << endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        cerr << "Program linking failed" << endl;
    }

    glUseProgram(program);

    // Define a cube
    Vertex vertices[] = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}},
        {{ 0.5f, -0.5f,  0.5f}},
        {{ 0.5f,  0.5f,  0.5f}},
        {{-0.5f,  0.5f,  0.5f}},
        // Back face
        {{-0.5f, -0.5f, -0.5f}},
        {{ 0.5f, -0.5f, -0.5f}},
        {{ 0.5f,  0.5f, -0.5f}},
        {{-0.5f,  0.5f, -0.5f}},
    };
    GLushort indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        1, 2, 6, 6, 5, 1,
        2, 3, 7, 7, 6, 2,
        3, 0, 4, 4, 7, 3,
    };

    // VBOs
    GLuint vbo, ibo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLint pos = glGetAttribLocation(program, "aPosition");

    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    GLint mvp = glGetUniformLocation(program, "uModelViewProjection");

    glEnable(GL_DEPTH_TEST);
    glClearDepthf(1.0f);
    glDepthFunc(GL_LEQUAL);
    eglSwapInterval(egl_display, 1);

    // Loop
    float rotation = 0.0f;

    dState = {drm_fd, crtc_id, conn_id, mode, gbm, gbm_surf, egl_display, egl_surf};

    eglSwapInterval(dState.egl_display, 1);

    const float dt = 0.016f;

    for (;;) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);
      glClearDepthf(1.0f);
      
      rotation += dt;
      
      glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 100.0f);
      glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
      
      glm::mat4 model = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 1, 0));
      
      glm::mat4 mvpMat = projection * view * model;
      
      glUniformMatrix4fv(mvp, 1, GL_FALSE, &mvpMat[0][0]);
      
      glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
      eglSwapBuffers(dState.egl_display, dState.egl_surf);
      if (!flipper.present())
	break;
    }
    flipper.cleanup();

    cleanup();

    return 0;
}
