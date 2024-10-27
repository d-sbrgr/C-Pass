//
// Created by dave_ on 24.10.2024.
//

#include "util.h"
#include <stdio.h>


/*
 * Check whether a file under the given name exists
 *
 * param const char* filename: The name of the file to check for existence
 * return bool: true if the file exists, false otherwise
 */
bool file_exists(const char *path){
  FILE *file = fopen(path, "r");
  if(!file)
    return false;
  fclose(file);
  return true;
}