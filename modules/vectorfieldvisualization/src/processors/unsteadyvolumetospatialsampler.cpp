/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2020 Inviwo Foundation
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

#include <modules/vectorfieldvisualization/processors/unsteadyvolumetospatialsampler.h>
#include <modules/vectorfieldvisualization/util/unsteadyspatialsampler.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo UnsteadyVolumeToSpatialSampler::processorInfo_{
    "org.inviwo.UnsteadyVolumeToSpatialSampler",  // Class identifier
    "Unsteady Volume To Spatial Sampler",         // Display name
    "Spatial Sampler",                            // Category
    CodeState::Stable,                            // Code state
    Tags::None,                                   // Tags
};

const ProcessorInfo UnsteadyVolumeToSpatialSampler::getProcessorInfo() const {
    return processorInfo_;
}

UnsteadyVolumeToSpatialSampler::UnsteadyVolumeToSpatialSampler()
    : Processor()
    , volume_("volume")
    , sampler_("sampler")
    , periodicTime_("periodicTime", "Periodic Time", false)
    , maxTime_("maxTime", "Max Time", 1, {0, ConstraintBehavior::Immutable},
               {100, ConstraintBehavior::Ignore}, 1, InvalidationLevel::InvalidOutput,
               PropertySemantics::Text) {
    addPorts(volume_, sampler_);
    addProperties(periodicTime_, maxTime_);

    maxTime_.visibilityDependsOn(periodicTime_,
                                 [](const BoolProperty& prop) { return prop.get(); });
}

void UnsteadyVolumeToSpatialSampler::process() {
    auto sampler = std::make_shared<UnsteadVolumeDoubleSampler>(
        volume_.getData(), CoordinateSpace::Data, periodicTime_.get(), maxTime_.get());
    sampler_.setData(sampler);
}

}  // namespace inviwo
