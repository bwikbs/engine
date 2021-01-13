// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_TIZEN_WINDOW_H_
#define EMBEDDER_TIZEN_WINDOW_H_

#undef EFL_BETA_API_SUPPORT
// #ifndef EFL_BETA_API_SUPPORT
// #define EFL_BETA_API_SUPPORT
// #endif
#include <Elementary.h>
#include <Evas_GL.h>
#include <memory>

void LogLastEGLError();

class TizenNativeWindow {
 public:
  struct TizenNativeWindowGeometry {
    int32_t x{0}, y{0}, w{0}, h{0};
  };

  TizenNativeWindow(int32_t x, int32_t y, int32_t w, int32_t h);
  ~TizenNativeWindow();
  bool IsValid() { return is_valid_; }
  Evas_Object* GetWindowHandle() { return evas_window_; }
  Evas_GL* GetEvasGLHandle() { return evas_gl_; }
  Evas_GL_API* GetEvasGLApiHandle() { return evas_glGlapi; }
  TizenNativeWindowGeometry GetGeometry();
  void ResizeWithRotation(int32_t dx, int32_t dy, int32_t width, int32_t height,
                          int32_t degree);

 private:
  Evas_Object* evas_window_{nullptr};
  Evas_GL* evas_gl_{nullptr};
  Evas_GL_API* evas_glGlapi{nullptr};;
  bool is_valid_{false};
};

#endif
