/*
 * Parameter.h
 *
 * Copyright (C) 2019 by Universitaet Stuttgart (VIS).
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOL_GUI_GRAPH_PARAMETER_H_INCLUDED
#define MEGAMOL_GUI_GRAPH_PARAMETER_H_INCLUDED


#include "FileUtils.h"
#include "GUIUtils.h"
#include "TransferFunctionEditor.h"

#include "mmcore/param/BoolParam.h"
#include "mmcore/param/ButtonParam.h"
#include "mmcore/param/ColorParam.h"
#include "mmcore/param/EnumParam.h"
#include "mmcore/param/FilePathParam.h"
#include "mmcore/param/FlexEnumParam.h"
#include "mmcore/param/FloatParam.h"
#include "mmcore/param/IntParam.h"
#include "mmcore/param/StringParam.h"
#include "mmcore/param/TernaryParam.h"
#include "mmcore/param/TransferFunctionParam.h"
#include "mmcore/param/Vector2fParam.h"
#include "mmcore/param/Vector3fParam.h"
#include "mmcore/param/Vector4fParam.h"

#include <variant>

// Used for platform independent clipboard (ImGui so far only provides windows implementation)
#ifdef GUI_USE_GLFW
#    include "GLFW/glfw3.h"
#endif


namespace megamol {
namespace gui {
namespace configurator {

// Forward declarations
class Parameter;
class Call;
class CallSlot;
class Module;
typedef std::shared_ptr<Call> CallPtrType;
typedef std::shared_ptr<CallSlot> CallSlotPtrType;
typedef std::shared_ptr<Module> ModulePtrType;

// Types
typedef std::shared_ptr<Parameter> ParamPtrType;
typedef std::vector<Parameter> ParamVectorType;


/** ************************************************************************
 * Defines GUI parameter presentation.
 */
class ParameterPresentation : public megamol::core::param::AbstractParamPresentation {
public:
    friend class Parameter;

    /*
     * Globally scoped widgets (widget parts) are always called each frame.
     * Locally scoped widgets (widget parts) are only called if respective parameter appears in GUI.
     */
    enum WidgetScope { GLOBAL, LOCAL };

    // VARIABLES --------------------------------------------------------------

    bool extended;

    // FUCNTIONS --------------------------------------------------------------

    ParameterPresentation(ParamType type);
    ~ParameterPresentation(void);

    bool IsGUIStateDirty(void) { return this->guistate_dirty; }
    void ResetGUIStateDirty(void) { this->guistate_dirty = false; }
    void ForceSetGUIStateDirty(void) { this->guistate_dirty = true; }

    void SetTransferFunctionEditorHash(size_t hash) { this->tf_editor_hash = hash; }

    inline void ConnectExternalTransferFunctionEditor(std::shared_ptr<megamol::gui::TransferFunctionEditor> tfe_ptr) {
        if (this->tf_editor_external_ptr != tfe_ptr) {
            this->tf_editor_external_ptr = tfe_ptr;
            this->use_external_tf_editor = true;
        }
    }

    void SetTransferFunctionTexture(GLuint tex_id) { this->tf_texture = tex_id; }

    /** "Point in Circle" Button */
    static bool PointCircleButton(const std::string& label = "", bool dirty = false);

    /** Extended parameter mode button. */
    static bool ParameterExtendedModeButton(bool& inout_extended_mode);

private:
    // VARIABLES --------------------------------------------------------------

    std::string help;
    std::string description;
    megamol::gui::GUIUtils utils;
    megamol::gui::FileUtils file_utils;
    std::variant<std::monostate, std::string, int, float, glm::vec2, glm::vec3, glm::vec4> widget_store;
    const std::string float_format;
    float height;
    UINT set_focus;
    bool guistate_dirty;

    std::shared_ptr<megamol::gui::TransferFunctionEditor> tf_editor_external_ptr;
    megamol::gui::TransferFunctionEditor tf_editor_internal;
    bool use_external_tf_editor;
    bool show_tf_editor;
    size_t tf_editor_hash;
    GLuint tf_texture;

    // FUNCTIONS --------------------------------------------------------------
    bool Present(Parameter& inout_param, WidgetScope scope);
    float GetHeight(Parameter& inout_param);

    bool present_parameter(Parameter& inout_parameter, WidgetScope scope);

