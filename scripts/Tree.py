from unittest import *
from  zope.interface import Interface
from CalculateMatricies import *
from Rect import Rect
from QuadTreeNode import QuadTreeNode
from Drawable import IDrawable

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

class LeafData(Interface):
    pass

class LeafDataFactory(Interface):
    # noinspection PyMethodMayBeStatic
    def should_split(self, node: QuadTreeNode) -> bool:
        pass

    # noinspection PyMethodMayBeStatic
    def create_leaf_data(self, node: QuadTreeNode) -> LeafData:
        pass


class Tree:
    def __init__(self, root):
        self.root = root

    def breadth_wise_get_node(self):
        children = [self.root]
        for child in children:
            children.append(child.get_children())
            yield child

    def get_leafs(self):
        to_check = [self.root]
        leafs = []
        while to_check:
            current = to_check.pop(0)
            if not current.get_children():
                leafs = leafs + [current]
            else:
                to_check = to_check + current.get_children()
        return leafs

    def top_down(self, leaf_data_factory):
        to_grow = [self.root]
        while len(to_grow):
            leaf = to_grow.pop(0)
            if leaf_data_factory.should_split(leaf):
                leaf.split(leaf_data_factory)
                to_grow = to_grow + leaf.get_children()
            else:
                leaf.set_leaf_data(leaf_data_factory.create_leaf_data(leaf))

    #def encode(self, image):
    #    self.top_down(image)

    def draw(self, im, debug=False):
        to_visit = [self.root]
        while to_visit:
            current = to_visit.pop(0)
            current.draw(im, debug)
            to_visit = to_visit + current.get_children()
        return im

    #def decode(self):
    #    im = np.zeros((self.roi.width, self.roi.height))
    #    self.root.decode(im)
    #    return im


# Example of a Leaf_Data type that can be used to store data in the leaf nodes of the tree

class ExampleMaxLeaf(IDrawable):
    def __init__(self, im):
        self.__max = np.amax(im)

    def draw(self, im, debug=False):
        im.fill(self.__max)


def example_should_split(node: QuadTreeNode):
        # reached bottom
        if node.roi.width == 1 or node.roi.height == 1:
            return False
        return True


def example_leaf_data_factory(node: QuadTreeNode):
    return ExampleMaxLeaf(node.roi.sub_image(node.image))


class TestTree(TestCase):
    # TODO
    def test_top_down(self):
        #image = cv2.imread('bgExampleDepth.tif')
        #root = QuadTreeNode(image, Rect(0, 0, 16, 16))
        #tree = Tree(root)
        pass

    def Test_TreeWithImage(self):
        im = cv2.imread('bgExampleDepth.tif')
        im = cv2.resize(im[:, :, 1], (512, 512), 0, 0, cv2.INTER_NEAREST).copy()

        root = QuadTreeNode(im, Rect(0, 0, 512, 512))
        tree = Tree(root)
        tree.top_down(example_should_split, example_leaf_data_factory)

        decompressed = im.copy()
        tree.root.draw(decompressed, True)

        plt.imshow(im - decompressed)
        plt.figure()

        plt.imshow(decompressed)
        plt.figure()

        plt.imshow(im)
        plt.pause(0)


if __name__ == '__main__':
    main()
