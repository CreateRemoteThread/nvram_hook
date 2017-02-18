#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nvram_hook.h"

#define NVRAM_PRELOAD_INIT if(init == 0){ init = 1; __init(); }
#define BUFSIZE 1024

struct nvram_entry{
  char *name;
  char *value;
  struct nvram_entry *next;
} nvram_entry;

struct nvram_entry *head = NULL;

FILE *f = NULL;

int init = 0;
int nvram_ask = 0;
char *default_filedb = "/tmp/nvram.dat";
char *filedb = NULL;
char *default_cms = "/tmp/cms.dat";

void nvram_load_value(char *name, char *data)
{
  struct nvram_entry *read_head = head;
  struct nvram_entry *new_entry = (struct nvram_entry *)malloc(sizeof(struct nvram_entry));
  new_entry->name = strdup(name);
  new_entry->value = strdup(data);
  new_entry->next = NULL;
  if(head == NULL)
  {
    head = new_entry;
  }
  else
  {
    while(read_head->next != NULL)
    {
      read_head = read_head->next;
    }
    read_head->next = new_entry;
  }
  return;
}

void __init()
{
  filedb = getenv("NVRAM_ASK");
  if(filedb == NULL)
  {
    if(strcmp(filedb,"true") == 0 || strcmp(filedb,"TRUE") == 0)
    {
      nvram_ask = 1;
    } 
  }
  filedb = getenv("NVRAM_DB");
  if(filedb == NULL)
  {
    filedb = default_filedb;
  }
  f = fopen(filedb,"r+");
  if(f == NULL)
  {
    f = fopen(filedb,"w+");
  }
  if(f == NULL)
  {
    printf(" [nvram] cannot initialize database '%s', set this with NVRAM_DB or initialize /tmp/nvram.dat. exiting...\n",filedb);
    exit(0);
  }
  char nvram_data[BUFSIZE];
  // char *nvram_value_start = NULL;
  int nvram_load_value_count = 0;
  while(fgets(nvram_data,BUFSIZE,f))
  {
    char *nvram_value_start = NULL;
    int i =0;
    for(;i < BUFSIZE;i++)
    {
      if(nvram_data[i] == '\r' || nvram_data[i] == '\n')
      {
        nvram_data[i] = '\0';
      }
      else if(nvram_data[i] == ':')
      {
        nvram_data[i] = '\0';
        nvram_value_start = (char *)(&(nvram_data[i+1]));
      }
    }
    nvram_load_value((char *)&nvram_data,nvram_value_start);
    nvram_load_value_count++;
  }
  fclose(f);
  printf(" [nvram] init success, %d nvram entries loaded\n",nvram_load_value_count);
}

int nvram_init()
{
  NVRAM_PRELOAD_INIT;
  printf(" [nvram] nvram_init()...\n");
  return 0;
}

int nvram_close()
{
  NVRAM_PRELOAD_INIT;
  printf(" [nvram] nvram_close()...\n");
  return 0;
}

char *nvram_get(char *bufstr)
{
  NVRAM_PRELOAD_INIT;
  return nvram_bufget(0,bufstr);
}

int nvram_set(char *name, char *value)
{
  NVRAM_PRELOAD_INIT; 
  return nvram_bufset(0,name,value);
}

char *nvram_bufget(int mode, char *bufstr)
{
  NVRAM_PRELOAD_INIT;
  printf(" [nvram] bufget(\"%s\")...\n",bufstr);
  struct nvram_entry *rh = head;
  while(rh != NULL)
  {
    if(strcmp(bufstr,rh->name) == 0)
    {
      return rh->value;
    }
    rh = rh->next;
  }
  if(nvram_ask == 1)
  {
    printf(" [nvram] bufget(\"%s\") = ");
    char nvram_newanswer[BUFSIZE];
    fgets(nvram_newanswer,BUFSIZE,stdin);
    int i = 0;
    for(;i < BUFSIZE;i++)
    {
      if(nvram_newanswer[i] == '\r' || nvram_newanswer[i] == '\n')
      {
        nvram_newanswer[i] = '\0';
      }
    }
    // load this into memory
    nvram_load_value(bufstr,nvram_newanswer);
    f = fopen(filedb,"r+");
    fseek(f,0,SEEK_END);
    fprintf(f,"%s:%s\n",bufstr,nvram_newanswer);
    fclose(f);
    return strdup(nvram_newanswer);
  }
  else
  {
    printf(" [nvram] bufget(\"%s\") unknown, please fill in %s\n",bufstr,filedb);
    char *nvram_newanswer = "";
    nvram_load_value(bufstr,nvram_newanswer);
    f = fopen(filedb,"r+");
    fseek(f,0,SEEK_END);
    fprintf(f,"%s:%s\n",bufstr,nvram_newanswer);
    fclose(f);
    return "";
  }
}

int nvram_bufset(int mode, char *bufstr, char *bufvalue)
{
  NVRAM_PRELOAD_INIT;
  printf(" [nvram] bufset(\"%s\",\"%s\")...\n"); 
  return 0;
}

int nvram_commit()
{
  NVRAM_PRELOAD_INIT;
  return 0;
}

int cmsMsg_init(int code, char *msgHandle)
{
  printf(" [cms_hook] cmsMsg_init called\n");
  return 0;
}

int cmsMdm_init(int code, char *msgHandle)
{
  printf(" [cms_hook] cmsMdm_init called\n");
  return 0;
}

int cmsLck_acquireLockWithTimeoutTraced(char *msgHandle)
{
  printf(" [cms_hook] acquiring lock\n");
  return 0;
}

int cmsLck_releaseLockWithTimeoutTraced(char *msgHandle)
{
  printf(" [cms_hook] releasing lock\n");
  return 0;
}

char *cmsObj_get(char *request)
{
  printf(" [cms_hook] hit cmsObj_get with %s\n",request);
  return NULL;
}

char *cmsObj_free(char *obj)
{
  printf(" [cms_hook] hit cmsObj_free with %p\n",obj);
  return NULL;
}
