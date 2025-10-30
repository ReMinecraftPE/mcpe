/********************************************************************
Minecraft: Pocket Edition - Decompilation Project
    Copyright (C) 2023 iProgramInCpp

    The following code is licensed under the BSD 1 clause license.
    SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>
#include "compat/LegacyCPP.hpp"

struct AssetFile {
    int64_t size;
    unsigned char *data;

    AssetFile(): size(-1), data(nullptr) {}
    AssetFile(int64_t size, unsigned char *data): size(size), data(data) {}
};