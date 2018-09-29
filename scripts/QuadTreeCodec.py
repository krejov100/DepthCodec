from bitstream import BitStream

from QuadTreeNode import QuadTreeNode
from Rect import Rect
from CompressedLeafFactory import CompressedLeafFactory
from Tree import Tree
import numpy as np

# TODO Comment
class QuadTreeCodec:
    def __init__(self, quad_tree_size, min_cell_snr, debug=False):
        self.quad_tree_size = quad_tree_size
        self.min_cell_snr = min_cell_snr
        self.debug = debug
        self.tree = None

    # TODO Comment
    def get_codec_shape(self):
        return [self.quad_tree_size, self.quad_tree_size]

    # TODO Comment
    def compress(self, image: np.ndarray):
        root = QuadTreeNode(image, Rect(0, 0, self.quad_tree_size, self.quad_tree_size))
        self.tree = Tree(root)
        leaf_factory = CompressedLeafFactory(self.min_cell_snr)
        self.tree.top_down(leaf_factory)

    # TODO Comment
    # noinspection PyMethodMayBeStatic
    def uncompress(self, cell: np.ndarray):
        self.tree.draw(cell, self.debug)

    # TODO Comment
    def encode(self, stream: BitStream):
        self.encode_node(self.tree.root, stream)
        return stream

    # TODO Comment
    def encode_node(self, node, stream: BitStream):
        is_leaf = node.is_leaf()
        stream.write(is_leaf)
        if is_leaf:
            node.get_leaf_data().encode(stream)
        else:
            for child in node.get_children():
                self.encode_node(child, stream)
        return stream

    # TODO Comment
    def decode(self, stream: BitStream):
        return stream