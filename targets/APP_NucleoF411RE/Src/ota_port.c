



#include <osport.h>
#include <stdint.h>
#include <stddef.h>

#include <ota_api.h>


void hal_init_ota(void)
{
    return;
}

void hal_get_ota_opt(ota_opt_s *opt)
{
    if (opt == NULL)
    {
        return;
    }

    memset(opt, 0, sizeof(*opt));
    opt->read_flash = NULL;
    opt->write_flash = NULL;
    opt->flash_block_size = 0x1000;
}

int flag_enable_hwpatch(const uint8_t *patch_head, int32_t patch_len)
{
    return -1;
}
