/*
 * SpotLight.h
 * Copyright (C) 2009-2017 by MegaMol Team
 * Alle Rechte vorbehalten.
 */
#pragma once

#include "mmstd/light/AbstractLight.h"

namespace megamol {
namespace core {
namespace view {
namespace light {

struct SpotLightType : public BaseLightType {
    std::array<float, 3> position;
    std::array<float, 3> direction;
    float openingAngle;
    float penumbraAngle;
    float radius;
};

class SpotLight : public AbstractLight {
public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName(void) {
        return "SpotLight";
    }

    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description(void) {
        return "Configuration module for a spot light source.";
    }

    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable(void) {
        return true;
    }

    /**
     * Add the lightsource of this module to a given collection
     */
    void addLight(LightCollection& light_collection);

    /** Ctor. */
    SpotLight(void);

    /** Dtor. */
    virtual ~SpotLight(void);

private:
    core::param::ParamSlot position;
    core::param::ParamSlot direction;
    core::param::ParamSlot openingAngle;
    core::param::ParamSlot penumbraAngle;
    core::param::ParamSlot radius;

    virtual bool InterfaceIsDirty();
    virtual void readParams();
};

} // namespace light
} // namespace view
} // namespace core
} // namespace megamol
