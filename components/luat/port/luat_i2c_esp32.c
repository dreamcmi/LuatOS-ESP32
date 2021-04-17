#include "luat_base.h"
#include "luat_i2c.h"

#include "driver/i2c.h"

#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */


int luat_i2c_exist(int id) {
    if (id == 1) {
        return 1;
    }
    return 0;
}

int luat_i2c_setup(int id, int speed, int slaveaddr)
{
    if (id == 1)
    {
        i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = 18;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = 19;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = speed;
        i2c_param_config(I2C_NUM_1, &conf);
        i2c_driver_install(I2C_NUM_1, conf.mode, 0, 0, 0);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_ic2_close(int id)
{
    if (id == 1)
    {
        i2c_driver_delete(I2C_NUM_1);
        return 0;
    }
    else
    {
        return -1;
    }
}

int luat_i2c_send(int id, int addr, void *buff, size_t len)
{
    if (id == 1)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
        i2c_master_write(cmd, buff, len, ACK_CHECK_EN);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
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
    if (id == 1)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);
        if (len > 1)
        {
            i2c_master_read(cmd, buff, len - 1, ACK_VAL);
        }
        i2c_master_read_byte(cmd, buff + len - 1, NACK_VAL);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(cmd);
        return 0;
    }
    else
    {
        return -1;
    }
}

