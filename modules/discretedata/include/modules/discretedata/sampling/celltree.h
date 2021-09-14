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

#include <modules/discretedata/connectivity/connectivity.h>
#include <modules/discretedata/channels/datachannel.h>
#include <modules/discretedata/sampling/interpolant.h>
#include <modules/discretedata/sampling/datasetsampler.h>

namespace inviwo {
namespace discretedata {
/**
 * \brief A spatial tree for fast cell location
 * Allows all types of cell in up to 8 dimensions.
 * Following the 2010 paper by Garth and Joy (https://doi.org/10.1109/TVCG.2010.156).
 **/
template <unsigned int SpatialDims>
class CellTree : public DataSetSampler<SpatialDims> {
    static_assert(SpatialDims < 8, "More than 8 dimensions exceed the node capacity.");

public:
    CellTree(std::shared_ptr<const Connectivity> grid,
             std::shared_ptr<const DataChannel<double, SpatialDims>> coordinates,
             const Interpolant<SpatialDims>& interpolant,
             const std::array<float, SpatialDims>& coordsMin,
             const std::array<float, SpatialDims>& coordsMax);
    ~CellTree() = default;
    CellTree(CellTree<SpatialDims>&& tree);
    CellTree(const CellTree<SpatialDims>& tree);
    CellTree& operator=(CellTree<SpatialDims>&& tree);
    CellTree& operator=(const CellTree<SpatialDims>& tree);
    virtual SpatialEntity<SpatialDims>* clone() const override;

    // template <typename T>
    bool sampleCell(ind cellId, const std::array<float, SpatialDims>& pos,
                    std::vector<ind>& vertices, std::vector<double>& weights,
                    InterpolationType interpolationType = InterpolationType::Ignore) const;
    ind locateAndSampleCell(const std::array<float, SpatialDims>& pos,
                            std::vector<double>& returnWeights, std::vector<ind>& returnVertices,
                            InterpolationType interpolationType = InterpolationType::Ignore) const;

    // template <typename T, ind N>
    // ind locateAndSampleCell(const std::array<float, SpatialDims>& pos,
    //                         std::vector<double>& returnWeights, std::vector<ind>& returnVertices,
    //                         InterpolationType interpolationType = InterpolationType::Ignore)
    //                         const;

    virtual std::string getIdentifier() const override;

    virtual Mesh* getDebugMesh() const override;

    // static CellTree* createSampler(
    //     std::shared_ptr<const Connectivity> grid,
    //     std::shared_ptr<const DataChannel<double, SpatialDims>> coordinates,
    //     const Interpolant<SpatialDims>& interpolant);

protected:
    void buildCellTree(std::shared_ptr<const DataChannel<double, SpatialDims>> coordinates);
    struct Node {
        unsigned int dim : 3;
        unsigned int child : 29;  // This index and next.
        bool isLeaf() const { return child == 0; }

        union {
            struct {
                float Lmax, Rmin;
            } node;
            struct {
                unsigned int start, size;
            } leaf;
        };
    };

    // struct CellTreeBuilder {
    //     template <typename T, ind N>
    //     void operator()(const DataChannel<T, N>* positions, CellTree<SpatialDims>* tree);
    // };

    std::vector<Node> nodes_;
    std::vector<ind> cells_;
    // const Interpolant<SpatialDims>* interpolant;
    // ind (CellTree<SpatialDims>::*locateCellFunction)(const std::array<float, SpatialDims>&,
    //                                                  std::vector<double>&, std::vector<ind>&,
    //                                                  InterpolationType) const;

    // Fixed values taken from the paper.
    static constexpr unsigned MAX_CELLS_PER_NODE = 32;
    static constexpr unsigned NUM_SPLIT_BUCKETS = 5;

    using DataSetSamplerBase::coordinates_;
    using DataSetSamplerBase::grid_;
    using DataSetSampler<SpatialDims>::coordsMin_;
    using DataSetSampler<SpatialDims>::coordsMax_;

    // virtual std::array<float, SpatialDims> getMin() const override { return coordsMin_; }
    // virtual std::array<float, SpatialDims> getMax() const override { return coordsMax_; }
};

}  // namespace discretedata
}  // namespace inviwo

#include "celltree.inl"
