/*
 * AbstractView.h
 *
 * Copyright (C) 2008 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOLCORE_ABSTRACTVIEW_H_INCLUDED
#define MEGAMOLCORE_ABSTRACTVIEW_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#    pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include "mmcore/CalleeSlot.h"
#include "mmcore/Module.h"
#include "mmcore/api/MegaMolCore.h"
#include "mmcore/api/MegaMolCore.std.h"
#include "mmcore/param/AbstractParam.h"
#include "vislib/Array.h"
#include "vislib/Serialiser.h"
#include "vislib/SingleLinkedList.h"
#include "vislib/SmartPtr.h"
#include "vislib/String.h"
#include <AbstractInputScope.h>

#include "mmcore/view/CameraSerializer.h"
#include "mmcore/param/ParamSlot.h"
#include "mmcore/view/Camera_2.h"

namespace megamol {
namespace core {
namespace view {

using megamol::frontend_resources::Key;
using megamol::frontend_resources::KeyAction;
using megamol::frontend_resources::KeyCode;
using megamol::frontend_resources::Modifier;
using megamol::frontend_resources::Modifiers;
using megamol::frontend_resources::MouseButton;
using megamol::frontend_resources::MouseButtonAction;

/**
 * Abstract base class of rendering views
 */
class MEGAMOLCORE_API AbstractView : public Module, public megamol::frontend_resources::AbstractInputScope {


public:
    /**
     * Interfaces class for hooking into view processes
     */
    class MEGAMOLCORE_API Hooks {
    public:
        /**
         * Empty ctor.
         */
        Hooks(void) {
            // intentionally empty
        }

        /**
         * Empty but virtual dtor.
         */
        virtual ~Hooks(void) {
            // intentionally empty
        }

        /**
         * Hook method to be called before the view is rendered.
         *
         * @param view The calling view
         */
        virtual void BeforeRender(AbstractView* view) {
            // intentionally empty
        }

        /**
         * Hook method to be called after the view is rendered.
         *
         * @param view The calling view
         */
        virtual void AfterRender(AbstractView* view) {
            // intentionally empty
        }
    };

    /** Ctor. */
    AbstractView(void);

    /** Dtor. */
    virtual ~AbstractView(void);

    /**
     * Answer the default time for this view
     *
     * @return The default time
     */
    virtual float DefaultTime(double instTime) const = 0; /* {
        return 0.0f;
    }*/

    /**
     * Answers whether the given parameter is relevant for this view.
     *
     * @param param The parameter to test.
     *
     * @return 'true' if 'param' is relevant, 'false' otherwise.
     */
    virtual bool IsParamRelevant(const vislib::SmartPtr<param::AbstractParam>& param) const;

    /**
     * Answer the camera synchronization number.
     *
     * @return The camera synchronization number
     */
    virtual unsigned int GetCameraSyncNumber(void) const = 0;

    /**
     * Renders this AbstractView3D in the currently active OpenGL context.
     *
     * @param context The context information like time or GPU affinity.
     */
    virtual void Render(const mmcRenderViewContext& context) = 0;

    /**
     * Resets the view. This normally sets the camera parameters to
     * default values.
     */
    virtual void ResetView(void) = 0;

    /**
     * Resizes the AbstractView3D.
     *
     * @param width The new width.
     * @param height The new height.
     */
    virtual void Resize(unsigned int width, unsigned int height) = 0;

    /**
     * Answers the desired window position configuration of this view.
     *
     * @param x To receive the coordinate of the upper left corner
     * @param y To recieve the coordinate of the upper left corner
     * @param w To receive the width
     * @param h To receive the height
     * @param nd To receive the flag deactivating window decorations
     *
     * @return 'true' if this view has a desired window position
     *         configuration, 'false' if not. In the latter case the value
     *         the parameters are pointing to are not altered.
     */
    virtual bool DesiredWindowPosition(int* x, int* y, int* w, int* h, bool* nd);

    /**
     * Registers a hook
     *
     * @param hook The hook to register
     */
    void RegisterHook(Hooks* hook) {
        if (!this->hooks.Contains(hook)) {
            this->hooks.Add(hook);
        }
    }

    /**
     * Unregisters a hook
     *
     * @param hook The hook to unregister
     */
    void UnregisterHook(Hooks* hook) { this->hooks.RemoveAll(hook); }

    /**
     * Callback requesting a rendering of this view
     *
     * @param call The calling call
     *
     * @return The return value
     */
    virtual bool OnRenderView(Call& call);


    /**
     * Callback requesting the extents of this view
     *
     * @param call The calling call
     *
     * @return The return value
     */
    virtual bool GetExtents(Call& call);

