
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

import unittest
import numpy as np
import tempfile
import os

import scsdk
from scsdk import scsdk_native

from scsdk_test.utils import FIXTURES_ROOT

class TestGeometry(unittest.TestCase):

    def test_vertexCount(self):
        g = scsdk_native.Geometry()
        g.positions = np.array([ [0,0,0], [1,0,0], [0,1,0] ])
        self.assertEqual(g.vertexCount(), 3)


    def test_defaultConstructor(self):
        g = scsdk_native.Geometry()
        
        self.assertEqual(g.positions.shape[0], 0)
        self.assertEqual(g.normals.shape[0], 0)
        self.assertEqual(g.colors.shape[0], 0)
        self.assertEqual(g.faces.shape[0], 0)
        
    def test_property(self):
        g = scsdk_native.Geometry()
        
        g.positions = np.array([ [0,0,0], [1,0,0], [0,1,0] ])
        g.normals = np.array([ [0,0,1], [0,0,1], [0,0,1] ])
        g.colors = np.array([ [1,0,0], [1,0,0], [0,1,0] ])
        g.texCoords = np.array([ [0.2,0.3], [0.6,0.1], [0.9,0.6] ])
        g.faces = np.array([ [0,1,2] ])

        g.texture = scsdk_native.ColorImage(width=2, height=2, rgba=np.array([ 
                                [0.0, 0.0, 0.0, 1.0], [0.0, 1.0, 0.0, 1.0],
                                [0.0, 0.0, 0.0, 1.0], [0.3, 0.9, 0.6, 1.0]]))
        
        self.assertTrue( (g.positions == np.array([ [0,0,0], [1,0,0], [0,1,0] ])).all())
        self.assertTrue( (g.normals == np.array([ [0,0,1], [0,0,1], [0,0,1] ])).all())
        self.assertTrue( (g.colors == np.array([ [1,0,0], [1,0,0], [0,1,0] ])).all())
        self.assertTrue( np.allclose(g.texCoords, np.array([ [0.2,0.3], [0.6,0.1], [0.9,0.6] ])))
        self.assertTrue( (g.faces == np.array([ [0,1,2] ])).all())

        self.assertEqual(g.texture.width, 2)
        self.assertEqual(g.texture.height, 2)
        self.assertTrue( np.allclose(g.texture.data, np.array([ [0.0, 0.0, 0.0, 1.0], [0.0, 1.0, 0.0, 1.0], [0.0, 0.0, 0.0, 1.0], [0.3, 0.9, 0.6, 1.0] ])))

        
# test positions normals, colors, faces
class TestNode(unittest.TestCase):

    def test_name(self):
        n = scsdk_native.Node()
        n.name = "foo"
        self.assertEqual(n.name, "foo")

    def test_visibility(self):
        n = scsdk_native.Node()
        n.visibility = False
        self.assertEqual(n.visibility, False)

        n.visibility = True
        self.assertEqual(n.visibility, True)
        


    def test_material(self):
        n = scsdk_native.Node()

        n.material.objectColor = np.array([1.0,0.2,0.3])
        n.material.materialModel = scsdk_native.MaterialModel.Phong

        self.assertEqual(n.material.materialModel, scsdk_native.MaterialModel.Phong)
        self.assertTrue( np.allclose(n.material.objectColor, np.array([1.0,0.2,0.3])))

    def test_transform(self):
        n = scsdk_native.Node()

        n.transform = np.array([[2,0,0,0.9], [0,9,0,0.1], [0,0,13,0.3]])
        self.assertTrue( np.allclose(n.transform,  np.array([[2,0,0,0.9], [0,9,0,0.1], [0,0,13,0.3]])))


    def test_child(self):
        root = scsdk_native.Node()
        root.name = "root"

        child1 = scsdk_native.Node()
        child1.name = "child1"

        child2 = scsdk_native.Node()
        child2.name = "child2"

        grandchild2 = scsdk_native.Node()
        grandchild2.name = "grandchild2"

        self.assertTrue(root.appendChild(child1))
        self.assertTrue(root.appendChild(child2))

        self.assertTrue(child2.appendChild(grandchild2))

        self.assertEqual(root.numChildren(), 2)
        self.assertEqual(child1.numChildren(), 0)
        self.assertEqual(child2.numChildren(), 1)
        self.assertEqual(grandchild2.numChildren(), 0)

        children = list(root.children())

        self.assertEqual(children[0], child1)
        self.assertEqual(children[1], child2)

        children = list(child2.children())
        self.assertEqual(children[0], grandchild2)
        
        children = list(root.children())

        self.assertEqual(children[0], child1)
        self.assertEqual(children[1], child2)

        children = list(child2.children())
        self.assertEqual(children[0], grandchild2)

    def test_remove(self):
        root = scsdk_native.Node()
        root.name = "root"

        child1 = scsdk_native.Node()
        child1.name = "child1"

        child2 = scsdk_native.Node()
        child2.name = "child2"

        grandchild2 = scsdk_native.Node()
        grandchild2.name = "grandchild2"

        root.appendChild(child1)
        root.appendChild(child2)

        child2.appendChild(grandchild2)

        child2.removeChild(grandchild2)

        self.assertEqual(child2.numChildren(), 0)

        scsdk_native.Node.remove(root, child1)
        self.assertEqual(root.numChildren(), 1)

        children = list(root.children())
        self.assertEqual(children[0], child2)

