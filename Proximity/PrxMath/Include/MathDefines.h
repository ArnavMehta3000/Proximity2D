#pragma once
#include <cstdint>

using U64 = uint64_t;
using U32 = uint32_t;  // This is the standard int
using U16 = uint16_t;
using U8  = uint8_t;

using I64 = int64_t;
using I32 = int32_t;
using I16 = int16_t;
using I8  = int8_t;

using F32 = float;

#define XMVECTOR_LOAD3(varName, value) DX::XMVECTOR varName = DX::XMLoadFloat3(value)
#define XMVECTOR_LOAD4(varName, value) DX::XMVECTOR varName = DX::XMLoadFloat4(value)
#define XMMATRIX_LOAD(varName, value)  DX::XMMATRIX varName = DX::XMLoadFloat4x4(value)