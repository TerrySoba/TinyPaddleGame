import xml.etree.ElementTree as ET
import argparse
import re

def colorDiff(color1, color2):
    # very simplistic euclidean distance...
    return (color1[0] - color2[0])**2 + (color1[1] - color2[1])**2 + (color1[2] - color2[2])**2

rgbiColors = [
    (0x00,0x00,0x00),
    (0x00,0x00,0xAA),
    (0x00,0xAA,0x00),
    (0x00,0xAA,0xAA),
    (0xAA,0x00,0x00),
    (0xAA,0x00,0xAA),
    (0xAA,0x55,0x00),
    (0xAA,0xAA,0xAA),
    (0x55,0x55,0x55),
    (0x55,0x55,0xFF),
    (0x55,0xFF,0x55),
    (0x55,0xFF,0xFF),
    (0xFF,0x55,0xFF),
    (0xFF,0x55,0xFF),
    (0xFF,0xFF,0x55),
    (0xFF,0xFF,0xFF),
]

def getBestRgbiColor(r,g,b):
    bestIndex = 0
    bestDiff = None
    i = 0
    for color in rgbiColors:
        diff = colorDiff(rgbiColors[i], (r,g,b))
        if (bestDiff is None) or (diff < bestDiff):
            bestIndex = i
            bestDiff = diff
        i = i + 1

    return bestIndex


ns = {'svg': 'http://www.w3.org/2000/svg'}

def rotateRect(x,y,w,h,angle):
    "Rotates the rectangle around the middle point of the rectangle."
    while angle > 180:
        angle = angle - 360
    while angle < -180:
        angle = angle + 360
    
    middleX = x + w/2
    middleY = y + h/2

    # move rectangle to origin
    x = x - middleX
    y = y - middleY

class Rectangle:
    def __init__(self, x,y,w,h,color):
        self.x = float(x)
        self.y = float(y)
        self.w = float(w)
        self.h = float(h)
        self.color = int(color)



def parseTranform(transform):
    print(transform)
    m = re.search("translate\((.*)\,(.*)\)", transform)

    try:
        off_x = float(m.group(1))
        off_y = float(m.group(2))
    except:
        off_x = 0
        off_y = 0

    def translate(rect):
        rect.x = rect.x + off_x
        rect.y = rect.y + off_y
        return rect

    return translate


def parseViewbox(viewBox):

    m = re.match("(.*)\s+(.*)\s+(.*)\s+(.*)", viewBox)

    # print(,m.group(2),m.group(3),m.group(4))

    offset_x = float(m.group(1))
    offset_y = float(m.group(2))
    width = float(m.group(3))
    height = float(m.group(4))

    def transform(rect, target_width, target_height):
        rect.x = ((rect.x - offset_x) / width) * target_width
        rect.y = ((rect.y - offset_y) / height) * target_height
        rect.w = (rect.w / width) * target_width
        rect.h = (rect.h / height) * target_height
        return rect

    return transform


def parseStyle(style):
    m = re.search("fill\s*:\s*#(..)(..)(..)\s*;", style)

    r = int(m.group(1), base=16)
    g = int(m.group(2), base=16)
    b = int(m.group(3), base=16)

    return getBestRgbiColor(r,g,b)



def loadSvgRectangles(file_path, target_width, target_height):
    tree = ET.parse(file_path)
    root = tree.getroot()

    viewbox_transform = parseViewbox(root.get("viewBox"))

    rectangles = []

    g_elements = root.findall(".//svg:g", ns)

    for g in g_elements:
        global_transform = parseTranform(g.get("transform"))

        rect_elements = g.findall(".//svg:rect", ns)
        
        for rect in rect_elements:
            rgbiColor = parseStyle(rect.get("style"))
            rectangle = Rectangle(rect.get("x"), rect.get("y"),rect.get("width"), rect.get("height"), rgbiColor)
            rectangle = viewbox_transform(global_transform(rectangle), target_width, target_height)
            # transform = rect.get("transform")
            print("x:{} y:{} w:{} h:{}".format(rectangle.x,rectangle.y,rectangle.w,rectangle.h))
            rectangles.append(rectangle)

    return rectangles
    


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_svg", help="The SVG file to be converted")
    parser.add_argument("output_c", help="The C file that is created from the SVG file")
    args = parser.parse_args()


    rectangles  = loadSvgRectangles(args.input_svg, 320, 200)

    for rect in rectangles:
        print("drawRect({}, {}, {}, {}, {});".format(
            int(rect.x),
            int(rect.y),
            int(rect.w),
            int(rect.h),
            rect.color))


    print(args.input_svg, args.output_c)
