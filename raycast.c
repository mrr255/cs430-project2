#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


//#define DEBUG

typedef struct {
  int kind; // 0 = plane, 1 = sphere, 2 = camera
  double color[3];
  union {
    struct {
      double color[3];
      double position[3];
      double normal[3];
    } plane;
    struct {
      double color[3];
      double position[3];
      int radius;
    } sphere;
    struct {
      int width;
      int height;
    } camera;
  };
} Object;

typedef struct Pixel
  {
  unsigned char r, g, b;
  } Pixel;

Object** parseScene(char* input);
int nextChar(FILE* json);
int getC(FILE* json);
int checkNextChar(FILE* json, int val);
char* nextString(FILE* json);
char* checkNextString(FILE* json, char* value);
double* nextVector(FILE* json);
double nextNumber(FILE* json);
int raycast(Object** objects, int pxW, int pxH);
int planeIntersect(Object* object, double* rO, double* rD);


int line = 1;

static inline double sqr(double v)
{
  return v*v;
}

static inline void normalize(double* v)
{
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}

int main (int c, char** argv)
{
  Object** r = parseScene(argv[1]);
  int pxW = 20;
  int PxH = 20;
  return 0;
}
int planeIntersect(Object* object, double* rO, double* rD)
{
  double* norm = object->plane.normal;
  double* pnt = object->plane.position;
  //a(x-x0) + b(y-y0)+c(z-z0) = 0
  //a((rOx + t*rDx - x0)) + b((rOy + t*rDy - y0)) + c((rOz + t*rDz - z0))
  //a*rOx + t*a*rDx - a*x0 + b*rOy + t*b*rDy - b*y0 + c*rOz + t*c*rDz - c*z0
  //t(a*rDx+ b*rDy+ c*rDz) + (a*rOx + b*roy + c*rOz - a*xO - b*yO - c*zO) = 0
  //t = -(a*rOx + b*roy + c*rOz - a*xO - b*yO - c*zO) / (a*rDx+ b*rDy+ c*rDz)
  double m = norm[0]*rD[0] + norm[1]*rD[1] + norm[2]*rD[2];
  double b = norm[0]*rO[0] + norm[1]*rO[1] + norm[2]*rO[2] - norm[0]*pnt[0] - norm[1]*pnt[1] - norm[2]*pnt[2];
  double t = (-1*b)/m;
  if(t > 0)
  {
    return t;
  }
  else
  {
    return -1;
  }
}

int sphereIntersect(Object* object, double* rO, double* rD)
{
  double r = object->sphere.radius;
  double* pnt = object->sphere.position;
  // (x-h)^2 + (y-j)^2 + (z-k)^2 = r^2
  // (rOx - t*rDx-x0)^2 + (rOy - t*rDy-y0)^2 + (rOz - t*rDz-z0)^2 = r^2

  //rDx^2t^2 - 2rDxrOxt + 2rDxtx0+ rOx^2 - 2rOx x0 + x0^2
  // + rDy^2t^2 - 2rDyrOyt + 2rDyty0+ rOy^2 - 2rOyy0 + y0^2
  // + rDZ^2t^2 - 2rDZrOZt + 2rDZtZ0+ rOZ^2 - 2rOzz0 + z0^2
  // - r^2 = 0

  //t^2(rDx^2 + rDy^2 + rDZ^2t)
  //+ t(- 2rDxrOx + 2rDxx0 - 2rDyrOy + 2rDyy0 - 2rDZrOZ + 2rDZZ0)
  //+(rOx^2 - 2rOx x0 + x0^2 + rOy^2 - 2rOyy0 + y0^2 + rOZ^2 - 2rOzz0 + z0^2 - r^2) = 0
  double a = sqr(rD[0]) + sqr(rD[1]) + sqr(rD[2]);
  double b = -2* rD[0]* rO[0] + 2* rD[0]* pnt[0] -2* rD[1]* rO[1] + 2* rD[1]* pnt[1] -2* rD[2]* rO[2] + 2* rD[2]* pnt[2];
  double c = sqr(rO[0]) - 2* rO[0]* pnt[0] + sqr(pnt[0]) + sqr(rO[1]) - 2* rO[1]*pnt[1] + sqr(pnt[1]) + sqr(rO[2]) - 2*rO[2]*pnt[2] + sqr(pnt[2]) - sqr(r);

  double det = sqr(b) - 4 * a * c;
  if (det < 0)
    return -1;

  det = sqrt(det);

  double t0 = (-b - det) / (2*a);
  if (t0 > 0)
    return t0;

  double t1 = (-b + det) / (2*a);
  if (t1 > 0)
    return t1;

  return -1;
}

