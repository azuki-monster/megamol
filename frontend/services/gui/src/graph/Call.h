/*
 * Call.h
 *
 * Copyright (C) 2019 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOL_GUI_GRAPH_CALL_H_INCLUDED
#define MEGAMOL_GUI_GRAPH_CALL_H_INCLUDED
#pragma once


#ifdef PROFILING
#include "PerformanceManager.h"
#include "mmcore/MultiPerformanceHistory.h"
#endif
#include "mmcore/Call.h"
#include "mmcore/CallCapabilities.h"
#include "widgets/HoverToolTip.h"
#include "widgets/ImageWidget_gl.h"


namespace megamol {
namespace gui {


// Forward declarations
class Call;
class CallSlot;
class Module;
class Parameter;
class CallSlot;
#ifndef _CALL_SLOT_TYPE_
enum CallSlotType { CALLEE, CALLER };
#define _CALL_SLOT_TYPE_
#endif
typedef std::shared_ptr<CallSlot> CallSlotPtr_t;
typedef std::shared_ptr<Module> ModulePtr_t;

// Types
typedef std::shared_ptr<Call> CallPtr_t;
typedef std::vector<CallPtr_t> CallPtrVector_t;


/** ************************************************************************
 * Defines call data structure for graph
 */
class Call {
public:
    struct StockCall {
        std::string class_name;
        std::string description;
        std::string plugin_name;
        std::vector<std::string> functions;
    };

    Call(ImGuiID uid, const std::string& class_name, const std::string& description, const std::string& plugin_name,
        const std::vector<std::string>& functions);
    ~Call();

    bool IsConnected();
    bool ConnectCallSlots(CallSlotPtr_t callslot_1, CallSlotPtr_t callslot_2);
    bool DisconnectCallSlots(ImGuiID calling_callslot_uid = GUI_INVALID_ID);
    const CallSlotPtr_t& CallSlotPtr(CallSlotType type);

    void Draw(megamol::gui::PresentPhase phase, GraphItemsState_t& state);

    inline ImGuiID UID() const {
        return this->uid;
    }
    inline std::string ClassName() const {
        return this->class_name;
    }
    std::string SlotsLabel() const {
        return std::string(this->caller_slot_name + this->slot_name_separator + this->callee_slot_name);
    }

    void SetCapabilities(core::CallCapabilities caps) {
        capabilities = caps;
    }

#ifdef PROFILING

    void SetProfilingData(void* ptr, uint32_t num_callbacks) {
        this->profiling_parent_pointer = ptr;
        cpu_perf_history.resize(num_callbacks);
        gl_perf_history.resize(num_callbacks);
        for (auto i = 0; i < num_callbacks; ++i) {
            const auto& cb_name = (static_cast<core::Call*>(ptr))->GetCallbackName(i);
            cpu_perf_history[i].set_name(cb_name);
            gl_perf_history[i].set_name(cb_name);
        }
    }

    void* GetProfilingParent() {
        return this->profiling_parent_pointer;
    }

    void AppendPerformanceData(frontend_resources::PerformanceManager::frame_type frame,
        const frontend_resources::PerformanceManager::timer_entry& entry);

#endif // PROFILING

private:
    // VARIABLES --------------------------------------------------------------

    const ImGuiID uid;
    // TODO Place StackCall (Properties?) here
    const std::string class_name;
    const std::string description;
    const std::string plugin_name;
    const std::vector<std::string> functions;
    core::CallCapabilities capabilities;

    std::map<CallSlotType, CallSlotPtr_t> connected_callslots;

    bool gui_selected;
    const std::string slot_name_separator = " > ";
    std::string caller_slot_name;
    std::string callee_slot_name;

    HoverToolTip gui_tooltip;
    ImageWidget gui_profiling_button;
    bool gui_profiling_btn_hovered;

#ifdef PROFILING
    std::vector<core::MultiPerformanceHistory> cpu_perf_history;
    std::vector<core::MultiPerformanceHistory> gl_perf_history;
    void* profiling_parent_pointer;
    float profiling_window_height; // determine dynamically
    bool show_profiling_data;
    ImageWidget gui_profiling_run_button;
    bool pause_profiling_history_update;

    void draw_profiling_data(GraphItemsState_t& state);
#endif // PROFILING
};


} // namespace gui
} // namespace megamol

#endif // MEGAMOL_GUI_GRAPH_CALL_H_INCLUDED
