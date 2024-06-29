class DStoreId:
    def __init__(self, id: int):
        self._id = id

    def clone(self) -> 'DStoreId':
        return DStoreId(self._id)