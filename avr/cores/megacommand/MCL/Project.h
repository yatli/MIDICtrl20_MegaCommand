/* Copyright Justin Mammarella jmamma@gmail.com 2018 */

#ifndef PROJECT_H__
#define PROJECT_H__
#include "ProjectPages.h"
#include "MCLSysConfig.h"
#define PROJ_VERSION 2025

class ProjectHeader {
public:
  uint32_t version;
  uint8_t reserved[16];
  uint32_t hash;
  MCLSysConfigData cfg;
};

class Project : public ProjectHeader {
public:
  File file;
  bool project_loaded = false;
  void setup();
  bool new_project();
  bool load_project(const char *projectname);
  bool check_project_version();
  bool new_project(const char *projectname);
  bool write_header();
};

extern Project proj;

#endif /* PROJECT_H__ */
