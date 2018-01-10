/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 2018 Mingdong Luo (github.com/mdluo)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 */

#ifndef SRC_FFTFRAME_H_
#define SRC_FFTFRAME_H_

extern "C" {
#include <libavcodec/avfft.h>
}

#include <complex>

struct RDFTContext;

namespace naaa {

class FFTFrame {
 public:
  explicit FFTFrame(unsigned fft_size);
  FFTFrame();
  FFTFrame(const FFTFrame& frame);
  ~FFTFrame();

  static void Initialize();
  static void Cleanup();
  void DoFFT(const float* data);
  void DoInverseFFT(float* data);

  float* RealData() const { return real_data_; }
  float* ImagData() const { return imag_data_; }

  unsigned FftSize() const { return fft_size_; }
  unsigned Log2FFTSize() const { return log2fft_size_; }

  // CROSS-PLATFORM
  // The remaining public methods have cross-platform implementations:

  // Interpolates from frame1 -> frame2 as x goes from 0.0 -> 1.0
  static std::unique_ptr<FFTFrame> CreateInterpolatedFrame(
     const FFTFrame& frame1,
     const FFTFrame& frame2,
     double x);
  // zero-padding with dataSize <= fftSize
  void DoPaddedFFT(const float* data, size_t data_size);
  double ExtractAverageGroupDelay();
  void AddConstantGroupDelay(double sample_frame_delay);
  // multiplies ourself with frame : effectively operator*=()
  void Multiply(const FFTFrame&);

 private:
  void InterpolateFrequencyComponents(const FFTFrame& frame1,
                                     const FFTFrame& frame2,
                                     double x);

  unsigned fft_size_;
  unsigned log2fft_size_;
  float* real_data_;
  float* imag_data_;

  static RDFTContext* ContextForSize(unsigned fft_size, int trans);
  RDFTContext* forward_context_;
  RDFTContext* inverse_context_;
  float* GetUpToDateComplexData();
  float* complex_data_;
};

}  // namespace naaa

#endif  // SRC_FFTFRAME_H_
