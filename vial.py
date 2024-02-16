class vial:
    def __init__(self, containerId, searchStr: str, qty: int) -> None:
        self._containerId = containerId
        self.qty = qty
        self.attributes = []
        for word in searchStr:
            self.attributes.append(word)
    
    def __str__(self) -> str:
        retStr = ""
        retStr = retStr + str(self._containerId) + ' '.join(self.attributes) + str(self.qty)
        return retStr
    
    def find(self, query) -> bool:
        if isinstance(query, str):
            query = query.split()
        found = all(any(element in word for word in self.attributes) for element in query)
        return found
    
    def getContainerId(self) -> int:
        return self._containerId