class GeometryNode(unittest.TestCase):
    def test_all(self):
        n = scsdk_native.GeometryNode()

        n.geometry.positions = np.array([ [0,0,0], [1,0,0], [0,1,0] ])
        self.assertTrue( np.allclose(n.geometry.positions, np.array([ [0,0,0], [1,0,0], [0,1,0] ])))
        self.assertTrue( n.isGeometryNode())


class PolylineNode(unittest.TestCase):
    def test_all(self):
        n = scsdk_native.PolylineNode()

        n.polyline.positions = np.array([ [0,0,0], [1,0,0], [0,1,0] ])
        self.assertTrue( np.allclose(n.polyline.positions, np.array([ [0,0,0], [1,0,0], [0,1,0] ])))
        self.assertTrue( n.isPolylineNode())

        self.assertAlmostEqual(n.polyline.length(), 2.4142135623730950, places=6)

class PointNode(unittest.TestCase):
    def test_all(self):
        n = scsdk_native.PointNode()
        self.assertTrue( n.isPointNode())


class CoordinateFrameNode(unittest.TestCase):
    def test_all(self):
        n = scsdk_native.CoordinateFrameNode()
        self.assertTrue( n.isCoordinateFrameNode())


class ColorImage(unittest.TestCase):

    def test_all(self):

        # A 2x2 image
        RGBA_IMG = np.array([ 
                        [0.0, 0.0, 0.0, 1.0], [0.0, 1.0, 0.0, 1.0],
                        [0.0, 0.0, 0.0, 1.0], [0.3, 0.9, 0.6, 1.0]])
        ci = scsdk_native.ColorImage(width=2, height=2, rgba=RGBA_IMG)
        
        self.assertEqual(ci.width, 2)
        self.assertEqual(ci.height, 2)

        self.assertTrue( np.allclose(ci.data, RGBA_IMG) )

class DepthImage(unittest.TestCase):

    def test_all(self):
        ci = scsdk_native.DepthImage(width=2, height=2, depth=np.array([0.2, 0.4, 0.7, 0.9]))
        
        self.assertEqual(ci.width, 2)
        self.assertEqual(ci.height, 2)

        self.assertTrue( np.allclose(ci.data, np.array([0.2, 0.4, 0.7, 0.9])))


class ColorImageNode(unittest.TestCase):
    def test_all(self):
        n = scsdk_native.ColorImageNode()

        n.colorImage = scsdk_native.ColorImage(width=2, height=2, rgba=np.array([ 
                                [0.0, 0.0, 0.0, 1.0], [0.0, 1.0, 0.0, 1.0],
                                [0.0, 0.0, 0.0, 1.0], [0.3, 0.9, 0.6, 1.0]]))

        self.assertEqual(n.colorImage.width, 2)
        self.assertEqual(n.colorImage.height, 2)

        self.assertTrue( np.allclose(n.colorImage.data, np.array([ [0.0, 0.0, 0.0, 1.0], [0.0, 1.0, 0.0, 1.0], [0.0, 0.0, 0.0, 1.0], [0.3, 0.9, 0.6, 1.0] ])))

