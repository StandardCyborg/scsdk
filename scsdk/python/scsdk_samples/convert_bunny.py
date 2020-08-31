import sys
import numpy as np
from scsdk import scsdk_native
import pywavefront

# This script convert the Stanford bunny into a SceneGraph

# root node
root = scsdk_native.Node()
root.name = "root"

# geometry node.
childNode = scsdk_native.GeometryNode()
root.appendChild(childNode)
childNode.name = "bunny"

x_position = 0.3
y_position = 1.2
z_position = 0.7
childNode.transform = np.array([ 
                                [1,0,0,x_position],
                                [0,1,0,y_position],
                                [0,0,1,z_position]])


rabbitObj = pywavefront.Wavefront('./bunny.obj')

for name, material in rabbitObj.materials.items():
    assert("N3F_V3F" == material.vertex_format)

    numVertices = int(len(material.vertices) / 6)
    numFaces = int(numVertices / 3)

    positions = np.zeros((numVertices, 3))
    normals = np.zeros((numVertices, 3))
    colors = np.zeros((numVertices, 3))
    
    for ii in range(numVertices):
        positions[ii, 0] = material.vertices[6 * ii + 3]
        positions[ii, 1] = material.vertices[6 * ii + 4]
        positions[ii, 2] = material.vertices[6 * ii + 5]
        
        normals[ii, 0] = material.vertices[6 * ii + 0]
        normals[ii, 1] = material.vertices[6 * ii + 1]
        normals[ii, 2] = material.vertices[6 * ii + 2]

    colors = normals

    faces = np.zeros((numFaces, 3))
    for iFace in range(numFaces):
        faces[iFace, 0] = iFace * 3 + 0
        faces[iFace, 1] = iFace * 3 + 1
        faces[iFace, 2] = iFace * 3 + 2
        
    # specify the geometry attributes of the geometry node.
    childNode.geometry.positions = positions
    childNode.geometry.normals = normals
    childNode.geometry.colors = colors

    childNode.geometry.faces = faces
    break

scsdk_native.WriteSceneGraphToGltf(root, "./bunny.gltf")
print("created bunny.gltf")