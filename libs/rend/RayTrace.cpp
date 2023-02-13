#include "RayTrace.h"

#include "Draw.h"

float distanceFromCenter( Vector centerMass, Vector rayOrigin, Vector rayDir) {
  Vector originToCenter = centerMass - rayOrigin;
  return size(cross(rayDir, originToCenter)) / size(rayDir);       // Perpendicular distance of point to segment.
}

std::tuple<bool, Vector, Triangle *> getClosestPointOnRay(Scene &scene, Vector ray, Vector rayOrigin){
  Vector intersectionPoint = scene.camera + Vector(99999, 99999, 99999);
  Triangle *intersectionTriangle = new Triangle();
  bool intersected = false;
  float intersectionDistance = 99999;
  for(SceneObject *obj : scene.objects){
    if(distanceFromCenter(obj->centerMass, rayOrigin, ray) < obj->collisionSphereRadius){
      for(Triangle *tri : obj->triangles){
    		Vector e0 = tri->p1 - tri->p0;
    		Vector e1 = tri->p2 - tri->p0;
    		Vector SPVector = rayOrigin - tri->p0;
    		Matrix DEMatrix = transpose(Matrix(-ray, e0, e1));
    		Vector tempSol = inverse(DEMatrix) * SPVector;
    		float t = tempSol.x;
    		float u = tempSol.y;
    		float v = tempSol.z;
    		if(intersectionDistance > t && t > 0.01 && (u >= 0.0) && (u <= 1.0) && (v >= 0.0) && (v <= 1.0) && (u + v) <= 1.0){
          intersectionDistance = t;
          intersectionPoint = tri->p0 + u*e0 + v*e1;
    			intersectionTriangle = tri;
          intersected = true;
    		}
      }
    }
	}
  return std::make_tuple(intersected, intersectionPoint, intersectionTriangle);
}

uint32_t getColourOfTriAtPoint(Vector p, Triangle *tri){
  if(tri->mat->textureSet){

    float AOV0 = size(cross(p-tri->p1, tri->p2-tri->p1))/2; //calculate areas of subtriangles
    float AOV1 = size(cross(p-tri->p2, tri->p0-tri->p2))/2;
    float AOV2 = size(cross(p-tri->p0, tri->p1-tri->p0))/2;
    float totalA = AOV0 + AOV1 + AOV2;	//proportional areas instead of absolute areas
    AOV0 = AOV0 / totalA;
    AOV1 = AOV1 / totalA;
    AOV2 = AOV2 / totalA;

    float textureX = (AOV0*tri->t0.x + AOV1*tri->t1.x + AOV2*tri->t2.x)*tri->mat->texture.width;
    float textureY = (AOV0*tri->t0.y + AOV1*tri->t1.y + AOV2*tri->t2.y)*tri->mat->texture.height;

    float floorX = (float)floor(textureX);
    float floorY = (float)floor(textureY);
    float ceilX = (float)ceil(textureX);
    float ceilY = (float)ceil(textureY);
    Colour q11 = unpackCol(tri->mat->texture.diffuseMap[floorY*tri->mat->texture.width + floorX]);
    Colour q12 = unpackCol(tri->mat->texture.diffuseMap[ceilY*tri->mat->texture.width + floorX]);
    Colour q21 = unpackCol(tri->mat->texture.diffuseMap[floorY*tri->mat->texture.width + ceilX]);
    Colour q22 = unpackCol(tri->mat->texture.diffuseMap[ceilY*tri->mat->texture.width + ceilX]);
    float x2x1 = ceilX - floorX;
    float y2y1 = ceilY - floorY;
    float x2x = ceilX - textureX;
    float y2y = ceilY - textureY;
    float yy1 = textureY - floorY;
    float xx1 = textureX - floorX;
    Colour col;
    col.red = 1.0 / (x2x1 * y2y1) * ( (q11.red * x2x * y2y) + (q21.red * xx1 * y2y) + (q12.red * x2x * yy1) + (q22.red * xx1 * yy1) );
    col.green = 1.0 / (x2x1 * y2y1) * ( (q11.green * x2x * y2y) + (q21.green * xx1 * y2y) + (q12.green * x2x * yy1) + (q22.green * xx1 * yy1) );
    col.blue = 1.0 / (x2x1 * y2y1) * ( (q11.blue * x2x * y2y) + (q21.blue * xx1 * y2y) + (q12.blue * x2x * yy1) + (q22.blue * xx1 * yy1) );

    //Colour col = unpackCol(texture.pixels[round(textureY)*texture.width + round(textureX)]);
    return packCol(col);

  } else {
    return packCol(tri->mat->col);
  }
}

void drawRayTrace(DrawingWindow &window, Scene &scene, Vector pixel){
  float right = -((2*pixel.x)/scene.windowDim.x) + 1.0;
  float down = -((2*pixel.y)/scene.windowDim.y) + 1.0;
  Vector rayVec = scene.cameraAngle * Vector(right, down, -1.0);
  std::tuple<bool, Vector, Triangle *> pointAndTriangle = getClosestPointOnRay(scene, rayVec, scene.camera);
  bool intersected = std::get<0>(pointAndTriangle);
  Vector intersectionPoint = std::get<1>(pointAndTriangle);
  Triangle *intersectionTriangle = std::get<2>(pointAndTriangle);
  if(intersected){
    uint32_t col = getColourOfTriAtPoint(intersectionPoint, intersectionTriangle);
    window.setPixelColour(pixel.x, pixel.y, col);
  }
}
