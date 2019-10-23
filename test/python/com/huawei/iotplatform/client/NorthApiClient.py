import logging
import requests
from requests import RequestException, ReadTimeout
from requests.adapters import HTTPAdapter
from com.huawei.iotplatform.constant.RestConstant import RestConstant
from com.huawei.iotplatform.constant.Constant import Constant


class NorthApiClient(object):
    requests.packages.urllib3.disable_warnings()
    cert = (Constant().readCertificate())

    def invokeAPI(httpMethod, authUrl, payload, accessToken):
        nc = NorthApiClient()

        clientInfo = Constant().clientInfo()
        url = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
            'platformPort'] + authUrl
        if accessToken == None:
            headers = {'Content-Type': 'application/json'}
        else:
            headers = {
                "app_key": clientInfo['appId'],
                "Authorization": "Bearer " + accessToken,
                "Content-Type": "application/json"
            }
        try:
            request = requests.Session()
            request.mount('https://', HTTPAdapter(pool_connections=10, pool_maxsize=10))

            global response
            if httpMethod == "POST":
                response = requests.post(url, headers=headers, data=payload, cert=nc.cert, verify=False)
            elif httpMethod == "PUT":
                response = request.put(url, headers=headers, data=payload, cert=nc.cert, verify=False)
            elif httpMethod == "DELETE":
                response = request.delete(url, headers=headers, data=payload, cert=nc.cert, verify=False)
            elif httpMethod == "GET":
                response = request.get(url, headers=headers, params=payload, cert=nc.cert, verify=False)
            logging.info(url), logging.info(headers), logging.info(payload), logging.info(response.text)
            return response.text

        except ReadTimeout as e:
            logging.error(e)
            raise ReadTimeout(e)
        except ConnectionError as e:
            logging.error(e)
            raise ConnectionError(e)
        except RequestException as e:
            logging.error(e)
            raise RequestException(e)
        except Exception as e:
            logging.error(e)
            raise Exception(e)

    def invokeAPI2(httpMethod, url, headers, payload):
        nc = NorthApiClient()

        try:
            request = requests.Session()
            request.mount('https://', HTTPAdapter(pool_connections=11, pool_maxsize=11))

            global response
            if httpMethod == "POST":
                response = request.post(url, headers=headers, data=payload, cert=nc.cert, verify=False, timeout=10)
            logging.info(url), logging.info(headers), logging.info(payload), logging.info(response.text)
            return response.text

        except ReadTimeout as e:
            logging.error(e)
            raise ReadTimeout(e)
        except ConnectionError as e:
            logging.error(e)
            raise ConnectionError(e)
        except RequestException as e:
            logging.error(e)
            raise RequestException(e)
        except Exception as e:
            logging.error(e)
            raise Exception(e)

    # def invokeAPI(httpMethod, url, payload, clientInfo, accessToken):
    #     authUrl = RestConstant.BASE_URL + clientInfo['platformIp'] + ":" + clientInfo[
    #         'platformPort'] + url
    #     headers = {
    #         "app_key": clientInfo['appId'],
    #         "Authorization": "Bearer " + accessToken,
    #         "Content-Type": "application/json"
    #     }
    #     requests.packages.urllib3.disable_warnings()
    #     cert = (
    #         '../../../../../conf/client.crt',
    #         '../../../../../conf/client.key'
    #     )
    #
    #     global response
    #     if httpMethod == "POST":
    #         response = requests.post(authUrl, headers=headers, data=payload, cert=cert, verify=False)
    #     elif httpMethod == "PUT":
    #         response = requests.put(authUrl, headers=headers, data=payload, cert=cert, verify=False)
    #     elif httpMethod == "DELETE":
    #         response = requests.delete(authUrl, headers=headers, data=payload, cert=cert, verify=False)
    #     elif httpMethod == "GET":
    #         response = requests.get(authUrl, headers=headers, data=payload, cert=cert, verify=False)
    #
    #     print("-----------------------------------------")
    #     print('Request URL:', response.url)
    #     print('Request Status_code:', response.status_code)
    #     print('Request Result:', response.text)
    #     print("-----------------------------------------")
    #     return response.text
