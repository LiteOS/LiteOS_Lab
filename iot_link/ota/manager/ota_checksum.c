
#include "mbedtls/md.h"
#include "stdint.h"
#include "ota_flag.h"
#include <string.h>
#include <osal.h>

struct input_stream {
  int start;
  int end;
};

static struct input_stream calc_stream;
static uint8_t *hash_cache = NULL;
static uint8_t *sign_cache = NULL;

#define HASH_LEN   (32)

#if defined(MBEDTLS_PK_PARSE_C)
#include "mbedtls/pk.h"
static uint8_t prv_public_key[] = "-----BEGIN PUBLIC KEY-----\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAo1++6Iw1EgsZCUwBpg/i\n"
"NlEgIjaBwDqLABFTmnPFSW9KDwaffjd1xzRwWxUPaQqpq3UEzj7ZWJFx07uLar1+\n"
"ub9wRmv+fTUWwnuMnMZd14K7CXasFiwnvule+uEoomOytf9bx7E/8wFE+/IlYbQ/\n"
"62+tViPuClazV118lbG8moMVJfSBgdGavk65KFO03ZFP5HpP7yK968R/otOcJOCN\n"
"frq4WaeuqxCyv/MnGzxpmDp7kYrTejBlOH+czHSLPZlW1p8mzOUrVDCT3ngg0oHw\n"
"5pibPkfjjawioq1KCIMQNeAM/VtmpuKyDznHrgtLA1hOXeK7F63FEJ9T1Ib/00a5\n"
"PQIDAQAB\n"
"-----END PUBLIC KEY-----";
#else
#include "mbedtls/aes.h"
static uint8_t prv_psk[] = "01234567";       // max: 16 bytes
#endif


static int ota_pack_calc_stream_init(int sign_len, int file_size)
{

  calc_stream.start = sign_len;
  calc_stream.end = file_size;

  hash_cache = osal_malloc(HASH_LEN);
  if (hash_cache == NULL)
    return -1;


  sign_cache = osal_malloc(sign_len);
  if (sign_cache == NULL) {
    osal_free(hash_cache);
    return -1;
  }

  memset(hash_cache, 0, 32);
  memset(sign_cache, 0, 256);
  return 0;
}

static inline void ota_pack_calc_stream_deinit()
{
  calc_stream.start = 0;
  calc_stream.end = 0;

  if (sign_cache) {
    osal_free(sign_cache);
    sign_cache = NULL;
  }

  if (hash_cache) {
    osal_free(hash_cache);
    hash_cache = NULL;
  }
}
static int ota_pack_read_stream(uint8_t *buf, int32_t buf_len)
{
  int read_len = 0;
  if (calc_stream.end - calc_stream.start > buf_len) {
    read_len = buf_len;
  } else {
    read_len = calc_stream.end - calc_stream.start;
  }

  if (read_len < 0) {
    return read_len;
  }

  ota_storage_bin_read(calc_stream.start, buf, read_len);

  calc_stream.start += read_len;

  return read_len;

}

int ota_pack_calc_hash(uint8_t *hash_out)
{
  int ret;
  int read_len;
  mbedtls_md_context_t ctx;
  const mbedtls_md_info_t *md_info;
  unsigned char buf[1024];

  if (hash_out == NULL) {
    return -1;
  }

  md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  if (md_info == NULL) {
    return -1;
  }

  mbedtls_md_init( &ctx );

  if( ( ret = mbedtls_md_setup( &ctx, md_info, 0 ) ) != 0 )
    goto cleanup;

  if( ( ret = mbedtls_md_starts( &ctx ) ) != 0 )
    goto cleanup;


  while ((read_len = ota_pack_read_stream(buf, sizeof(buf))) > 0) {
    mbedtls_md_update( &ctx, buf, read_len );
  }

  mbedtls_md_finish(&ctx, hash_out);

 cleanup:
  mbedtls_md_free( &ctx );
  return( ret );
}

static int ota_pack_verify_signature(uint8_t *hash, int32_t hash_len, uint8_t *sig, int32_t sig_len)
{
  int ret = 0;
#if defined(MBEDTLS_PK_PARSE_C)
  mbedtls_pk_context pk;
  //load public key
  mbedtls_pk_init(&pk);
  ret = mbedtls_pk_parse_public_key(&pk, prv_public_key, sizeof(prv_public_key));
  if (ret != 0) {
    printf ("parse pk failed!, ret = %x\n", ret);
    goto exit;
  }

  if ((ret = mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, hash, 0, sig, sig_len)) != 0) {
    printf (" pk verify failed , ret = %x\n", -ret);
    goto exit;
  }
  ret = 0;

 exit:
  mbedtls_pk_free( &pk );
#endif
  return ret;
}

int ota_pack_get_signature_verify_result(int sign_len, int file_len)
{
  int ret;

  ota_pack_calc_stream_init(sign_len, file_len);
  ota_pack_calc_hash(hash_cache);
  ota_storage_bin_read(0, sign_cache, sign_len);

  if ( (ret = ota_pack_verify_signature(hash_cache, 32, sign_cache, sign_len)) != 0) {
    printf("ota Binary signature check failed!, ret = %x\n", ret);
  }

  ota_pack_calc_stream_deinit();
  return ret;
}


