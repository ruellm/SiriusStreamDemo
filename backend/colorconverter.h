#pragma once
#include <stdint.h>

namespace colorconverter{
void Yuv420ProgPlanarToRgb(uint8_t* input, uint8_t* output, const int& width, const int& height);

}
