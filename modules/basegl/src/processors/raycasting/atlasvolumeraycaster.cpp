/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2021 Inviwo Foundation
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

#include <modules/basegl/processors/raycasting/atlasvolumeraycaster.h>

#include <inviwo/core/algorithm/boundingbox.h>
#include <array>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo AtlasVolumeRaycaster::processorInfo_{
    "org.inviwo.AtlasVolumeRaycaster",          // Class identifier
    "Atlas Volume Raycaster",                   // Display name
    "Volume Rendering",                         // Category
    CodeState::Experimental,                    // Code state
    Tag::GL | Tag{"Volume"} | Tag{"Raycaster"}  // Tags
};

const ProcessorInfo AtlasVolumeRaycaster::getProcessorInfo() const { return processorInfo_; }

AtlasVolumeRaycaster::AtlasVolumeRaycaster(std::string_view identifier,
                                           std::string_view displayName)
    : VolumeRaycasterBase(identifier, displayName)
    , time_{"time", [this](InvalidationLevel level) { invalidate(level); }}
    , volume_{"volume"}
    , entryExit_{}
    , classify_{volume_.getName()}
    , background_{*this}
    , raycasting_{volume_.getName()}
    , isoTF_{&volume_.volumePort}
    , camera_{"camera", util::boundingBox(volume_.volumePort)}
    , light_{&camera_.camera}
    , positionIndicator_{}
    , sampleTransform_{}
    , atlas_{this, volume_.getName(), &time_} {

    std::array<RaycasterComponent*, 12> comps{
        &time_,       &volume_,          &entryExit_,  &isoTF_,  &atlas_, &classify_,
        &background_, &sampleTransform_, &raycasting_, &camera_, &light_, &positionIndicator_};
    registerComponents(comps);
}

void AtlasVolumeRaycaster::process() { VolumeRaycasterBase::process(); }

}  // namespace inviwo
