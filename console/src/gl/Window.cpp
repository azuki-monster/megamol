/*
 * Window.cpp
 *
 * Copyright (C) 2008, 2016 MegaMol Team
 * Alle Rechte vorbehalten.
 */

#include "stdafx.h"
#include "gl/Window.h"
#include "utility/HotFixes.h"
#include "vislib/sys/Log.h"
#include "WindowManager.h"
#include <cassert>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#ifdef _WIN32
#ifndef USE_EGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#endif
#endif

//#include "HotKeyButtonParam.h"
//#include "vislib/RawStorage.h"
//#include "vislib/types.h"
//#include "WindowManager.h"
//#include <chrono>
//#include <vector>
//#include <algorithm>

using namespace megamol;
using namespace megamol::console;

#ifndef USE_EGL
gl::Window::Window(const char* titlePrefix, const char* instance, void* hCore,
    const utility::WindowPlacement& placement, GLFWwindow* share)
        : glfw(), 
#else
gl::Window::Window(const char* titlePrefix, const char* instance, void* hCore,
    const utility::WindowPlacement& placement, EGLContext* share)
        : 
#endif
        hView(), hWnd(nullptr), width(-1), height(-1), renderContext(), uiLayers(), mouseCapture(),
        name(std::string(titlePrefix) + std::string(instance)), fpsCntr(), fps(1000.0f), fpsList(), showFpsInTitle(true), fpsSyncTime(), topMost(false),
        fragmentQuery(0), showFragmentsInTitle(false), showPrimsInTitle(false), associatedInstance(instance), hCore(hCore) {

    if (::memcmp(name.c_str(), WindowManager::TitlePrefix, WindowManager::TitlePrefixLength) == 0) {
        name = name.substr(WindowManager::TitlePrefixLength);
    }
    for (float& f : fpsList) f = 0.0f;

    memset(&renderContext, 0, sizeof(mmcRenderViewContext));
    renderContext.Size = sizeof(mmcRenderViewContext);
    renderContext.ContinuousRedraw = true;
    renderContext.GpuAffinity = nullptr;
    renderContext.Direct3DRenderTarget = nullptr;
    renderContext.InstanceTime = 0.0; // will be generated by core
    renderContext.Time = 0.0; // will be generated by core

#ifndef USE_EGL
    glfw = glfwInst::Instance(); // we use glfw
    if (glfw->OK()) {
        if (utility::HotFixes::Instance().IsHotFixed("usealphabuffer")) {
            ::glfwWindowHint(GLFW_ALPHA_BITS, 8);
        }

//#ifndef NOWINDOWPOSFIX
//        if (wndX != predictedX || wndY != predictedY ||
//            wndW != predictedWidth || wndH != predictedHeight) {
//            Log::DefaultLog.WriteMsg(Log::LEVEL_WARN, "The actual "
//                "view window location reported by the core (%d, %d), "
//                "size (%d, %d) is "
//                "different from the one predicted. GPU affinity "
//                "may have been set incorrectly.", wndX, wndY, wndW,
//                wndH);
//        }
//#endif

        this->topMost = placement.topMost;
        
        if (!placement.fullScreen) {
            // window mode
            ::glfwWindowHint(GLFW_DECORATED, placement.noDec ? GL_FALSE : GL_TRUE);
            ::glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

            int w = placement.w;
            int h = placement.h;
            if (!placement.size || (w <= 0) || (h <= 0)) {
                GLFWmonitor* primary = ::glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = ::glfwGetVideoMode(primary);
                w = mode->width * 3 / 4;
                h = mode->height * 3 / 4;
            }

            hWnd = ::glfwCreateWindow(w, h, (std::string(titlePrefix) + std::string(associatedInstance)).c_str(), nullptr, share);
            vislib::sys::Log::DefaultLog.WriteInfo("Console::Window: Create window with size w: %d, h: %d\n", w, h);
            if (hWnd != nullptr) {
                if (placement.pos) ::glfwSetWindowPos(hWnd, placement.x, placement.y);
            }

        } else {
            // fullscreen mode
            int monCnt = 0;
            GLFWmonitor **mons = ::glfwGetMonitors(&monCnt);
            GLFWmonitor *mon = mons[std::min<int>(monCnt - 1, placement.mon)];
            const GLFWvidmode* mode = glfwGetVideoMode(mon);

            if (placement.pos) vislib::sys::Log::DefaultLog.WriteWarn("Ignoring window placement position when requesting fullscreen.");
            if (placement.size) {
                if ((placement.w != mode->width) || (placement.h != mode->height)) {
                    vislib::sys::Log::DefaultLog.WriteWarn("Changing screen resolution is currently not supported.");
                }
            }
            if (placement.noDec) vislib::sys::Log::DefaultLog.WriteWarn("Ignoring no-decorations setting when requesting fullscreen.");

            ::glfwWindowHint(GLFW_DECORATED, GL_FALSE);
            ::glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
            ::glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            ::glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            ::glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            ::glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
            // this only works since we are NOT setting a monitor
            ::glfwWindowHint(GLFW_FLOATING, GL_TRUE);

            /* note we do not use a real fullscrene mode, since then we would have focus-iconify problems */
            hWnd = ::glfwCreateWindow(mode->width, mode->height,
                (std::string(titlePrefix) + std::string(associatedInstance)).c_str(), nullptr, share);
            vislib::sys::Log::DefaultLog.WriteInfo("Console::Window: Create window with size w: %d, h: %d\n", mode->width, mode->height);
            int x, y;
            ::glfwGetMonitorPos(mon, &x, &y);
            ::glfwSetWindowPos(hWnd, x, y);
        }

        if (hWnd != nullptr) {
            ::glfwSetWindowUserPointer(hWnd, this); // this is ok, as long as no one derives from Window at this point
            ::glfwShowWindow(hWnd);
            ::glfwMakeContextCurrent(hWnd);
            if ((placement.fullScreen || placement.noDec) && (!utility::HotFixes::Instance().IsHotFixed("DontHideCursor"))) {
                ::glfwSetInputMode(hWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            vislib::graphics::gl::LoadAllGL();
            ::glfwSetKeyCallback(hWnd, &Window::glfw_onKey_func);
            ::glfwSetMouseButtonCallback(hWnd, &Window::glfw_onMouseButton_func);
            ::glfwSetCursorPosCallback(hWnd, &Window::glfw_onMouseMove_func);
            ::glfwSetScrollCallback(hWnd, &Window::glfw_onMouseWheel_func);
            ::glfwSetCharCallback(hWnd, &Window::glfw_onChar_func);
        }
    }
#else
    // TODO initialize EGL display, context etc.
    // TODO error checking!!!
    //printf("0\n");
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == EGL_NO_DISPLAY)
        vislib::sys::Log::DefaultLog.WriteError("EGL_NO_DISPLAY");
    //printf("1\n");

    EGLint major, minor;
    EGLBoolean retval = eglInitialize(eglDisplay, &major, &minor);
    if (retval == EGL_FALSE)
        vislib::sys::Log::DefaultLog.WriteError("eglInitialize failed.");
    //printf("2 OGL%i.%i\n", major, minor);

    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    EGLint numConfigs;
    EGLConfig config;
    retval = eglChooseConfig(eglDisplay, configAttribs, &config, 1, &numConfigs);
    if (retval == EGL_FALSE)
        vislib::sys::Log::DefaultLog.WriteError("eglChooseConfig failed.");
    //printf("3\n");

    int w = placement.w;
    int h = placement.h;
    if (!placement.size || (w <= 0) || (h <= 0)) {
        w = 800;
        h = 600;
    }

    const EGLint pbufferAttribs[] = {
        EGL_WIDTH, w,
        EGL_HEIGHT, h,
        EGL_NONE,
    };

    eglSurface = eglCreatePbufferSurface(eglDisplay, config, pbufferAttribs);
    if (eglSurface == EGL_NO_SURFACE)
        vislib::sys::Log::DefaultLog.WriteError("EGL_NO_SURFACE");
    //printf("4\n");

    retval = eglBindAPI(EGL_OPENGL_API);
    if (retval == EGL_FALSE)
        vislib::sys::Log::DefaultLog.WriteError("eglBindAPI failed.");
    auto context = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, NULL);
    if (context == EGL_NO_CONTEXT)
        vislib::sys::Log::DefaultLog.WriteError("EGL_NO_CONTEXT");
    //printf("5\n");
    hWnd = &context;
    retval = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, *hWnd);
    if (retval == EGL_FALSE)
        vislib::sys::Log::DefaultLog.WriteError("eglMakeCurrent failed.");
    //printf("6\n");
    
    vislib::graphics::gl::LoadAllGL();
    vislib::sys::Log::DefaultLog.WriteInfo("Successfully created EGL context.");
