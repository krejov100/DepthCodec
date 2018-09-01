import cv2
import matplotlib.pyplot as plt

class Rect():
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

    def tl(self):
        return (int(self.x), int(self.y))

    def br(self):
        return (int(self.x + self.width), int(self.y + self.height))


def reachedBottom(ROI):
    if ROI.width == 1 or ROI.height == 1:
        return True
    return False

class QuadROILeaf():
    def __init__(self, ROI):
        self.ROI = ROI

    def combine(self, child_leafs):
        x = child_leafs[0].ROI.x
        y = child_leafs[0].ROI.y
        width = child_leafs[0].ROI.width * 2
        height = child_leafs[0].ROI.height * 2
        self.ROI = Rect(x,y,width, height)
        return self

    def split(self):
        x = self.ROI.x
        y = self.ROI.y
        half_width = self.ROI.width / 2
        half_height = self.ROI.height / 2

        if not reachedBottom(self.ROI):
            return [QuadROILeaf(Rect(x, y, half_width, half_height)),
                    QuadROILeaf(Rect(x + half_width, y, half_height, half_height)),
                    QuadROILeaf(Rect(x, y + half_height, half_width, half_height)),
                    QuadROILeaf(Rect(x + half_width, y + half_height, half_width, half_height))
                    ]
        else:
            return None

    def draw(self, im):
        cv2.rectangle(im, self.ROI.tl(), self.ROI.br(), 255)

class Node():
    def __init__(self, leaf_data, parent):
        self.children = []
        self.leaf_data = leaf_data
        self.parent = parent

    def prune(self):
        self.leaf_data = self.leaf_data.prune([x.leaf_data for x in self.children])
        self.children = None

    def grow(self):
        child_data = self.leaf_data.split()
        if child_data:
            self.children = [Node(x, self) for x in child_data]
    def draw(self, im):
        self.leaf_data.draw(im)
        for child in self.children:
            child.draw(im)


def shouldSplit(image, leaf):
    roi = leaf.leaf_data.ROI
    subImage = image[int(roi.y):int(roi.y) + int(roi.height), int(roi.x):int(roi.x) + int(roi.width)]
    cv2.imshow('erts',subImage)
    cv2.waitKey(1)
    return True



class Tree():
    def __init__(self):
        self.root = None

    def bottomUp(self, image):
        to_grow = [self.root]
        while(len(to_grow)):
            leaf = to_grow.pop(0)

            if shouldSplit(image, leaf):
                leaf.grow()
                to_grow  = to_grow + leaf.children

    def draw(self, im):
        self.root.draw(im);



def main():
    tree = Tree()
    tree.root = Node(QuadROILeaf(Rect(0, 0, 256, 256)), None)
    im = cv2.imread('bgExampleDepth.tif')

    im = cv2.resize(im, (256, 256), 0, 0, cv2.INTER_NEAREST)
    tree.bottomUp(im)
    tree.draw(im)

    plt.imshow(im)
    plt.pause(0)

if __name__ == '__main__':
    main()


