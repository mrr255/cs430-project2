#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int parseScene(char* input);
int nextChar(FILE* json);
int getC(FILE* json);
int checkNextChar(FILE* json, int val);

int line = 1;

int main (int c, char** argv)
{
  FILE* json = fopen(argv[1],"r");
  return 0;
}

int parseScene(char* input)
{

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