class DepthImageNode(unittest.TestCase):
    def test_all(self):
        n = scsdk_native.DepthImageNode()

        n.depthImage = scsdk_native.DepthImage(width=2, height=2, depth=np.array([0.1, 0.2, 0.9, 1.0]))

        self.assertEqual(n.depthImage.width, 2)
        self.assertEqual(n.depthImage.height, 2)

        self.assertTrue( np.allclose(n.depthImage.data, np.array([0.1, 0.2, 0.9, 1.0])))

class PerspectiveCamera(unittest.TestCase):
    def test_all(self):
        camera = scsdk_native.PerspectiveCamera()

        # The default camera has I for intrinsics and zeros for most other attributes
        np.testing.assert_equal(
            np.zeros((2, )),
            camera.getIntrinsicMatrixReferenceSize())
        np.testing.assert_equal(
            np.eye(3, 3),
            camera.getNominalIntrinsicMatrix())
        np.testing.assert_equal(
            np.eye(3, 3),
            camera.getIntrinsicMatrix())

        RT = np.zeros((3, 4))
        RT[:3, :3] = np.eye(3, 3)
        np.testing.assert_equal(
            RT,
            camera.getExtrinsicMatrix())
        np.testing.assert_equal(
            RT,
            camera.getOrientationMatrix())

        assert 1. == camera.getFocalLengthScaleFactor()
        assert [] == camera.getLensDistortionCalibration()
        np.testing.assert_equal(
            np.zeros(2),
            camera.getLegacyImageSize())
    
    def test_unproject(self):
        camera = scsdk_native.PerspectiveCamera()
        camera.setFocalLengthScaleFactor(1.019)
        id_transform = np.array([
            [1, 0, 0, 0],
            [0, 1, 0, 0],
            [0, 0, 1, 0],
        ])
        camera.setExtrinsicMatrix(id_transform)

        desired_orientation = np.array([
            [0,  1,  0,  0],
            [1,  0,  0,  0],
            [0,  0, -1,  0],
        ])
        # orientation_matrix = desired_orientation * np.linalg.inv(id_transform)
        camera.setOrientationMatrix(desired_orientation)

        camera.setNominalIntrinsicMatrix(np.array([
            [2874.18774,    0,              1911.88721],
            [0,             2874.18774,     1078.8269],
            [0,             0,                      1],
        ]))

        camera.setIntrinsicMatrixReferenceSize(3840, 2160)
        camera.setLegacyImageSize(320, 180)
    

        depth = 0.2141;
        unprojected_pos = camera.unprojectDepthSample(320, 180, 124, 174, depth)

        np.testing.assert_allclose(
            unprojected_pos,
            np.array([0.073601, 0.030987, 0.214100]),
            rtol=1e-5, atol=0)

class GLTF_IO(unittest.TestCase):
    def test_all(self):
        root = scsdk_native.Node()
        root.name = "root"

        filename = tempfile.NamedTemporaryFile(mode='w', delete=True)                                                 
        scsdk_native.WriteSceneGraphToGltf(root, filename.name)

        with open(filename.name, 'r') as file:
            data = file.read().replace('\n', '')

            readRoot = scsdk_native.ReadSceneGraphFromGltf(data)[0]
            self.assertEqual(readRoot.name, "root")

