// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tizen_surface_gl.h"

#include <EGL/egl.h>
#include <Ecore.h>
#include <Elementary.h>
#include <Evas_GL.h>

#include "flutter/shell/platform/tizen/tizen_log.h"

TizenGLSurface::~TizenGLSurface() { gl_surface_ = nullptr; }

TizenGLContext::TizenGLContext(
    std::shared_ptr<TizenNativeWindow> tizen_native_window)
    : tizen_native_window_(tizen_native_window) {
  gl_context_ = evas_gl_context_version_create(
      tizen_native_window_->GetEvasGLHandle(), NULL, EVAS_GL_GLES_2_X);

  if (!gl_context_) {
    FT_LOGE("Failed to create gl context");
    return;
  }
}

TizenGLContext::~TizenGLContext() {
  evas_gl_context_destroy(tizen_native_window_->GetEvasGLHandle(), gl_context_);
  tizen_native_window_ = nullptr;
  gl_context_ = nullptr;
}

bool TizenGLContext::IsValid() {
  return tizen_native_window_ && tizen_native_window_->IsValid() &&
         gl_context_ != nullptr;
}

std::unique_ptr<TizenGLSurface> TizenSurfaceGL::CreateTizenGLWindowSurface() {
  auto surface = evas_gl_surface_create(tizen_native_window_->GetEvasGLHandle(),
                                        tizen_native_window_->GetEvasGLConfig(),
                                        tizen_native_window_->GetGeometry().w,
                                        tizen_native_window_->GetGeometry().h);
  return std::make_unique<TizenGLSurface>(surface);
}

std::unique_ptr<TizenGLSurface> TizenSurfaceGL::CreateTizenGLPbufferSurface() {
  auto surface = evas_gl_pbuffer_surface_create(
      tizen_native_window_->GetEvasGLHandle(),
      tizen_native_window_->GetEvasGLConfig(),
      tizen_native_window_->GetGeometry().w,
      tizen_native_window_->GetGeometry().h, NULL);
  return std::make_unique<TizenGLSurface>(surface);
}

TizenSurfaceGL::TizenSurfaceGL(
    std::shared_ptr<TizenNativeWindow> tizen_native_window)
    : tizen_native_window_(tizen_native_window) {
  if (!tizen_native_window_->IsValid()) {
    FT_LOGE("Invalid native window");
    return;
  }

  tizen_context_gl_ = std::make_unique<TizenGLContext>(tizen_native_window_);
  if (!tizen_context_gl_->IsValid()) {
    FT_LOGE("Invalid context tizen_context_gl_");
    return;
  }

  tizen_context_gl_resource_ =
      std::make_unique<TizenGLContext>(tizen_native_window_);
  if (!tizen_context_gl_resource_->IsValid()) {
    FT_LOGE("Invalid context tizen_context_gl_resource_");
    return;
  }

  tizen_gl_window_surface_ = CreateTizenGLWindowSurface();
  if (!tizen_gl_window_surface_->IsValid()) {
    FT_LOGE("Invalid gl window surface");
    return;
  }

  tizen_gl_pbuffer_surface_ = CreateTizenGLPbufferSurface();
  if (!tizen_gl_pbuffer_surface_->IsValid()) {
    FT_LOGE("Invalid gl resource surface");
    return;
  }

  Evas_Native_Surface ns;
  evas_gl_native_surface_get(tizen_native_window_->GetEvasGLHandle(),
                             tizen_gl_window_surface_->GetGLSurfaceHandle(),
                             &ns);
  evas_object_image_native_surface_set(tizen_native_window_->GetImageHandle(),
                                       &ns);
  evas_object_show(tizen_native_window_->GetImageHandle());

  pixelDirtyCallback_ = [](void* data, Evas_Object* o) {};
  evas_object_image_pixels_get_callback_set(
      tizen_native_window_->GetImageHandle(), pixelDirtyCallback_, NULL);
  is_valid_ = true;
}

