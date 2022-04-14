#include "luat_base.h"
#include "luat_u8g2.h"

int luat_u8g2_setup_default(luat_u8g2_conf_t *conf) ;

int luat_u8g2_setup(luat_u8g2_conf_t *conf) {
    return luat_u8g2_setup_default(conf);
}
