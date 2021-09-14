/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2019 Inviwo Foundation
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

#include <modules/discretedata/processors/imagefromdataset.h>
#include <modules/discretedata/connectivity/structuredgrid.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <modules/discretedata/channels/formatconversionchannel.h>

namespace inviwo {
namespace discretedata {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ImageFromDataSet::processorInfo_{
    "org.inviwo.ImageFromDataSet",  // Class identifier
    "Image From DataSet",           // Display name
    "DataSet",                      // Category
    CodeState::Experimental,        // Code state
    Tags::None,                     // Tags
};

const ProcessorInfo ImageFromDataSet::getProcessorInfo() const { return processorInfo_; }

ImageFromDataSet::ImageFromDataSet()
    : dataIn_("InDataSet")
    , imageOut_("OutImage", DataFormat<vec4>::get(), false)
    , datasetSamplerName_("datasetSampler", "DataSet Sampler")
    , interpolationType_("interpolationType", "Interpolation Type",
                         {{"nearest", "Nearest Neighbor", InterpolationType::Nearest},
                          {"squared", "Squared Distance", InterpolationType::SquaredDistance},
                          {"linear", "Linear", InterpolationType::Linear}},
                         2)
    , dataChannel_(dataIn_, "channelName", "Data Channel",
                   [&](const std::shared_ptr<const Channel> channel) -> bool {
                       //    return std::dynamic_pointer_cast<const DataChannel<double,
                       //    2>>(ch).get();
                       return channel->getGridPrimitiveType() == GridPrimitive::Vertex &&
                              channel->getNumComponents() ==
                                  static_cast<ind>(dataIn_.getData()->getGrid()->getDimension());
                   })
    //, useVoxelData("useVoxelData", "Use Voxel Data", false)
    // , floatImageOutput("floatImageOutput", "Convert Data to Float 32")
    , imageSize_("imageSize", "Image Size", {200, 100}, {2, 2}, {2048, 2048}) {
    addPort(dataIn_);
    addPort(imageOut_);
    interpolationType_.setCurrentStateAsDefault();

    addProperties(datasetSamplerName_, interpolationType_, dataChannel_, imageSize_);
    // addProperty(useVoxelData);
    // addProperty(floatImageOutput);
}

void ImageFromDataSet::process() {
    // Get data
    if (!dataIn_.isChanged() && !dataIn_.hasData()) return;

    // No samplers in the dataset??
    if (dataIn_.isChanged() &&
        (!dataIn_.hasData() || dataIn_.getData()->getSamplers().size() == 0)) {
        datasetSamplerName_.clearOptions();
        imageOut_.clear();
        return;
    }

    const auto& samplerMap = dataIn_.getData()->getSamplers();

    // Refresh the sampler options.
    if (dataIn_.isChanged() && dataIn_.hasData()) {

        // std::string selected =
        //     datasetSamplerName_.size() ? datasetSamplerName_.getSelectedIdentifier() : "";

        // datasetSamplerName_.clearOptions();
        // for (auto& sampler : samplerMap) {
        //     datasetSamplerName_.addOption(sampler.first, sampler.first);
        // }
        // datasetSamplerName_.setSelectedValue(selected);
        std::vector<std::string> samplerOptions;
        for (auto& sampler : samplerMap) {
            samplerOptions.push_back(sampler.first);
        }
        datasetSamplerName_.replaceOptions(samplerOptions);
    }

    // Something missing to create a SpatialSampler?
    if (!datasetSamplerName_.size() || !dataChannel_.getCurrentChannel()) {
        std::cout << "We didn't get a channel!" << std::endl;
        imageOut_.clear();
        return;
    }

    auto samplerIt = samplerMap.find(datasetSamplerName_.getSelectedValue());
    if (samplerIt == samplerMap.end()) {
        return;
    }

    auto image = sampleImage(dataChannel_.getCurrentChannel(), samplerIt->second);
    if (!image) {
        LogWarn("Invalid sampled image.");
        std::cout << "Invalid image" << std::endl;
        return;
    }

    std::cout << "! Data format II: " << image->getDataFormat()->getString() << std::endl;
    imageOut_.setData(image);
}

// template <typename T, ind N>
std::shared_ptr<inviwo::Image> ImageFromDataSet::sampleImage(
    const std::shared_ptr<const Channel>& channel,
    const std::shared_ptr<const DataSetSamplerBase>& samplerBase) const {

    // using VecType = Vector<N, T>;
    // using CoordVec = Vector<2, double>;
    std::cout << "! Made it to function" << std::endl;

    auto sampler = std::dynamic_pointer_cast<const DataSetSampler<2>>(samplerBase);
    auto dataChannel = std::dynamic_pointer_cast<const DataChannel<double, 2>>(channel);

    if (!dataChannel) {
        dd_detail::CreateFormatConversionChannelToTN<double, 2> dispatcher;
        dataChannel = dispatching::dispatch<std::shared_ptr<DataChannel<double, 2>>,
                                            dispatching::filter::Scalars>(
            channel->getDataFormatId(), dispatcher, channel,
            fmt::format("{}_as_Coordinate", channel->getName()));
    }
    if (!sampler || !dataChannel) {
        LogError("Sampler or data channel of wrong type (expected 2D and double precision).");
        return nullptr;
    }

    std::cout << "! Input can be cast" << std::endl;
    ivec2 imageSize = imageSize_.get();

    // vec4* data = new vec4[imageSize[0] * imageSize[1]];
    auto image =
        std::make_shared<Image>(size2_t(imageSize.x, imageSize.y), DataFormat<vec4>::get());
    std::cout << "! Got image" << std::endl;

    auto layer = image->getColorLayer();
    if (!layer) return nullptr;
    std::cout << "! Got layer" << std::endl;

    auto layerRam =
        dynamic_cast<LayerRAMPrecision<vec4>*>(layer->getEditableRepresentation<LayerRAM>());
    if (!layerRam) return nullptr;
    std::cout << "! Got layer ram" << std::endl;

    auto data = layerRam->getDataTyped();
    // auto data = reinterpret_cast<vec4*>(dataVoid);
    if (!data) return nullptr;
    std::cout << "! Got vec4 data" << std::endl;

    if (!data) {
        LogError("Could not create image");
        std::cout << "! Could not create image" << std::endl;
    }
    std::cout << "! Got data pointer" << std::endl;

    // const auto minBound = sampler->getMin();
    // const auto maxBound = sampler->getMax();
    // dvec2 extent = {maxBound[0] - minBound[0], maxBound[1] - minBound[1]};

    auto spatialSampler = DataSetSpatialSampler2D(sampler, interpolationType_.get(), dataChannel);

    for (size_t y = 0; y < imageSize.y; ++y)
        for (size_t x = 0; x < imageSize.x; ++x)
        // for (size_t y = 0.2 * imageSize[1]; y < 0.8 * imageSize[1]; ++y)
        //     for (size_t x = 0.2 * imageSize[0]; x < 0.8 * imageSize[0]; ++x)

        {
            // size_t x = 40;  // 204;
            // size_t y = 80;  // 150;
            // auto samplePos = offset;
            // samplePos.x += (extent.x * x) / imageSize.x;
            // samplePos.y += (extent.y * y) / imageSize.y;
            // if (x == imageSize.x / 2 && y == imageSize.y / 2) {
            //     std::cout << "Midpoint!" << std::endl;
            // }
            dvec2 samplePos = {double(x) / (imageSize.x - 1), double(y) / (imageSize.y - 1)};
            dvec2 sampled = spatialSampler.sampleDataSpace(samplePos);
            bool color = std::abs(sampled.x) > 0;
            data[x + y * imageSize.x] = {sampled.x, sampled.y, color ? 0.0 : 1.0, 1};

            // if (!color) {
            //     std::cout << "Sampling failed!!! " << x << " - " << y << std::endl;
            //     return nullptr;
            //     // break;
            // }
            // data[x + y * imageSize.x] =
            //     color ? vec4{samplePos.x, samplePos.y, 0, 1} : vec4{0, 0, 1, 1};
            // data[x + y * imageSize.x] = {1, 128.0, 0.1, 1.0};
        }

    std::cout << "! Let's create the image" << std::endl;

    // auto ramData = std::make_shared<LayerRAMPrecision<vec4>>(
    //     data, size2_t(imageSize.x, imageSize.y), LayerType::Color);

    // auto layer = std::make_shared<Layer>(ramData);
    // auto image = std::make_shared<inviwo::Image>(layer);

    std::cout << "! Made image!" << std::endl;
    LogWarn(image->getDataFormat()->getString());

    std::cout << "! Data format: " << image->getDataFormat()->getString() << std::endl;

    // dvec2 min, max;
    // dataChannel->getMinMax(min, max);
    // // image->dataMap_.dataRange = dvec2(min, max);
    // // image->dataMap_.valueRange = dvec2(min, max);
    // mat2 baseMat(extent.x / imageSize.x, 0, 0, extent.y / imageSize.y);
    // // baseMat /= size.x;
    // // image->setBasis(baseMat);
    return image;
}

}  // namespace discretedata
}  // namespace inviwo
