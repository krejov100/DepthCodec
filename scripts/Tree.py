import cv2
import matplotlib.pyplot as plt

class Rect():
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

def reachedBottom(ROI):
    print(ROI.width)
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


class Tree():
    def __init__(self):
        self.root = None

    def bottomUp(self, image):
        to_grow = [self.root]
        while(len(to_grow)):
            leaf = to_grow.pop(0)
            leaf.grow()
            to_grow  = to_grow + leaf.children

def main():
    tree = Tree()
    tree.root = Node(QuadROILeaf(Rect(0,0,8,8)), None)
    im = cv2.imread('bgExampleDepth.tif')
    tree.bottomUp(im)

    plt.imshow(im)
    plt.pause(0)

if __name__ == '__main__':
    main()


