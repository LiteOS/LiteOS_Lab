class NorthApiException(Exception):

    def __init__(self, message):
        Exception.__init__(self)
        self.message = message

try:
    raise NorthApiException("A NorthApiException!")  # 主动触发异常
except:
    print("GOD, A NorthApiException!")
