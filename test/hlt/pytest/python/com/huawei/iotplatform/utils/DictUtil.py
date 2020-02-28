class DictUtil(object):
    # def __init__(self):
    #     pass

    # 将dto转dict
    def dto2dict(object):
        dict = {}
        for key in dir(object):
            value = getattr(object, key)
            if not key.startswith('__') and not callable(value):
                dict[key] = value
        return dict