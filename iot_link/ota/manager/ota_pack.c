#include <string.h>
#include "ota_flag.h"
#include "ota_manager.h"
#include "flash_adaptor.h"
#include "common.h"

int ota_pack_man_software_write(pack_storage_device_api_s *this, uint32_t offset, uint8_t *buf, uint32_t len)
{
  (void)this;
  return ota_storage_bin_write(offset, buf, len);
}

int ota_pack_man_software_write_end(pack_storage_device_api_s *this, pack_download_result_e result, uint32_t len)
{
  (void)this;
  ota_flag_t flag;
  int ret = OK;

  ota_storage_flag_read(&flag);
  flag.updater = UPDATER_FOTA;
  flag.file_size = len;
  flag.cur_state = (result == PACK_DOWNLOAD_OK ? EN_OTA_STATUS_UPGRADING : EN_OTA_STATUS_IDLE);

  if (ota_pack_get_signature_verify_result(256, len) != 0) {
	ret = ERR;
  	flag.cur_state = EN_OTA_STATUS_IDLE;
  }

  flag.crc = calc_crc32(0, &flag, sizeof(flag) - sizeof(flag.crc));
  ota_storage_flag_write(&flag);
  return ret;
}

int ota_pack_man_active_software(pack_storage_device_api_s *thi)
{
  //  printf("vaild actived\n");
  return OK;
}

static void pack_man_init_pack_device(pack_storage_device_s *device)
{
  if (device->init_flag == 1) {
    return;
  }

  (void)memset(device, 0, sizeof(*device));

  device->interface.write_software = ota_pack_man_software_write;
  device->interface.write_software_end = ota_pack_man_software_write_end;
  device->interface.active_software = ota_pack_man_active_software;
  device->init_flag = 1;
}

static pack_storage_device_s g_pack_man_storage_device;

pack_storage_device_api_s *pack_get_device(void)
{
  pack_man_init_pack_device(&g_pack_man_storage_device);
  return &g_pack_man_storage_device.interface;
}

int ota_pack_observe_info_read(uint8_t *buf, uint32_t len)
{
  int ret = storage_partition_read(PART_RESERVED_INFO, buf, len, 0);
  return ret == ERR ? ERR : OK;
}

int ota_pack_observe_info_write(uint8_t *buf, uint32_t len)
{
  int ret = storage_partition_write(PART_RESERVED_INFO, buf, len, 0);
  printf("%s return ret %d\n", __func__, ret);
  return ret == ERR ? ERR : OK;
}
