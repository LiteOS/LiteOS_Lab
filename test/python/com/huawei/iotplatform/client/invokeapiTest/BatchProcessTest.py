import json

from com.huawei.iotplatform.client.invokeapi.Authentication import Authentication
from com.huawei.iotplatform.client.invokeapi.BatchProcess import BatchProcess

from com.huawei.iotplatform.client.dto.AuthOutDTO import AuthOutDTO
from com.huawei.iotplatform.client.dto.BatchTaskCreateInDTO import BatchTaskCreateInDTO
from com.huawei.iotplatform.client.dto.BatchTaskCreateOutDTO import BatchTaskCreateOutDTO
from com.huawei.iotplatform.client.dto.QueryTaskDetailsInDTO import QueryTaskDetailsInDTO
from com.huawei.iotplatform.constant.Constant import Constant


class BatchProcessTest(object):
    def createBatchTaskInfo(self):
        btcInDTO = BatchTaskCreateInDTO()
        btcInDTO.appId = "3RQ9UnhymV409MfKPuiin75XroQa"
        btcInDTO.timeout = 100
        btcInDTO.taskName = "a1"
        btcInDTO.taskType = "DeviceCmd"
        return btcInDTO

    def queryTaskDetailsInfo(self):
        qtdInDTO = QueryTaskDetailsInDTO()
        qtdInDTO.taskID = "5bf8f8567dd2d86eab87edd9"
        return qtdInDTO


if __name__ == "__main__":
    bpTest = BatchProcessTest()
    authentication = Authentication()
    batchProcess = BatchProcess()

    # get accessToken at first
    result = authentication.getAuthToken(Constant().clientInfo())
    authOutDTO = AuthOutDTO()
    authOutDTO.setAccessToken(json.loads(result)['accessToken'])
    accessToken = authOutDTO.getAccessToken()

    # create a task begin
    bc = batchProcess.createBatchTask(bpTest.createBatchTaskInfo(), accessToken)
    print("====== create a task begin ======")
    print("result:", bc + "\n")

    # get taskID
    btcOutDTO = BatchTaskCreateOutDTO()
    btcOutDTO.setTaskID(json.loads(bc)['taskID'])
    taskID = btcOutDTO.getTaskID()
    # taskID = "11"
    print("taskID==", taskID+ "\n")

    # query a specified task
    bq = batchProcess.queryOneTask(taskID, None, None, accessToken)
    print("====== query a specified task ======")
    print("result:", bq + "\n")

    # query a specified task detail
    bq = batchProcess.queryTaskDetails(bpTest.queryTaskDetailsInfo(), accessToken)
    print("====== query a specified task detail ======")
    print("result:", bq + "\n")
