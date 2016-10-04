#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int line = 1;
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


Object** parseScene(char* input);
int nextChar(FILE* json);
int getC(FILE* json);
int checkNextChar(FILE* json, int val);
char* nextString(FILE* json);
char* checkNextString(FILE* json, char* value);
double* nextVector(FILE* json);
double nextNumber(FILE* json);

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
