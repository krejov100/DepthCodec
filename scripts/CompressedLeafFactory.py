from bitstream import BitStream

from zope.interface import implementer

from DepthFunctions import get_best_function
from FunctionLeaf import FunctionLeaf
from QuadTreeNode import QuadTreeNode
from Tree import LeafDataFactory


@implementer(LeafDataFactory)
class CompressedLeafFactory:
    def __init__(self, min_patch_snr:float):
        self.min_patch_snr = min_patch_snr
        self.__data_stream = BitStream()
        self.split_counts = [{}, {}, {}, {}]
        n = 10
        po2s = [2 ** j for j in range(0, n + 1)]
        for i in range(4):
            for po2 in po2s:
                self.split_counts[i][po2] = 0

    # noinspection PyMethodMayBeStatic
    def should_split(self, node: QuadTreeNode):
        # reached bottom
        if node.roi.width == 1 or node.roi.height == 1:
            return False
        sub_image = node.get_sub_image()

        best_function = get_best_function(sub_image, self.min_patch_snr)
        return best_function is None

    # noinspection PyMethodMayBeStatic
    def create_leaf_data(self, node: QuadTreeNode) -> FunctionLeaf:
        # this currently means the function sets are calculated twice,
        # only has compression time hit, and should be optimised out
        fl = FunctionLeaf()
        fl.set_depth_function(get_best_function(node.get_sub_image(), self.min_patch_snr))
        node.set_leaf_data(fl)

        self.split_counts[fl._depth_function.get_function_index()][node.roi.width] += 1
        return fl
