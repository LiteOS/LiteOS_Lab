from com.huawei.iotplatform.client.invokeapi.PushMessageReceiver import app

if __name__ == '__main__':
    callbackUrl = "10.63.166.253"
    port = 1122

    # app.run(host=callbackUrl, port=port, ssl_context='adhoc')
    app.run(host=callbackUrl, port=port, ssl_context=('d:/python_sdk/cert/client.crt', 'd:/python_sdk/cert/client.key'))