    /**
     * Callback requesting a rendering of this view
     *
     * @param call The calling call
     *
     * @return The return value
     */
    virtual bool OnFreezeView(Call& call) {
        this->UpdateFreeze(true);
        return true;
    }

    /**
     * Callback requesting a rendering of this view
     *
     * @param call The calling call
     *
     * @return The return value
     */
    virtual bool OnUnfreezeView(Call& call) {
        this->UpdateFreeze(false);
        return true;
    }

    /**
     * Freezes, updates, or unfreezes the view onto the scene (not the
     * rendering, but camera settings, timing, etc).
     *
     * @param freeze true means freeze or update freezed settings,
     *               false means unfreeze
     */
    virtual void UpdateFreeze(bool freeze) = 0;

    /**
     * cursor input callback
     *
     * @param call The calling call
     *
     * @return The return value
     */
    bool onResetView(Call& call);

    bool OnKeyCallback(Call& call);

    bool OnCharCallback(Call& call);

    bool OnMouseButtonCallback(Call& call);

    bool OnMouseMoveCallback(Call& call);

    bool OnMouseScrollCallback(Call& call);

protected:
    /** Typedef alias */
    typedef vislib::SingleLinkedList<Hooks*>::Iterator HooksIterator;

    /**
     * Tries to load the desired window position configuration form the
     * configuration value with the given name.
     *
     * @param str The value to be parsed
     * @param x To receive the coordinate of the upper left corner
     * @param y To recieve the coordinate of the upper left corner
     * @param w To receive the width
     * @param h To receive the height
     * @param nd To receive the flag deactivating window decorations
     *
     * @return 'true' if this view has a desired window position
     *         configuration, 'false' if not. In the latter case the value
     *         the parameters are pointing to are not altered.
     */
    bool desiredWindowPosition(const vislib::StringW& str, int* x, int* y, int* w, int* h, bool* nd);

    /**
     * Answer if hook code should be executed.
     *
     * @return 'true' if hook code should be run
     */
    inline bool doHookCode(void) const { return !this->hooks.IsEmpty(); }

    /**
     * Gets an iterator to the list or registered hooks.
     *
     * @return An iterator to the list of registered hooks.
     */
    inline HooksIterator getHookIterator(void) { return this->hooks.GetIterator(); }

    /**
     * The code triggering the pre render hook
     */
    inline void doBeforeRenderHook(void) {
        HooksIterator i = this->getHookIterator();
        while (i.HasNext()) {
            i.Next()->BeforeRender(this);
        }
    }

    /**
     * The code triggering the post render hook
     */
    inline void doAfterRenderHook(void) {
        HooksIterator i = this->getHookIterator();
        while (i.HasNext()) {
            i.Next()->AfterRender(this);
        }
    }

    /**
     * Unpacks the mouse coordinates, which are relative to the virtual
     * viewport size.
     *
     * @param x The x coordinate of the mouse position
     * @param y The y coordinate of the mouse position
     */
    virtual void unpackMouseCoordinates(float& x, float& y);

        /**
     * Stores the current camera settings
     *
     * @param p Must be storeCameraSettingsSlot
     * @return true
     */
    bool onStoreCamera(param::ParamSlot& p);

    /**
     * Restores the camera settings
     *
     * @param p Must be restoreCameraSettingsSlot
     *
     * @return true
     */
    bool onRestoreCamera(param::ParamSlot& p);

    /**
     * This method determines the file path the camera file should have
     *
     * @return The file path of the camera file as string
     */
    std::string determineCameraFilePath(void) const;

    /** Slot for incoming rendering requests */
    CalleeSlot renderSlot;

    /** The camera */
    Camera_2 cam;

    /** Slot containing the settings of the currently stored camera */
    param::ParamSlot cameraSettingsSlot;

    /** Triggers the storage of the camera settings */
    param::ParamSlot storeCameraSettingsSlot;

    /** Triggers the restore of the camera settings */
    param::ParamSlot restoreCameraSettingsSlot;

    /** Slot activating or deactivating the override of already present camera settings */
    param::ParamSlot overrideCamSettingsSlot;

    /** Slot activating or deactivating the automatic save of camera parameters to disk when a camera is saved */
    param::ParamSlot autoSaveCamSettingsSlot;

    /** Slot activating or deactivating the automatic load of camera parameters at program startup */
    param::ParamSlot autoLoadCamSettingsSlot;

    /** Array that holds the saved camera states */
    std::array<std::pair<Camera_2::minimal_state_type, bool>, 11> savedCameras;

    /** The object responsible for camera serialization */
    CameraSerializer serializer;

private:
    /** List of registered hooks */
    vislib::SingleLinkedList<Hooks*> hooks;
};


} /* end namespace view */
} /* end namespace core */
} /* end namespace megamol */

#endif /* MEGAMOLCORE_ABSTRACTVIEW_H_INCLUDED */
