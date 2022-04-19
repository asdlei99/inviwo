/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2020 Inviwo Foundation
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

#include <modules/discretedata/sampling/datasetspatialsampler.h>
#include <inviwo/core/util/interpolation.h>
#include <inviwo/core/util/spatialsampler.h>

// #include <modules/base

namespace inviwo {
namespace discretedata {

template <unsigned int SpatialDims, unsigned int DataDims, typename T>
DataSetSpatialSampler<SpatialDims, DataDims, T>::DataSetSpatialSampler(
    std::shared_ptr<const DataSetSampler<SpatialDims>> sampler, InterpolationType interpolationType,
    std::shared_ptr<const DataChannel<T, DataDims>> data, T invalidValue)
    : SpatialSampler<SpatialDims, DataDims, T>(*sampler)
    , interpolationType_(interpolationType)
    , sampler_(sampler)
    , data_(data)
    , invalidValue_(invalidValue) {

    if (interpolationType == InterpolationType::Ignore) {
        interpolationType = InterpolationType::Nearest;
    }
}

template <unsigned int SpatialDims, unsigned int DataDims, typename T>
Vector<DataDims, T> DataSetSpatialSampler<SpatialDims, DataDims, T>::sampleDataSpace(
    const Vector<SpatialDims, double>& posData) const {

    using VecT = glm::vec<DataDims, T>;
    using TypeNT = inviwo::Vector<DataDims, T>;
    // Sample weights by position.
    const auto min = sampler_->getMin();
    const auto max = sampler_->getMax();

    std::array<float, SpatialDims> arrayPos;
    for (size_t i = 0; i < SpatialDims; ++i) {
        arrayPos[i] = (posData[i] * (max[i] - min[i])) + min[i];
    }
    std::vector<double> returnWeights;
    std::vector<ind> returnVertices;
    ind cell =
        sampler_->locateAndSampleCell(arrayPos, returnWeights, returnVertices, interpolationType_);

    VecT result{invalidValue_};

    // If the cell does not exist, return zero vector.
    if (cell < 0 || cell > data_->size()) return util::glm_convert<TypeNT>(result);

    // Add up weighted samples
    VecT sample;
    bool hasValidSample = false;
    result = VecT{0};
    for (auto&& [weight, index] : util::zip(returnWeights, returnVertices)) {
        if (weight == 0) continue;

        data_->fill(sample, index, 1);

        /////////////// removing this for now!!!!!!! ///////////////
        // ------------------------------------------------------ //
        // if (sample[0] < 1.0e-19) {
        //     // std::cout << "= Ran into a border!" << std::endl;
        //     return Vector<DataDims, T>(0.0);
        // }
        // result += sample * weight;
        if (data_->isValid(sample[0])) {
            hasValidSample = true;
            result += (glm::vec<DataDims, double>)sample *
                      weight;  // util::glm_convert<Vector<DataDims, T>>(sample) * weight;
        }
    }
    if (!hasValidSample) return TypeNT{invalidValue_};
    return util::glm_convert<TypeNT>(result);
}

template <unsigned int SpatialDims, unsigned int DataDims, typename T>
bool DataSetSpatialSampler<SpatialDims, DataDims, T>::withinBoundsDataSpace(
    const Vector<SpatialDims, double>& pos) const {

    // Sample weights by position.
    std::array<float, SpatialDims> arrayPos;
    for (size_t i = 0; i < SpatialDims; ++i) arrayPos[i] = pos[i];
    std::vector<double> returnWeights;
    std::vector<ind> returnVertices;
    ind cell =
        sampler_->locateAndSampleCell(arrayPos, returnWeights, returnVertices, interpolationType_);
    return cell >= 0 && returnVertices.size() > 0;
}

}  // namespace discretedata
}  // namespace inviwo
