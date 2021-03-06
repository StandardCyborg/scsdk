/*
Copyright 2020 Standard Cyborg

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#pragma once

namespace standard_cyborg {
namespace sc3d {

struct Size2D {
    int width = 0;
    int height = 0;
    
    inline int operator[](int i) const;
};

inline int Size2D::operator[](int i) const
{
    void* thisVoid = (void*)(this);
    return ((int*)thisVoid)[i];
}

inline bool operator==(const Size2D &lhs, const Size2D &rhs) {
    return
        lhs.width == rhs.width &&
        lhs.height == rhs.height;
}

} // namespace sc3d
} // namespace standard_cyborg