class RawPLY_IO(unittest.TestCase):
    def test_all(self):
        colorImage = scsdk_native.ColorImage()
        depthImage = scsdk_native.DepthImage()
        camera = scsdk_native.PerspectiveCamera()
        metadata = scsdk_native.RawFrameMetadata()

        FIXTURE_PATH = os.path.join(FIXTURES_ROOT, 'sven-frame-000.ply')
        success = scsdk_native.ReadRawFrameDataFromPLYFile(
                            colorImage,
                            depthImage,
                            camera,
                            metadata,
                            FIXTURE_PATH)
        assert success, "Failed to read %s" % FIXTURE_PATH

        def test_image(img, fixture_path):
            from PIL import Image
            assert os.path.exists(fixture_path), \
                "Missing fixture %s" % fixture_path

            hwc_arr = img.getAsHWCFloatImage()
            # Rescale to [0, 255]
            rescaled = (
                (255. / (hwc_arr.max() - hwc_arr.min())) *
                    (hwc_arr - hwc_arr.min()))
            # rescaled = (255.0 / hwc_arr.max() * (hwc_arr - hwc_arr.min()))
            if rescaled.shape[-1] == 1:
                rescaled = np.tile(rescaled, (1, 1, 3))
            im = Image.fromarray(rescaled.astype(np.uint8))

            # For easier debugging, dump the computed image to a temp file
            # named after the test fixture
            fixture_name = os.path.basename(fixture_path)
            actual_path = os.path.join(
                                tempfile.gettempdir(),
                                fixture_name + '.actual.png')
            im.save(actual_path)

            # Compare files via binary because it's faster and the diff
            # itself isn't typically helpful
            with open(actual_path, 'rb') as f:
                actual_bytes = f.read()
            with open(fixture_path, 'rb') as f:
                expected_bytes = f.read()
            assert actual_bytes == expected_bytes, \
                "Image mismatch %s != %s" % (actual_path, fixture_path)

        test_image(colorImage, os.path.join(FIXTURES_ROOT, 'sven-extracted-color.png'))
        test_image(depthImage, os.path.join(FIXTURES_ROOT, 'sven-extracted-depth.png'))

        # Check the extracted camera
        np.testing.assert_equal(
            np.array([320, 240]),
            camera.getLegacyImageSize())

        np.testing.assert_equal(
            np.array([3088., 2316.]),
            camera.getIntrinsicMatrixReferenceSize())
        np.testing.assert_allclose(
            np.array([
                [2.8811558e+03, 0,             1.5365935e+03],
                [           0., 2.8811558e+03, 1.1493253e+03],
                [           0.,            0.,            1.],
            ]),
            camera.getIntrinsicMatrix())

        RT = np.zeros((3, 4))
        RT[:3, :3] = np.eye(3, 3)
        np.testing.assert_equal(
            RT,
            camera.getExtrinsicMatrix())
        np.testing.assert_equal(
            np.array([
                [ 0.,  1.,  0.,  0.],
                [ 1.,  0.,  0.,  0.],
                [ 0.,  0., -1.,  0.],
            ]),
            camera.getOrientationMatrix())

        assert 1. == camera.getFocalLengthScaleFactor()

        EXPECTED_DISTORTION = [
            -1.18213188216032e-06, 4.204883225611411e-06, 2.144029895134736e-05,
            5.3677387768402696e-05, 0.00010594235936878249, 0.00018480281869415194,
            0.0002979275304824114, 0.00045355973998084664, 0.0006599306361749768,
            0.0009246425470337272, 0.0012540527386590838, 0.0016526894178241491,
            0.0021227302495390177, 0.002663570921868086, 0.003271510824561119,
            0.003939574118703604, 0.00465748505666852, 0.005411807913333178,
            0.006186256185173988, 0.006962165702134371, 0.007719130255281925,
            0.008435776457190514, 0.009090657345950603, 0.009663235396146774,
            0.01013492513448, 0.010490155778825283, 0.010717418044805527,
            0.01081027276813984, 0.010768276639282703, 0.01059783436357975,
            0.0103129418566823, 0.009935847483575344, 0.009497622959315777,
            0.009038666263222694, 0.008609138429164886, 0.008269323036074638,
            0.008089856244623661, 0.00815171655267477, 0.008545768447220325,
            0.009371508844196796, 0.010734451934695244, 0.012741283513605595,
        ]
        np.testing.assert_allclose(
            np.array(EXPECTED_DISTORTION),
            np.array(camera.getLensDistortionCalibration()))

if __name__ == '__main__':
    unittest.main()
    
