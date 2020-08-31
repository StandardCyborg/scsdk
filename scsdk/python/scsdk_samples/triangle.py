# Copyright 2020 Standard Cyborg
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import numpy as np
from scsdk import scsdk_native as scsdk

# this sample creates a SceneGrpah, that simply contains a single triangle geometry.

# root node
root = scsdk.Node()
root.name = "root"

# child node, that is a geometry node.
childNode = scsdk.GeometryNode()
childNode.name = "triangle1"
childNode.geometry.positions = np.array([ [0,0,0], [1,0,0], [0,1,0] ]) # vertex positions of a triangle
childNode.geometry.normals = np.array([ [0,0,1], [0,0,1], [0,0,1] ]) # normals
childNode.geometry.colors = np.array([ [1,0,0], [1,0,0], [1,0,0] ]) # colors
childNode.geometry.faces = np.array([ [0,1,2] ]) # a single triangle face
childNode.transform = np.array([ 
                                [1,0,0,-0.1],
                                [0,1,0,0],
                                [0,0,1,0]]) # a 3x4 transform of the node-

root.appendChild(childNode)

scsdk.WriteSceneGraphToGltf(root, "./triangle.gltf")
print("created triangle.gltf")
