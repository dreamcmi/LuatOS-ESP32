#include "luat_base.h"
#include "luat_msgbus.h"
#include "luat_malloc.h"



// 先占个坑


#include "rotable.h"

static const rotable_Reg reg_esphttp[] =
{
	{ NULL,          NULL ,          0}
};

LUAMOD_API int luaopen_esphttp( lua_State *L ) {
    luat_newlib(L, reg_esphttp);
    return 1;
}

