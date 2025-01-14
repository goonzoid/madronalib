// madronalib: a C++ framework for DSP applications.
// Copyright (c) 2020 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#pragma once

#include <array>

#include "MLDSPBuffer.h"
#include "MLDSPProjections.h"

namespace ml
{
inline void mapIndices(float* pDest, size_t size, Projection p)
{
  for (int i = 0; i < size; ++i)
  {
    pDest[i] = p(i);
  }
}

inline void makeWindow(float* pDest, size_t size, Projection windowShape)
{
  auto domainToUnity = projections::linear({0.f, size - 1.f}, {0.f, 1.f});
  mapIndices(pDest, size, compose(windowShape, domainToUnity));
}

namespace windows
{
const Projection rectangle([](float x) { return (x > 0.75f) ? 0.f : ((x < 0.25f) ? 0.f : 1.f); });
const Projection triangle([](float x) { return (x > 0.5f) ? (2.f - 2.f * x) : (2.f * x); });
const Projection raisedCosine([](float x) { return 0.5f - 0.5f * cosf(kTwoPi * x); });
const Projection hamming([](float x) { return 0.54f - 0.46f * cosf(kTwoPi * x); });
const Projection blackman([](float x) {
  return 0.42f - 0.5f * cosf(kTwoPi * x) + 0.08f * cosf(2.f * kTwoPi * x);
});
const Projection flatTop([](float x) {
  const float a0 = 0.21557895;
  const float a1 = 0.41663158;
  const float a2 = 0.277263158;
  const float a3 = 0.083578947;
  const float a4 = 0.006947368;
  return a0 - a1 * cosf(kTwoPi * x) + a2 * cosf(2.f * kTwoPi * x) - a3 * cosf(3.f * kTwoPi * x) +
         a4 * cosf(4.f * kTwoPi * x);
});
}  // namespace windows

// VectorProcessBuffer: utility class to serve a main loop with varying
// arbitrary chunk sizes, buffer inputs and outputs, and compute DSP in
// DSPVector-sized chunks.

template <int IN_CHANNELS, int OUT_CHANNELS, int MAX_FRAMES>
class VectorProcessBuffer
{
  using VectorProcessFn = std::function<DSPVectorArray<OUT_CHANNELS>(
      const DSPVectorArray<IN_CHANNELS>&, void* stateData)>;

  DSPVectorArray<IN_CHANNELS> _inputVectors;
  DSPVectorArray<OUT_CHANNELS> _outputVectors;

 public:
  VectorProcessBuffer()
  {
    for (int i = 0; i < IN_CHANNELS; ++i)
    {
      mInputBuffers[i].resize(MAX_FRAMES);
    }
    for (int i = 0; i < OUT_CHANNELS; ++i)
    {
      mOutputBuffers[i].resize(MAX_FRAMES);
    }
  }

  ~VectorProcessBuffer() {}

  void process(const float** inputs, float** outputs, int nFrames, VectorProcessFn fn,
               void* stateData = nullptr)
  {
    if (nFrames > MAX_FRAMES) return;

    // write from inputs to inputBuffers
    for (int c = 0; c < IN_CHANNELS; c++)
    {
      if (inputs[c])
      {
        mInputBuffers[c].write(inputs[c], nFrames);
      }
    }

    // process
    while (mInputBuffers[0].getReadAvailable() >= kFloatsPerDSPVector)
    {
      // buffers to process input
      for (int c = 0; c < IN_CHANNELS; c++)
      {
        _inputVectors.row(c) = mInputBuffers[c].read();
      }

      _outputVectors = fn(_inputVectors, stateData);

      for (int c = 0; c < OUT_CHANNELS; c++)
      {
        mOutputBuffers[c].write(_outputVectors.row(c));
      }
    }

    // read from outputBuffers to outputs
    for (int c = 0; c < OUT_CHANNELS; c++)
    {
      if (outputs[c])
      {
        mOutputBuffers[c].read(outputs[c], nFrames);
      }
    }
  }

 private:
  std::array<ml::DSPBuffer, IN_CHANNELS> mInputBuffers;
  std::array<ml::DSPBuffer, OUT_CHANNELS> mOutputBuffers;
};

// This is a partial template specialization for a VectorProcessBuffer with
// no inputs, such as a synth.

template <int OUT_CHANNELS, int MAX_FRAMES>
class VectorProcessBuffer<0, OUT_CHANNELS, MAX_FRAMES>
{
  using VectorProcessFn = std::function<DSPVectorArray<OUT_CHANNELS>(void* stateData)>;

  DSPVectorArray<OUT_CHANNELS> _outputVectors;

 public:
  VectorProcessBuffer()
  {
    for (int i = 0; i < OUT_CHANNELS; ++i)
    {
      mOutputBuffers[i].resize(MAX_FRAMES);
    }
  }

  ~VectorProcessBuffer() {}

  void process(const float**, float** outputs, int nFrames, VectorProcessFn fn,
               void* stateData = nullptr)
  {
    if (nFrames > MAX_FRAMES) return;

    // no inputs, process until we have nFrames of output
    while (mOutputBuffers[0].getReadAvailable() < nFrames)
    {
      _outputVectors = fn(stateData);

      for (int c = 0; c < OUT_CHANNELS; c++)
      {
        mOutputBuffers[c].write(_outputVectors.row(c));
      }
    }

    // read from outputBuffers to outputs
    for (int c = 0; c < OUT_CHANNELS; c++)
    {
      if (outputs[c])
      {
        mOutputBuffers[c].read(outputs[c], nFrames);
      }
    }
  }

 private:
  std::array<ml::DSPBuffer, OUT_CHANNELS> mOutputBuffers;
};

// horiz -> vert -> horiz adapters can go here

// turn off denormal math so that (for example) IIR filters don't consume
// many more CPU cycles when they decay.
// thanks to: Dan Gillespie, Chris Santoro
struct FlushToZeroHandler
{
#if defined(__SSE__)
  uint32_t MXCRState = 0;
  
  void SetDenormalsAreZeroAndFlushToZeroOnCPU()
  {
    // Set the DAZ (denormals are zero) and FZ (flush to zero) in the Intel MXCSR register
    MXCRState = _mm_getcsr(); // read the old MXCSR setting
    int newMXCSR = MXCRState | 0x8040; // set DAZ and FZ bits
    _mm_setcsr(newMXCSR); // write the new MXCSR setting to the MXCSR
  }
  
  void UnsetDenormalsAreZeroAndFlushToZeroOnCPU()
  {
    _mm_setcsr(MXCRState);
  }
#elif defined(__aarch64__)
  uint64_t MXCRState = 0;
  
  void SetDenormalsAreZeroAndFlushToZeroOnCPU()
  {
    // read and store floating point control register (FPCR)
    uint64_t FPCR_prev = 0;
    asm volatile ("MRS %0, FPCR " : "=r" ( FPCR_prev));
    MXCRState = FPCR_prev;
    
    // set flush to zero bit and write FPCR
    uint64_t FPCR = FPCR_prev | (1ULL << 24);
    asm volatile ("MSR FPCR, %0 " : : "r" ( FPCR));
  }
  
  void UnsetDenormalsAreZeroAndFlushToZeroOnCPU()
  {
    asm volatile ("MSR FPCR, %0 " : : "r" (MXCRState));
  }
#else
  void SetDenormalsAreZeroAndFlushToZeroOnCPU()
  {
    return;
  }
  
  void UnsetDenormalsAreZeroAndFlushToZeroOnCPU()
  {
    return;
  }
#endif
};

}  // namespace ml
