from bitstream import BitStream

from QuadTreeNode import QuadTreeNode
from Rect import Rect
from CompressedLeafFactory import CompressedLeafFactory
from Tree import Tree
import numpy as np


class QuadTreeCodec:
    def __init__(self, quad_tree_size=64, min_cell_snr=70, debug=False):
        self.quad_tree_size = quad_tree_size
        self.min_cell_snr = min_cell_snr
        self.debug = debug
        self.tree = None

    def get_codec_shape(self):
        return [self.quad_tree_size, self.quad_tree_size]

    def compress(self, image: np.ndarray):
        root = QuadTreeNode(image, Rect(0, 0, self.quad_tree_size, self.quad_tree_size))
        self.tree = Tree(root)
        leaf_factory = CompressedLeafFactory(self.min_cell_snr)
        self.tree.top_down(leaf_factory)

    # noinspection PyMethodMayBeStatic
    def uncompress(self, cell: np.ndarray):
        self.tree.draw(cell, self.debug)

    def encode(self, stream: BitStream):
        return stream

    def decode(self, stream: BitStream):
        return stream