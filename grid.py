import board
import neopixel
import vial

class grid:
    def __init__(self, pin, numPixels: int) -> None:
        self.vials = {}
        self.pixels = neopixel.NeoPixel(pin, numPixels)
        self.foundColor = (10, 10, 10)
        self.allOff()

    def __str__(self) -> str:
        headerStr = "vial_id, name, qty\n"
        valuesStr = ""
        for tempVial in self.vials.values():
            valuesStr = valuesStr + str(tempVial) + '\n'
        return headerStr + valuesStr

    # converts the rows of alternating direction to an xy coordinate
    def XYtoLED(self, x: int, y: int) -> int:
        ledNum = -1
        # check if y is odd
        if y % 2 != 0:
            ledNum = (10 * y) + x
        else:
            ledNum = (10 * (y + 1)) - x
        return ledNum


    def addVial(self, newVial: vial, x: int, y: int):
        self.vials[self.XYtoLED(x,y)] = newVial

    def search(self, searchStr, illuminate: bool):
        for ledNum, tempVial in self.vials:
            if tempVial.find(searchStr):
                if illuminate:
                    self.pixels[ledNum] = self.foundColor
    
    def illuminate(self, containerId: int):
        for ledNum, tempVial in self.vials:
            if tempVial.getContainerId == containerId:
                self.pixels[ledNum] = self.foundColor
                self.pixels.show()

    def illuminate(self, x: int, y: int):
        pass
    
    def allOff(self) -> None:
        self.pixels.fill((0,0,0))
        self.pixels.show()