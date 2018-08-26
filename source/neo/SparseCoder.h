// ----------------------------------------------------------------------------
//  OgmaNeo
//  Copyright(c) 2016-2018 Ogma Intelligent Systems Corp. All rights reserved.
//
//  This copy of OgmaNeo is licensed to you under the terms described
//  in the OGMANEO_LICENSE.md file included in this distribution.
// ----------------------------------------------------------------------------

#pragma once

#include "Helpers.h"

namespace ogmaneo {
    /*!
    \brief Sparse Coder.
    A 2D sparse coding layer, using Hebbian/Anti-Hebbian learning
    */
    class SparseCoder {
    public:
        /*!
        \brief Visible layer descriptor
        */
        struct VisibleLayerDesc {
            /*!
            \brief Visible layer size
            */
            cl_int3 _visibleSize;

            /*!
            \brief Radius onto hidden layer
            */
            cl_int _radius;

            /*!
            \brief Initialize defaults
            */
            VisibleLayerDesc()
                : _visibleSize({ 8, 8, 16 }),
                _radius(2)
            {}
        };

        /*!
        \brief Visible layer
        */
        struct VisibleLayer {
            //!@{
            /*!
            \brief Visible layer values and buffers
            */
            cl::Buffer _weights;

            cl::Buffer _visibleActivations;

            cl_float2 _visibleToHidden;
            cl_float2 _hiddenToVisible;

            cl_int2 _reverseRadii;
            //!@}
        };

    private:
        /*!
        \brief Size of the hidden layer
        */
        cl_int3 _hiddenSize;

        //!@{
        /*!
        \brief Buffers
        */
        cl::Buffer _hiddenCs;

        cl::Buffer _hiddenActivations;
        //!@}

        //!@{
        /*!
        \brief Visible layers and associated descriptors
        */
        std::vector<VisibleLayer> _visibleLayers;
        std::vector<VisibleLayerDesc> _visibleLayerDescs;
        //!@}

        //!@{
        /*!
        \brief Kernels
        */
        cl::Kernel _forwardKernel;
        cl::Kernel _backwardKernel;
        cl::Kernel _inhibitKernel;
        cl::Kernel _learnKernel;
        //!@}

    public:
        /*!
        \brief Feed learning rate
        */
        cl_float _alpha;

        /*!
        \brief Explaining-away iterations
        */
        cl_int _explainIters;

        /*!
        \brief Initialize defaults
        */
        SparseCoder()
        : _alpha(0.01f), _explainIters(4)
        {}

        /*!
        \brief Create a sparse coding layer with random initialization
        \param cs is the ComputeSystem
        \param prog is the ComputeProgram associated with the ComputeSystem and loaded with the sparse coder kernel code
        \param hiddenSize size of the hidden layer
        \param visibleLayerDescs the descriptors for the visible layers
        \param rng a random number generator
        */
        void createRandom(ComputeSystem &cs, ComputeProgram &prog,
            cl_int3 hiddenSize, const std::vector<VisibleLayerDesc> &visibleLayerDescs,
            std::mt19937 &rng);

        /*!
        \brief Activate the sparse coder (perform sparse coding)
        \param cs is the ComputeSystem
        \param visibleCs the visible (input) layer states
        */
        void activate(ComputeSystem &cs, const std::vector<cl::Buffer> &visibleCs);

        /*!
        \brief Learn the sparse code
        \param cs is the ComputeSystem.
        \param visibleCs the visible (input) layer states
        */
        void learn(ComputeSystem &cs, const std::vector<cl::Buffer> &visibleCs);

        /*!
        \brief Get the number of visible layers
        */
        size_t getNumVisibleLayers() const {
            return _visibleLayers.size();
        }

        /*!
        \brief Get a visible layer
        */
        const VisibleLayer &getVisibleLayer(int index) const {
            return _visibleLayers[index];
        }

        /*!
        \brief Get a visible layer descriptor
        */
        const VisibleLayerDesc &getVisibleLayerDesc(int index) const {
            return _visibleLayerDescs[index];
        }

        /*!
        \brief Get the hidden activations (state)
        */
        const cl::Buffer &getHiddenCs() const {
            return _hiddenCs;
        }

        /*!
        \brief Get the hidden size
        */
        cl_int3 getHiddenSize() const {
            return _hiddenSize;
        }

        /*!
        \brief Get a visible layer's feed weights
        */
        const cl::Buffer &getWeights(int v) const {
            return _visibleLayers[v]._weights;
        }
    };
}