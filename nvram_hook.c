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

char *logLevel_debug = "Debug";
char *logDestination_debug = "Standard Error";
// extra padding...
char *fake_HTTP_CFG[8] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

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

int globalLockCount = 0;

int cmsLck_acquireLockWithTimeoutTraced(char *msgHandle,int lockSomething)
{
  globalLockCount += 1;
  if(globalLockCount == 2)
  {
    printf(" [cms_hook] SKIP acquiring lock(%p,%d)\n",msgHandle,lockSomething);
    return 1;
  }
  printf(" [cms_hook] acquiring lock(%p,%d)\n",msgHandle,lockSomething);
  return 0;
}

int cmsLck_dumpInfo()
{
  printf(" [cms_hook] cmsLck_dumpInfo(), skipping...\n");
  return 0;
}

/*
int cmsLog_setLevel(int logLevel)
{
  printf(" [cms_hook] cms_setLevel, skipping\n");
  return 0;
}
*/

/*
int cmsLog_setDestination(int logDest)
{
  printf(" [cms_hook] cms_setDestination, skipping\n");
  return 0;
}
*/

/*
int cmsUtl_logLevelStringToEnum(char *logLevel)
{
  printf(" [cms_hook] cmsUtl_logLevelStringToEnum(%s)\n",logLevel);
  return 0;
}
*/

int cmsObj_free(char *x)
{
  printf(" [cms_hook] attempting to free cmsObj [%p]\n");
  return 0;
}

// HTTPD_CFG = 0xC
int cmsObj_get(int request_type,char **arg2, int arg3, char **ret_addr)
{
  printf(" [cms_hook] hit cmsObj_get with (0x%x,%p,%d,%p)\n",request_type,arg2,arg3,ret_addr);
  if(request_type == 0xC)
  {
    fake_HTTP_CFG[2] = logLevel_debug;
    fake_HTTP_CFG[3] = logDestination_debug;
    ret_addr[0] = (char *)(&fake_HTTP_CFG);
    printf(" [cms_hook] request for HTTP_CFG, faking reply at %p\n",ret_addr[0]);
    return 0;
  }
  else if(request_type == 0x81)
  {
    fake_HTTP_CFG[0] = (char *)0x40404040;
    fake_HTTP_CFG[1] = (char *)0x41404040;
    fake_HTTP_CFG[2] = (char *)0x42404040;
    fake_HTTP_CFG[3] = (char *)0x43404040;
    fake_HTTP_CFG[4] = (char *)0x44404040;
    fake_HTTP_CFG[5] = (char *)0x45404040;
    fake_HTTP_CFG[6] = (char *)0x46404040;
    fake_HTTP_CFG[7] = (char *)0x47404040;
    ret_addr[0] = (char *)(&fake_HTTP_CFG);
    printf(" [cms_hook] request type 0x81, faking blob at %p\n",ret_addr[0]);
    return 0;
  }
  else
  {
    printf(" [cms_hook] i don't know how to handle this request\n");
    return 0;
  }
}

int cmsMsg_getEventHandle(char *x)
{
  printf(" [cmsMSg_getEventHandle] called on %p\n",x);
  // _memory_peek(x);
  return 0;
}

void _memory_peek(char *target)
{
  // char fucking_magic[16];
  int i =0;
  printf(" [0x%08p] : ",target);
  for(;i < 16;i++)
  {
    printf("%02x",(unsigned char )target[i]);
  }

  printf("\n");
}
