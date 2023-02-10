#include "Raster.h"

#include "Draw.h"

Vector getPointOnCanvas(Scene &scene, Vector p){
  Vector w = scene.windowDim;
  Vector camToPoint = p - scene.camera;
  camToPoint = scene.cameraAngle * camToPoint;
  float i = ( (camToPoint.x/camToPoint.z) + 1) * w.x/2;
  float j = ( (camToPoint.y/camToPoint.z) + 1) * w.y/2;
  return Vector(i, j, -camToPoint.z);
}

//triangle drawing function
void draw3DTriangle(DrawingWindow &window, Scene &scene, Triangle tri){
  Vector p0 = getPointOnCanvas(scene, tri.p0);
  Vector p1 = getPointOnCanvas(scene, tri.p1);
  Vector p2 = getPointOnCanvas(scene, tri.p2);
  drawLine(window, p0, p1, tri.col);
  drawLine(window, p1, p2, tri.col);
  drawLine(window, p2, p0, tri.col);
}

//triangle drawing function
void fill3DTriangle(DrawingWindow &window, Scene &scene, Triangle tri){
  uint32_t col = packCol(tri.col);
  Vector top = getPointOnCanvas(scene, tri.p0);
  Vector mid = getPointOnCanvas(scene, tri.p1);
  Vector bot = getPointOnCanvas(scene, tri.p2);
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
        if(scene.depthBuf[x][y] <= d){
          window.setPixelColour(x, y, col);
          scene.depthBuf[x][y] = d;
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
        if(scene.depthBuf[x][y] <= d){
          window.setPixelColour(x, y, col);
          scene.depthBuf[x][y] = d;
        }
      }
    }
  }
  for(Vector v : {top, mid, bot}){
    float d = 1/(v.z);
    if(scene.depthBuf[v.x][v.y] <= d){
      window.setPixelColour(v.x, v.y, col);
      scene.depthBuf[v.x][v.y] = d;
    }
  }
}
