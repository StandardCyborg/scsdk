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
from PIL import Image

# This script demonstrates a bunch of the different node types in a SceneGraph

root = scsdk.Node()
root.name = "root"

# geometry node, that repsents a geometry that is a single triangle. 
child1 = scsdk.GeometryNode()
child1.name = "triangle1"
child1.geometry.positions = np.array([ [0,0,0], [1,0,0], [0,1,0] ])
child1.geometry.normals = np.array([ [0,0,1], [0,0,1], [0,0,1] ])
child1.geometry.colors = np.array([ [1,0,0], [1,0,0], [0,1,0] ])
child1.geometry.faces = np.array([ [0,1,2] ])
child1.transform = np.array([ 
                                [1,0,0,-3.0],
                                [0,1,0,0],
                                [0,0,1,0]])
root.appendChild(child1)


# geometry node, that repsents a geometry that is a point cloud.
# since no faces are specified, it is considered a point cloud. 
pointcloud = scsdk.GeometryNode()
pointcloud.name = "point cloud"
N = 20
positions = np.zeros((N * N, 3))
colors = np.zeros((N * N, 3))
for y in range(N):
    for x in range(N):
        positions[y * N + x] = [float(x) / N, float(y) / N, 0.0]
        colors[y * N + x] = [float(x) / N, float(y) / N, 0.0]
pointcloud.geometry.positions = positions
pointcloud.geometry.colors = colors
pointcloud.transform = np.array([ 
                                [1,0,0,2.99],
                                [0,1,0,0.19],
                                [0,0,1,0.39]])
root.appendChild(pointcloud)

# This node contains a RGBA image. 
child3 = scsdk.ColorImageNode()
child3.name = "example image"
child3.colorImage = scsdk.ColorImage(width=2, height=2, rgba=np.array([ 
                                [0.0, 0.0, 0.0, 1.0], [0.0, 1.0, 0.0, 1.0],
                                [0.0, 0.0, 0.0, 1.0], [0.3, 0.9, 0.6, 1.0]]))
child3.transform = np.array([ 
                                [1,0,0,-2.0],
                                [0,1,0,2.6],
                                [0,0,1,0.01]])
root.appendChild(child3)

# This shows a geometry with a texture applied.
texturedGeo = scsdk.GeometryNode()
texturedGeo.name = "texturedGeo"
texturedGeo.geometry.positions = np.array([ [0,0,0], [1,0,0], [0,1,0] ])
# remember to add normals, or it will crash!
texturedGeo.geometry.normals = np.array([ [0,0,1], [0,0,1], [0,0,1] ])
texturedGeo.geometry.texCoords = np.array([ [0,0], [1,0], [0,1] ]) # texture coordinates are necessary for textured geometries.
texturedGeo.geometry.faces = np.array([ [0,1,2] ])
texturedGeo.transform = np.array([ 
                                [1,0,0,-3.0],
                                [0,1,0,5],
                                [0,0,1,0]])

RES = 16
rgba = np.zeros((RES * RES, 4))

# create the texture. 
for i in range(RES * RES):
    t = float(i) / float(RES * RES) 

    rgba[i, 0] = 0.3 + 0.5 * t
    rgba[i, 1] = 0.8 - 0.7 *t
    rgba[i, 2] = 1.0 * t
    rgba[i, 3] = 1.0
texturedGeo.geometry.texture = scsdk.ColorImage(width=RES, height=RES, rgba=rgba)

root.appendChild(texturedGeo)

# A depth image, is an image with depth value stored for every pixel. 
depthImageNode = scsdk.DepthImageNode()
depthImageNode.name = "example depth image"
depthImageNode.transform = np.array([ 
                                [1,0,0,0.31],
                                [0,1,0,-2.51],
                                [0,0,1,-0.29]])
RES = 16
depth = np.zeros((RES * RES))

for i in range(RES * RES):
    depth[i] = float(i) / float(RES * RES)
    
depthImageNode.depthImage = scsdk.DepthImage(width=RES, height=RES, depth=depth)
root.appendChild(depthImageNode)



# Load a png, and put it in a ColorImageNode
chickenImageNode = scsdk.ColorImageNode()
chickenImageNode.name = "chicken.png"
chickenImageNode.transform = np.array([ 
                                [1,0,0,-5.0],
                                [0,1,0,0.32],
                                [0,0,1,0.09]])
                                
im = Image.open("chicken.png")

rgba = np.zeros((im.size[1] * im.size[0], 4))
data = list(im.getdata())

for i in range(len(data)):
    rgba[i, 0] = pow(data[i][0] / 255.0, 2.2)
    rgba[i, 1] = pow(data[i][1] / 255.0, 2.2)
    rgba[i, 2] = pow(data[i][2] / 255.0, 2.2)
    rgba[i, 3] = 1.0
    
chickenImageNode.colorImage = scsdk.ColorImage(width=im.size[0], height=im.size[1], rgba=rgba)

root.appendChild(chickenImageNode)



# A polyline node, is a bunch of lines connected, specified by a list of points.
polylineNode = scsdk.PolylineNode()
polylineNode.name = "polyline"
polylineNode.polyline.positions = np.array([ [0,0,0], [1,0,0], [0,1,0], [0,1,1], [1,1,1] ])

polylineNode.transform = np.array([ 
                                [1,0,0,+3.0],
                                [0,1,0,1],
                                [0,0,1,1]])

polylineNode.material.objectColor = np.array([0.0,0.0,1.0])
polylineNode.material.materialModel = scsdk.MaterialModel.Unlit
root.appendChild(polylineNode)




# print the SceneGraph.
def printSceneGraph(node, indent):
    typee = "node"
    output = indent + node.name
    misc = ""
    if node.isGeometryNode():
        typee = "geometry"
        misc = misc +  "vertexCount: " + str(node.geometry.positions.shape[0]) + ", "
        misc = misc + "faceCount: " + str( node.geometry.faces.shape[0])
        
    elif node.isColorImageNode():
        typee = "colorImage"
        misc = misc + "size: " + str(node.colorImage.width) + "x" + str(node.colorImage.height)

    elif node.isDepthImageNode():
        typee = "depthImage"
        misc = misc + "size: " + str(node.depthImage.width) + "x" + str(node.depthImage.height)

    elif node.isPolylineNode():
        typee = "polyline"
        misc = misc + "vertexCount: " + str(node.polyline.positions.shape[0]) 


    output = output + ", type: " + typee + ", misc: " + misc
    print(output)

    for childNode in node.children():
        printSceneGraph(childNode, indent +  "  ")

# write SceneGraph
scsdk.WriteSceneGraphToGltf(root, "./node_types.gltf")

# now read it, and print the SceneGraph
with open("./node_types.gltf", 'r') as file:
    data = file.read().replace('\n', '')

readRoot = scsdk.ReadSceneGraphFromGltf(data)[0]
printSceneGraph(readRoot, "")     

