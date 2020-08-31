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

#include "standard_cyborg/io/pbio/SceneGraphNodePBIO.hpp"


#include "standard_cyborg/sc3d/Polyline.hpp"
#include "standard_cyborg/sc3d/ColorImage.hpp"
#include "standard_cyborg/sc3d/PerspectiveCamera.hpp"
#include "standard_cyborg/sc3d/Geometry.hpp"
#include "standard_cyborg/sc3d/Plane.hpp"
#include "standard_cyborg/sc3d/Landmark.hpp"


#include "standard_cyborg/io/pbio/PolylinePBIO.hpp"
#include "standard_cyborg/io/pbio/ImagePBIO.hpp"
#include "standard_cyborg/io/pbio/PerspectiveCameraPBIO.hpp"
#include "standard_cyborg/io/pbio/GeometryPBIO.hpp"
#include "standard_cyborg/io/pbio/PlanePBIO.hpp"
#include "standard_cyborg/io/pbio/LandmarkPBIO.hpp"

namespace standard_cyborg {
namespace io {
namespace pbio {

extern Result<std::shared_ptr<scene_graph::Node>> FromPB(const standard_cyborg::proto::scene_graph::SceneGraphNode &msg) {
    
    std::shared_ptr<scene_graph::Node> resultNode;
    
    if(msg.has_polyline()) {
        auto maybePolyline = FromPB(msg.polyline());
        
        if(!maybePolyline.IsOk()) {
            return { .error = maybePolyline.error };
        }
        
        sc3d::Polyline polyline = *maybePolyline.value;
        std::shared_ptr<scene_graph::PolylineNode>polylineNode(std::make_shared<scene_graph::PolylineNode>());
        polylineNode->setPolyline(polyline);
        
        resultNode = polylineNode;
    } else if(msg.has_plane()) {
        auto maybePlane = FromPB(msg.plane());
        
        if(!maybePlane.IsOk()) {
            return { .error = maybePlane.error };
        }
        
        sc3d::Plane Plane = *maybePlane.value;
        std::shared_ptr<scene_graph::PlaneNode> planeNode(std::make_shared<scene_graph::PlaneNode>());
        planeNode->setPlane(Plane);
        
        resultNode = planeNode;
    } else if(msg.has_landmark()) {
        auto maybeLandmark = FromPB(msg.landmark());
        
        if(!maybeLandmark.IsOk()) {
            return { .error = maybeLandmark.error };
        }
        
        sc3d::Landmark landmark = *maybeLandmark.value;
        std::shared_ptr<scene_graph::LandmarkNode> landmarkNode(std::make_shared<scene_graph::LandmarkNode>());
        landmarkNode->setLandmark(landmark);
        
        resultNode = landmarkNode;
    }  else if(msg.has_pointcloud()) {
        auto maybeGeometry = FromPB(msg.pointcloud());
        
        if(!maybeGeometry.IsOk()) {
            return { .error = maybeGeometry.error };
        }
        
        sc3d::Geometry geometry = *maybeGeometry.value;
        std::shared_ptr<scene_graph::GeometryNode> geometryNode(std::make_shared<scene_graph::GeometryNode>());
        geometryNode->setGeometry(geometry);
        
        resultNode = geometryNode;
    } else if(msg.has_trianglemesh()) {
        auto maybeGeometry = FromPB(msg.trianglemesh());
        
        if(!maybeGeometry.IsOk()) {
            return { .error = maybeGeometry.error };
        }
        
        sc3d::Geometry geometry = *maybeGeometry.value;
        std::shared_ptr<scene_graph::GeometryNode> geometryNode(std::make_shared<scene_graph::GeometryNode>());
        geometryNode->setGeometry(geometry);
        resultNode = geometryNode;
    } else if(msg.has_pinholecamera()) {
        auto maybePinholeCamera = FromPB(msg.pinholecamera());
        
        if(!maybePinholeCamera.IsOk()) {
            return { .error = maybePinholeCamera.error };
        }
        
        sc3d::PerspectiveCamera perspectiveCamera = *maybePinholeCamera.value;
        std::shared_ptr<scene_graph::PerspectiveCameraNode> perspectiveCameraNode(std::make_shared<scene_graph::PerspectiveCameraNode>());
        perspectiveCameraNode->setPerspectiveCamera(perspectiveCamera);
        resultNode = perspectiveCameraNode;
    } else if(msg.has_image()){
        auto maybeImage = FromPB(msg.image());
        
        if(!maybeImage.IsOk()) {
            return { .error = maybeImage.error };
        }
        
        if(maybeImage.value->color_image) {
            sc3d::ColorImage colorImage = *maybeImage.value->color_image;
            std::shared_ptr<scene_graph::ColorImageNode> colorImageNode(std::make_shared<scene_graph::ColorImageNode>());
            colorImageNode->setColorImage(colorImage);
            resultNode = colorImageNode;
        } else if(maybeImage.value->depth_image) {
            sc3d::DepthImage depthImage = *maybeImage.value->depth_image;
            
            std::shared_ptr<scene_graph::DepthImageNode> depthImageNode(std::make_shared<scene_graph::DepthImageNode>());
            depthImageNode->setDepthImage(depthImage);
            resultNode = depthImageNode;
        } else {
            return { .error = "ParsedImage has neither depth_image nor color_image." };
        }
        
    } else if( msg.isgenericnode()) {
        std::shared_ptr<scene_graph::Node>node(std::make_shared<scene_graph::Node>());
        resultNode = node;
    } else if(msg.iscoordinateframenode()) {
        std::shared_ptr<scene_graph::CoordinateFrameNode>node(std::make_shared<scene_graph::CoordinateFrameNode>());
        resultNode = node;
    } else if(msg.has_pointposition()) {
        std::shared_ptr<scene_graph::PointNode>node(std::make_shared<scene_graph::PointNode>());
        
        auto maybePosition = FromPB(msg.pointposition());
        
        if(!maybePosition.IsOk()) {
            return { .error = maybePosition.error };
        }
        node->setPosition(*maybePosition.value);
        
        resultNode = node;
    } else {
        return { .error = "Unhandled SceneGraph node type" };
    }
    
    resultNode->setName(msg.nodename());
    
    auto maybeObjectColor = FromPB(msg.objectcolor());
    if(!maybeObjectColor.IsOk()) {
        return { .error = maybeObjectColor.error };
    }
    resultNode->getMaterial().objectColor = *maybeObjectColor.value;
    
    resultNode->getMaterial().materialModel = scene_graph::MaterialModel(msg.materialmodel());
    
    return { .value = resultNode };
}

extern Result<standard_cyborg::proto::scene_graph::SceneGraphNode> ToPB(const std::shared_ptr<scene_graph::Node>& sceneGraphNode) {
    
    ::standard_cyborg::proto::scene_graph::SceneGraphNode msg;
    
    switch (sceneGraphNode->getType()) {
            
        case scene_graph::SGNodeType::Geometry:  {
            scene_graph::GeometryNode*  GeometryNode = sceneGraphNode->asGeometryNode();
            
            sc3d::Geometry geometry = GeometryNode->getGeometry();
            
            auto maybeMsg = ToPB(geometry);
            
            if(!maybeMsg.IsOk()) {
                return {.error = maybeMsg.error };
            }
            
            if(maybeMsg.value->point_cloud) {
                *msg.mutable_pointcloud() = *maybeMsg.value->point_cloud;
            } else if(maybeMsg.value->triangle_mesh) {
                *msg.mutable_trianglemesh() = *maybeMsg.value->triangle_mesh;
            } else {
                return {.error = "Neither point_cloud nor triangle_mesh contains a value" };
            }
            
            break;
        }
        case scene_graph::SGNodeType::ColorImage: {
            scene_graph::ColorImageNode*  dolorImageNode = sceneGraphNode->asColorImageNode();
            sc3d::ColorImage ColorImage = dolorImageNode->getColorImage();
            auto maybeMsg = ToPB(ColorImage);
            
            if(!maybeMsg.IsOk()) {
                return {.error = maybeMsg.error };
            }
            
            *msg.mutable_image() = *maybeMsg.value;
            break;
        }
            
        case scene_graph::SGNodeType::DepthImage: {
            scene_graph::DepthImageNode*  depthImageNode = sceneGraphNode->asDepthImageNode();
            sc3d::DepthImage depthImage = depthImageNode->getDepthImage();
            auto maybeMsg = ToPB(depthImage);
            
            if(!maybeMsg.IsOk()) {
                return {.error = maybeMsg.error };
            }
            
            *msg.mutable_image() = *maybeMsg.value;
            break;
        }
            
            
        case scene_graph::SGNodeType::PerspectiveCamera: {
            scene_graph::PerspectiveCameraNode*  perspectiveCameraNode = sceneGraphNode->asPerspectiveCameraNode();
            sc3d::PerspectiveCamera perspectiveCamera = perspectiveCameraNode->getPerspectiveCamera();
            auto maybeMsg = ToPB(perspectiveCamera);
            
            if(!maybeMsg.IsOk()) {
                return {.error = maybeMsg.error };
            }
            
            *msg.mutable_pinholecamera() = *maybeMsg.value;
            break;
        }
            
        case scene_graph::SGNodeType::Plane: {
            scene_graph::PlaneNode*  PlaneNode = sceneGraphNode->asPlaneNode();
            sc3d::Plane Plane = PlaneNode->getPlane();
            auto maybeMsg = ToPB(Plane);
            
            if(!maybeMsg.IsOk()) {
                return {.error = maybeMsg.error };
            }
            
            *msg.mutable_plane() = *maybeMsg.value;
            break;
        }
            
        case scene_graph::SGNodeType::Landmark: {
            scene_graph::LandmarkNode*  landmarkNode = sceneGraphNode->asLandmarkNode();
            sc3d::Landmark landmark = landmarkNode->getLandmark();
            auto maybeMsg = ToPB(landmark);
            
            if(!maybeMsg.IsOk()) {
                return {.error = maybeMsg.error };
            }
            
            *msg.mutable_landmark() = *maybeMsg.value;
            break;
        }
        case scene_graph::SGNodeType::Polyline: {
            scene_graph::PolylineNode*  polylineNode = sceneGraphNode->asPolylineNode();
            
            sc3d::Polyline polyline = polylineNode->getPolyline();
            
            auto maybeMsg = ToPB(polyline);
            
            if(!maybeMsg.IsOk()) {
                return {.error = maybeMsg.error };
            }
            
            *msg.mutable_polyline() = *maybeMsg.value;
            break;
            
        }
            
        case scene_graph::SGNodeType::Generic: {
            msg.set_isgenericnode(true);
            break;
        }
            
        case scene_graph::SGNodeType::CoordinateFrame: {
            msg.set_iscoordinateframenode(true);
            break;
        }
            
        case scene_graph::SGNodeType::Point: {
            scene_graph::PointNode*  pointNode = sceneGraphNode->asPointNode();
            
            auto maybeMsg = ToPB(pointNode->getPosition());
            
            if(!maybeMsg.IsOk()) {
                return {.error = maybeMsg.error };
            }
            
            *msg.mutable_pointposition() = *maybeMsg.value;
            //msg.set_ispointnode(true);
            break;
        }
    }
    
    msg.set_nodename(sceneGraphNode->getName());
    
    auto pbColor = ToPB(sceneGraphNode->getMaterial().objectColor);
    *msg.mutable_objectcolor() = *pbColor.value;
    
    msg.set_materialmodel(int(sceneGraphNode->getMaterial().materialModel));
    
    return { .value = msg };
}


} // namespace pbio
} // namespace io
} // namespace standard_cyborg