bool TizenSurfaceGL::OnMakeCurrent() {
  if (!is_valid_) {
    FT_LOGE("Invalid TizenSurfaceGL");
    return false;
  }
  evas_gl_make_current(tizen_native_window_->GetEvasGLHandle(),
                       tizen_gl_window_surface_->GetGLSurfaceHandle(),
                       tizen_context_gl_->GetGLContextHandle());

  evas_object_image_pixels_dirty_set(tizen_native_window_->GetImageHandle(),
                                     EINA_TRUE);
  return true;
}

bool TizenSurfaceGL::OnMakeResourceCurrent() {
  if (!is_valid_) {
    FT_LOGE("Invalid TizenSurfaceGL");
    return false;
  }

  FT_LOGE("[MONG] OnMakeResourceCurrent...");
  evas_gl_make_current(tizen_native_window_->GetEvasGLHandle(),
                       tizen_gl_pbuffer_surface_->GetGLSurfaceHandle(),
                       tizen_context_gl_resource_->GetGLContextHandle());
  return true;
}

bool TizenSurfaceGL::OnClearCurrent() {
  if (!is_valid_) {
    FT_LOGE("Invalid TizenSurfaceGL");
    return false;
  }
  evas_gl_make_current(tizen_native_window_->GetEvasGLHandle(), NULL, NULL);
  return true;
}

bool TizenSurfaceGL::OnPresent() { return true; }

uint32_t TizenSurfaceGL::OnGetFBO() {
  if (!is_valid_) {
    FT_LOGE("Invalid TizenSurfaceGL");
    return 999;
  }
  FT_LOGD("OnApplicationGetOnscreenFBO");
  return 0;  // FBO0
}

