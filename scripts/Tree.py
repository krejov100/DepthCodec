import cv2
import matplotlib.pyplot as plt
import numpy as np
import copy


class Rect:
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

    def sub_im(self, im):
        return im[int(self.y):int(self.y) + int(self.height), int(self.x):int(self.x) + int(self.width)]

    def tl(self):
        return int(self.x), int(self.y)

    def br(self):
        return int(self.x + self.width), int(self.y + self.height)


class GradiantEncoding:
    def __init__(self):
        self.mean = 0

    def encode(self, im):
        self.mean = np.mean(im)

    def decode(self, im):
        im.fill(self.mean)

    # currently min max, but might be better to use plane equation, and discard p2,p3 when sufficiently small
    def should_split(self, im):
        im.max() - im.min() < 20



def reached_bottom(roi):
    if roi.width == 4 or roi.height == 4:
        return True
    return False


class QuadROILeaf:
    def __init__(self, roi):
        self.roi = roi
        self.encoding = None

    def combine(self, child_leafs):
        x = child_leafs[0].ROI.x
        y = child_leafs[0].ROI.y
        width = child_leafs[0].ROI.width * 2
        height = child_leafs[0].ROI.height * 2
        self.roi = Rect(x, y, width, height)
        return self

    def split(self):
        x = self.roi.x
        y = self.roi.y
        half_width = self.roi.width / 2
        half_height = self.roi.height / 2

        if not reached_bottom(self.roi):
            return [QuadROILeaf(Rect(x, y, half_width, half_height)),
                    QuadROILeaf(Rect(x + half_width, y, half_height, half_height)),
                    QuadROILeaf(Rect(x, y + half_height, half_width, half_height)),
                    QuadROILeaf(Rect(x + half_width, y + half_height, half_width, half_height))
                    ]
        else:
            return None

    def draw(self, im):
        cv2.rectangle(im, self.roi.tl(), self.roi.br(), 255)

    def decode(self, im):
        self.encoding.decode(self.roi.sub_im(im))

    def encode(self, im):
        self.encoding.encode(self.roi.sub_im(im))


class Node:
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

    def decode(self, im):
        self.leaf_data.decode(im)
        for child in self.children:
            child.decode(im)


def mse(a, b):
    return np.mean(((a - b) ** 2))


def psnr(a, b):
    return (20*np.log10(0xffff)) - (10 * np.log10(mse(a, b)))

#TODO refactor to this
def should_split(image, parent, children):
    pass

def should_split(image, leaf):
    return True


class Tree:
    def __init__(self, roi):
        self.root = None
        self.roi = roi

    def get_leafs(self, ):
        to_check = [self.root]
        leafs = []
        while to_check:
            current = to_check.pop(0)
            if not current.children:
                leafs = leafs + [current]
            else:
                to_check = to_check + current.children
        return leafs

    def top_down(self, image):
        to_grow = [self.root]
        while len(to_grow):
            leaf = to_grow.pop(0)

    # TODO refactor
            if should_split(image, leaf):
                leaf.grow()
                to_grow = to_grow + leaf.children
    #

    def bottom_up(self, image):
        # grow all the way to the bottom
        # this is much slower then address based tree which is used in c++, as redundant children are made
        to_grow = [self.root]
        while len(to_grow):
            leaf = to_grow.pop(0)
            leaf.grow()
            to_grow = to_grow + leaf.children

        print(len(self.get_leafs()))

    def encode(self, image):
        self.top_down(image)

    def draw(self, im):
        self.root.draw(im)

    def decode(self):
        im = np.zeros((self.roi.width, self.roi.height))
        return self.root.decode(im)


def main():
    tree = Tree(Rect(0, 0, 256, 256))
    tree.root = Node(QuadROILeaf(Rect(0, 0, 256, 256)), None)
    im = cv2.imread('bgExampleDepth.tif')

    #im = cv2.resize(im, (256, 256), 0, 0, cv2.INTER_NEAREST)
    #tree.encode(im)
    #tree.draw(im)
    #decompressed = tree.decode()

    #plt.imshow(decompressed)
    #plt.figure()
    plt.imshow(im)
    plt.pause(0)


if __name__ == '__main__':
    main()