#endif
    glGenQueries(1, &fragmentQuery);
    glGenQueries(1, &primsQuery);
    fpsSyncTime = std::chrono::system_clock::now();
}

gl::Window::~Window() {
    assert(hWnd == nullptr);
    glDeleteQueries(1, &fragmentQuery);
    glDeleteQueries(1, &primsQuery);
}

void gl::Window::EnableVSync() {
#ifndef USE_EGL
    if (hWnd != nullptr) {
        ::glfwMakeContextCurrent(hWnd);
        ::glfwSwapInterval(0);
    }
#else
    eglMakeCurrent( eglDisplay, eglSurface, eglSurface, *hWnd);
    eglSwapInterval( eglDisplay, 0);
#endif
}

void gl::Window::AddUILayer(std::shared_ptr<AbstractUILayer> uiLayer) {
    auto it = std::find(uiLayers.begin(), uiLayers.end(), uiLayer);
    if (it != uiLayers.end()) {
        vislib::sys::Log::DefaultLog.WriteWarn("uiLayer already part of the window");
        return;
    }
    uiLayers.push_back(uiLayer);
}

void gl::Window::RemoveUILayer(std::shared_ptr<AbstractUILayer> uiLayer) {
    auto it = std::find(uiLayers.begin(), uiLayers.end(), uiLayer);
    if (it == uiLayers.end()) return;
    uiLayers.erase(it);
}

