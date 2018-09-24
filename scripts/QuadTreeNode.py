import cv2
from Rect import Rect
import numpy as np
from Node import Node
from zope.interface import implementer
from Drawable import IDrawable


@implementer(IDrawable)
class QuadTreeNode(Node):
    def __init__(self, image: np.ndarray, roi: Rect, parent=None):
        Node.__init__(self, parent)
        self.image = image
        self.roi = roi

    def get_sub_image(self):
        return self.roi.sub_image(self.image)

    def combine(self, child_leafs):
        x = child_leafs[0].ROI.x
        y = child_leafs[0].ROI.y
        width = child_leafs[0].ROI.width * 2
        height = child_leafs[0].ROI.height * 2
        self.roi = Rect(x, y, width, height)
        return self

    def split_into_children(self, leaf_data_factory):
        x = self.roi.x
        y = self.roi.y
        half_width = self.roi.width / 2
        half_height = self.roi.height / 2

        return [QuadTreeNode(self.image, Rect(x, y, half_width, half_height), self),
                QuadTreeNode(self.image, Rect(x + half_width, y, half_height, half_height), self),
                QuadTreeNode(self.image, Rect(x, y + half_height, half_width, half_height), self),
                QuadTreeNode(self.image, Rect(x + half_width, y + half_height, half_width, half_height), self)]

    # TODO add check that child is drawable
    def draw(self, im, debug=False):
        sub_im = self.roi.sub_image(im)
        if self._leaf_data is not None:
            self._leaf_data.draw(sub_im)
        if debug:
            cv2.rectangle(im, self.roi.tl(), self.roi.br(), 0xfff)
        return sub_im