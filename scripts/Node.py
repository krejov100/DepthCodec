
# TODO Comment
class Node:
    def __init__(self, parent):
        self._children = []
        self._leaf_data = None
        self._parent = parent

    # TODO Comment
    def get_children(self):
        return self._children

    # TODO Comment
    def get_parent(self):
        return self._parent

    # TODO Comment
    def is_leaf(self) -> bool:
        return self._leaf_data is not None

    # TODO Comment
    def set_leaf_data(self, leaf_data):
        self._leaf_data = leaf_data

    # TODO Comment
    def get_leaf_data(self):
        return self._leaf_data

    # TODO Comment
    def prune(self):
        self._leaf_data.combine(self, self._children)
        self._children = []

    # TODO Comment
    def split(self, leaf_data_factory):
        self._children = self.split_into_children(leaf_data_factory)
