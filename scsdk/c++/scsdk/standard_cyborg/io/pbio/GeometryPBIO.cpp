

#include "standard_cyborg/io/pbio/GeometryPBIO.hpp"
#include "standard_cyborg/io/pbio/ImagePBIO.hpp"

#include "standard_cyborg/sc3d/Geometry.hpp"

using Tensor = ::standard_cyborg::proto::math::Tensor;
using NumericType = ::standard_cyborg::proto::math::NumericType;

namespace standard_cyborg {
namespace io {
namespace pbio {


Result<sc3d::Geometry> FromPB(const ConvertedGeometry &convertedGeomety) {
    if (convertedGeomety.triangle_mesh.has_value()) {
      const standard_cyborg::proto::sc3d::TriangleMesh& mm  = convertedGeomety.triangle_mesh.value();
      return FromPB(mm);
    } else {
      return {.error = "Empty ConvertedGeometry"};
    }
}

Result<std::vector<math::Vec3>> TensorToPositions(const Tensor &inTensor) {
  const auto &props = inTensor.properties(); 
  const bool isCorrectShape = 
    props.shape_size() == 2 &&
    props.shape(1).size() == 3;
  if (!isCorrectShape) {
    return {.error = 
      "Geometry.position does have not the correct shape" 
    };
  } 
  const int vertexCount = props.shape(0).size();

  const auto &floatvs = inTensor.float_values();
  if (floatvs.size() != vertexCount * 3) {
    return {
      .error = 
        "NUMERIC_TYPE_FLOAT geometry.positions has unexpected number of position values"
    };
  }

  std::vector<math::Vec3> outPositions;
  for (int iVertex = 0; iVertex < vertexCount; ++iVertex) {
    const size_t p =  (iVertex * 3);
    math::Vec3 v(floatvs[p + 0], floatvs[p + 1],floatvs[p + 2]);
    outPositions.push_back(v);        
  }

  return { .value = outPositions };
} 

Result<std::vector<math::Vec3>> TensorToNormals(const Tensor &inTensor) {
  const auto &props = inTensor.properties(); 
  const bool isCorrectShape = 
    props.shape_size() == 2 &&
    props.shape(1).size() == 3;
  if (!isCorrectShape) {
    return {.error = 
      "Geometry.normal does have not the correct shape" 
    };
  } 
  const int vertexCount = props.shape(0).size();

  const auto &floatvs = inTensor.float_values();
  if (floatvs.size() != vertexCount * 3) {
    return {
      .error = 
        "NUMERIC_TYPE_FLOAT geometry.normals has unexpected number of normal values"
    };
  }

  std::vector<math::Vec3> outNormals;

  for (int iVertex = 0; iVertex < vertexCount; ++iVertex) {
    const size_t p =  (iVertex * 3);
    math::Vec3 v(floatvs[p + 0], floatvs[p + 1],floatvs[p + 2]);
    outNormals.push_back(v);        
  }

  return { .value = outNormals };
} 

Result<std::vector<math::Vec3>> TensorToColors(const Tensor &inTensor) {
  const auto &props = inTensor.properties();  
  const bool isCorrectShape = 
    props.shape_size() == 2 &&
    props.shape(1).size() == 3;
  if (!isCorrectShape) {
    return {.error = 
      "Geometry.color does have not the correct shape" 
    };
  } 
  const int vertexCount = props.shape(0).size();

  const auto &floatvs = inTensor.float_values();
  if (floatvs.size() != vertexCount * 3) {
    return {
      .error = 
        "NUMERIC_TYPE_FLOAT geometry.colors has unexpected number of color values"
    };
  }

  std::vector<math::Vec3> outColors;

  for (int iVertex = 0; iVertex < vertexCount; ++iVertex) {
    const size_t p =  (iVertex * 3);
    math::Vec3 v(floatvs[p + 0], floatvs[p + 1],floatvs[p + 2]);
    outColors.push_back(v);        
  }

  return { .value = outColors };
}

extern Result<sc3d::Geometry> FromPB(const standard_cyborg::proto::sc3d::TriangleMesh &msg) {

  std::vector<math::Vec3> geoPositions;  

  if (msg.has_positions()) 
  {
    Result<std::vector<math::Vec3>> result = TensorToPositions(msg.positions());

    if(!result.IsOk()) {
        return {.error = result.error };
    } else {
      geoPositions = *result.value;
    }
  }

  std::vector<math::Vec3> geoNormals;  

  if (msg.has_normals()) 
  {
    Result<std::vector<math::Vec3>> result = TensorToNormals(msg.normals());

    if(!result.IsOk()) {
      return { .error = result.error };
    } else {
      geoNormals = *result.value;
    }
  }

  std::vector<math::Vec3> geoColors;  

  if (msg.has_colors()) 
  {
    Result<std::vector<math::Vec3>> result = TensorToColors(msg.colors());

    if(!result.IsOk()) {
      return { .error = result.error };
    } else {
      geoColors = *result.value;
    }
  }

  std::vector<math::Vec2> geoTexCoords;  

  if (msg.has_tex_coords()) 
  {
    const Tensor &tensorTexCoords = msg.tex_coords();
    const auto &props = tensorTexCoords.properties();

    const bool isCorrectShape = 
      props.shape_size() == 2 &&
      props.shape(1).size() == 2;
    if (!isCorrectShape) {
      return {.error = 
        "Geometry.texCoord does have not the correct shape" 
      };
    }

    const int vertexCount = props.shape(0).size();
    
    const auto &floatvs = tensorTexCoords.float_values();
    if (floatvs.size() != vertexCount * 2) {
      return {
        .error = 
          "NUMERIC_TYPE_FLOAT geometry.texCoords has unexpected number of texCoord values"
      };
    }
    
    for (int iVertex = 0; iVertex < vertexCount; ++iVertex) {
      const size_t p =  (iVertex * 2);
      math::Vec2 v(floatvs[p + 0], floatvs[p + 1]);
      geoTexCoords.push_back(v);        
    }
  }

  std::vector<sc3d::Face3> geoFaces;  

  if (msg.has_faces()) 
  {
    const Tensor &tensorFaces = msg.faces();
    const auto &props = tensorFaces.properties();

    const bool isCorrectShape = 
      props.shape_size() == 2 &&
      props.shape(1).size() == 3;
    if (!isCorrectShape) {
      return {.error = 
        "Geometry.faces does have not the correct shape" 
      };
    }
    const int faceCount = props.shape(0).size();
    
    const auto &intvs = tensorFaces.int32_values();
    if (intvs.size() != faceCount * 3) {
      return {
        .error = 
          "NUMERIC_TYPE_INT32 geometry.faces has unexpected number of face values"
      };
    }
    
    for (int iFace = 0; iFace < faceCount; ++iFace) {
      const size_t p =  (iFace* 3);
      sc3d::Face3 v(intvs[p + 0], intvs[p + 1], intvs[p + 2]);
      geoFaces.push_back(v);        
    }
  }

  sc3d::Geometry geometry;

  if (msg.has_texture()) {
    const ::standard_cyborg::proto::sc3d::Image  &protoImage= msg.texture();

    auto maybeParsedImage = FromPB(protoImage);
    if (!maybeParsedImage.IsOk()) { return {.error = maybeParsedImage.error}; }
    auto maybeColorImage = *maybeParsedImage.value;
    if (!maybeColorImage.color_image.has_value()) {
      return {.error = "geometry.texture had no color image"};
    }
    const sc3d::ColorImage &colorImage = *maybeColorImage.color_image;

    geometry.setTexture(colorImage);
  }

  if(geoPositions.size() > 0) {
    geometry.setPositions(geoPositions);
  }
  
  if(geoNormals.size() > 0) {
    geometry.setNormals(geoNormals);
  }
  
  if(geoColors.size() > 0) {
    geometry.setColors(geoColors);
  }
  
  if(geoTexCoords.size() > 0) {
    geometry.setTexCoords(geoTexCoords);
  }

  if(geoFaces.size() > 0) {
    geometry.setFaces(geoFaces);
  }

  return {.value = geometry};
}

extern Result<sc3d::Geometry> FromPB(const standard_cyborg::proto::sc3d::PointCloud &msg) {

  std::vector<math::Vec3> geoPositions;  

  if (msg.has_positions()) 
  {
    Result<std::vector<math::Vec3>> result = TensorToPositions(msg.positions());

    if(!result.IsOk()) {
    return {.error = result.error };;
    } else {
        geoPositions = *result.value;
    }
  }

  std::vector<math::Vec3> geoNormals;  

  if (msg.has_normals()) 
  {
    Result<std::vector<math::Vec3>> result = TensorToNormals(msg.normals());

    if(!result.IsOk()) {
      return { .error = result.error };
    } else {
      geoNormals = *result.value;
    }
  }

  std::vector<math::Vec3> geoColors;  

  if (msg.has_colors()) 
  {
    Result<std::vector<math::Vec3>> result = TensorToColors(msg.colors());

    if(!result.IsOk()) {
      return { .error = result.error };
    } else {
      geoColors = *result.value;
    }
  }

  sc3d::Geometry geometry;

  if(geoPositions.size() > 0) {
    geometry.setPositions(geoPositions);
  }
  
  if(geoNormals.size() > 0) {
    geometry.setNormals(geoNormals);
  }
  
  if(geoColors.size() > 0) {
    geometry.setColors(geoColors);
  }

  return {.value = geometry};
}

void NormalsToTensor(const std::vector<math::Vec3>& inNormals, Tensor &outTensor) 
{
  const int vertexCount = inNormals.size();

  auto &props = *outTensor.mutable_properties();
  auto &sVertexCount = *props.add_shape();
  sVertexCount.set_name("normalCount");
  sVertexCount.set_size(vertexCount);
  auto &sComponents = *props.add_shape();
  sComponents.set_name("components");
  sComponents.set_size(3);
  sComponents.add_field_names("x");
  sComponents.add_field_names("y");
  sComponents.add_field_names("z");

  outTensor.mutable_properties()->set_numeric_type(
        NumericType::NUMERIC_TYPE_FLOAT);
  auto &tvs = *outTensor.mutable_float_values();
  tvs.Resize(inNormals.size() * 3, 0);
  
  for (int iVertex = 0; iVertex < vertexCount; ++iVertex) {
    const size_t p =  iVertex * 3;
    const auto &normal = inNormals[iVertex];
    tvs[p + 0] = normal.x;
    tvs[p + 1] = normal.y;
    tvs[p + 2] = normal.z;
  }
} 

void PositionsToTensor(const std::vector<math::Vec3>& inPositions, Tensor &outTensor) 
{
  const int vertexCount = inPositions.size();

  auto &props = *outTensor.mutable_properties();  
  auto &sVertexCount = *props.add_shape();
  sVertexCount.set_name("positionCount");
  sVertexCount.set_size(vertexCount); 
  auto &sComponents = *props.add_shape();
  sComponents.set_name("components");
  sComponents.set_size(3);
  sComponents.add_field_names("x");
  sComponents.add_field_names("y");
  sComponents.add_field_names("z");

  outTensor.mutable_properties()->set_numeric_type(
        NumericType::NUMERIC_TYPE_FLOAT); 
  auto &tvs = *outTensor.mutable_float_values();
  tvs.Resize(inPositions.size() * 3, 0);

  for (int iVertex = 0; iVertex < vertexCount; ++iVertex) {
    const size_t p =  iVertex * 3;
    const auto &position = inPositions[iVertex];
    tvs[p + 0] = position.x;
    tvs[p + 1] = position.y;
    tvs[p + 2] = position.z;
  }
} 

void ColorsToTensor(const std::vector<math::Vec3>& inColors, Tensor &outTensor) 
{
  const int vertexCount = inColors.size();

  auto &props = *outTensor.mutable_properties(); 
  auto &sVertexCount = *props.add_shape();
  sVertexCount.set_name("colorCount");
  sVertexCount.set_size(vertexCount); 
  auto &sComponents = *props.add_shape();
  sComponents.set_name("components");
  sComponents.set_size(3);
  sComponents.add_field_names("r");
  sComponents.add_field_names("g");
  sComponents.add_field_names("b");

  outTensor.mutable_properties()->set_numeric_type(
        NumericType::NUMERIC_TYPE_FLOAT); 
  auto &tvs = *outTensor.mutable_float_values();
  tvs.Resize(inColors.size() * 3, 0);

  for (int iVertex = 0; iVertex < vertexCount; ++iVertex) {
    const size_t p =  iVertex * 3;
    const auto &color = inColors[iVertex];
    tvs[p + 0] = color.x;
    tvs[p + 1] = color.y;
    tvs[p + 2] = color.z;
  }
}

Result<ConvertedGeometry> ToPB(const sc3d::Geometry &geometry){
  ConvertedGeometry convertedGeometry;
    
  const int vertexCount = geometry.vertexCount();
  const int faceCount = geometry.faceCount();

  const std::vector<math::Vec3>& geoPositions = geometry.getPositions();
  const std::vector<math::Vec3>& geoNormals = geometry.getNormals();
  const std::vector<math::Vec3>& geoColors = geometry.getColors();
  const std::vector<math::Vec2>& geoTexCoords = geometry.getTexCoords();
  const std::vector<sc3d::Face3>& geoFaces = geometry.getFaces();

  if(geoPositions.size() == 0) {
    return {
        .error = 
          "geometry.getPositions().size() == 0, is an invalid Geometry"
    };
  }
  if(geoPositions.size() != geoNormals.size() && geoNormals.size() != 0) {
    return {
        .error = 
          "geometry.getPositions().size() != geometry.getNormals().size() is an invalid Geometry"
    };
  }
  if(geoPositions.size() != geoColors.size() && geoColors.size() != 0) {
    return {
        .error = 
          "geometry.getPositions().size() != geometry.getColors().size() is an invalid Geometry"
    };
  }

  if(geoPositions.size() != geoTexCoords.size() && geoTexCoords.size() != 0) {
    return {
        .error = 
          "geometry.getPositions().size() != geometry.getTexCoords().size() is an invalid Geometry"
    };
  }


  if(geometry.hasFaces()) {

    convertedGeometry.triangle_mesh = ::standard_cyborg::proto::sc3d::TriangleMesh();

    convertedGeometry.triangle_mesh->set_frame(geometry.getFrame());
      

    if(geoPositions.size() > 0)
    {
      PositionsToTensor(geoPositions, *convertedGeometry.triangle_mesh->mutable_positions());
    } 

    if(geoNormals.size() > 0)
    {
      NormalsToTensor(geoNormals, *convertedGeometry.triangle_mesh->mutable_normals());
    } 
    
    if(geoColors.size() > 0)
    {
      ColorsToTensor(geoColors, *convertedGeometry.triangle_mesh->mutable_colors());
    } 


    if(geoTexCoords.size() > 0)
    {
      Tensor &tensorTexCoords = *convertedGeometry.triangle_mesh->mutable_tex_coords();

      auto &props = *tensorTexCoords.mutable_properties();

      auto &sVertexCount = *props.add_shape();
      sVertexCount.set_name("texCoordCount");
      sVertexCount.set_size(vertexCount);

      auto &sComponents = *props.add_shape();
      sComponents.set_name("components");
      sComponents.set_size(2);
      sComponents.add_field_names("u");
      sComponents.add_field_names("v");

      tensorTexCoords.mutable_properties()->set_numeric_type(
            NumericType::NUMERIC_TYPE_FLOAT);

      auto &tvs = *tensorTexCoords.mutable_float_values();
      tvs.Resize(geoTexCoords.size() * 2, 0);

      for (int iVertex = 0; iVertex < vertexCount; ++iVertex) {
        const size_t p =  iVertex * 2;
        const auto &texCoord = geoTexCoords[iVertex];
        tvs[p + 0] = texCoord.x;
        tvs[p + 1] = texCoord.y;
      }
    } 

    if(geoFaces.size() > 0)
    {
      Tensor &tensorFaces = *convertedGeometry.triangle_mesh->mutable_faces();
    
      auto &props = *tensorFaces.mutable_properties();

      auto &sFaceCount = *props.add_shape();
      sFaceCount.set_name("faceCount");
      sFaceCount.set_size(faceCount);

      auto &sComponents = *props.add_shape();
      sComponents.set_name("components");
      sComponents.set_size(3);
      sComponents.add_field_names("i0");
      sComponents.add_field_names("i1");
      sComponents.add_field_names("i2");
    
      tensorFaces.mutable_properties()->set_numeric_type(
            NumericType::NUMERIC_TYPE_INT32);

      auto &tvs = *tensorFaces.mutable_int32_values();
      tvs.Resize(geoFaces.size() * 3, 0);

      for (int iFace = 0; iFace < geoFaces.size(); ++iFace) {
        const size_t p =  iFace * 3;
        const auto &face = geoFaces[iFace];
        tvs[p + 0] = face[0];
        tvs[p + 1] = face[1];
        tvs[p + 2] = face[2];
      }
    } 

    if(geometry.hasTexture())
    {
        sc3d::ColorImage gt = geometry.getTexture();


        Result<::standard_cyborg::proto::sc3d::Image> result = ToPB(gt);

      ::standard_cyborg::proto::sc3d::Image img = *(result.value);
      *convertedGeometry.triangle_mesh->mutable_texture() = img;    
    }
      
  } else { // PointCloud

    convertedGeometry.point_cloud = ::standard_cyborg::proto::sc3d::PointCloud();
    convertedGeometry.point_cloud->set_frame(geometry.getFrame());
      
    if(geoPositions.size() > 0)
    {
      PositionsToTensor(geoPositions, *convertedGeometry.point_cloud->mutable_positions());
    } 

    if(geoNormals.size() > 0)
    {
      NormalsToTensor(geoNormals, *convertedGeometry.point_cloud->mutable_normals());
    } 
  
    if(geoColors.size() > 0)
    {
      ColorsToTensor(geoColors, *convertedGeometry.point_cloud->mutable_colors());
    } 

  }
    
    

  return {.value = convertedGeometry};

}

} // namespace pbio
} // namespace io
} // namespace standard_cyborg
