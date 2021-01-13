// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_TIZEN_SURFACE_GL_H_
#define EMBEDDER_TIZEN_SURFACE_GL_H_

#include <wayland-client.h>
#include <wayland-egl.h>

#include <algorithm>
#include <string>
#include <vector>
#define EFL_BETA_API_SUPPORT
#include <Ecore_Input.h>
#include <Ecore_Wl2.h>

#include "flutter/shell/platform/embedder/embedder.h"
#include "flutter/shell/platform/tizen/tizen_native_window.h"
#include "flutter/shell/platform/tizen/tizen_surface.h"

class TizenGLSurface {
 public:
  TizenGLSurface(std::shared_ptr<TizenNativeWindow> tizen_native_window, Evas_GL_Surface* gl_surface)
      : gl_surface_(gl_surface){};
  ~TizenGLSurface();
  bool IsValid() { return gl_surface_ != nullptr; }
  Evas_GL_Surface* GetGLSurfaceHandle() { return gl_surface_; };

 private:
  std::shared_ptr<TizenNativeWindow> tizen_native_window_;
  Evas_GL_Surface* gl_surface_{nullptr};
};

class TizenGLContext {
 public:
  TizenGLContext(
      std::shared_ptr<TizenNativeWindow> tizen_native_window);
  ~TizenGLContext();
  bool IsValid();
  std::unique_ptr<TizenGLSurface> CreateTizenGLWindowSurface();
  std::unique_ptr<TizenGLSurface> CreateTizenGLPbufferSurface();
  Evas_GL_Context*  GetGLContextHandle() { return gl_context_; }
  Evas_GL_Context*  GetGLResourceContextHandle() { return gl_resource_context_; }

 public:
  std::shared_ptr<TizenNativeWindow> tizen_native_window_;
  Evas_GL_Config* gl_config_; 
  Evas_GL_Context* gl_context_;
  Evas_GL_Context* gl_resource_context_;
};

class TizenSurfaceGL : public TizenSurface {
 public:
  TizenSurfaceGL(std::shared_ptr<TizenNativeWindow> tizen_native_window);
  ~TizenSurfaceGL();
  bool OnMakeCurrent() override;
  bool OnClearCurrent() override;
  bool OnMakeResourceCurrent() override;
  bool OnPresent() override;
  uint32_t OnGetFBO() override;
  void* OnProcResolver(const char* name) override;
  bool IsValid() override { return is_valid_; };

 private:
  bool is_valid_{false};
  std::shared_ptr<TizenNativeWindow> tizen_native_window_;
  std::unique_ptr<TizenGLContext> tizen_context_gl_;
  std::unique_ptr<TizenGLSurface> tizen_gl_window_surface_;
  std::unique_ptr<TizenGLSurface> tizen_gl_pbuffer_surface_;
};

#endif  // EMBEDDER_TIZEN_SURFACE_GL_H_
