#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

typedef struct pack_storage_device_api_tag_s pack_storage_device_api_s;

typedef enum
{
    PACK_DOWNLOAD_OK,
    PACK_DOWNLOAD_FAIL
}pack_download_result_e;

struct pack_storage_device_api_tag_s
{
    int (*write_software)(pack_storage_device_api_s *thi, uint32_t offset, const uint8_t *buffer, uint32_t len);
    int (*write_software_end)(pack_storage_device_api_s *thi, pack_download_result_e result, uint32_t total_len);
    int (*active_software)(pack_storage_device_api_s *thi);
};

typedef struct
{
    pack_storage_device_api_s interface;
    // pack_hardware_s hardware;
    // pack_params_s params;
    // pack_head_s head;
    // pack_writer_s writer;
    uint32_t total_len;
    // ota_flash_type_e type;
    int32_t init_flag;
} pack_storage_device_s;

pack_storage_device_api_s *pack_get_device(void);

int ota_pack_get_signature_verify_result(int sign_len, int file_len);

#endif /* OTA_MANAGER_H */
