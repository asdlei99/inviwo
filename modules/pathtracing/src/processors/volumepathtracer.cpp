/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023 Inviwo Foundation
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

#include <inviwo/pathtracing/processors/volumepathtracer.h>
#include <modules/opengl/openglmodule.h>
#include <modules/opengl/shader/shadermanager.h>

#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <modules/opengl/texture/texture2d.h>
#include <modules/opengl/image/layergl.h>
#include <modules/opengl/image/imagegl.h>

#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/texture/textureunit.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/volume/volumegl.h>                             // IWYU pragma: keep
#include <modules/opengl/volume/volumeutils.h>                          // for bindAndSetUniforms

#include <inviwo/core/properties/cameraproperty.h>                      // for CameraProperty
#include <inviwo/core/properties/raycastingproperty.h>                  // for RaycastingProperty
#include <inviwo/core/properties/volumeindicatorproperty.h>             // for VolumeIndicatorPr...
#include <inviwo/core/properties/optionproperty.h>                      // for OptionPropertyOption
#include <inviwo/core/algorithm/boundingbox.h>                          // for boundingBox

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumePathTracer::processorInfo_{
    "org.inviwo.VolumePathTracer",  // Class identifier
    "VolumePathTracer",        // Display name
    "Volume",                   // Category
    CodeState::Experimental,       // Code state
    Tags::GL,                    // Tags
    R"(<Explanation of how to use the processor.>)"_unindentHelp};

const ProcessorInfo VolumePathTracer::getProcessorInfo() const { return processorInfo_; }

VolumePathTracer::VolumePathTracer()
    : Processor()
    , volumePort_("Volume")
    , entryPort_("entry")
    , exitPort_("exit")
    , lights_("LightSources")
    //, minMaxOpacity_{"VolumeMinMaxOpacity"}
    , outport_("outport")
    , shader_({{ShaderType::Compute, "bidirectionalvolumepathtracer.comp"}}) 
    //, lighting_("lighting", "Lighting", &camera_)
    , channel_("channel", "Render Channel", {{"Channel 1", "Channel 1", 0}}, 0)
    , raycasting_("raycaster", "Raycasting")
    , camera_("camera", "Camera", util::boundingBox(volumePort_))
    , positionIndicator_("positionindicator", "Position Indicator")
    
    {
    addPort(volumePort_, "VolumePortGroup");
    addPort(entryPort_, "ImagePortGroup1");
    addPort(exitPort_, "ImagePortGroup1");
    addPort(outport_, "ImagePortGroup1");
    addPort(lights_, "LightPortGroup");
    //addPort(minMaxOpacity_);
    //minMaxOpacity_.setOptional(true);
    /*
    volumePort_.onChange([this]() { invalidateProgressiveRendering(); });
    entryPort_.onChange([this]() { invalidateProgressiveRendering(); });
    exitPort_.onChange([this]() { invalidateProgressiveRendering(); });
    lights_.onChange([this]() { updateLightSources(); });
    */
    /*
    minMaxOpacity_.onConnect([this]() { partitionedTransmittance_.setVisible(true); });
    minMaxOpacity_.onDisconnect([this]() {
        partitionedTransmittance_.set(false);
        partitionedTransmittance_.setVisible(false);
    }
    */
    
    channel_.setSerializationMode(PropertySerializationMode::All);
    // from volumeraycasetr.cpp TODO: What does this mean exactly?
    volumePort_.onChange([this]() {
        if (volumePort_.hasData()) {
            size_t channels = volumePort_.getData()->getDataFormat()->getComponents();

            if (channels == channel_.size()) return;

            std::vector<OptionPropertyIntOption> channelOptions;
            for (size_t i = 0; i < channels; i++) {
                channelOptions.emplace_back("Channel " + toString(i + 1),
                                            "Channel " + toString(i + 1), static_cast<int>(i));
            }
            channel_.replaceOptions(channelOptions);
            channel_.setCurrentStateAsDefault();
        }
    });

    // from volumeraycasetr.cpp TODO: What does this mean exactly?
    raycasting_.gradientComputation_.onChange([this]() {
        if (channel_.size() == 4) {
            if (raycasting_.gradientComputation_.get() ==
                RaycastingProperty::GradientComputation::PrecomputedXYZ) {
                channel_.set(3);
            } else if (raycasting_.gradientComputation_.get() ==
                       RaycastingProperty::GradientComputation::PrecomputedYZW) {
                channel_.set(0);
            }
        }
    });
    
    addProperty(channel_);
    addProperty(raycasting_);

    addProperty(camera_);
    addProperty(positionIndicator_);
    
    // What can we set up before process
    shader_.onReload([this]() { invalidate(InvalidationLevel::InvalidOutput); } );

    //exitPort_.setOptional(true);
    lights_.setOptional(true);
    volumePort_.setOptional(true);
}

void VolumePathTracer::initializeResources() {
    //utilgl::addDefines(shader_, raycasting_, /*isotfComposite_,*/camera_, /*lighting_,*/
    //                   positionIndicator_);
    //utilgl::addShaderDefinesBGPort(shader_, backgroundPort_);
    shader_.build();
}

// TODO: Copy volumeraycaster and daniels bidirVolumePathTracer
void VolumePathTracer::process() {

    shader_.activate();

    TextureUnitContainer units;
    //utilgl::bindAndSetUniforms(shader_, units, volume, "volume");
    //utilgl::bindAndSetUniforms(shader_, units, isotfComposite_);
    /* utilgl::bindAndSetUniforms(shader_, units, outport_, ImageType::ColorDepthPicking); */{
        TextureUnit unit1, unit2, unit3;
        auto image = outport_.getEditableData();
        auto colorLayerGL = image->getColorLayer()->getEditableRepresentation<LayerGL>();
        colorLayerGL->bindImageTexture(unit1, GL_WRITE_ONLY);
        auto depthLayerGL = image->getDepthLayer()->getEditableRepresentation<LayerGL>();
        depthLayerGL->bindImageTexture(unit2, GL_WRITE_ONLY);
        auto pickingLayerGL = image->getPickingLayer()->getEditableRepresentation<LayerGL>();
        pickingLayerGL->bindImageTexture(unit3, GL_WRITE_ONLY);
        
        shader_.setUniform("outportColor", unit1);
        shader_.setUniform("outportDepth", unit2);
        shader_.setUniform("outportPicking", unit3);
        
        units.push_back(std::move(unit1));
        units.push_back(std::move(unit2));
        units.push_back(std::move(unit3));

        StrBuffer buff;
        utilgl::setShaderUniforms(shader_, *image, buff.replace("{}Parameters", outport_.getIdentifier()));
    }
    utilgl::bindAndSetUniforms(shader_, units, entryPort_, ImageType::ColorDepthPicking);
    utilgl::bindAndSetUniforms(shader_, units, exitPort_, ImageType::ColorDepth);

    
    utilgl::setUniforms(shader_, camera_/*, lighting_*/, raycasting_, positionIndicator_,
                        channel_/*, isotfComposite_*/);


    glDispatchCompute(512/16, 512/16, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    shader_.deactivate();
}

void VolumePathTracer::dispatchPathTracerComputeShader(LayerGL* entryGL, LayerGL* exitGL, LayerGL* outportGL) {
    shader_.activate();

    
    entryGL->getTexture()->bind();
    shader_.setUniform("entry", 0);

    exitGL->getTexture()->bind();
    shader_.setUniform("exit", 1);

    outportGL->getTexture()->bind();
    shader_.setUniform("outImg", 2);

    glDispatchCompute(512/16, 512/16, 1);

    shader_.deactivate();
}

}  // namespace inviwo
