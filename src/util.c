//
// Created by dave_ on 24.10.2024.
//

#include "util.h"
#include <stdio.h>

bool file_exists(const char *filename){
  FILE *file = fopen(filename, "r");
  if(!file)
    return false;
  fclose(file);
  return true;
}