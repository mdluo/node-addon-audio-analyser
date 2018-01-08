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

#ifndef SRC_REALTIMEANALYSER_CC_
#define SRC_REALTIMEANALYSER_CC_

#include "RealtimeAnalyser.h"

namespace naaa {

Nan::Persistent<v8::Function> RealtimeAnalyser::constructor;

NAN_MODULE_INIT(RealtimeAnalyser::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("RealtimeAnalyser").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "WriteInput", WriteInput);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("RealtimeAnalyser").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

RealtimeAnalyser::RealtimeAnalyser() {
}

RealtimeAnalyser::~RealtimeAnalyser() {
}

NAN_METHOD(RealtimeAnalyser::New) {
  RealtimeAnalyser *obj = new RealtimeAnalyser();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(RealtimeAnalyser::WriteInput) {
  char* input_buffer_ = reinterpret_cast<char*>(node::Buffer::Data(info[0]));
  uint32_t frames_to_process = Nan::To<uint32_t>(info[1]).FromJust();
  Nan::MaybeLocal<v8::Object> buffer = Nan::NewBuffer(data, size);

}

}  // namespace naaa

#endif
