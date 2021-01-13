// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tizen_native_window.h"

#include <Elementary.h>
#include "flutter/shell/platform/tizen/tizen_log.h"

static Evas_GL* kEvasGl = nullptr;
static Evas_GL_API* kEvasGLApi = nullptr;

void LogLastEGLError() {}

void TizenNativeWindow::ResizeWithRotation(int32_t dx, int32_t dy,
                                           int32_t width, int32_t height,
                                           int32_t degree) {
  evas_object_resize(evas_window_, width, height);
  // needs to rotation.

}

TizenNativeWindow::TizenNativeWindow(int32_t x, int32_t y, int32_t w,
                                     int32_t h) {
  if (w == 0 || h == 0) {
    FT_LOGE("Failed to create because of the wrong size");
    return;
  }
  elm_config_accel_preference_set("opengl");

  evas_window_ = elm_win_add(NULL, NULL, ELM_WIN_BASIC);
  elm_win_alpha_set(evas_window_, EINA_FALSE);
  elm_win_aux_hint_add(evas_window_, "wm.policy.win.user.geometry", "1");
  evas_object_move(evas_window_, x, y);
  evas_object_resize(evas_window_, w, h);
  evas_object_raise(evas_window_);

  gl_config_ = evas_gl_config_new();
  gl_config_->color_format = EVAS_GL_RGBA_8888;
  gl_config_->depth_bits = EVAS_GL_DEPTH_BIT_24;
  gl_config_->stencil_bits = EVAS_GL_STENCIL_NONE;
  gl_config_->options_bits = EVAS_GL_OPTIONS_NONE;

#define EVAS_GL_OPTIONS_DIRECT_MEMORY_OPTIMIZE (1 << 12)
#define EVAS_GL_OPTIONS_DIRECT_OVERRIDE (1 << 13)
  // gl_config_->options_bits = (Evas_GL_Options_Bits)(
  //     EVAS_GL_OPTIONS_DIRECT | EVAS_GL_OPTIONS_DIRECT_OVERRIDE |
  //     EVAS_GL_OPTIONS_DIRECT_MEMORY_OPTIMIZE |
  //     EVAS_GL_OPTIONS_CLIENT_SIDE_ROTATION);

  if (!gl_config_) {
    FT_LOGE("Failed to ChooseGLConfiguration");
    return;
  }

  evas_gl_ = evas_gl_new(evas_object_evas_get(evas_window_));
  evas_glGlapi = evas_gl_api_get(evas_gl_);

  kEvasGl = evas_gl_;
  kEvasGLApi = evas_glGlapi;

  Evas_Object* bg = elm_bg_add(evas_window_);
  evas_object_color_set(bg, 0x00, 0x00, 0x00, 0x00);

  evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(evas_window_, bg);

  graphicsAdapter_ =
      evas_object_image_filled_add(evas_object_evas_get(evas_window_));
  evas_object_resize(graphicsAdapter_, w, h);
  evas_object_move(graphicsAdapter_, x, y);
  evas_object_image_size_set(graphicsAdapter_, w, h);
  evas_object_image_alpha_set(graphicsAdapter_, EINA_TRUE);
  elm_win_resize_object_add(evas_window_, graphicsAdapter_);

  evas_object_show(evas_window_);
  is_valid_ = true;
  {
    #include <pthread.h>
    pthread_t id;
    id = pthread_self();
    FT_LOGE("[MONG] TizenNativeWindow Thread ID %u",(int)id);
  }

}

TizenNativeWindow::~TizenNativeWindow() {
  if (evas_window_) {
    evas_object_del(evas_window_);
    evas_gl_config_free(gl_config_);
    evas_gl_free(evas_gl_);

    evas_window_ = nullptr;
    evas_gl_ = nullptr;
    evas_glGlapi = nullptr;
    gl_config_ = nullptr;
  }
}

TizenNativeWindow::TizenNativeWindowGeometry TizenNativeWindow::GetGeometry() {
  TizenNativeWindowGeometry result;
  evas_object_geometry_get(evas_window_, &result.x, &result.y, &result.w,
                           &result.h);
  return result;
}
