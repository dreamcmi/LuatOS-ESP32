/*
 * SPDX-FileCopyrightText: 2021-2022 Darren <1912544842@qq.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "luat_base.h"
#include "luat_i2c.h"

// #define LUAT_LOG_TAG "luat.i2c"
// #include "luat_log.h"

#include "pinmap.h"
#include "driver/i2c.h"
#include "sdkconfig.h"

#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */

/*
//写IIC寄存器
// SensorAdd 从机地址
// addr 寄存器地址
// val 要写入的值
int IIC_WR_Reg(i2c_port_t num, int SensorAdd, uint8_t addr, uint8_t val)
{
    // uint8_t data = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SensorAdd << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, val, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

//读IIC寄存器
// SensorAdd 从机地址
// addr 寄存器地址
// return 读到的值
uint8_t IIC_RD_Reg(i2c_port_t num, int SensorAdd, uint8_t addr)
{
    uint8_t data = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SensorAdd << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SensorAdd << 1 | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &data, NACK_VAL);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return data;
}
*/

int luat_i2c_exist(int id)
{
#if CONFIG_IDF_TARGET_ESP32C3
    if (id == 0)
    {
        return 1;
    }
    return 0;
#elif CONFIG_IDF_TARGET_ESP32S3
    if (id >= 0 && id <= 1)
    {
        return 1;
    }
    return 0;
#else
    return 0;
#endif
}

int luat_i2c_setup(int id, int speed, int slaveaddr)
{
    if (luat_i2c_exist(id))
    {
        i2c_config_t conf = {0};
        conf.mode = I2C_MODE_MASTER;
#if CONFIG_IDF_TARGET_ESP32C3
        conf.sda_io_num = _C3_SDA0;
        conf.scl_io_num = _C3_SCL0;
#elif CONFIG_IDF_TARGET_ESP32S3
        if (id == 0)
        {
            conf.sda_io_num = _S3_SDA0;
            conf.scl_io_num = _S3_SCL0;
        }
        else
        {
            conf.sda_io_num = _S3_SCL1;
            conf.scl_io_num = _S3_SCL1;
        }
#endif
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        if (speed == 0)
        {
            conf.master.clk_speed = 100 * 1000;
        }
        else if (speed == 1)
        {
            conf.master.clk_speed = 400 * 1000;
        }
        else
        {
            conf.master.clk_speed = speed;
        }
        conf.clk_flags = I2C_SCLK_DEFAULT;
        i2c_param_config(id, &conf);
        i2c_driver_install(id, conf.mode, 0, 0, 0);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_i2c_close(int id)
{
    if (luat_i2c_exist(id))
    {
        i2c_driver_delete(id);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_i2c_send(int id, int addr, void *buff, size_t len)
{
    if (luat_i2c_exist(id))
    {
#if 0
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        if (cmd != NULL)
        {
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
            i2c_master_write(cmd, (const uint8_t *)buff, len, ACK_CHECK_EN);
            i2c_master_stop(cmd);
            i2c_master_cmd_begin(id, cmd, 1000 / portTICK_RATE_MS);
            i2c_cmd_link_delete(cmd);
            return 0;
        }
        else
        {
            return -2; // 内存不足
        }
#else
        esp_err_t err = i2c_master_write_to_device(id, addr, (const uint8_t *)buff, len, 100 / portTICK_RATE_MS);
        return err == 0 ? 0 : -1;
#endif
    }
    else
    {
        return -1;
    }
}

int luat_i2c_recv(int id, int addr, void *buff, size_t len)
{
    if (luat_i2c_exist(id))
    {
#if 0
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        if (cmd != NULL)
        {
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);
            i2c_master_read(cmd, (uint8_t *)buff, len, I2C_MASTER_LAST_NACK);
            i2c_master_stop(cmd);
            i2c_master_cmd_begin(id, cmd, 1000 / portTICK_RATE_MS);
            i2c_cmd_link_delete(cmd);
            return 0;
        }
        else
        {
            return -2; // 内存不足
        }
#else
        esp_err_t err = i2c_master_read_from_device(id, addr, (uint8_t *)buff, len, 100 / portTICK_RATE_MS);
        return err == 0 ? 0 : -1;
#endif
    }
    else
    {
        return -1;
    }
}

// int luat_i2c_write_reg(int id, int addr, int reg, uint16_t value)
// {
//     if (luat_i2c_exist(id))
//     {
//         int ret = IIC_WR_Reg(id, addr, reg, value);
//         return ret == 0 ? 0 : -1;
//     }
//     else
//     {
//         return -1;
//     }
// }

// int luat_i2c_read_reg(int id, int addr, int reg, uint16_t *value)
// {
//     if (luat_i2c_exist(id))
//     {
//         *value = IIC_RD_Reg(id, addr, reg);
//         return 0;
//     }
//     else
//     {
//         return -1;
//     }
// }
