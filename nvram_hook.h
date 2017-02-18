/*
  NVRAM_HOOK FUNCTIONS
*/

void nvram_load_value(char *name, char *data);
void __init();
int nvram_init();
int nvram_close();
char *nvram_get(char *bufstr);
int nvram_set(char *name, char *value);
char *nvram_bufget(int mode, char *bufstr);
int nvram_bufset(int mode, char *bufstr, char *bufvalue);
int nvram_commit();

/*
  DEBUGGING MAGIC
*/
void _memory_peek(char *target);

