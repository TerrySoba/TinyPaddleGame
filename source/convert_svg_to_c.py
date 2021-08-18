import xml.etree.ElementTree as ET
import argparse
import re
import math

def colorDiff(color1, color2):
    # very simplistic euclidean distance...
    return (color1[0] - color2[0])**2 + (color1[1] - color2[1])**2 + (color1[2] - color2[2])**2



class Transformation:
    def __init__(self, a, b, c, d, e, f):
        self._matrix = [a,b,c,d,e,f]

    def __init__(self, off_x, off_y):
        self._matrix = [0,0,0,0,off_x,off_y]
        pass

    def __init__(self, angle):
        self._matrix = [math.cos(angle),math.sin(angle),-math.sin(angle),math.cos(angle),0,0]
        pass

    def apply(self, x_in, y_in):
        x = self._matrix[0] * x_in + self._matrix[2] * y_in + self._matrix[4]
        y = self._matrix[1] * x_in + self._matrix[3] * y_in + self._matrix[5]
        return [x,y]



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

    def normalize(self):
        if self.w < 0:
            self.x += self.w
            self.w *= -1
        if self.h < 0:
            self.y += self.h
            self.h *= -1


def parseTranform(transformInput):

    def nullTranform(rect):
        return rect

    if transformInput is None:
        return nullTranform

    # print(transformInput)
    translate = re.search("translate\((.*)\,(.*)\)", transformInput)
    scale1 = re.search("scale\(([^,]*)\)", transformInput)
    scale2 = re.search("scale\((.*)\,(.*)\)", transformInput)

    
    if translate is not None:
        try:
            off_x = float(translate.group(1))
            off_y = float(translate.group(2))
        except:
            off_x = 0
            off_y = 0

        def transform(rect):
            rect.x = rect.x + off_x
            rect.y = rect.y + off_y
            return rect

        return transform

    if scale1 is not None:
        try:
            scale = float(scale1.group(1))
        except:
            scale = 1

        def transform(rect):
            rect.x = rect.x * scale
            rect.y = rect.y * scale
            rect.w = rect.w * scale
            rect.h = rect.h * scale
            return rect

        return transform

    if scale2 is not None:
        try:
            scale_x = float(scale2.group(1))
            scale_y = float(scale2.group(2))
        except:
            scale_x = 1
            scale_y = 1

        def transform(rect):
            rect.x = rect.x * scale_x
            rect.y = rect.y * scale_y
            rect.w = rect.w * scale_x
            rect.h = rect.h * scale_y
            return rect

        return transform

    


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


def isFloat(val):
    try:
        float(val)
        return True
    except:
        return False

def parsePath(path):
    tmp = path.split(",")
    entries = []
    for x in tmp:
        for s in x.split(" "):
            entries.append(s)


    x = 0
    y = 0
    pathCoords = []
    while len(entries) > 0:
        entry = entries.pop(0)
        if entry == "m":
            while len(entries) >= 2:
                if isFloat(entries[0]) and isFloat(entries[1]):
                    x += float(entries.pop(0))
                    y += float(entries.pop(0))
                    pathCoords.append([x, y])

    return pathCoords



def loadSvgRectangles(file_path, target_width, target_height):
    tree = ET.parse(file_path)
    root = tree.getroot()

    viewbox_transform = parseViewbox(root.get("viewBox"))

    rectangles = []
    paths = []
    circles = []

    g_elements = root.findall(".//svg:g", ns)

    for g in g_elements:
        global_transform = parseTranform(g.get("transform"))

        rect_elements = g.findall(".//svg:rect", ns)
        
        for rect in rect_elements:
            rect_transform = parseTranform(rect.get("transform"))
            rgbiColor = parseStyle(rect.get("style"))
            rectangle = Rectangle(rect.get("x"), rect.get("y"),rect.get("width"), rect.get("height"), rgbiColor)
            rectangle = viewbox_transform(global_transform(rect_transform(rectangle)), target_width, target_height)
            rectangle.normalize()
            # transform = rect.get("transform")
            # print("x:{} y:{} w:{} h:{}".format(rectangle.x,rectangle.y,rectangle.w,rectangle.h))
            rectangles.append(rectangle)

        path_elements = g.findall(".//svg:path", ns)
        for path in path_elements:
            rgbiColor = parseStyle(path.get("style"))
            path_coords = parsePath(path.get("d"))
            transformed = []
            for coord in path_coords:
                rect = Rectangle(coord[0], coord[1], 0, 0, 0)
                rect = viewbox_transform(global_transform(rect), target_width, target_height)
                transformed.append([rect.x, rect.y])
            paths.append(transformed)

        circle_elements = g.findall(".//svg:circle", ns)
        for circle in circle_elements:
            rgbiColor = parseStyle(circle.get("style"))
            cx = float(circle.get("cx"))
            cy = float(circle.get("cy"))
            r = float(circle.get("r"))
            rect = Rectangle(cx, cy, r, r, rgbiColor)
            rect = viewbox_transform(global_transform(rect), target_width, target_height)
            circles.append(rect)


    return [rectangles, paths, circles]
    


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_svg", help="The SVG file to be converted")
    parser.add_argument("output_c", help="The C file that is created from the SVG file")
    args = parser.parse_args()


    [rectangles, paths, circles]  = loadSvgRectangles(args.input_svg, 320, 200)

    with open(args.output_c, "w") as file:
        for rect in rectangles:
            file.write("drawRect({}, {}, {}, {}, {});\n".format(
                round(rect.x),
                round(rect.y),
                round(rect.w),
                round(rect.h),
                rect.color))
        for circle in circles:
            file.write("drawCircle({}, {}, {}, {}, true);\n".format(
                round(circle.x),
                round(circle.y),
                round(circle.w),
                circle.color))
        for path in paths:
            for i in range(len(path)):
                print("drawLine({}, {}, {}, {}, 1);".format(
                    round(path[i][0]),
                    round(path[i][1]),
                    round(path[(i+1) % len(path)][0]),
                    round(path[(i+1) % len(path)][1])))


    # print(args.input_svg, args.output_c)
