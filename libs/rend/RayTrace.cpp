#include "RayTrace.h"

#include "Draw.h"

std::tuple<bool, Vector, Triangle> getClosestPointOnRay(Scene &scene, Vector ray, Vector rayOrigin){
  Vector intersectionPoint = scene.camera + Vector(99999, 99999, 99999);
  Triangle intersectionTriangle = Triangle();
  bool intersected = false;
  float intersectionDistance = 99999;
  for(SceneObject obj : scene.objects){
    for(Triangle tri : obj.triangles){
  		Vector e0 = tri.p1 - tri.p0;
  		Vector e1 = tri.p2 - tri.p0;
  		Vector SPVector = rayOrigin - tri.p0;
  		Matrix DEMatrix = transpose(Matrix(-ray, e0, e1));
  		Vector tempSol = inverse(DEMatrix) * SPVector;
  		float t = tempSol.x;
  		float u = tempSol.y;
  		float v = tempSol.z;
  		if(intersectionDistance > t && t > 0.01 && (u >= 0.0) && (u <= 1.0) && (v >= 0.0) && (v <= 1.0) && (u + v) <= 1.0){
        intersectionDistance = t;
        intersectionPoint = tri.p0 + u*e0 + v*e1;
  			intersectionTriangle = tri;
        intersected = true;
  		}
    }
	}
  return std::make_tuple(intersected, intersectionPoint, intersectionTriangle);
}

Colour getColourOfTriAtPoint(Vector p, Triangle tri){
  return tri.col;
}

void drawRayTrace(DrawingWindow &window, Scene &scene, Vector pixel){
  float right = -((2*pixel.x)/scene.windowDim.x) + 1.0;
  float down = -((2*pixel.y)/scene.windowDim.y) + 1.0;
  Vector rayVec = scene.cameraAngle * Vector(right, down, -1.0);
  std::tuple<bool, Vector, Triangle> pointAndTriangle = getClosestPointOnRay(scene, rayVec, scene.camera);
  bool intersected = std::get<0>(pointAndTriangle);
  Vector intersectionPoint = std::get<1>(pointAndTriangle);
  Triangle intersectionTriangle = std::get<2>(pointAndTriangle);
  if(intersected){
    Colour col = getColourOfTriAtPoint(intersectionPoint, intersectionTriangle);
    window.setPixelColour(pixel.x, pixel.y, packCol(col));
  }
}
