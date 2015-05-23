#ifndef _TFTGFX_H
#define _TFTGFX_H

const int pi = 3.141592;


// 2d/3d helpers

typedef struct {
  int x;
  int y;
  int z;
} point3d;

typedef struct {
  int x;
  int y;
} point2d;

inline point3d scale3DPoint(const point3d p, const int f) { return point3d { p.x * f, p.y * f, p.z * f }; }
inline point3d rotate3DPoint(const point3d p, const point3d rot)
{
  
  // this does x, y, then z axis rotation
  // the order you rotate things is important
  // you may have to do 3 separate calls if you want to rotate z, y, then x
  
  point3d t = { p.x, p.y, p.z };
  
  // need in rads
  float rx = rot.x * pi / 180.0f;
  float ry = rot.y * pi / 180.0f;
  float rz = rot.z * pi / 180.0f;
  
  int tx, ty, tz;
    
  // x-axis rot
  ty = t.y;
  tz = t.z;
  t.y = ty*cos(rx) - tz*sin(rx);
  t.z = ty*sin(rx) + tz*cos(rx);
  // y-axis rot
  tz = t.z;
  tx = t.x;
  t.x = tz*sin(ry) + tx*cos(ry);
  t.z = tz*cos(ry) - tx*sin(ry);
  // z-axis rot
  tx = t.x;
  ty = t.y;
  t.x = tx*cos(rz) - ty*sin(rz);
  t.y = tx*sin(rz) + ty*cos(rz);
  
  return point3d { t.x, t.y, t.z };
}
inline point3d translate3DPoint(const point3d p, const point3d t) { return point3d { p.x + t.x, p.y + t.y, p.z + t.z }; }
inline point2d translate2DPoint(const point2d p, const point2d t) { return point2d { p.x + t.x, p.y + t.y }; }
inline point2d project3DPoint(const point3d p) { return point2d { p.x * 256 / p.z, p.y * 256 / p.z }; }


// drawing functions

typedef struct {
  point2d p[4];
} quad2d;

typedef struct {
  point2d p[2];
} line2d;

//inline int* sort4(const int a, const int b, const int c, const int d) {
//  int low1, high1, low2, high2, lowest, middle1, middle2, highest;
//  
//  // in 5 comparisons
//  // from https://stackoverflow.com/questions/6145364/sort-4-number-with-few-comparisons
//  if(a < b) { low1 = a; high1 = b; }
//  else { low1 = b; high1 = a; }
//  if(c < d) { low2 = c; high2 = d; }
//  else { low2 = d; high2 = c; }
//  if(low1 < low2) { lowest = low1; middle1 = low2; }
//  else { lowest = low2; middle1 = low1; }
//  if(high1 > high2) { highest = high1; middle2 = high2; }
//  else { highest = high2; middle2 = high1; }
//  if(middle1 < middle2) return new int[4] { lowest, middle1, middle2, highest };
//  else return new int[4] { lowest, middle2, middle1, highest };
//}

inline int* sort2(const int a, const int b)
{
  if(a < b) return new int[2] { a, b };
  else return new int[2] { b, a };
}

// strictly convex quads only..for now 
void fillQuad(const quad2d q, INT16U color)
{
  int sha = 0, shb = 0;
  int sva = 0, svb = 0;

  line2d l[4];
  
  for(int i = 0; i < 4; i++) { 
    // simultaneously generate lines
    // and sort line sub points by horizontal position
    if(q.p[i].y < q.p[(i+1)%4].y) {
      memcpy(&l[i].p[0], &q.p[i], sizeof(point2d));
      memcpy(&l[i].p[1], &q.p[(i+1)%4], sizeof(point2d));
    } else {
      memcpy(&l[i].p[1], &q.p[i], sizeof(point2d));
      memcpy(&l[i].p[0], &q.p[(i+1)%4], sizeof(point2d));
    }
    
    if(i == 0) {
      sha = q.p[i].x;
      shb = q.p[i].x;
      sva = q.p[i].y;
      svb = q.p[i].y;
    } else {
      if(q.p[i].x < sha) sha = q.p[i].x;
      if(q.p[i].x > shb) shb = q.p[i].x;
      if(q.p[i].y < sva) sva = q.p[i].y;
      if(q.p[i].y > svb) svb = q.p[i].y;
    }
    
    // uncomment for wireframe
//    Tft.drawLine(l[i].p[0].x, l[i].p[0].y, l[i].p[1].x, l[i].p[1].y, BLACK);
  }
  
  // scan through horizontal lines and find intersections
  // (vertical and horizontal line draw calls cost ~ same)
  // but I've committed to horizontal scanlines
  for(int y = sva; y <= svb; ++y) {
    int a = 0, b = 0;
    bool setflag = false;
    
    for(int i = 0; i < 4; ++i) {
      if(l[i].p[0].y <= y && l[i].p[1].y >= y) {
        // definite intersection was made
       
        if(l[i].p[0].y == l[i].p[1].y) { // horizontal line
          int *sx = sort2(l[i].p[0].x, l[i].p[1].x);
          if(!setflag) {
            // init bounds
            a = sx[0];
            b = sx[1];
            setflag = true;
          } else {
            // expand bounds
            if(sx[0] < a) a = sx[0];
            if(sx[1] > b) b = sx[0];
          }
        } else { // diagonal or vertical line
          // use vertical intersection ratio
          // to determine horizontal intersection.
          float x = ((float)(l[i].p[1].x - l[i].p[0].x) * ((float)(y - l[i].p[0].y)/(float)(l[i].p[1].y - l[i].p[0].y))) + l[i].p[0].x;
          if(!setflag) {
            // initialize bounds
            a = x;
            b = x;
            setflag = true;
          } else {
            // expand bounds
            if(x < a) a = x;
            if(x > b) b = x;
          }
        }
      }
    }

    // sanity check clipping
    if(a < 0) a = 0;
    if(b > 240) b = 240;
    if(y < 0) continue;
    if(y > 320) continue;
  
    Tft.drawHorizontalLine(a, y, b-a, color);
    
  }
}


#endif
