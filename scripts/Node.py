from  zope.interface import Interface, Attribute


class Node(Interface):
    _children = Attribute("child Nodes")
    _parent = Attribute("Reference to Parent")
    _leaf_data = Attribute("Data for this leaf_node")

    def __init__(self, parent):
        self._children = []
        self._leaf_data = None
        self._parent = parent

    def get_children(self):
        return self.children

    def get_parent(self):
        return self._parent

    def set_leaf_data(self, leaf_data):
        self._leaf_data = leaf_data

    def get_leaf_data(self):
        return self._leaf_data

    def prune(self):
        self.leaf_data.combine(self, self.children)
        self.children = []

    def split(self, leaf_data_factory):
        pass

