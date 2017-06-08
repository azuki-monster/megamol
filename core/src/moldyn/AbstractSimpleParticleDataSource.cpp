/*
 * AbstractSimpleParticleDataSource.cpp
 *
 * Copyright (C) 2012 by TU Dresden (CGV)
 * Alle Rechte vorbehalten.
 */

#include "stdafx.h"
#include "mmcore/moldyn/AbstractSimpleParticleDataSource.h"
#include "mmcore/param/FilePathParam.h"

using namespace megamol::core;
using namespace megamol::core::moldyn;


/*
 * AbstractSimpleParticleDataSource::AbstractSimpleParticleDataSource
 */
AbstractSimpleParticleDataSource::AbstractSimpleParticleDataSource(void) : Module(),
        filenameSlot("filename", "Full path to the file to load"),
        getDataSlot("getdata", "Publishes data for other modules") {

    this->filenameSlot << new param::FilePathParam("");
    this->MakeSlotAvailable(&this->filenameSlot);

    this->getDataSlot.SetCallback("MultiParticleDataCall", "GetData", &AbstractSimpleParticleDataSource::getDataCallback);
    this->getDataSlot.SetCallback("MultiParticleDataCall", "GetExtent", &AbstractSimpleParticleDataSource::getExtentCallback);
    this->MakeSlotAvailable(&this->getDataSlot);
}


/*
 * AbstractSimpleParticleDataSource::AbstractSimpleParticleDataSource
 */
AbstractSimpleParticleDataSource::~AbstractSimpleParticleDataSource(void) {
    this->Release();
}


/*
 * AbstractSimpleParticleDataSource::getDataCallback
 */
bool AbstractSimpleParticleDataSource::getDataCallback(Call& caller) {
    MultiParticleDataCall *mpdc = dynamic_cast<MultiParticleDataCall *>(&caller);
    if (mpdc == NULL) return false;
    return this->getData(*mpdc);
}


/*
 * AbstractSimpleParticleDataSource::getDataCallback
 */
bool AbstractSimpleParticleDataSource::getExtentCallback(Call& caller) {
    MultiParticleDataCall *mpdc = dynamic_cast<MultiParticleDataCall *>(&caller);
    if (mpdc == NULL) return false;
    return this->getExtent(*mpdc);
}
