from zope.interface import Interface
import numpy as np


class IDrawable(Interface):
	def draw(self, image: np.ndarray, debug=False):
		# render object to image
		pass