#define GL_FUNC(FunctionName)                                      \
  else if (strcmp(name, #FunctionName) == 0) {                     \
    return reinterpret_cast<void*>(                                \
        tizen_native_window_->GetEvasGLApiHandle()->FunctionName); \
  }

void* TizenSurfaceGL::OnProcResolver(const char* name) {
  auto address =
      evas_gl_proc_address_get(tizen_native_window_->GetEvasGLHandle(), name);
  if (address != nullptr) {
    return reinterpret_cast<void*>(address);
  }
  GL_FUNC(glActiveTexture)
  GL_FUNC(glAttachShader)
  GL_FUNC(glBindAttribLocation)
  GL_FUNC(glBindBuffer)
  GL_FUNC(glBindFramebuffer)
  GL_FUNC(glBindRenderbuffer)
  GL_FUNC(glBindTexture)
  GL_FUNC(glBlendColor)
  GL_FUNC(glBlendEquation)
  GL_FUNC(glBlendFunc)
  GL_FUNC(glBufferData)
  GL_FUNC(glBufferSubData)
  GL_FUNC(glCheckFramebufferStatus)
  GL_FUNC(glClear)
  GL_FUNC(glClearColor)
  GL_FUNC(glClearStencil)
  GL_FUNC(glColorMask)
  GL_FUNC(glCompileShader)
  GL_FUNC(glCompressedTexImage2D)
  GL_FUNC(glCompressedTexSubImage2D)
  GL_FUNC(glCopyTexSubImage2D)
  GL_FUNC(glCreateProgram)
  GL_FUNC(glCreateShader)
  GL_FUNC(glCullFace)
  GL_FUNC(glDeleteBuffers)
  GL_FUNC(glDeleteFramebuffers)
  GL_FUNC(glDeleteProgram)
  GL_FUNC(glDeleteRenderbuffers)
  GL_FUNC(glDeleteShader)
  GL_FUNC(glDeleteTextures)
  GL_FUNC(glDepthMask)
  GL_FUNC(glDisable)
  GL_FUNC(glDisableVertexAttribArray)
  GL_FUNC(glDrawArrays)
  GL_FUNC(glDrawElements)
  GL_FUNC(glEnable)
  GL_FUNC(glEnableVertexAttribArray)
  GL_FUNC(glFinish)
  GL_FUNC(glFlush)
  GL_FUNC(glFramebufferRenderbuffer)
  GL_FUNC(glFramebufferTexture2D)
  GL_FUNC(glFrontFace)
  GL_FUNC(glGenBuffers)
  GL_FUNC(glGenerateMipmap)
  GL_FUNC(glGenFramebuffers)
  GL_FUNC(glGenRenderbuffers)
  GL_FUNC(glGenTextures)
  GL_FUNC(glGetBufferParameteriv)
  GL_FUNC(glGetError)
  GL_FUNC(glGetFramebufferAttachmentParameteriv)
  GL_FUNC(glGetIntegerv)
  GL_FUNC(glGetProgramInfoLog)
  GL_FUNC(glGetProgramiv)
  GL_FUNC(glGetRenderbufferParameteriv)
  GL_FUNC(glGetShaderInfoLog)
  GL_FUNC(glGetShaderiv)
  GL_FUNC(glGetShaderPrecisionFormat)
  GL_FUNC(glGetString)
  GL_FUNC(glGetUniformLocation)
  GL_FUNC(glIsTexture)
  GL_FUNC(glLineWidth)
  GL_FUNC(glLinkProgram)
  GL_FUNC(glPixelStorei)
  GL_FUNC(glReadPixels)
  GL_FUNC(glRenderbufferStorage)
  GL_FUNC(glScissor)
  GL_FUNC(glShaderSource)
  GL_FUNC(glStencilFunc)
  GL_FUNC(glStencilFuncSeparate)
  GL_FUNC(glStencilMask)
  GL_FUNC(glStencilMaskSeparate)
  GL_FUNC(glStencilOp)
  GL_FUNC(glStencilOpSeparate)
  GL_FUNC(glTexImage2D)
  GL_FUNC(glTexParameterf)
  GL_FUNC(glTexParameterfv)
  GL_FUNC(glTexParameteri)
  GL_FUNC(glTexParameteriv)
  GL_FUNC(glTexSubImage2D)
  GL_FUNC(glUniform1f)
  GL_FUNC(glUniform1fv)
  GL_FUNC(glUniform1i)
  GL_FUNC(glUniform1iv)
  GL_FUNC(glUniform2f)
  GL_FUNC(glUniform2fv)
  GL_FUNC(glUniform2i)
  GL_FUNC(glUniform2iv)
  GL_FUNC(glUniform3f)
  GL_FUNC(glUniform3fv)
  GL_FUNC(glUniform3i)
  GL_FUNC(glUniform3iv)
  GL_FUNC(glUniform4f)
  GL_FUNC(glUniform4fv)
  GL_FUNC(glUniform4i)
  GL_FUNC(glUniform4iv)
  GL_FUNC(glUniformMatrix2fv)
  GL_FUNC(glUniformMatrix3fv)
  GL_FUNC(glUniformMatrix4fv)
  GL_FUNC(glUseProgram)
  GL_FUNC(glVertexAttrib1f)
  GL_FUNC(glVertexAttrib2fv)
  GL_FUNC(glVertexAttrib3fv)
  GL_FUNC(glVertexAttrib4fv)
  GL_FUNC(glVertexAttribPointer)
  GL_FUNC(glViewport)
  GL_FUNC(glGetStringi)
  else if (strcmp(name, "eglGetCurrentDisplay") == 0) {
    return reinterpret_cast<void*>(eglGetCurrentDisplay);
  }
  else if (strcmp(name, "eglQueryString") == 0) {
    return reinterpret_cast<void*>(eglQueryString);
  }
  FT_LOGD("Could not resolve: %s", name);
  return nullptr;
}
#undef GL_FUNC

TizenSurfaceGL::~TizenSurfaceGL() {
  OnClearCurrent();
  tizen_gl_window_surface_ = nullptr;
  tizen_gl_pbuffer_surface_ = nullptr;
  tizen_context_gl_ = nullptr;
  tizen_native_window_ = nullptr;
}
