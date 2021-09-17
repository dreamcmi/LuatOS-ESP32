#include "luat_base.h"
#include "luat_i2c.h"

#define LUAT_LOG_TAG "luat.i2c"
#include "luat_log.h"

#include "driver/i2c.h"

#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */

//写IIC寄存器
//SensorAdd 从机地址
//addr 寄存器地址
//val 要写入的值
void IIC_WR_Reg(int SensorAdd, uint8_t addr, uint8_t val)
{
    // uint8_t data = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SensorAdd << 1 | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, val, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}

//读IIC寄存器
//SensorAdd 从机地址
//addr 寄存器地址
//return 读到的值
uint8_t IIC_RD_Reg(int SensorAdd, uint8_t addr)
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
    i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return data;
}

int luat_i2c_exist(int id)
{
    if (id == 0)
    {
        return 1;
    }
    return 0;
}

int luat_i2c_setup(int id, int speed, int slaveaddr)
{
    if (luat_i2c_exist(id))
    {
        i2c_config_t conf = {};
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = 6;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = 7;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = speed;
        ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
        ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_ic2_close(int id)
{
    if (luat_i2c_exist(id))
    {
        ESP_ERROR_CHECK(i2c_driver_delete(I2C_NUM_0));
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
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
        i2c_master_write(cmd, buff, len, ACK_CHECK_EN);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);
        return 0;
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
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);
        if (len > 1)
        {
            i2c_master_read(cmd, buff, len - 1, ACK_VAL);
        }
        i2c_master_read_byte(cmd, buff, NACK_VAL);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_i2c_write_reg(int id, int addr, int reg, uint16_t value)
{
    if (luat_i2c_exist(id))
    {
        IIC_WR_Reg(addr, reg, value);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_i2c_read_reg(int id, int addr, int reg, uint16_t *value)
{
    if (luat_i2c_exist(id))
    {
        *value = IIC_RD_Reg(addr,reg);
        return 0;
    }
    else
    {
        return -1;
    }
}