void gl::Window::SetShowFPSinTitle(bool show) {
    showFpsInTitle = show;
#ifndef USE_EGL
    if (!showFpsInTitle && !showFragmentsInTitle) {
        ::glfwSetWindowTitle(hWnd, (std::string(WindowManager::TitlePrefix) + name).c_str());
    }
#endif
}

void gl::Window::SetShowSamplesinTitle(bool show) {
    showFragmentsInTitle = show;
#ifndef USE_EGL
    if (!showFpsInTitle && !showFragmentsInTitle) {
        ::glfwSetWindowTitle(hWnd, (std::string(WindowManager::TitlePrefix) + name).c_str());
    }
#endif
}

void gl::Window::SetShowPrimsinTitle(bool show) {
    showPrimsInTitle = show;
#ifndef USE_EGL
    if (!showFpsInTitle && !showFragmentsInTitle && !showPrimsInTitle) {
        ::glfwSetWindowTitle(hWnd, (std::string(WindowManager::TitlePrefix) + name).c_str());
    }
#endif
}

void gl::Window::RequestClose() {
    if (hWnd != nullptr) {
#ifndef USE_EGL
        ::glfwSetWindowShouldClose(hWnd, true);
#else
        // TODO
#endif
        ::mmcRequestViewDeInstantiation(this->hCore, this->associatedInstance.c_str());
    }
}

void gl::Window::Update() {
    if (hWnd == nullptr) return;

#ifndef USE_EGL
    // this also issues the callbacks, which might close this window
    ::glfwPollEvents();

    if (hWnd == nullptr) return;
    if (::glfwWindowShouldClose(hWnd)) {
        uiLayers.clear();

        hView.DestroyHandle();

        ::glfwDestroyWindow(hWnd);
        hWnd = nullptr;
        return;
    }

    ::glfwMakeContextCurrent(hWnd);
    int frame_width, frame_height;
    ::glfwGetFramebufferSize(hWnd, &frame_width, &frame_height);
    if ((frame_width != width) || (frame_height != height)) {
        on_resize(frame_width, frame_height);
        width = frame_width;
        height = frame_height;
    }
#else
    eglMakeCurrent( eglDisplay, eglSurface, eglSurface, *hWnd);
    int frame_width, frame_height;
    eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &frame_width);
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &frame_height);
    if ((frame_width != width) || (frame_height != height)) {
        on_resize(frame_width, frame_height);
        width = frame_width;
        height = frame_height;
    }
