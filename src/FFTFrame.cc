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

#include "FFTFrame.h"

namespace naaa {

// Normal constructor: allocates for a given fftSize.
FFTFrame::FFTFrame(unsigned fft_size)
    : fft_size_(fft_size),
      log2fft_size_(static_cast<unsigned>(log2(fft_size))),
      real_data_(nullptr),
      imag_data_(nullptr),
      forward_context_(nullptr),
      inverse_context_(nullptr),
      complex_data_(nullptr) {
  real_data_ = new float[fft_size / 2];
  imag_data_ = new float[fft_size / 2];
  forward_context_ = ContextForSize(fft_size, DFT_R2C);
  inverse_context_ = ContextForSize(fft_size, IDFT_C2R);
  complex_data_ = new float[fft_size];
}

void FFTFrame::Initialize() {}

void FFTFrame::Cleanup() {}

FFTFrame::~FFTFrame() {
  delete real_data_;
  delete imag_data_;
  delete complex_data_;
  av_rdft_end(forward_context_);
  av_rdft_end(inverse_context_);
}

void FFTFrame::DoFFT(const float* data) {
  float* p = complex_data_;
  memcpy(p, data, sizeof(float) * fft_size_);
  av_rdft_calc(forward_context_, p);
  int len = fft_size_ / 2;
  float* real = real_data_;
  float* imag = imag_data_;
  for (int i = 0; i < len; ++i) {
    int base_complex_index = 2 * i;
    real[i] = p[base_complex_index];
    imag[i] = p[base_complex_index + 1];
  }
}

float* FFTFrame::GetUpToDateComplexData() {
  int len = fft_size_ / 2;
  const float* real = real_data_;
  const float* imag = imag_data_;
  float* c = complex_data_;
  for (int i = 0; i < len; ++i) {
    int base_complex_index = 2 * i;
    c[base_complex_index] = real[i];
    c[base_complex_index + 1] = imag[i];
  }
  return const_cast<float*>(complex_data_);
}

RDFTContext* FFTFrame::ContextForSize(unsigned fft_size, int trans) {
  int pow2size = static_cast<int>(log2(fft_size));
  RDFTContext* context = av_rdft_init(pow2size, (RDFTransformType)trans);
  return context;
}

}  // namespace naaa
