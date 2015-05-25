
void drawFlash()
{
  static bool flash = true;
  
  if(flash) Tft.fillRectangle(0, 0, 240, 320, WHITE);
  else Tft.fillRectangle(0, 0, 240, 320, RED);
  
  flash = !flash;
}

inline INT16U getColor(const int r, const int g, const int b) {
  // formula from https://stackoverflow.com/questions/13720937/c-defined-16bit-high-color
  return (((31 * (r + 4)) / 255) << 11) | 
         (((63 * (g + 2)) / 255) << 5) | 
         (( 31 * (b + 4)) / 255);
}


void drawCube()
{  
  //  y  z
  //  |/
  //  +-- x
  
  static point3d rot = {0, 0, 0};
  static int animation_step = 0;
  static int cx1, cy1, cx2, cy2;
  static line2d boundingbox;
  static line2d bbold;
  static bool bbinit = false;
  
  animation_step+=10;
  if(animation_step > 628) animation_step -= 628;
  
  // zoom in and out
  float distance = 200.0f + sin((float)animation_step / 100.0f) * 100.0f;
//  float distance = 120.0f;
  
  static const point3d cube[] = { 
    point3d{ -1,  1, -1 },
    point3d{  1,  1, -1 },
    point3d{ -1, -1, -1 },
    point3d{  1, -1, -1 },
    point3d{ -1,  1,  1 },
    point3d{  1,  1,  1 },
    point3d{ -1, -1,  1 },
    point3d{  1, -1,  1 } };
  
  rot.x += 6;
  rot.y += 12;
  
  if(rot.x > 360) rot.x -= 360;
  if(rot.y > 360) rot.y -= 360;
  
  point2d verts[8];

  // scale -> rotation -> translate3d -> project -> translate to center of screen  
  for(int i = 0; i < 8; i++) {
    verts[i] = translate2DPoint(project3DPoint(translate3DPoint( rotate3DPoint(scale3DPoint(cube[i], 10), rot), point3d { 0, 0, distance })), point2d { 120, 160 });
    
    // bounding box
    if(i == 0) {
      // initialize bounding box
      boundingbox.p[0].x = verts[i].x;
      boundingbox.p[0].y = verts[i].y;
      boundingbox.p[1].x = verts[i].x;
      boundingbox.p[1].y = verts[i].y;
    } else {
      if(verts[i].x < boundingbox.p[0].x) boundingbox.p[0].x = verts[i].x;
      if(verts[i].x > boundingbox.p[1].x) boundingbox.p[1].x = verts[i].x;
      if(verts[i].y < boundingbox.p[0].y) boundingbox.p[0].y = verts[i].y;
      if(verts[i].y > boundingbox.p[1].y) boundingbox.p[1].y = verts[i].y;
    }
  }
  

  // fill quads
  
  quad2d quads[] = {
    quad2d { verts[0], verts[1], verts[3], verts[2] },// font
    quad2d { verts[4], verts[5], verts[7], verts[6] },// back
    quad2d { verts[0], verts[4], verts[6], verts[2] },// left
    quad2d { verts[1], verts[5], verts[7], verts[3] },// right
    quad2d { verts[0], verts[4], verts[5], verts[1] },// top
    quad2d { verts[2], verts[6], verts[7], verts[3] },// bottom
  };
  
  // normals for the faces of this cube
  static const point3d normals[6] = {
    {  0,  0, -1 },
    {  0,  0,  1 },
    { -1,  0,  0 },
    {  1,  0,  0 },
    {  0,  1,  0 },
    {  0, -1,  0 },
  };
    
  // find the 3 "closest" faces
  int dist[6];
  int sface[6] = {0,1,2,3,4,5};
  for(int i = 0; i < 6; ++i) {
    // calculate normals and distance from normals to origin. higher numbers are facing farther away
    point3d tnorm = translate3DPoint(rotate3DPoint(scale3DPoint(normals[i], 10), rot), point3d { 0, 0, 10 } );
    dist[i] = sq(tnorm.x) + sq(tnorm.y) + sq(tnorm.z);
//    Serial.print(dist[i]);
//    Serial.print("\n");
  }
  
  // sort distance
  for(int i = 0; i < 6; ++i) {
    for(int j = 0; j < 5; ++j) {
      if(dist[sface[j]] > dist[sface[j+1]]) {
        int t = sface[j+1];
        sface[j+1] = sface[j];
        sface[j] = t;
      }
    }
  }
  
  // clear the old cube drawing based on the previous bounding box
  if(bbinit) Tft.fillRectangle(bbold.p[0].x, bbold.p[0].y, bbold.p[1].x - bbold.p[0].x, bbold.p[1].y - bbold.p[0].y, CYAN);
  
  static const INT16U faceColors[] = { WHITE, RED, GREEN, BLUE, YELLOW, GRAY2 };
  // painter's algorithm (back->front)
  // https://en.wikipedia.org/wiki/Painter%27s_algorithm
  for(int i = 2; i >= 0; --i) {
    fillQuad(quads[sface[i]], faceColors[sface[i]]);
    
    // this is an incomplete shading method
    // doesn't really work well with gray since there's only 16-bit color
//    fillQuad(quads[sface[i]], getColor((float)dist[i] * 0.5f, (float)dist[i] * 0.5f, (float)dist[i] * 0.5f));
  }
  
  // draw wireframe
//  Tft.drawLine(verts[0].x, verts[0].y, verts[1].x, verts[1].y, BLACK);
//  Tft.drawLine(verts[1].x, verts[1].y, verts[3].x, verts[3].y, BLACK);
//  Tft.drawLine(verts[3].x, verts[3].y, verts[2].x, verts[2].y, BLACK);
//  Tft.drawLine(verts[2].x, verts[2].y, verts[0].x, verts[0].y, BLACK);
//  
//  Tft.drawLine(verts[4].x, verts[4].y, verts[5].x, verts[5].y, BLACK);
//  Tft.drawLine(verts[5].x, verts[5].y, verts[7].x, verts[7].y, BLACK);
//  Tft.drawLine(verts[7].x, verts[7].y, verts[6].x, verts[6].y, BLACK);
//  Tft.drawLine(verts[6].x, verts[6].y, verts[4].x, verts[4].y, BLACK);
//  
//  Tft.drawLine(verts[0].x, verts[0].y, verts[4].x, verts[4].y, BLACK);
//  Tft.drawLine(verts[2].x, verts[2].y, verts[6].x, verts[6].y, BLACK);
//  
//  Tft.drawLine(verts[1].x, verts[1].y, verts[5].x, verts[5].y, BLACK);
//  Tft.drawLine(verts[3].x, verts[3].y, verts[7].x, verts[7].y, BLACK);
  
  memcpy(&bbold, &boundingbox, sizeof(line2d));
  bbinit = true;
  delay(33);
}

void drawPlasma()
{
  //work in progress
  const int w = 240;
  const int h = 320;
  
  int p1 = random();
  int p2 = random();
  int p3 = random();
  int p4 = random();
}

class plasma {
  plasma() {}
};