#endif

    fpsCntr.FrameBegin();
    if ((width > 0) && (height > 0)) {
        if (showFragmentsInTitle) glBeginQuery(GL_SAMPLES_PASSED, fragmentQuery);
        if (showPrimsInTitle) glBeginQuery(GL_PRIMITIVES_GENERATED, primsQuery);
        ::mmcRenderView(hView, &renderContext);
        if (showFragmentsInTitle) glEndQuery(GL_SAMPLES_PASSED);
        if (showPrimsInTitle) glEndQuery(GL_PRIMITIVES_GENERATED);
    }

    for (std::shared_ptr<AbstractUILayer> uil : this->uiLayers) {
        if (!uil->Enabled()) continue;
        uil->OnDraw();
    }

    // done rendering. swap and next turn
#ifndef USE_EGL
    ::glfwSwapBuffers(hWnd);
#else
    eglSwapBuffers( eglDisplay, eglSurface);
    
    // Export rendered image for verification
    //captureFramebufferPPM(0, width, height, "egl-test.ppm");
#endif
    fpsCntr.FrameEnd();

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    if (now - fpsSyncTime > std::chrono::seconds(1)) {
        on_fps_value(fpsCntr.FPS());
        fpsSyncTime = now;
#ifdef _WIN32
        // TODO fix this for EGL + Win
        if (this->topMost) {
            vislib::sys::Log::DefaultLog.WriteInfo("Periodic reordering of windows.");
            SetWindowPos(glfwGetWin32Window(this->hWnd), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        }
#endif
    }

}

