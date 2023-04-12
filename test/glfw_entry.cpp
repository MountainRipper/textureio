#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/egl.h>
#include <glad/gl.h>
#if  defined(__linux__)
#include <glad/glx.h>
#elif defined(WIN32) || defined(WIN64)
#include <glad/wgl.h>
#endif

#include <libavutil/pixfmt.h>
#include <logger.h>
#include <ttf/ttf_notosans_sc_level_1s.h>
#include <ttf/ttf_notosans_sc_level_1.h>
#include <ttf/ttf_notosans_sc_level_1_2.h>
#include <ttf/ttf_material_icon.h>
#include <ttf/ttf_font_awesome_solid.h>
#include <ttf/IconsMaterialDesign.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <mr/imgui_mr.h>
#include "textureio_example.h"


#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

/* -------------------------------------------- */

#if defined(__linux)
#define USE_GL_LINUX USE_GL
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#define GLFW_EXPOSE_NATIVE_EGL
#endif

#if defined(_WIN32)
#define USE_GL_WIN USE_GL
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#if defined(__linux)
#include <unistd.h>
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

using namespace mr;

uint32_t win_w = 1280;
uint32_t win_h = 720;
bool show_demo_window_ = false;
ExampleBase *g_example = nullptr;

void key_callback(GLFWwindow *win, int key, int scancode, int action,
                  int mods) {

  if (GLFW_RELEASE == action) {
    return;
  }

  switch (key) {
  case GLFW_KEY_ESCAPE: {
    glfwSetWindowShouldClose(win, GL_TRUE);
    break;
  }
  case GLFW_KEY_B: {
    show_demo_window_ = !show_demo_window_;
    break;
  }
  };

  g_example->key_callback(key,scancode,action,mods);
}

void resize_callback(GLFWwindow *window, int width, int height) {
  win_w = width;
  win_h = height;
  g_example->resize_callback(width, height);
}
void cursor_callback(GLFWwindow *window, double x, double y) {
  g_example->cursor_callback(x, y);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  g_example->scroll_callback(xoffset, yoffset);
}
void button_callback(GLFWwindow *window, int bt, int action, int mods) {
  g_example->button_callback(bt, action, mods);
}
void char_callback(GLFWwindow *window, unsigned int key) {
  g_example->char_callback(key);
}
void error_callback(int err, const char *desc) {
  printf("GLFW error: %s (%d)\n", desc, err);
}

int main(int argc, char *argv[]) {

  GLFWwindow *window = NULL;
  void *native_display = nullptr;
  void *native_window = nullptr;
  void *opengl_context = nullptr;
  void *native_window_glx = nullptr;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    printf("Error: cannot setup glfw.\n");
    exit(EXIT_FAILURE);
  }
#ifdef USE_EGL
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#endif

  bool use_gles = false;
