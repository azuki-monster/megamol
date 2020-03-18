/*
 * TableSelectionTx.cpp
 *
 * Copyright (C) 2020 by VISUS (University of Stuttgart)
 * Alle Rechte vorbehalten.
 */

#include "stdafx.h"
#include "TableSelectionTx.h"

#include "mmcore/param/EnumParam.h"
#include "vislib/sys/Log.h"

using namespace megamol::stdplugin::datatools;
using namespace megamol::stdplugin::datatools::table;
using namespace megamol;

TableSelectionTx::TableSelectionTx()
    : core::Module()
    , tableInSlot("getTableIn", "Float table input")
    , flagStorageReadInSlot("readFlagStorageIn", "Flag storage read input")
    , flagStorageWriteInSlot("writeFlagStorageIn", "Flag storage write input")
    , flagStorageReadOutSlot("readFlagStorageOut", "Flag storage read output")
    , flagStorageWriteOutSlot("writeFlagStorageOut", "Flag storage write output")
{
    this->tableInSlot.SetCompatibleCall<TableDataCallDescription>();
    this->MakeSlotAvailable(&this->tableInSlot);

    this->flagStorageReadInSlot.SetCompatibleCall<core::FlagCallRead_GLDescription>();
    this->MakeSlotAvailable(&this->flagStorageReadInSlot);

    this->flagStorageWriteInSlot.SetCompatibleCall<core::FlagCallWrite_GLDescription>();
    this->MakeSlotAvailable(&this->flagStorageWriteInSlot);

    this->flagStorageReadOutSlot.SetCallback(core::FlagCallRead_GL::ClassName(), core::FlagCallRead_GL::FunctionName(core::FlagCallRead_GL::CallGetData), &TableSelectionTx::readDataCallback);
    this->flagStorageReadOutSlot.SetCallback(core::FlagCallRead_GL::ClassName(), core::FlagCallRead_GL::FunctionName(core::FlagCallRead_GL::CallGetMetaData), &TableSelectionTx::readMetaDataCallback);
    this->MakeSlotAvailable(&this->flagStorageReadOutSlot);

    this->flagStorageWriteOutSlot.SetCallback(core::FlagCallWrite_GL::ClassName(), core::FlagCallWrite_GL::FunctionName(core::FlagCallWrite_GL::CallGetData), &TableSelectionTx::writeDataCallback);
    this->flagStorageWriteOutSlot.SetCallback(core::FlagCallWrite_GL::ClassName(), core::FlagCallWrite_GL::FunctionName(core::FlagCallWrite_GL::CallGetMetaData), &TableSelectionTx::writeMetaDataCallback);
    this->MakeSlotAvailable(&this->flagStorageWriteOutSlot);
}

TableSelectionTx::~TableSelectionTx() {
    this->Release();
}

bool TableSelectionTx::create() {
    return true;
}

void TableSelectionTx::release() {
}

bool TableSelectionTx::readDataCallback(core::Call& call) {
    return handleReadCall(call, core::FlagCallRead_GL::CallGetData);
}

bool TableSelectionTx::readMetaDataCallback(core::Call& call) {
    // FlagCall_GL has empty meta data
    return true;
}

bool TableSelectionTx::writeDataCallback(core::Call& call) {
    if (!handleWriteCall(call, core::FlagCallWrite_GL::CallGetData)) {
        return false;
    }

    auto *flagsWriteOutCall = dynamic_cast<core::FlagCallWrite_GL*>(&call);
    auto *tableInCall = this->tableInSlot.CallAs<TableDataCall>();

    tableInCall->SetFrameID(0);
    (*tableInCall)(1);
    (*tableInCall)(0);

    auto flags = flagsWriteOutCall->getData()->flags;
    size_t numberOfFlags = flags->getByteSize() / sizeof(uint32_t);
    size_t numberOfRows = tableInCall->GetRowsCount();
    size_t numberOfCols = tableInCall->GetColumnsCount();

    if (numberOfFlags != numberOfRows) {
        vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR, "TableSelectionTx: invalid table/flag storage size!");
        return false;
    }

    const float *tableInData = tableInCall->GetData();

    uint32_t *flagsData = new uint32_t[numberOfFlags];
    flags->bind();
    glGetBufferSubData(flags->getTarget(), 0, flags->getByteSize(), flagsData);

    core::FlagStorage::FlagItemType testMask = core::FlagStorage::ENABLED | core::FlagStorage::FILTERED;
    core::FlagStorage::FlagItemType passMask = core::FlagStorage::ENABLED;

    std::vector<uint64_t> selected;
    for (size_t i = 0; i < numberOfRows; ++i) {
        if ((flagsData[i] & testMask) == passMask) {
            uint32_t time = static_cast<uint32_t>(tableInData[numberOfCols * i + 0]); // 0 = id of time column
            uint32_t number = static_cast<uint32_t>(tableInData[numberOfCols * i + 1]); // 1 = id of number column
            uint64_t name = (static_cast<uint64_t>(time) << 32u) + static_cast<uint64_t>(number);
            if (flagsData[i] & core::FlagStorage::SELECTED) {
                selected.push_back(name);
            } else {
                // not selected
            }
        }
    }

    delete[] flagsData;

    // TODO send data

    return true;
}

bool TableSelectionTx::writeMetaDataCallback(core::Call& call) {
    // FlagCall_GL has empty meta data
    return true;
}

bool TableSelectionTx::validateCalls() {
    auto *tableInCall = this->tableInSlot.CallAs<TableDataCall>();
    auto *flagsReadInCall = this->flagStorageReadInSlot.CallAs<core::FlagCallRead_GL>();
    auto *flagsWriteInCall = this->flagStorageWriteInSlot.CallAs<core::FlagCallWrite_GL>();

    if (tableInCall == nullptr) {
        vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR, "TableSelectionTx requires a table!");
        return false;
    }

    if (flagsReadInCall == nullptr) {
        vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR, "TableSelectionTx requires a read flag storage!");
        return false;
    }

    if (flagsWriteInCall == nullptr) {
        vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR, "TableSelectionTx requires a write flag storage!");
        return false;
    }

    return true;
}

bool TableSelectionTx::handleReadCall(core::Call& call, unsigned int function) {
    auto *flagsReadOutCall = dynamic_cast<core::FlagCallRead_GL*>(&call);
    if (flagsReadOutCall == nullptr) {
        return false;
    }

    if (!validateCalls()) {
        return false;
    }

    auto *flagsReadInCall = this->flagStorageReadInSlot.CallAs<core::FlagCallRead_GL>();

    (*flagsReadInCall)(function);
    flagsReadOutCall->setData(flagsReadInCall->getData(), flagsReadInCall->version());

    return true;
}

bool TableSelectionTx::handleWriteCall(core::Call& call, unsigned int function) {
    auto *flagsWriteOutCall = dynamic_cast<core::FlagCallWrite_GL*>(&call);
    if (flagsWriteOutCall == nullptr) {
        return false;
    }

    if (!validateCalls()) {
        return false;
    }

    auto *flagsWriteInCall = this->flagStorageWriteInSlot.CallAs<core::FlagCallWrite_GL>();

    flagsWriteInCall->setData(flagsWriteOutCall->getData(), flagsWriteOutCall->version());
    (*flagsWriteInCall)(function);

    return true;
}
