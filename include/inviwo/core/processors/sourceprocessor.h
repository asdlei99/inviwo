/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017 Inviwo Foundation
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

#ifndef IVW_SOURCEPROCESSOR_H
#define IVW_SOURCEPROCESSOR_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/datatraits.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/processors/processortraits.h>
#include <inviwo/core/ports/inport.h>
#include <inviwo/core/ports/outport.h>
#include <inviwo/core/ports/porttraits.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/utilities.h>

namespace inviwo {

/**
 * Base class for all Source processors
 * @see SourceProcessor
 */
class IVW_CORE_API SourceProcessorBase : public Processor {
public:
    SourceProcessorBase();
    virtual ~SourceProcessorBase() = default;

    /**
     * Inport to be used by the CompositeProcessor to put data into its sub network.
     */
    virtual Inport& getSuperInport() = 0;
};

/**
 * Processor used to connect inports in a sub network inside of a CompositeProcessor to outports in
 * the super network. The CompositeProcessor will find all SourceProcessors in its sub network and
 * add the SourceProcessors super inports to it self. Whenever the sub network gets evaluated the
 * SourceProcessors will get data from its super inport and put in its output. Making the data
 * available to the sub network.
 * @see CompositeProcessor
 * @see SinkProcessor
 */
template <typename InportType, typename OutportType>
class SourceProcessor : public SourceProcessorBase {
public:
    SourceProcessor();
    virtual ~SourceProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;

    /**
     * Inport to be used by the CompositeProcessor to put data into its sub network.
     */
    virtual Inport& getSuperInport() override;

    virtual void serialize(Serializer& s) const override;
    virtual void deserialize(Deserializer& d) override;

    virtual void propagateEvent(Event* event, Outport* source) override;

private:
    InportType superInport_;
    OutportType outport_;
};

template <typename InportType, typename OutportType>
struct ProcessorTraits<SourceProcessor<InportType, OutportType>> {
    static ProcessorInfo getProcessorInfo() {
        using intype = typename InportType::type;
        using outtype = typename InportType::type;
        static_assert(std::is_same<intype, outtype>::value, "type mismatch");
        auto name = util::cleanIdentifier(DataTraits<intype>::dataName() + " Meta Source", " ");
        auto id = util::appendIfNotEmpty(PortTraits<OutportType>::classIdentifier(), ".metasource");
        return {
            id,                 // Class identifier
            name,               // Display name
            "Meta",             // Category
            CodeState::Stable,  // Code state
            "Meta",             // Tags
        };
    }
};

template <typename InportType, typename OutportType>
const ProcessorInfo SourceProcessor<InportType, OutportType>::getProcessorInfo() const {
    return ProcessorTraits<SourceProcessor<InportType, OutportType>>::getProcessorInfo();
}

template <typename InportType, typename OutportType>
SourceProcessor<InportType, OutportType>::SourceProcessor()
    : SourceProcessorBase(), superInport_{"inport"}, outport_{"outport"} {
    addPort(outport_);
    addPortToGroup(&superInport_, "default");
}

template <typename InportType, typename OutportType>
void SourceProcessor<InportType, OutportType>::process() {
    outport_.setData(superInport_.getData());
}

template <typename InportType, typename OutportType>
Inport& SourceProcessor<InportType, OutportType>::getSuperInport() {
    return superInport_;
}

template <typename InportType, typename OutportType>
void SourceProcessor<InportType, OutportType>::serialize(Serializer& s) const {
    SourceProcessorBase::serialize(s);
    // Need to use pointers here, since the port will be serialized in portconnections etc and we
    // what the serialization to know they are the same object.
    auto ptr = &superInport_;
    s.serialize("SuperInport", ptr);
}

template <typename InportType, typename OutportType>
void SourceProcessor<InportType, OutportType>::deserialize(Deserializer& d) {
    SourceProcessorBase::deserialize(d);
    // Need to use pointers here, since the port will be deserialized in portconnections etc and we
    // what the serialization to know they are the same object.
    auto ptr = &superInport_;
    d.deserialize("SuperInport", ptr);
}

template <typename InportType, typename OutportType>
void SourceProcessor<InportType, OutportType>::propagateEvent(Event* event, Outport* source) {
    if (event->hasVisitedProcessor(this)) return;
    event->markAsVisited(this);
    invokeEvent(event);
    if (event->hasBeenUsed()) return;
    if (event->shouldPropagateTo(&superInport_, this, source)) {
        superInport_.propagateEvent(event);
    }
}

}  // namespace inviwo

#endif  // IVW_SOURCEPROCESSOR_H
