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

#ifndef SRC_REALTIMEANALYSER_H_
#define SRC_REALTIMEANALYSER_H_

#include <nan.h>

namespace naaa {

class RealtimeAnalyser : public Nan::ObjectWrap {
 public:
    static NAN_MODULE_INIT(Init);
 private:
    RealtimeAnalyser();
    ~RealtimeAnalyser();
    static NAN_METHOD(New);
    static Nan::Persistent<v8::Function> constructor;
};

}  // namespace naaa

#endif  // SRC_REALTIMEANALYSER_H_
