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

#ifndef SRC_ANALYSER_CC_
#define SRC_ANALYSER_CC_

#include "Analyser.h"

namespace naaa {

Nan::Persistent<v8::Function> Analyser::constructor;

NAN_MODULE_INIT(Analyser::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("Analyser").ToLocalChecked());

  Nan::SetAccessor(target,
    Nan::New<v8::String>("fftSize").ToLocalChecked(),
    FftSize, SetFftSize);
  Nan::SetAccessor(target,
    Nan::New<v8::String>("maxDecibels").ToLocalChecked(),
    MaxDecibels, SetMaxDecibels);
  Nan::SetAccessor(target,
    Nan::New<v8::String>("minDecibels").ToLocalChecked(),
    MinDecibels, SetMinDecibels);
  Nan::SetAccessor(target,
    Nan::New<v8::String>("smoothingTimeConstant").ToLocalChecked(),
    SmoothingTimeConstant, SetSmoothingTimeConstant);
  Nan::SetAccessor(target,
    Nan::New<v8::String>("inputBuffer").ToLocalChecked(),
    InputBuffer, SetInputBuffer);

  Nan::SetPrototypeMethod(tpl, "GetFloatFrequencyData", GetFloatFrequencyData);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Analyser").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

Analyser::Analyser() {
}

Analyser::~Analyser() {
}

NAN_METHOD(Analyser::New) {
  Analyser *obj = new Analyser();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_GETTER(Analyser::FftSize) {
  Analyser *obj =
    Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  info.GetReturnValue().Set(Nan::New<v8::Number>(obj->fft_size_));
}

NAN_SETTER(Analyser::SetFftSize) {
  if (value->IsNumber()) {
    Analyser *obj =
      Nan::ObjectWrap::Unwrap<Analyser>(info.This());
    obj->fft_size_ = value->Uint32Value();
  }
}

NAN_GETTER(Analyser::MaxDecibels) {
  Analyser *obj =
    Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  info.GetReturnValue().Set(Nan::New<v8::Number>(obj->max_decibels_));
}

NAN_SETTER(Analyser::SetMaxDecibels) {
  if (value->IsNumber()) {
    Analyser *obj =
      Nan::ObjectWrap::Unwrap<Analyser>(info.This());
    obj->max_decibels_ = value->NumberValue();
  }
}

NAN_GETTER(Analyser::MinDecibels) {
  Analyser *obj =
    Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  info.GetReturnValue().Set(Nan::New<v8::Number>(obj->min_decibels_));
}

NAN_SETTER(Analyser::SetMinDecibels) {
  if (value->IsNumber()) {
    Analyser *obj =
      Nan::ObjectWrap::Unwrap<Analyser>(info.This());
    obj->min_decibels_ = value->NumberValue();
  }
}

NAN_GETTER(Analyser::SmoothingTimeConstant) {
  Analyser *obj =
    Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  info.GetReturnValue().Set(
    Nan::New<v8::Number>(obj->smoothing_time_constant_));
}

NAN_SETTER(Analyser::SetSmoothingTimeConstant) {
  if (value->IsNumber()) {
    Analyser *obj =
      Nan::ObjectWrap::Unwrap<Analyser>(info.This());
    obj->smoothing_time_constant_ = value->NumberValue();
  }
}

NAN_GETTER(Analyser::InputBuffer) {
  Analyser *obj =
    Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  info.GetReturnValue().Set(
    Nan::NewBuffer(obj->input_buffer_, obj->fft_size_ * 4).ToLocalChecked());
}

NAN_SETTER(Analyser::SetInputBuffer) {
  Analyser *obj =
    Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  obj->input_buffer_ =
    reinterpret_cast<char*>(node::Buffer::Data(value->ToObject()));
}

void Analyser::DoFFTAnalysis() {
  
}

NAN_METHOD(Analyser::GetFloatFrequencyData) {
  Analyser *obj =
    Nan::ObjectWrap::Unwrap<Analyser>(info.This());
  char* destination_array =
    reinterpret_cast<char*>(node::Buffer::Data(info[0]->ToObject()));
  destination_array[0] = (char)(obj->fft_size_) / 16;
}

}  // namespace naaa

#endif