    bool widget_button(WidgetScope scope, const std::string& labelel, const megamol::core::view::KeyCode& keycode);
    bool widget_bool(WidgetScope scope, const std::string& labelel, bool& value);
    bool widget_string(WidgetScope scope, const std::string& labelel, std::string& value);
    bool widget_color(WidgetScope scope, const std::string& labelel, glm::vec4& value);
    bool widget_enum(WidgetScope scope, const std::string& labelel, int& value, EnumStorageType storage);
    bool widget_flexenum(WidgetScope scope, const std::string& label, std::string& value,
        megamol::core::param::FlexEnumParam::Storage_t storage);
    bool widget_filepath(WidgetScope scope, const std::string& labelel, std::string& value);
    bool widget_ternary(WidgetScope scope, const std::string& labelel, vislib::math::Ternary& value);
    bool widget_int(WidgetScope scope, const std::string& labelel, int& value, int min, int max);
    bool widget_float(WidgetScope scope, const std::string& labelel, float& value, float min, float max);
    bool widget_vector2f(WidgetScope scope, const std::string& labelel, glm::vec2& value, glm::vec2 min, glm::vec2 max);
    bool widget_vector3f(WidgetScope scope, const std::string& labelel, glm::vec3& value, glm::vec3 min, glm::vec3 max);
    bool widget_vector4f(WidgetScope scope, const std::string& labelel, glm::vec4& value, glm::vec4 min, glm::vec4 max);
    bool widget_pinvaluetomouse(WidgetScope scope, const std::string& label, const std::string& value);
    bool widget_transfer_function_editor(WidgetScope scope, Parameter& inout_parameter);
};


/** ************************************************************************
 * Defines parameter data structure for graph.
 */
class Parameter {
public:
    typedef std::variant<std::monostate, // default  BUTTON
        bool,                            // BOOL
        float,                           // FLOAT
        int,                             // INT      ENUM
        std::string,                     // STRING   TRANSFERFUNCTION    FILEPATH    FLEXENUM
        vislib::math::Ternary,           // TERNARY
        glm::vec2,                       // VECTOR2F
        glm::vec3,                       // VECTOR3F
        glm::vec4                        // VECTOR4F, COLOR
        >
        ValueType;

    typedef std::variant<std::monostate, // default (unused/unavailable)
        float,                           // FLOAT
        int,                             // INT
        glm::vec2,                       // VECTOR_2f
        glm::vec3,                       // VECTOR_3f
        glm::vec4                        // VECTOR_4f
        >
        MinType;

    typedef std::variant<std::monostate, // default (unused/unavailable)
        float,                           // FLOAT
        int,                             // INT
        glm::vec2,                       // VECTOR_2f
        glm::vec3,                       // VECTOR_3f
        glm::vec4                        // VECTOR_4f
        >
        MaxType;

    typedef std::variant<std::monostate,               // default (unused/unavailable)
        megamol::core::view::KeyCode,                  // BUTTON
        EnumStorageType,                               // ENUM
        megamol::core::param::FlexEnumParam::Storage_t // FLEXENUM
        >
        StroageType;

    struct StockParameter {
        std::string full_name;
        std::string description;
        ParamType type;
        std::string default_value;
        MinType minval;
        MaxType maxval;
        StroageType storage;
        bool gui_visibility;
        bool gui_read_only;
        PresentType gui_presentation;
    };

    // VARIABLES --------------------------------------------------------------

    const ImGuiID uid;
    const ParamType type;
    ParameterPresentation present;

    // Init when adding parameter from stock
    std::string full_name;
    std::string description;

    vislib::SmartPtr<megamol::core::param::AbstractParam> core_param_ptr;

    // FUNCTIONS --------------------------------------------------------------

    Parameter(ImGuiID uid, ParamType type, StroageType store, MinType min, MaxType max);
    ~Parameter(void);

    bool IsValueDirty(void) { return this->value_dirty; }
    void ResetValueDirty(void) { this->value_dirty = false; }
    void ForceSetValueDirty(void) { this->value_dirty = true; }

    static bool ReadNewCoreParameterToStockParameter(megamol::core::param::ParamSlot& in_param_slot,
        megamol::gui::configurator::Parameter::StockParameter& out_param);

    static bool ReadNewCoreParameterToNewParameter(megamol::core::param::ParamSlot& in_param_slot,
        std::shared_ptr<megamol::gui::configurator::Parameter>& out_param, bool set_default_val,
        bool save_core_param_pointer);

    static bool ReadCoreParameterToParameter(vislib::SmartPtr<megamol::core::param::AbstractParam>& in_param_ptr,
        megamol::gui::configurator::Parameter& out_param, bool set_default_val);

