#include "Raster.h"

#include "Draw.h"

Vector getPointOnCanvas(Scene &scene, Vector p){
  Vector w = scene.windowDim;
  Vector camToPoint = p - scene.camera;
  camToPoint = scene.cameraAngle * camToPoint;
  float pseudoZ = fabs(camToPoint.z);
  if(pseudoZ < 1){ pseudoZ = 1; }
  float i = ( (camToPoint.x/pseudoZ) + 1) * w.x/2;
  float j = ( (camToPoint.y/pseudoZ) + 1) * w.y/2;
  return Vector(i, j, -camToPoint.z);
}

bool isPointInView(Scene &scene, Vector p){
  Vector w = scene.windowDim;
  Vector camToPoint = p - scene.camera;
  camToPoint = scene.cameraAngle * camToPoint;
  float pseudoZ = fabs(camToPoint.z);
  float i = ( (camToPoint.x/pseudoZ) + 1) * w.x/2;
  float j = ( (camToPoint.y/pseudoZ) + 1) * w.y/2;
  return (pseudoZ >= 1) && (i >= 0) && (i <= w.x) && (j >= 0) && (j <= w.y);
}

std::vector<Triangle *> sliceTriangle(Scene &scene, Triangle *tri){
  bool p0_inView = isPointInView(scene, tri->p0);
  bool p1_inView = isPointInView(scene, tri->p1);
  bool p2_inView = isPointInView(scene, tri->p2);
  Vector v0 = tri->p0;
  Vector v1 = tri->p1;
  Vector v2 = tri->p2;
  int numPointsInView = p0_inView + p1_inView + p2_inView;
  //std::cout << numPointsInView << std::endl;
  std::vector<Triangle *> tris;
  if(numPointsInView == 3){
    tris.push_back(tri);
  } else if(numPointsInView == 2){
    if(!p1_inView){
      std::swap(v1, v0);
    } else if(!p2_inView){
      std::swap(v2, v0);
    }
    //CREATE TWO TRIANGLES TO MAKE UP QUAD
    //v0 is not in view
    tris.push_back(tri);

  } else if(numPointsInView == 1){
    if(p1_inView){
      std::swap(v1, v0);
    } else if(p2_inView){
      std::swap(v2, v0);
    }
    //MOVE EXT POINTS TO EDGES OF VIEW
    //v0 is in view
    tris.push_back(tri);

  }
  return tris;
}

//triangle drawing function
void draw3DTriangle_Simple(DrawingWindow &window, Scene &scene, Triangle *tri){
  Vector p0 = getPointOnCanvas(scene, tri->p0);
  Vector p1 = getPointOnCanvas(scene, tri->p1);
  Vector p2 = getPointOnCanvas(scene, tri->p2);
  drawLine(window, p0, p1, tri->mat->col);
  drawLine(window, p1, p2, tri->mat->col);
  drawLine(window, p2, p0, tri->mat->col);
}

void draw3DTriangle(DrawingWindow &window, Scene &scene, Triangle *tri){
  std::vector<Triangle *> tris = sliceTriangle(scene, tri);
  for(Triangle *t : tris){
    draw3DTriangle_Simple(window, scene, t);
  }
}

//triangle drawing function
void fill3DTriangle_Simple(DrawingWindow &window, Scene &scene, Triangle *tri){
  uint32_t col = packCol(tri->mat->col);
  Vector top = getPointOnCanvas(scene, tri->p0);
  Vector mid = getPointOnCanvas(scene, tri->p1);
  Vector bot = getPointOnCanvas(scene, tri->p2);
  if(bot.y > mid.y){
    std::swap(bot, mid);
  }
  if(mid.y > top.y){
    std::swap(mid, top);
    if(bot.y > mid.y){
      std::swap(bot, mid);
    }
  }
  float fracUpOfMidAdj = (mid.y-bot.y) / (top.y-bot.y);
  float xVal = (bot.x + fracUpOfMidAdj*(top.x-bot.x));
  float midAdjDepth = bot.z + fracUpOfMidAdj*(top.z-bot.z);
  Vector midAdj = Vector(xVal, mid.y, midAdjDepth); //cutoff of top-------bot
  if(midAdj.x < mid.x){
    std::swap(midAdj, mid);
  }


  float topH = top.y-mid.y;
  float botH = mid.y-bot.y;

  if(topH > 0){
    float leftTopStep = (mid.x - top.x) / topH;
    float rightTopStep = (midAdj.x - top.x) / topH;

    float leftTopStepDepth = (mid.z - top.z) / topH;
    float rightTopStepDepth = (midAdj.z - top.z) / topH;

    float left = top.x;
    float right = top.x;
    float leftDepth = top.z;
    float rightDepth = top.z;

    for(int i = 0; i < topH; i++){
      int startOfLine = left + (leftTopStep * i);
      int endOfLine = right + (rightTopStep * i);
      float startOfLineDepth = leftDepth + (leftTopStepDepth * i);
      float endOfLineDepth = rightDepth + (rightTopStepDepth * i);
      float lineLength = (endOfLine - startOfLine);
      float depthStep = (endOfLineDepth - startOfLineDepth) / lineLength;
      for(int j = 0; j <= lineLength; j++){
        int x = j + startOfLine;
        int y = top.y - i;
        float d = 1/(startOfLineDepth + (j*depthStep));
        if(x >= 0 && x < scene.windowDim.x && y > 0 && y < scene.windowDim.y){
          if(scene.depthBuf[x][y] <= d){
            window.setPixelColour(x, y, col);
            scene.depthBuf[x][y] = d;
          }
        }
      }
    }
  }
  if(botH > 0){
    float leftBotStep = (bot.x - mid.x) / botH;
    float rightBotStep = (bot.x - midAdj.x) / botH;

    float leftBotStepDepth = (bot.z - mid.z) / botH;
    float rightBotStepDepth = (bot.z - midAdj.z) / botH;


    float left = mid.x;
    float right = midAdj.x;
    float leftDepth = mid.z;
    float rightDepth = midAdj.z;

    for(int i = 0; i < botH; i++){
      int startOfLine = left + (leftBotStep * i);
      int endOfLine = right + (rightBotStep * i);
      float startOfLineDepth = leftDepth + (leftBotStepDepth * i);
      float endOfLineDepth = rightDepth + (rightBotStepDepth * i);
      float lineLength = (endOfLine - startOfLine);
      float depthStep = (endOfLineDepth - startOfLineDepth) / lineLength;
      for(int j = 0; j <= lineLength; j++){
        int x = j + startOfLine;
        int y = mid.y - i;
        float d = 1/(startOfLineDepth + (j*depthStep));
        if(x >= 0 && x < scene.windowDim.x && y > 0 && y < scene.windowDim.y){
          if(scene.depthBuf[x][y] <= d){
            window.setPixelColour(x, y, col);
            scene.depthBuf[x][y] = d;
          }
        }
      }
    }
  }
  for(Vector v : {top, mid, bot}){
    float d = 1/(v.z);
    if(v.x >= 0 && v.x < scene.windowDim.x && v.y > 0 && v.y < scene.windowDim.y){
      if(scene.depthBuf[v.x][v.y] <= d){
        window.setPixelColour(v.x, v.y, col);
        scene.depthBuf[v.x][v.y] = d;
      }
    }
  }
}


void fill3DTriangle(DrawingWindow &window, Scene &scene, Triangle *tri){
  std::vector<Triangle *> tris = sliceTriangle(scene, tri);
  for(Triangle *t : tris){
    fill3DTriangle_Simple(window, scene, t);
  }
}
