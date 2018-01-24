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

#ifndef SRC_ANALYSER_H_
#define SRC_ANALYSER_H_

#include <nan.h>
#include <node_buffer.h>
#include "FFTFrame.h"

namespace naaa {

class Analyser : public Nan::ObjectWrap {
 public:
  static NAN_MODULE_INIT(Init);

 private:
  static Nan::Persistent<v8::Function> constructor;

  Analyser(size_t fft_size, double max_decibels, double min_decibels,
    double smoothing_time_constant, float* input_buffer,
    float* magnitude_buffer);

  ~Analyser();

  static NAN_METHOD(New);

  void DoFFTAnalysis();
  static NAN_METHOD(GetFloatMagnitudeData);
  void ConvertFloatToDb(float* destination);
  static NAN_METHOD(GetFloatFrequencyData);
  void ConvertToByteData(unsigned char* destination);
  static NAN_METHOD(GetByteFrequencyData);

  size_t fft_size_;
  std::unique_ptr<FFTFrame> analysis_frame_;
  double max_decibels_;
  double min_decibels_;
  double smoothing_time_constant_;
  float* input_buffer_;
  float* magnitude_buffer_;
};

}  // namespace naaa

#endif  // SRC_ANALYSER_H_
