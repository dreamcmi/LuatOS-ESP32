#ifndef LUAT_CONF_BSP
#define LUAT_CONF_BSP
#define LUAT_BSP_VERSION "V0001"

#define LUAT_MODULE_SEARCH_PATH   "/spiffs/%s.luac", "/spiffs/%s.lua",\
  "/spiffs/lua/%s.luac", "/spiffs/lua/%s.lua",\
  "/spiffs/luadb/%s.luac", "/spiffs/luadb/%s.lua",\
  "",

// vfs启用
// #define LUAT_USE_FS_VFS
#endif
