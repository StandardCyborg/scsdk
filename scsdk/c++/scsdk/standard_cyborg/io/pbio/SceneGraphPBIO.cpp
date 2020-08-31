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

#include "standard_cyborg/io/pbio/SceneGraphPBIO.hpp"

#include <queue>

#include <standard_cyborg/sc3d/Polyline.hpp>
#include <standard_cyborg/sc3d/Geometry.hpp>
#include <standard_cyborg/sc3d/DepthImage.hpp>
#include <standard_cyborg/sc3d/PerspectiveCamera.hpp>
#include <standard_cyborg/sc3d/Plane.hpp>
#include <standard_cyborg/sc3d/Landmark.hpp>
#include "standard_cyborg/io/pbio/SceneGraphNodePBIO.hpp"
#include <protobag/Utils/PBUtils.hpp>
#include "standard_cyborg/io/pbio/TransformPBIO.hpp"


using standard_cyborg::math::TransformRegistry;
using standard_cyborg::math::Transform;

using namespace standard_cyborg;
namespace sg = standard_cyborg::scene_graph;


namespace standard_cyborg {
namespace io {
namespace pbio {



Result<SceneGraph> FromPB(const standard_cyborg::proto::scene_graph::SceneGraph &msg){
    
    std::vector<std::shared_ptr<scene_graph::Node>> nodes;
    
    for(int iNode = 0; iNode < msg.nodes_size(); ++iNode) {
        const standard_cyborg::proto::scene_graph::SceneGraphNode& pbNode = msg.nodes(iNode);
        
        Result<std::shared_ptr<scene_graph::Node>> maybeNodeMsg = standard_cyborg::io::pbio::FromPB(pbNode);
        
        if(!maybeNodeMsg.IsOk()) {
            return { .error = maybeNodeMsg.error };
        }
        
        std::shared_ptr<scene_graph::Node> node = *maybeNodeMsg.value;
        
        nodes.push_back(node);
        
        //printf("node %d %s\n", iNode, node->getName().c_str())
    }
    
    if(nodes.size() == 0) {
        return { .error = "SceneGraph message contains no nodes" };
    }
    
    std::shared_ptr<scene_graph::Node> rootNode = nodes[0];
    
    for(int iNode = 0; iNode < msg.nodes_size(); ++iNode) {
        
        const standard_cyborg::proto::scene_graph::SceneGraphNode& pbNode = msg.nodes(iNode);
        std::shared_ptr<scene_graph::Node> node = nodes[iNode];
        
        for(int iChild = 0; iChild < pbNode.children_size(); ++iChild) {
            
            int childIndex = pbNode.children(iChild);
            
            node->appendChild(nodes[childIndex]);
        }
        
    }
    
    SceneGraph sceneGraph;
    sceneGraph.sceneGraph = rootNode;
    
    return { .value = sceneGraph };
}


Result<standard_cyborg::proto::scene_graph::SceneGraph>  ToPB(const SceneGraph& sceneGraph)
{
    typedef std::pair<std::shared_ptr<sg::Node>, int> QueueEntry;
    
    std::queue<QueueEntry> queue;
    queue.push(QueueEntry{sceneGraph.sceneGraph, -1});
    
    ::standard_cyborg::proto::scene_graph::SceneGraph sceneGraphMsg;
    
    int nodeIndex = 0;
    while (!queue.empty()) {
        QueueEntry queueEntry = queue.front();
        queue.pop();
        std::shared_ptr<sg::Node> curNode = queueEntry.first;
        
        auto maybeMsg = io::pbio::ToPB(curNode);
        
        if(!maybeMsg.IsOk()) {
            return { .error = maybeMsg.error };
        }
        
        const standard_cyborg::proto::scene_graph::SceneGraphNode    &msg = *maybeMsg.value;
        
        *sceneGraphMsg.add_nodes() = msg;
        
        
        int parentNodeIndex = queueEntry.second;
        int curNodeIndex = (int)sceneGraphMsg.nodes_size() - 1;
        
        if (parentNodeIndex != -1) {
            sceneGraphMsg.mutable_nodes(parentNodeIndex)->add_children(curNodeIndex);
        }
        
        for (int iChild = 0; iChild < curNode->numChildren(); ++iChild) {
            std::shared_ptr<sg::Node> childNode = curNode->getChildSharedPtr(iChild);
            queue.push(QueueEntry{childNode, curNodeIndex});
        }
    }
    
    std::vector<Transform> transforms = sceneGraph.transformRegistry.toList ();
    for(const Transform& transform : transforms) {
        auto maybeTransformMsg = standard_cyborg::io::pbio::ToPB(transform);
        
        if(!maybeTransformMsg.IsOk()) {
            return { .error = maybeTransformMsg.error };
        }
        
        ::standard_cyborg::proto::math::Transform lol = *maybeTransformMsg.value;
        
        *sceneGraphMsg.add_transforms() = lol;
    }
    
    return { .value = sceneGraphMsg };
    
}

} // namespace pbio
} // namespace io
} // namespace standard_cyborg
