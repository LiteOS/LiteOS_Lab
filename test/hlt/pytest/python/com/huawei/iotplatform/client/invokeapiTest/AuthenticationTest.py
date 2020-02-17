import json

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.AuthRefreshInDTO import AuthRefreshInDTO
from com.huawei.iotplatform.constant.Constant import Constant


class AuthenticationTest(object):

    def refreshAuthTokenInfo(self):
        arInDTO = AuthRefreshInDTO()
        arInDTO.appId = (Constant().readConfFile())[2]
        arInDTO.secret = (Constant().readConfFile())[3]
        authOutDTO = AuthOutDTO()
        result = Authentication().getAuthToken(Constant().clientInfo())
        authOutDTO.setRefreshToken(json.loads(result)['refreshToken'])
        arInDTO.refreshToken = authOutDTO.getRefreshToken()
        return arInDTO


if __name__ == "__main__":
    auTest = AuthenticationTest()
    authentication = Authentication()

    ag = authentication.getAuthToken(Constant().clientInfo())
    print("====== get access token ======")
    print("result:", ag + "\n")

    ar = authentication.refreshAuthToken(auTest.refreshAuthTokenInfo())
    print("====== refresh token ======")
    print("result:", ar + "\n")
