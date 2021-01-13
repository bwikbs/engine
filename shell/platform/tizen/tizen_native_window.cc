// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tizen_native_window.h"

#include <Elementary.h>
#include "flutter/shell/platform/tizen/tizen_log.h"

void LogLastEGLError() {
}

void TizenNativeWindow::ResizeWithRotation(int32_t dx, int32_t dy,
                                              int32_t width, int32_t height,
                                              int32_t degree) {
  // ecore_wl2_egl_window_resize_with_rotation(egl_window_, dx, dy, width, height,
  //                                           degree);
}

TizenNativeWindow::TizenNativeWindow(int32_t x, int32_t y, int32_t w,
                                     int32_t h) {
  if (w == 0 || h == 0) {
    FT_LOGE("Failed to create because of the wrong size");
    return;
  }
  elm_config_accel_preference_set("opengl");
 
  evas_window_ = elm_win_util_standard_add("", "");
  evas_object_move(evas_window_,x,y);
  evas_object_resize(evas_window_,w,h);
  evas_object_show(evas_window_);
  // ecore_wl2_window_type_set(evas_window_, ECORE_WL2_WINDOW_TYPE_TOPLEVEL);
  // ecore_wl2_window_alpha_set(evas_window_, EINA_FALSE);
  // ecore_wl2_window_aux_hint_add(evas_window_, 0, "wm.policy.win.user.geometry",
  //                               "1");
  // ecore_wl2_window_show(evas_window_);
  evas_gl_ = evas_gl_new(evas_object_evas_get(evas_window_));
  evas_glGlapi = evas_gl_api_get(evas_gl_);
  is_valid_ = true;
}

TizenNativeWindow::~TizenNativeWindow() {
  if (evas_window_) {
    evas_object_del(evas_window_);
    evas_gl_free(evas_gl_);
    evas_window_ = nullptr;
    evas_gl_ = nullptr;
  }
}

TizenNativeWindow::TizenNativeWindowGeometry TizenNativeWindow::GetGeometry() {
  TizenNativeWindowGeometry result;
  result.w=100; 
  result.h=100;
  // ecore_wl2_window_geometry_get(evas_window_, &result.x, &result.y, &result.w,
  //                               &result.h);
  return result;
}
