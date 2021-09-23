/*
 * UniFlagStorage.h
 *
 * Copyright (C) 2019-2021 by Universitaet Stuttgart (VISUS).
 * Alle Rechte vorbehalten.
 */

#pragma once

#include "vislib/graphics/gl/IncludeAllGL.h"
#include <glowl/GLSLProgram.hpp>

#include "FlagCollections.h"
#include "FlagStorage.h"
#include "glowl/BufferObject.hpp"
#include "mmcore/Call.h"
#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/Module.h"
#include "tbb/tbb.h"

namespace megamol {
namespace core {

    /**
     * Class holding a GL buffer of uints which contain flags that say something
     * about a synchronized other piece of data (index equality).
     * Can be used for storing selection etc. Should be kept in sync with the normal
     * FlagStorage, which resides on CPU.
     */
    class MEGAMOLCORE_API UniFlagStorage : public core::Module {
    public:
        // enum { ENABLED = 1 << 0, FILTERED = 1 << 1, SELECTED = 1 << 2 };

        using index_type = int32_t;
        using index_vector = std::vector<index_type>;

        /**
         * Answer the name of this module.
         *
         * @return The name of this module.
         */
        static const char* ClassName(void) {
            return "UniFlagStorage";
        }

        /**
         * Answer a human readable description of this module.
         *
         * @return A human readable description of this module.
         */
        static const char* Description(void) {
            return "Module representing an index-synced array of flag uints as a GPU buffer";
        }

        /**
         * Answers whether this module is available on the current system.
         *
         * @return 'true' if the module is available, 'false' otherwise.
         */
        static bool IsAvailable(void) {
            return ogl_IsVersionGEQ(4, 3);
        }

        /** Ctor. */
        UniFlagStorage(void);

        /** Dtor. */
        virtual ~UniFlagStorage(void);

    protected:
        /**
         * Implementation of 'Create'.
         *
         * @return 'true' on success, 'false' otherwise.
         */
        virtual bool create(void);

        /**
         * Implementation of 'Release'.
         */
        virtual void release(void);

