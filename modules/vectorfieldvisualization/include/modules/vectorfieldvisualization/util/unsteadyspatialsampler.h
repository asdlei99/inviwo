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

#pragma once

#include <modules/vectorfieldvisualization/vectorfieldvisualizationmoduledefine.h>
#include <inviwo/core/util/volumesampler.h>

namespace inviwo {

/**
 * \class UnsteadVolumeDoubleSampler
 * \brief Same as VolumeDoubleSampler, but adding a 1.0 to each sample.
 */
class IVW_MODULE_VECTORFIELDVISUALIZATION_API UnsteadVolumeDoubleSampler
    : public VolumeDoubleSampler<3> {
public:
    UnsteadVolumeDoubleSampler(std::shared_ptr<const Volume> vol,
                               CoordinateSpace space = CoordinateSpace::Data,
                               bool periodicTime = false, float maxTime = 0);
    UnsteadVolumeDoubleSampler(const Volume& vol, CoordinateSpace space = CoordinateSpace::Data,
                               bool periodicTime = false, float maxTime = 0);
    virtual ~UnsteadVolumeDoubleSampler() = default;

    virtual Vector<3, double> sampleDataSpace(const dvec3& pos) const override;
    virtual bool withinBoundsDataSpace(const dvec3& pos) const override;
    bool periodicTime_;
    float maxTime_;
};
}  // namespace inviwo