#ifndef USE_EGL
void gl::Window::glfw_onKey_func(GLFWwindow* wnd, int k, int s, int a, int m) {
    ::glfwMakeContextCurrent(wnd);
    Window* that = static_cast<Window*>(::glfwGetWindowUserPointer(wnd));

    core::view::Key key = static_cast<core::view::Key>(k);

    core::view::KeyAction action(core::view::KeyAction::RELEASE);
    switch (a) {
    case GLFW_PRESS: action = core::view::KeyAction::PRESS; break;
    case GLFW_REPEAT: action = core::view::KeyAction::REPEAT; break;
    case GLFW_RELEASE: action = core::view::KeyAction::RELEASE; break;
    }

    core::view::Modifiers mods;
    if ((m & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) mods |=  core::view::Modifier::SHIFT;
    if ((m & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) mods |= core::view::Modifier::CTRL;
    if ((m & GLFW_MOD_ALT) == GLFW_MOD_ALT) mods |= core::view::Modifier::ALT;

    for (std::shared_ptr<AbstractUILayer> uil : that->uiLayers) {
        if (!uil->Enabled()) continue;
        if (uil->OnKey(key, action, mods)) break;
    }
}

void gl::Window::glfw_onChar_func(GLFWwindow* wnd, unsigned int charcode) {
    ::glfwMakeContextCurrent(wnd);
    Window* that = static_cast<Window*>(::glfwGetWindowUserPointer(wnd));
    for (std::shared_ptr<AbstractUILayer> uil : that->uiLayers) {
        if (!uil->Enabled()) continue;
        if (uil->OnChar(charcode)) break;
    }
}

void gl::Window::glfw_onMouseMove_func(GLFWwindow* wnd, double x, double y) {
    ::glfwMakeContextCurrent(wnd);
    Window* that = static_cast<Window*>(::glfwGetWindowUserPointer(wnd));
    if (that->mouseCapture) {
        that->mouseCapture->OnMouseMove(x, y);
    } else {
        for (std::shared_ptr<AbstractUILayer> uil : that->uiLayers) {
            if (!uil->Enabled()) continue;
            if (uil->OnMouseMove(x, y)) break;
        }
    }
}

void gl::Window::glfw_onMouseButton_func(GLFWwindow* wnd, int b, int a, int m) {
    ::glfwMakeContextCurrent(wnd);
    Window* that = static_cast<Window*>(::glfwGetWindowUserPointer(wnd));

    core::view::MouseButton btn = static_cast<core::view::MouseButton>(b);

    core::view::MouseButtonAction action =
        (a == GLFW_PRESS) ? core::view::MouseButtonAction::PRESS
            : core::view::MouseButtonAction::RELEASE;

    core::view::Modifiers mods;
    if ((m & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT) mods |= core::view::Modifier::SHIFT;
    if ((m & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) mods |= core::view::Modifier::CTRL;
    if ((m & GLFW_MOD_ALT) == GLFW_MOD_ALT) mods |= core::view::Modifier::ALT;

    if (that->mouseCapture) {
        that->mouseCapture->OnMouseButton(btn, action, mods);
    } else {
        for (std::shared_ptr<AbstractUILayer> uil : that->uiLayers) {
            if (!uil->Enabled()) continue;
            if (uil->OnMouseButton(btn, action, mods)) {
                if (action == core::view::MouseButtonAction::PRESS) {
                    that->mouseCapture = uil;
                }
                break;
            }
        }
    }

    if (that->mouseCapture) {
        bool anyPressed = false;
        for (int mbi = GLFW_MOUSE_BUTTON_1; mbi <= GLFW_MOUSE_BUTTON_LAST; ++mbi) {
            if (::glfwGetMouseButton(wnd, mbi) == GLFW_PRESS) {
                anyPressed = true;
                break;
            }
        }
        if (!anyPressed) {
            that->mouseCapture.reset();
            double x, y;
            ::glfwGetCursorPos(wnd, &x, &y);
            glfw_onMouseMove_func(wnd, x, y); // to inform all of the new location
        }
    }
}

void gl::Window::glfw_onMouseWheel_func(GLFWwindow* wnd, double x, double y) {
    ::glfwMakeContextCurrent(wnd);
    Window* that = static_cast<Window*>(::glfwGetWindowUserPointer(wnd));
    if (that->mouseCapture) {
        that->mouseCapture->OnMouseScroll(x, y);
    } else {
        for (std::shared_ptr<AbstractUILayer> uil : that->uiLayers) {
            if (!uil->Enabled()) continue;
            if (uil->OnMouseScroll(x, y)) break;
        }
    }
}
#endif // USE_EGL

void gl::Window::on_resize(int w, int h) {
#ifndef USE_EGL
    ::glfwMakeContextCurrent(hWnd);
    if ((w > 0) && (h > 0)) {
        ::glViewport(0, 0, w, h);
        ::mmcResizeView(hView, w, h);
        vislib::sys::Log::DefaultLog.WriteInfo("Console::Window: Resize window (w: %d, h: %d)\n", w, h);
        for (std::shared_ptr<AbstractUILayer> uil : uiLayers) {
            // we inform even disabled layers, since we would need to know and update as soon as they get enabled.
            uil->OnResize(w, h);
        }
    }
#else
    eglMakeCurrent( eglDisplay, eglSurface, eglSurface, *hWnd);
    if ((w > 0) && (h > 0)) {
        ::glViewport(0, 0, w, h);
        ::mmcResizeView(hView, w, h);
        for (std::shared_ptr<AbstractUILayer> uil : uiLayers) {
            // we inform even disabled layers, since we would need to know and update as soon as they get enabled.
            uil->onResize(w, h);
        }
    }
#endif
}

void gl::Window::on_fps_value(float fps_val) {
    fps = fps_val;

    auto i1 = fpsList.begin();
    auto i2 = i1 + 1;
    auto e = fpsList.end();
    while (i2 != e) {
        *i1 = *i2;
        ++i1;
        ++i2;
    }
    fpsList[fpsList.size() - 1] = fps;

#ifndef USE_EGL
    //if (showFpsInTitle) {
    //    std::stringstream title;
    //    if (showFragmentsInTitle) {
    //        GLuint64 samp;
    //        glGetQueryObjectui64v(fragmentQuery, GL_QUERY_RESULT, &samp);
    //        title << WindowManager::TitlePrefix << name << " - [" << fps << " fps, " << samp << " samples]";
    //    } else {
    //        title << WindowManager::TitlePrefix << name << " - [" << fps << " fps]";
    //    }
    //    ::glfwSetWindowTitle(hWnd, title.str().c_str());
    //}
    std::stringstream title;
    title.imbue(std::locale(""));
    title << WindowManager::TitlePrefix << name;
    if (showFpsInTitle || showFragmentsInTitle || showPrimsInTitle) title << " - [ ";
    if (showFpsInTitle) {
        title << fps << " fps ";
    }
    if (showFragmentsInTitle) {
        GLuint64 samp;
        glGetQueryObjectui64v(fragmentQuery, GL_QUERY_RESULT, &samp);
        title << samp << " samples ";
    }
    if (showPrimsInTitle) {
        GLuint64 prims;
        glGetQueryObjectui64v(primsQuery, GL_QUERY_RESULT, &prims);
        title << prims << " primitives ";
    }
    if (showFpsInTitle || showFragmentsInTitle || showPrimsInTitle) title << "]";
    ::glfwSetWindowTitle(hWnd, title.str().c_str());
#endif
}

#ifdef USE_EGL
void gl::Window::captureFramebufferPPM(GLuint framebuffer, uint32_t width, uint32_t height, const std::string& path) {
    // For verification...
    size_t numBytes = width * height * 3;
    uint8_t* rgb = new uint8_t[numBytes];

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, rgb);

    std::ofstream outFile;
    outFile.open(path.c_str(), std::ios::binary);

    outFile << "P6" << "\n"
            << width << " " << height << "\n"
            << "255\n";

    outFile.write((char*) rgb, numBytes);

    delete[] rgb;
}
#endif


/****************************************************************************/

//#ifdef HAS_ANTTWEAKBAR
//
//namespace {
//    void MEGAMOLCORE_CALLBACK collectParams(const char *paramName, void *contextPtr) {
//        std::vector<vislib::StringA> *paramNames = static_cast<std::vector<vislib::StringA>* >(contextPtr);
//        assert(paramNames != nullptr);
//        paramNames->push_back(paramName);
//    }
//}
//
///*
// * megamol::console::Window::InitGUI
// */
//void megamol::console::Window::InitGUI(CoreHandle& hCore) {
//    this->gui.BeginInitialisation();
//
//    std::vector<vislib::StringA> params;
//    ::mmcEnumParametersA(hCore, &collectParams, &params);
//
//    for (const vislib::StringA& paramName : params) {
//        vislib::SmartPtr<megamol::console::CoreHandle> hParam = new megamol::console::CoreHandle();
//        vislib::RawStorage desc;
//        if (!::mmcGetParameterA(hCore, paramName, *hParam)) continue;
//
//        unsigned int len = 0;
//        ::mmcGetParameterTypeDescription(*hParam, NULL, &len);
//        desc.AssertSize(len);
//        ::mmcGetParameterTypeDescription(*hParam, desc.As<unsigned char>(), &len);
//
//        this->gui.AddParameter(hParam, paramName, desc.As<unsigned char>(), len);
//    }
//
//    this->gui.EndInitialisation();
//}
//
//
//void megamol::console::Window::UpdateGUI(CoreHandle& hCore) {
//    std::vector<vislib::StringA> params;
//    std::vector<vislib::StringA> deadParams = gui.ParametersNames();
//
//    ::mmcEnumParametersA(hCore, &collectParams, &params);
//
//    for (const vislib::StringA& paramName : params) {
//
//        // search if param already exist
//        auto dpi = std::find(deadParams.begin(), deadParams.end(), vislib::StringA(paramName));
//        if (dpi != deadParams.end()) {
//            deadParams.erase(dpi); // this gui parameter is in use and will not be deleted
//            continue;
//        }
//
//        // parameter does not yet exist
//        vislib::SmartPtr<megamol::console::CoreHandle> hParam = new megamol::console::CoreHandle();
//        vislib::RawStorage desc;
//        if (!::mmcGetParameterA(hCore, paramName, *hParam)) continue;
//
//        unsigned int len = 0;
//        ::mmcGetParameterTypeDescription(*hParam, NULL, &len);
//        desc.AssertSize(len);
//        ::mmcGetParameterTypeDescription(*hParam, desc.As<unsigned char>(), &len);
//
//        this->gui.AddParameter(hParam, paramName, desc.As<unsigned char>(), len);
//
//    }
//
//    // now we delete all the orphaned gui parameters
//    for (const vislib::StringA& paramName : deadParams) {
//        this->gui.RemoveParameter(paramName);
//    }
//
//}
//#endif /* HAS_ANTTWEAKBAR */
//
///*
// * megamol::console::Window::Update
// */
//void megamol::console::Window::Update(CoreHandle& hCore) {
//#ifdef HAS_ANTTWEAKBAR
//    // update GUI once a second
//    static std::chrono::system_clock::time_point last = std::chrono::system_clock::now();
//    if (gui.IsActive()) {
//        std::chrono::system_clock::time_point n = std::chrono::system_clock::now();
//        if (std::chrono::duration_cast<std::chrono::seconds>(n - last).count() > 0) {
//            last = n;
//            UpdateGUI(hCore);
//        }
//    }
//#endif /* HAS_ANTTWEAKBAR */
//
//}