int raycast(Object** objects, int pxW, int pxH)
{
  double cx = 0;
  double cy = 0;
  int i = 0;
  double ch = 0;
  double cw = 0;

  while (objects[i] != NULL) {
    if(objects[i]->kind == 2)
    {
      cw = objects[i]->camera.width;
      ch = objects[i]->camera.height;
      break;
    }
  }
  if(cw == 0 || ch == 0)
  {
    //ERROR
  }
  double pixHeight = ch / pxH;
  double pixWidth = cw / pxW;
  double rO[3] = {cx, cy, 0};
  Pixel* image;
  image = malloc(sizeof(Pixel) * pxW * pxH); //Prepare memory for image data
  for (int y = pxH; y > 0; y -= 1) {
    for (int x = 0; x < pxW; x += 1) {
      double rD[3] = {cx - (cw/2) + pixWidth * (x + 0.5),cy - (ch/2) + pixHeight * (y + 0.5),1};
      normalize(rD);
      double bestT = INFINITY;
      int bestO = -1;
      double* color;
      for (int i=0; objects[i] != 0; i += 1)
      {
	       double t = 0;

	        switch(objects[i]->kind)
          {
	           case 0:
	            t = planeIntersect(objects[i],rO, rD);
              color = objects[i]->plane.color;
	           break;
             case 1:
              t = sphereIntersect(objects[i],rO, rD);
              color = objects[i]->plane.color;
             break;
             case 2:
             break;
             default:
             // Horrible error
              exit(1);
	        }
          if (t > 0 && t < bestT)
          {
            bestT = t;
            bestO = i;
          }
        }
        if (bestT > 0 && bestT != INFINITY) // Collect color data
        {
          image[pxW*(pxH-y) + x].r = color[0];
          image[pxW*(pxH-y) + x].g = color[1];
          image[pxW*(pxH-y) + x].b = color[2];
        }
        else
        {

        }
      }
      printf("\n");
  }

  return 0;
}
//Parsing JSON
Object** parseScene(char* input)
{

  #ifdef DEBUG
    printf("DEBUG MODE ENGAGE!\n");
  #endif
  int c;
  int objectI = 0;
  Object** objects;
  objects = malloc(sizeof(Object*)*129);

  FILE* json = fopen(input,"r");
  if (json == NULL)
  {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", input);
    exit(1);
  }

  checkNextChar(json, '[');

  while(1)
  {
    c = getC(json);
    if (c == ']')
    {
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      exit(1);
    }
    if (c == '{')
    {
      checkNextString(json,"type");
      checkNextChar(json,':');
      char* value = nextString(json);
      #ifdef DEBUG
      printf("%s\n", value);
      #endif
      objects[objectI] = malloc(sizeof(Object));
      if (strcmp(value, "camera") == 0)
      {
        objects[objectI]->kind = 2;
      }
      else if (strcmp(value, "sphere") == 0)
      {
        objects[objectI]->kind = 1;
      }
      else if (strcmp(value, "plane") == 0)
      {
        objects[objectI]->kind = 0;
      }
      else
      {
        fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
        exit(1);
      }

      while (1)
      {
        c = nextChar(json);
        if (c == '}')
        {
      	  // stop parsing this object
          objectI++;
      	  break;
      	}
        else if (c == ',')
        {
      	  // read another field
      	  char* key = nextString(json);
          #ifdef DEBUG
          printf("%s:", key);
          #endif
      	  checkNextChar(json, ':');
      	  if ((strcmp(key, "width") == 0))
              {
      	    double value = nextNumber(json);
            #ifdef DEBUG
            printf("%lf\n", value);
            #endif
            if (objects[objectI]->kind == 2) {
              objects[objectI]->camera.width = value;
            }
            else
            {
              fprintf(stderr, "Error: Invalid property, \"%s\", on line number %d.\n", key, line);
              exit(1);
            }
      	      }
          else if ((strcmp(key, "height") == 0))
              {
      	    double value = nextNumber(json);
            #ifdef DEBUG
            printf("%lf\n", value);
            #endif
              if (objects[objectI]->kind == 2) {
                objects[objectI]->camera.height = value;
              }
              else
              {
                fprintf(stderr, "Error: Invalid property, \"%s\", on line number %d.\n", key, line);
                exit(1);
              }
      	      }
          else if ((strcmp(key, "radius") == 0))
              {
          	    double value = nextNumber(json);
                #ifdef DEBUG
                printf("%lf\n", value);
                #endif
                if (objects[objectI]->kind == 1)
                {
                  objects[objectI]->sphere.radius = value;
                }
                else
                {
                  fprintf(stderr, "Error: Invalid property, \"%s\", on line number %d.\n", key, line);
                  exit(1);
                }
      	      }
          //Vectors
          else if ((strcmp(key, "color") == 0))
               {
      	          double* value = nextVector(json);
                  if (objects[objectI]->kind == 1)
                  {
                    for(int i = 0;i<3;i++)
                    {
                    objects[objectI]->sphere.color[i] = value[i];
                    #ifdef DEBUG
                    printf("%lf ", value[i]);
                    #endif
                    }
                    #ifdef DEBUG
                    printf("\n");
                    #endif
                  }
                  else if (objects[objectI]->kind == 0)
                  {
                    for(int i = 0;i<3;i++)
                    {
                    objects[objectI]->plane.color[i] = value[i];
                    #ifdef DEBUG
                    printf("%lf ", value[i]);
                    #endif
                    }
                    #ifdef DEBUG
                    printf("\n");
                    #endif
                  }
                  else
                  {
                    fprintf(stderr, "Error: Invalid property, \"%s\", on line number %d.\n", key, line);
                    exit(1);
                  }
      	       }
          else if ((strcmp(key, "position") == 0))
              {
                double* value = nextVector(json);
                if (objects[objectI]->kind == 1)
                {
                  for(int i = 0;i<3;i++)
                  {
                  objects[objectI]->sphere.position[i] = value[i];
                  #ifdef DEBUG
                  printf("%lf ", value[i]);
                  #endif
                  }
                  #ifdef DEBUG
                  printf("\n");
                  #endif
                }
                else if (objects[objectI]->kind == 0)
                {
                  for(int i = 0;i<3;i++)
                  {
                  objects[objectI]->plane.position[i] = value[i];
                  #ifdef DEBUG
                  printf("%lf ", value[i]);
                  #endif
                  }
                  #ifdef DEBUG
                  printf("\n");
                  #endif
                }
                else
                {
                  fprintf(stderr, "Error: Invalid property, \"%s\", on line number %d.\n", key, line);
                  exit(1);
                }
              }
         else if ((strcmp(key, "normal") == 0))
              {
                double* value = nextVector(json);
                if (objects[objectI]->kind == 0)
                {
                  for(int i = 0;i<3;i++)
                  {
                  objects[objectI]->plane.normal[i] = value[i];
                  #ifdef DEBUG
                  printf("%lf ", value[i]);
                  #endif
                  }
                  #ifdef DEBUG
                  printf("\n");
                  #endif
                }
                else
                {
                  fprintf(stderr, "Error: Invalid property, \"%s\", on line number %d.\n", key, line);
                  exit(1);
                }
              }
          else {
      	    fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n", key, line);
              exit(1);
      	    //char* value = next_string(json);
      	       }

      	}
        else
        {
      	  fprintf(stderr, "Error: Unexpected value on line %d\n", line);
      	  exit(1);
      	}
      }
      c = nextChar(json);
      if (c == ',')
      {
        // noop
      }
      else if (c == ']')
      {
        objects[objectI] = NULL;
        fclose(json);
        return objects;
      }
      else
      {
        fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
        exit(1);
      }
    }
  }
}

