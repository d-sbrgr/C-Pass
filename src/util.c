//
// Created by dave_ on 24.10.2024.
//

#include "util.h"
#include <stdio.h>
#include <stdlib.h>


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

void clear_console() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}