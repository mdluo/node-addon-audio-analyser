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
  Analyser(size_t fft_size, double max_decibels, double min_decibels,
    double smoothing_time_constant, float* input_buffer,
    float* magnitude_buffer);

  ~Analyser();

  static NAN_METHOD(New);

  void DoFFTAnalysis();
  void ConvertFloatToDb(float* destination);

  static NAN_METHOD(GetFloatFrequencyData);

  static Nan::Persistent<v8::Function> constructor;

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
