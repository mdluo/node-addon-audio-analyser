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

#include "Analyser.h"
#include "Utilities.h"

namespace naaa {

Nan::Persistent<v8::Function> Analyser::constructor;

NAN_MODULE_INIT(Analyser::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("Analyser").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "GetFloatMagnitudeData", GetFloatMagnitudeData);
  Nan::SetPrototypeMethod(tpl, "GetFloatFrequencyData", GetFloatFrequencyData);
  Nan::SetPrototypeMethod(tpl, "GetByteFrequencyData", GetByteFrequencyData);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Analyser").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

Analyser::Analyser(size_t fft_size, double max_decibels, double min_decibels,
  double smoothing_time_constant, float* input_buffer, float* magnitude_buffer)
  : fft_size_(fft_size),
    max_decibels_(max_decibels),
    min_decibels_(min_decibels),
    smoothing_time_constant_(smoothing_time_constant),
    input_buffer_(input_buffer),
    magnitude_buffer_(magnitude_buffer) {
  analysis_frame_ = make_unique<FFTFrame>(fft_size);
}

Analyser::~Analyser() {
}

NAN_METHOD(Analyser::New) {
  Analyser *obj = new Analyser(
    (size_t)Nan::To<uint32_t>(info[0]).FromJust(),
    Nan::To<double>(info[1]).FromJust(),
    Nan::To<double>(info[2]).FromJust(),
    Nan::To<double>(info[3]).FromJust(),
    reinterpret_cast<float*>(node::Buffer::Data(info[4]->ToObject())),
    reinterpret_cast<float*>(node::Buffer::Data(info[5]->ToObject())));
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

namespace {

void ApplyWindow(float* p, size_t n) {
  // Blackman window
  double alpha = 0.16;
  double a0 = 0.5 * (1 - alpha);
  double a1 = 0.5;
  double a2 = 0.5 * alpha;
  for (unsigned i = 0; i < n; ++i) {
    double x = static_cast<double>(i) / static_cast<double>(n);
    double window =
        a0 - a1 * cos(twoPiDouble * x) + a2 * cos(twoPiDouble * 2.0 * x);
    p[i] *= static_cast<float>(window);
  }
}

}  // namespace

void Analyser::DoFFTAnalysis() {
  size_t fft_size = fft_size_;
  float* temp_p = input_buffer_;

  ApplyWindow(temp_p, fft_size);

  analysis_frame_->DoFFT(temp_p);
  float* real_p = analysis_frame_->RealData();
  float* imag_p = analysis_frame_->ImagData();

  imag_p[0] = 0;
  const double magnitude_scale = 1.0 / fft_size;

  double k = smoothing_time_constant_;
  k = std::max(0.0, k);
  k = std::min(1.0, k);

  float* destination = magnitude_buffer_;
  size_t n = fft_size / 2;
  for (size_t i = 0; i < n; ++i) {
    std::complex<double> c(real_p[i], imag_p[i]);
    double scalar_magnitude = abs(c) * magnitude_scale;
    destination[i] = static_cast<float>(
      k * destination[i] + (1 - k) * scalar_magnitude);
  }
}

NAN_METHOD(Analyser::GetFloatMagnitudeData) {
  Analyser *obj =
      Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  obj->DoFFTAnalysis();
}

void Analyser::ConvertFloatToDb(float* destination) {
  size_t len = fft_size_ / 2;
  if (len > 0) {
    const float* source = magnitude_buffer_;
    for (unsigned i = 0; i < len; ++i) {
      float linear_value = source[i];
      double db_mag = LinearToDecibels(linear_value);
      destination[i] = static_cast<float>(db_mag);
    }
  }
}

NAN_METHOD(Analyser::GetFloatFrequencyData) {
  Analyser *obj =
      Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  float* destination_array =
    reinterpret_cast<float*>(node::Buffer::Data(info[0]));
  obj->DoFFTAnalysis();
  obj->ConvertFloatToDb(destination_array);
}

void Analyser::ConvertToByteData(unsigned char* destination_array) {
  size_t len = fft_size_ / 2;
  if (len > 0) {
    const double range_scale_factor = max_decibels_ == min_decibels_
                                          ? 1
                                          : 1 / (max_decibels_ - min_decibels_);
    const double min_decibels = min_decibels_;
    const float* source = magnitude_buffer_;
    unsigned char* destination = destination_array;
    for (unsigned i = 0; i < len; ++i) {
      float linear_value = source[i];
      double db_mag = LinearToDecibels(linear_value);
      double scaled_value =
          UCHAR_MAX * (db_mag - min_decibels) * range_scale_factor;
      if (scaled_value < 0)
        scaled_value = 0;
      if (scaled_value > UCHAR_MAX)
        scaled_value = UCHAR_MAX;
      destination[i] = static_cast<unsigned char>(scaled_value);
    }
  }
}

NAN_METHOD(Analyser::GetByteFrequencyData) {
  Analyser *obj =
      Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  unsigned char* destination_array =
    reinterpret_cast<unsigned char*>(node::Buffer::Data(info[0]));
  obj->DoFFTAnalysis();
  obj->ConvertToByteData(destination_array);
}

}  // namespace naaa
