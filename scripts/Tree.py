from sklearn import metrics
from CalculateMatricies import *
from scipy.ndimage.morphology import grey_closing

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


class MeanEncoding:
    def __init__(self):
        self.mean = 0

    def encode(self, im):
        self.mean = np.mean(im)

    def decode(self, im):
        im.fill(self.mean)

    # currently min max, but might be better to use plane equation, and discard p2,p3 when sufficiently small
    def should_split(self, im):
        return (im.max() - im.min()) > 10

class GradiantEncoding:
    def __init__(self):
        self.plane_eq = 0
        self.value = 0

    def encode(self, im):
        n = im.shape[0]
        if n > 1:
            self.plane_eq = compute_gradiant(im)
        else:
            self.value = im[0,0]
    def decode(self, im):
        n = im.shape[0]
        if n > 1:
            im[:,:] = render_gradiant(n, self.plane_eq)
        else:
            im[:,:].fill(self.value)


    def should_split(self, im):
        n = im.shape[0]
        if n == 1:
            return True;
        should_split = False
        plane_eq = compute_gradiant(im)

        rslt_im = render_gradiant(n,plane_eq)
        v = psnr(im, rslt_im)
        if v < 80:
            return True
        return im.min() == 0
        #return False


def reached_bottom(roi):
    if roi.width == 1 or roi.height == 1:
        return True
    return False


class QuadROILeaf:
    def __init__(self, roi):
        self.roi = roi
        self.encoding = GradiantEncoding()

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

    def should_split(self, im):
        return self.encoding.should_split(self.roi.sub_im(im))

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

    def encode(self, im):
        self.leaf_data.encode(im)
        for child in self.children:
            child.encode(im)

    def decode(self, im):
        self.leaf_data.decode(im)
        for child in self.children:
            child.decode(im)

    def should_split(self, im):
        return self.leaf_data.should_split(im)


def mse(a, b):
    return metrics.mean_squared_error(a,b)


def psnr(a, b):
    return (20*np.log10(0xffff)) - (10 * np.log10(mse(a, b)))

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
            if leaf.should_split(image):
                leaf.grow()
                leaf.encode(image)
                to_grow = to_grow + leaf.children


    def bottom_up(self, image):
        # grow all the way to the bottom
        # this is much slower then address based tree which is used in c++, as redundant children are made
        to_grow = [self.root]
        while len(to_grow):
            leaf = to_grow.pop(0)
            leaf.grow()
            to_grow = to_grow + leaf.children

    def encode(self, image):
        self.top_down(image)

    def draw(self, im):
        self.root.draw(im)
        return im

    def decode(self):
        im = np.zeros((self.roi.width, self.roi.height))
        self.root.decode(im)
        return im


def main():
    tree = Tree(Rect(0, 0, 512, 512))
    tree.root = Node(QuadROILeaf(Rect(0, 0, 512, 512)), None)
    im = cv2.imread('bgExampleDepth.tif')
    original_size = im.shape
    im = cv2.resize(im[:,:,1], (512, 512), 0, 0, cv2.INTER_NEAREST)
    #im = grey_closing(im, (5,5))
    tree.encode(im)
    print(len(tree.get_leafs()) * 32 / 1000/ 1000)
    print(512 * 512 * 16 / 1000 / 1000)
    decompressed = tree.decode()
    plt.imshow(im - decompressed)
    plt.figure()
    decompressed = cv2.resize(decompressed, (original_size[1], original_size[0]), 0, 0, cv2.INTER_NEAREST)

    plt.imshow(decompressed)
    plt.figure()
    im = tree.draw(im)
    plt.imshow(im)
    plt.pause(0)


if __name__ == '__main__':
    main()
