/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2023 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <inviwo/core/util/settings/systemsettings.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/logstream.h>
#include <inviwo/core/util/stringconversion.h>

namespace inviwo {

SystemSettings::SystemSettings(InviwoApplication* app)
    : Settings("System Settings", app)
    , poolSize_("poolSize", "Pool Size", defaultPoolSize(), 0, 32)
    , enablePortInspectors_("enablePortInspectors", "Enable port inspectors", true)
    , portInspectorSize_("portInspectorSize", "Port inspector size", 128, 1, 1024)
#if __APPLE__
    , enableTouchProperty_("enableTouch", "Enable touch", false)
#else
    , enableTouchProperty_("enableTouch", "Enable touch", true)
#endif
    , enableGesturesProperty_("enableGestures", "Enable gestures", false)
    , enablePickingProperty_("enablePicking", "Enable picking", true)
    , enableSoundProperty_("enableSound", "Enable sound", true)
    , logStackTraceProperty_("logStackTraceProperty", "Error stack trace log", false)
    , moduleSearchPaths_(
          "moduleSearchPaths", "Module Search Paths",
          "The system will look for inviwo module lib in these path to load at start up. "
          "Make sure that the build type matches."_help,
          {}, AcceptMode::Open, FileMode::Directory)
    , runtimeModuleReloading_("runtimeModuleReloding", "Runtime Module Reloading", false)
    , enableResourceManager_("enableResourceManager", "Enable Resource Manager", false)
    , breakOnMessage_{"breakOnMessage",
                      "Break on Message",
                      {MessageBreakLevel::Off, MessageBreakLevel::Error, MessageBreakLevel::Warn,
                       MessageBreakLevel::Info},
                      0}
    , breakOnException_{"breakOnException", "Break on Exception", false}
    , stackTraceInException_{"stackTraceInException", "Create Stack Trace for Exceptions", false}
    , redirectCout_{"redirectCout", "Redirect cout to LogCentral", false}
    , redirectCerr_{"redirectCerr", "Redirect cerr to LogCentral", false} {

    addProperties(poolSize_, enablePortInspectors_, portInspectorSize_, enableTouchProperty_,
                  enableGesturesProperty_, enablePickingProperty_, enableSoundProperty_,
                  logStackTraceProperty_, moduleSearchPaths_, runtimeModuleReloading_,
                  enableResourceManager_, breakOnMessage_, breakOnException_,
                  stackTraceInException_, redirectCout_, redirectCerr_);

    logStackTraceProperty_.onChange(
        [this]() { LogCentral::getPtr()->setLogStacktrace(logStackTraceProperty_.get()); });

    runtimeModuleReloading_.onChange([this]() {
        if (isDeserializing_) return;
        LogInfo("Inviwo needs to be restarted for Runtime Module Reloading change to take effect");
    });

    breakOnMessage_.onChange(
        [this]() { LogCentral::getPtr()->setMessageBreakLevel(breakOnMessage_.get()); });

    redirectCout_.onChange([&]() {
        if (redirectCout_ && !cout_) {
            cout_ = std::make_unique<LogStream>(std::cout, "cout", LogLevel::Info,
                                                LogAudience::Developer);
        } else if (!redirectCout_ && cout_) {
            cout_.reset();
        }
    });

    redirectCerr_.onChange([&]() {
        if (redirectCout_ && !cerr_) {
            cerr_ = std::make_unique<LogStream>(std::cerr, "cerr", LogLevel::Error,
                                                LogAudience::Developer);
        } else if (!redirectCout_ && cerr_) {
            cerr_.reset();
        }
    });

    load();
}

SystemSettings::~SystemSettings() = default;

size_t SystemSettings::defaultPoolSize() { return std::thread::hardware_concurrency() / 2; }

}  // namespace inviwo
