/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
@module  miniz
@summary esp32专用miniz压缩库
@version 1.0
@date    2022.3.20
*/

#include "luat_base.h"

#include "esp_log.h"
#include "miniz.h"

static const char *TAG = "MINIZ";
static const size_t _MINIZ_UNCOMPRESS_BUFF = 4096;

/*
miniz压缩
@api miniz.compress(str)
@string 待压缩字符串
@return string 压缩结果
@return int 压缩结果长度
@usage
local s_str = "Hello world! Hello world! Hello world! Hello world! Hello world! Hello world! "
data, len = miniz.compress(s_str)
*/
static int l_miniz_compress(lua_State *L)
{
    size_t len = 0;
    int cmp_status = 0;
    const char *src = luaL_checklstring(L, 1, &len);
    ulong cmp_len = compressBound(len);
    uint8_t *pCmp = (mz_uint8 *)malloc(cmp_len);

    if (!pCmp)
    {
        ESP_LOGE(TAG, "compress malloc out of memory!");
        free(pCmp);
        return 0;
    }

    cmp_status = compress(pCmp, &cmp_len, (const unsigned char *)src, len);
    if (cmp_status != Z_OK)
    {
        ESP_LOGE(TAG, "compress() failed! return value is %d", cmp_status);
        free(pCmp);
        lua_pushnil(L);
        lua_pushinteger(L, 0);
    }
    else
    {
        lua_pushlstring(L, (const char *)pCmp, cmp_len);
        lua_pushinteger(L, cmp_len);
        free(pCmp);
    }

    return 2;
}

/*
miniz解压缩
@api miniz.compress(str)
@string 待解压字符串
@return string 解压结果
@return int 解压结果长度
@usage
udata, ulen = miniz.uncompress(data)
*/
static int l_miniz_uncompress(lua_State *L)
{
    size_t len = 0;
    int cmp_status = 0;
    char *src = (char *)luaL_checklstring(L, 1, &len);

    ulong uncomp_len = _MINIZ_UNCOMPRESS_BUFF;
    uint8_t *pUncomp = (mz_uint8 *)malloc(_MINIZ_UNCOMPRESS_BUFF);
    if (!pUncomp)
    {
        ESP_LOGE(TAG, "uncompress malloc out of memory!");
        free(pUncomp);
        return 0;
    }

    cmp_status = uncompress(pUncomp, &uncomp_len, (const unsigned char *)src, len);

    if (cmp_status != Z_OK)
    {
        ESP_LOGE(TAG, "uncompress() failed! return value is %d", cmp_status);
        free(pUncomp);
        lua_pushnil(L);
        lua_pushinteger(L, 0);
    }
    else
    {
        lua_pushlstring(L, (const char *)pUncomp, uncomp_len);
        lua_pushinteger(L, uncomp_len);
        free(pUncomp);
    }

    return 2;
}

#include "rotable.h"
static const rotable_Reg reg_miniz[] =
    {
        {"compress", l_miniz_compress, 0},
        {"uncompress", l_miniz_uncompress, 0},

        {NULL, NULL, 0}};

LUAMOD_API int luaopen_miniz(lua_State *L)
{
    luat_newlib(L, reg_miniz);
    return 1;
}