#if USE_GLES
  const char* glsl_version = "#version 300 es";
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  use_gles = true;
#else
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

  glfwWindowHint(GLFW_MAXIMIZED, true);
  window = glfwCreateWindow(win_w, win_h, "SDMP Usecase Test", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetFramebufferSizeCallback(window, resize_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCharCallback(window, char_callback);
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetMouseButtonCallback(window, button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwMakeContextCurrent(window);

  if (!gladLoaderLoadGL()) {
    printf("Cannot load GL.\n");
    exit(1);
  }

  EGLDisplay mEGLDisplay = nullptr;
#if defined(__linux)
    #ifdef USE_EGL
      native_window = (void *)glfwGetX11Window(win);
      native_display = (void *)glfwGetEGLDisplay();
      opengl_context = (void *)glfwGetEGLContext(win);

      if (!gladLoaderLoadEGL(native_display)) {
        printf("Cannot load GL.\n");
        exit(1);
      }

      if (eglGetCurrentContext()) {
        auto display = glfwGetEGLDisplay();
        fprintf(stderr, "EGL_VERSION : %s\n", eglQueryString(display, EGL_VERSION));
        fprintf(stderr, "EGL_VENDOR : %s\n", eglQueryString(display, EGL_VENDOR));
      }

    #else
      native_window = (void *)glfwGetX11Window(window);
      native_window_glx = (void *)glfwGetGLXWindow(window);
      native_display = (void *)glfwGetX11Display();
      opengl_context = (void *)glfwGetGLXContext(window);

      if (!gladLoaderLoadGLX((Display*)native_display,0)) {
        printf("Cannot load GL.\n");
        exit(1);
      }

      if (glXGetCurrentContext()) {
        auto display = glXGetCurrentDisplay();
        int majv, minv;
        glXQueryVersion(display, &majv, &minv);
        fprintf(stderr, "GLX VERSION : %d.%d\n", majv, minv);
      }
    #endif
#endif

#if defined(_WIN32) || defined(_WIN64)
  native_window = (void *)glfwGetWin32Window(window);
  opengl_context = (void *)glfwGetWGLContext(window);
#endif

  auto video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  auto global_scale  = video_mode->width / 1920.0;

  glfwSwapInterval(1);

  fprintf(stderr, "GL_VENDOR : %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "GL_VERSION  : %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "GL_RENDERER : %s\n", glGetString(GL_RENDERER));

  // fprintf(stderr,"GL_EXTENSIONS : %s\n", glGetString(GL_EXTENSIONS) );

  /* -------------------------------------------- */
  // glEnable(GL_TEXTURE_2D);
  // VGFX_GL_CHECK(mp::Logger::kLogLevelError,"glEnable(GL_TEXTURE_2D)");
  glDisable(GL_DEPTH_TEST);
  VGFX_GL_CHECK(mp::Logger::kLogLevelError, "glDisable(GL_DEPTH_TEST)");
  glDisable(GL_DITHER);
  VGFX_GL_CHECK(mp::Logger::kLogLevelError, "glDisable(GL_DITHER)");
  glEnable(GL_BLEND);
  VGFX_GL_CHECK(mp::Logger::kLogLevelError, "glEnable(GL_BLEND)");
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  VGFX_GL_CHECK(mp::Logger::kLogLevelError,
                "glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)");
  /* -------------------------------------------- */

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  float baseFontSize = 17.0f * global_scale;
  ImFontConfig icons_config;
  icons_config.PixelSnapH = true;
  icons_config.OversampleH = 1;
  static const ImWchar icons_ranges[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };

  ImguiFontResource font_noto_sc{"",notosans_sc_level_1_compressed_data_base85,nullptr,0,false,1,io.Fonts->GetGlyphRangesChineseFull(),icons_config};
  icons_config.GlyphMinAdvanceX = baseFontSize;
  ImguiFontResource font_awesome{"",font_awesome_solid_compressed_data_base85,nullptr,0,false,0.8,icons_ranges,icons_config};
  std::vector<ImguiFontResource> fonts{font_noto_sc,font_awesome};
  auto& font_helper = ImGuiHelper::get();
  font_helper.create_default_font(baseFontSize,fonts);
  font_helper.build();
  /* -------------------------------------------- */

  g_example = new TextureioExample();
  g_example->on_init(window);
  g_example->resize_callback(win_w, win_h);
  while (!glfwWindowShouldClose(window)) {

   // glfwMakeContextCurrent(win);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, win_w, win_h);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "glViewport()");
    glClearColor(.5, .75, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "ImGui_ImplOpenGL3_NewFrame()");
    ImGui_ImplGlfw_NewFrame();
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "ImGui_ImplGlfw_NewFrame()");
    ImGui::NewFrame();
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "ImGui::NewFrame()");

    g_example->on_frame();
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "ImGui::Render()");

    if(show_demo_window_)
        ImGui::ShowDemoWindow(&show_demo_window_);

    ImGui::Render();
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "ImGui::Render()");

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "ImGui::GetDrawData()");
#if defined(__linux)
      //usleep(100*1000);
#endif

    glfwSwapBuffers(window);
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "glfwSwapBuffers()");
    glfwPollEvents();
    VGFX_GL_CHECK(mp::Logger::kLogLevelError, "glfwPollEvents()");
  }

  g_example->on_deinit();
  delete g_example;

  glfwTerminate();

  return EXIT_SUCCESS;
}
