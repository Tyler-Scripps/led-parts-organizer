class vial:
    def __init__(self, containerId, searchStr: str, qty) -> None:
        self.containerId = containerId
        self.qty = qty
        self.attributes = []
        for word in searchStr:
            self.attributes.append(word)
    
    def find(self, query) -> bool:
        if isinstance(query, str):
            query = query.split()
        found = all(any(element in word for word in self.attributes) for element in query)
        return found