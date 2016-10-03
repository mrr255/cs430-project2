#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int line = 1;

int parseScene(char* input);
int nextChar(FILE* json);
int getC(FILE* json);
int checkNextChar(FILE* json, int val);
char* nextString(FILE* json);
char* checkNextString(FILE* json, char* value);
double* nextVector(FILE* json);
double nextNumber(FILE* json);



int main (int c, char** argv)
{
  int r = parseScene(argv[1]);
  return r;
}

int parseScene(char* input)
{
int c;

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
    return 1;
  }
  if (c == '{')
  {
    checkNextString(json,"type");
    checkNextChar(json,':');
    char* value = nextString(json);
    if (strcmp(value, "camera") == 0)
    {
    }
    else if (strcmp(value, "sphere") == 0)
    {
    }
    else if (strcmp(value, "plane") == 0)
    {
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
    	  break;
    	}
      else if (c == ',')
      {
    	  // read another field
    	  char* key = nextString(json);

    	  checkNextChar(json, ':');
    	  if ((strcmp(key, "width") == 0) ||
    	      (strcmp(key, "height") == 0) ||
    	      (strcmp(key, "radius") == 0))
            {
    	    double value = nextNumber(json);
    	      }
        else if ((strcmp(key, "color") == 0) ||
    		     (strcmp(key, "position") == 0) ||
    		     (strcmp(key, "normal") == 0))
             {
    	    double* value = nextVector(json);
    	       }
        else {
    	    fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
    		    key, line);
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
      fclose(json);
      return 0;
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