    private:
        /**
         * Access the flags provided by the UniFlagStorage
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool readDataCallback(core::Call& caller);

        /**
         * Write/update the flags provided by the UniFlagStorage
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool writeDataCallback(core::Call& caller);

        /**
         * Access the flags provided by the UniFlagStorage
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool readCPUDataCallback(core::Call& caller);

        /**
         * Write/update the flags provided by the UniFlagStorage
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool writeCPUDataCallback(core::Call& caller);

        /**
         * Access the metadata provided by the UniFlagStorage
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool readMetaDataCallback(core::Call& caller);

        /**
         * Write/update the metadata provided by the UniFlagStorage
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool writeMetaDataCallback(core::Call& caller);

        void serializeData();
        static void check_bits(FlagStorage::FlagItemType flag_bit, index_vector& bit_starts, index_vector& bit_ends,
            index_type& curr_bit_start, index_type x, const std::shared_ptr<FlagStorage::FlagVectorType>& flags);
        static void terminate_bit(const std::shared_ptr<FlagStorage::FlagVectorType>& flags, index_vector& bit_ends,
            index_type curr_bit_start);
        static nlohmann::json make_bit_array(const index_vector& bit_starts, const index_vector& bit_ends);
        void array_to_bits(const nlohmann::json& json, FlagStorage::FlagItemType flag_bit);
        void serializeCPUData();
        void deserializeCPUData();
        bool onJSONChanged(param::ParamSlot& slot);

        class BitsChecker {
        public:
            void operator()(const tbb::blocked_range<int32_t>& r) {
                for (int32_t i = r.begin(); i != r.end(); ++i) {
                    check_bits(FlagStorage::ENABLED, enabled_starts, enabled_ends, curr_enabled_start, i, flags);
                    check_bits(FlagStorage::FILTERED, filtered_starts, filtered_ends, curr_filtered_start, i, flags);
                    check_bits(FlagStorage::SELECTED, selected_starts, selected_ends, curr_selected_start, i, flags);
                }
            }

            BitsChecker(BitsChecker& b, tbb::split) : flags(b.flags) {
                // what to do?
            }

            // when done, copies the result into the out parameters, so they can be identical to one or other.
            void join_ranges(const index_vector& one_starts, const index_vector& one_ends, const index_vector& other_starts, const index_vector& other_ends, index_vector& out_starts, index_vector& out_ends) {
                index_type my_pos = 0, other_pos = 0;
                index_vector result_starts, result_ends;
                while (my_pos < one_starts.size() && other_pos < other_starts.size()) {
                    const auto mystart = one_starts[my_pos];
                    const auto otherstart = other_starts[other_pos];
                    const auto myend = one_ends[my_pos];
                    const auto otherend = other_ends[other_pos];

                    if (mystart < otherstart) {
                        if (myend < otherstart - 1) {
                            result_starts.push_back(mystart);
                            result_ends.push_back(myend);
                            my_pos++;
                        } else {
                            result_starts.push_back(mystart);
                            result_ends.push_back(otherend);
                            my_pos++;
                            other_pos++;
                        }
                    } else {
                        ASSERT(mystart != otherstart);
                        if (otherend < mystart - 1) {
                            result_starts.push_back(otherstart);
                            result_ends.push_back(otherend);
                            other_pos++;
                        } else {
                            result_starts.push_back(otherstart);
                            result_ends.push_back(myend);
                            my_pos++;
                            other_pos++;
                        }
                    }
                }
                // push everything after *_pos in one go
                const auto total_elems = one_starts.size() + other_starts.size();
                if (my_pos < one_starts.size()) {
                    result_starts.reserve(total_elems);
                    result_starts.insert(result_starts.end(), one_starts.begin() + my_pos, one_starts.end());
                    result_ends.reserve(total_elems);
                    result_ends.insert(result_ends.end(), one_ends.begin() + my_pos, one_ends.end());
                }
                if (other_pos < other_starts.size()) {
                    result_starts.reserve(total_elems);
                    result_starts.insert(result_starts.end(), other_starts.begin() + my_pos, other_starts.end());
                    result_ends.reserve(total_elems);
                    result_ends.insert(result_ends.end(), other_ends.begin() + my_pos, other_ends.end());
                }
                out_starts = result_starts;
                out_ends = result_ends;
            }

            void join(const BitsChecker& other) {

                join_ranges(this->enabled_starts, this->enabled_ends, other.enabled_starts, other.enabled_ends, this->enabled_starts, this->enabled_ends);
                join_ranges(this->filtered_starts, this->filtered_ends, other.filtered_starts, other.filtered_ends, this->filtered_starts, this->filtered_ends);
                join_ranges(this->selected_starts, this->selected_ends, other.selected_starts, other.selected_ends, this->selected_starts, this->selected_ends);
            }

            BitsChecker(const std::shared_ptr<FlagStorage::FlagVectorType>& flags) : flags(flags) {}
            
        private:
            index_vector enabled_starts, enabled_ends;
            index_vector filtered_starts, filtered_ends;
            index_vector selected_starts, selected_ends;
            index_type curr_enabled_start = -1, curr_filtered_start = -1, curr_selected_start = -1;
            const std::shared_ptr<FlagStorage::FlagVectorType>& flags;
        };

        /**
         * Helper to copy CPU flags to GL flags
         */
        void CPU2GLCopy();

        /**
         * Helper to copy GL flags to CPU flags
         */
        void GL2CPUCopy();

        /** The slot for reading the data */
        core::CalleeSlot readFlagsSlot;

        /** The slot for writing the data */
        core::CalleeSlot writeFlagsSlot;

        /** The slot for reading the data */
        core::CalleeSlot readCPUFlagsSlot;

        /** The slot for writing the data */
        core::CalleeSlot writeCPUFlagsSlot;

        core::param::ParamSlot serializedFlags;

        std::unique_ptr<glowl::GLSLProgram> compressGPUFlagsProgram;

        std::shared_ptr<FlagCollection_GL> theData;
        std::shared_ptr<FlagCollection_CPU> theCPUData;
        bool cpu_stale = true, gpu_stale = true;
        uint32_t version = 0;
    };

} // namespace core
} /* end namespace megamol */