    static bool ReadNewCoreParameterToExistingParameter(megamol::core::param::ParamSlot& in_param_slot,
        megamol::gui::configurator::Parameter& out_param, bool set_default_val, bool save_core_param_pointer);

    static bool WriteCoreParameterGUIState(megamol::gui::configurator::Parameter& in_param,
        vislib::SmartPtr<megamol::core::param::AbstractParam>& out_param_ptr);

    static bool WriteCoreParameterValue(megamol::gui::configurator::Parameter& in_param,
        vislib::SmartPtr<megamol::core::param::AbstractParam>& out_param_ptr);

    // Get ----------------------------------

    std::string GetName(void) {
        std::string name = this->full_name;
        auto idx = this->full_name.rfind(':');
        if (idx != std::string::npos) {
            name = name.substr(idx + 1);
        }
        return name;
    }
    std::string GetNameSpace(void) {
        std::string name_space = "";
        auto idx = this->full_name.rfind(':');
        if (idx != std::string::npos) {
            name_space = this->full_name.substr(0, idx - 1);
            name_space.erase(std::remove(name_space.begin(), name_space.end(), ':'), name_space.end());
        }
        return name_space;
    }

    std::string GetValueString(void);

    ValueType& GetValue(void) { return this->value; }

    template <typename T> const T& GetMinValue(void) const { return std::get<T>(this->minval); }

    template <typename T> const T& GetMaxValue(void) const { return std::get<T>(this->maxval); }

    template <typename T> const T& GetStorage(void) const { return std::get<T>(this->storage); }

    bool DefaultValueMismatch(void) { return this->default_value_mismatch; }

    size_t GetTransferFunctionHash(void) const { return this->tf_string_hash; }

    // SET ----------------------------------

    bool SetValueString(const std::string& val_str, bool set_default_val = false);

    template <typename T> void SetValue(T val, bool set_default_val = false) {
        if (std::holds_alternative<T>(this->value)) {

            // Set value
            if (std::get<T>(this->value) != val) {
                this->value = val;
                this->value_dirty = true;

                // Check for new flex enum entry
                if (this->type == ParamType::FLEXENUM) {
                    auto storage = this->GetStorage<megamol::core::param::FlexEnumParam::Storage_t>();
                    storage.insert(std::get<std::string>(this->value));
                    this->SetStorage(storage);
                } else if (this->type == ParamType::TRANSFERFUNCTION) {
                    if constexpr (std::is_same_v<T, std::string>) {
                        this->present.SetTransferFunctionTexture(megamol::gui::TransferFunctionEditor::GetTexture(val));
                        this->tf_string_hash = std::hash<std::string>()(val);
                    }
                }
            }

            // Check default value
            if (set_default_val) {
                this->value_dirty = false;
                this->default_value = val;
                this->default_value_mismatch = false;
            } else {
                try {
                    this->default_value_mismatch = (std::get<T>(this->default_value) != val);
                } catch (...) {
                }
            }
        } else {
            vislib::sys::Log::DefaultLog.WriteError(
                "Bad variant access. [%s, %s, line %d]\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }

    template <typename T> void SetMinValue(T min) {
        if (std::holds_alternative<T>(this->minval)) {
            this->minval = min;
        } else {
            vislib::sys::Log::DefaultLog.WriteError(
                "Bad variant access. [%s, %s, line %d]\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }

    template <typename T> void SetMaxValue(T max) {
        if (std::holds_alternative<T>(this->maxval)) {
            this->maxval = max;
        } else {
            vislib::sys::Log::DefaultLog.WriteError(
                "Bad variant access. [%s, %s, line %d]\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }

    template <typename T> void SetStorage(T store) {
        if (std::holds_alternative<T>(this->storage)) {
            this->storage = store;
        } else {
            vislib::sys::Log::DefaultLog.WriteError(
                "Bad variant access. [%s, %s, line %d]\n", __FILE__, __FUNCTION__, __LINE__);
        }
    }

    // Presentation ----------------------------------------------------

    inline bool PresentGUI(ParameterPresentation::WidgetScope scope) { return this->present.Present(*this, scope); }
    inline float GetGUIHeight(void) { return this->present.GetHeight(*this); }

private:
    // VARIABLES --------------------------------------------------------------

    MinType minval;
    MaxType maxval;
    StroageType storage;
    ValueType value;
    size_t tf_string_hash;
    ValueType default_value;
    bool default_value_mismatch;
    bool value_dirty;
};


} // namespace configurator
} // namespace gui
} // namespace megamol

#endif // MEGAMOL_GUI_GRAPH_PARAMETER_H_INCLUDED