int getC(FILE* json)
{
  int c = fgetc(json);

  if (c == '\n')
  {
    line += 1;
  }

  if (c == EOF)
  {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}
// Grabs the next non whitespace character from the file and returns it.
int nextChar(FILE* json)
{
  int c = getC(json);
  while (isspace(c))
  {
    c = getC(json);
  }
  return c;
}

int checkNextChar(FILE* json, int val)
{
  int c = nextChar(json);
  if(c==val)
  {
    return c;
  }
  else
  {
    fprintf(stderr, "Error: Expected '%c' on line %d.\n", val, line);
    exit(1);
  }
}

char* nextString(FILE* json)
{
  char buffer[129];
  int c = checkNextChar(json,'"');
  c = nextChar(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = nextChar(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

char* checkNextString(FILE* json, char* value)
{
  char* key = nextString(json);
  if (strcmp(key, value) != 0)
  {
    fprintf(stderr, "Error: Expected %s key on line number %d.\n", value, line);
    exit(1);
  }
  else
  {
    return key;
  }
}

double* nextVector(FILE* json)
{
  double* v = malloc(3*sizeof(double));
  checkNextChar(json, '[');
  v[0] = nextNumber(json);
  checkNextChar(json, ',');
  v[1] = nextNumber(json);
  checkNextChar(json, ',');
  v[2] = nextNumber(json);
  checkNextChar(json, ']');
  return v;
}

double nextNumber(FILE* json)
{
  double value;
  fscanf(json, "%lf", &value);
  // Error check this..
  return value;
}
