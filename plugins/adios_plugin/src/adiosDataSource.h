#pragma once

#include "mmcore/view/AnimDataModule.h"
#include "mmcore/param/ParamSlot.h"
#include "mmcore/CalleeSlot.h"


namespace megamol {
namespace adios {

class adiosDataSource : public view::AnimDataModule {
public:
/**
 * Answer the name of this module.
 *
 * @return The name of this module.
 */
static const char *ClassName(void) {
return "adiosDataSource";
}

/**
 * Answer a human readable description of this module.
 *
 * @return A human readable description of this module.
 */
static const char *Description(void) {
return "Data source module for ADIOS-based IO.";
}

/**
 * Answers whether this module is available on the current system.
 *
 * @return 'true' if the module is available, 'false' otherwise.
 */
static bool IsAvailable(void) {
return true;
}

/** Ctor. */
adiosDataSource(void);

/** Dtor. */
virtual ~adiosDataSource(void);

protected:

        /**
         * Gets the data from the source.
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool getDataCallback(Call& caller);

        /**
         * Gets the data from the source.
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool getExtentCallback(Call& caller);


private:

        /** The slot for requesting data */
        CalleeSlot getData;

        /** The frame index table */
	std::vector<UINT64> frameIdx;

        /** Data file load id counter */
        size_t data_hash;

        /** The data set bounding box */
        vislib::math::Cuboid<float> bbox;


};


} /* end namespace adios */
} /* end namespace megamol */
