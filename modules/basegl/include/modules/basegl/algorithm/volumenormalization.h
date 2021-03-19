/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2021 Inviwo Foundation
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

#include <modules/basegl/baseglmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <modules/opengl/shader/shader.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <modules/opengl/buffer/framebufferobject.h>
#include <array>
#include <string_view>

namespace inviwo {

class IVW_MODULE_BASEGL_API VolumeNormalization {
public:
    template <typename Callback>
    VolumeNormalization(Callback C) : VolumeNormalization() {
        shader_.onReload(C);
    }
    VolumeNormalization();

    virtual ~VolumeNormalization() {}

    std::shared_ptr<Volume> normalize(const Volume& volume);

    void setNormalizeChannel(const size_t channel, const bool normalize);

    void reset();

protected:
    Shader shader_;
    FrameBufferObject fbo_;

private:
    // Some string_view indirection...
    const std::array<std::string_view, 4> defines_{"NORMALIZE_CHANNEL_0", "NORMALIZE_CHANNEL_1",
                                                   "NORMALIZE_CHANNEL_2", "NORMALIZE_CHANNEL_3"};
    bool needsCompilation_;
};

}  // namespace inviwo
