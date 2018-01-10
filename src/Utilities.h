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

#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

#include <math.h>

const double piDouble = M_PI;
const float piFloat = static_cast<float>(M_PI);

const double twoPiDouble = piDouble * 2.0;
const float twoPiFloat = piFloat * 2.0f;

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

float DecibelsToLinear(float decibels) {
  return powf(10, 0.05f * decibels);
}

float LinearToDecibels(float linear) {
  return 20 * log10f(linear);
}

#endif  // SRC_UTILITIES_H_
