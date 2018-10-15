from PIL import Image
from DepthFunctions import *
from progressbar import progressbar
from scipy.misc import imresize

# TODO Comment
class TiledCodec:
    def __init__(self, codec_factory):
        self.codecs = []
        self.original_size = []
        self.codec_factory = codec_factory

    # TODO Comment
    def compress(self, image: np.ndarray):
        self.original_size = np.asarray(image.shape)
        codec_shape = self.codec_factory.get_codec_shape()
        # resize the image to a compatible size, this for prototype purpose
        new_size = (self.original_size // codec_shape) * codec_shape
        # having to use pil and openCV bug #9096
        resized_image = cv2.resize(image, tuple([new_size[1],new_size[0]]), interpolation=cv2.INTER_NEAREST)
        #image = np.asarray(Image.fromarray(image).resize((new_size[1], new_size[0]), Image.NEAREST))

        for y in progressbar(range(0, new_size[0], codec_shape[0])):
            for x in range(0, new_size[1], codec_shape[1]):
                codec = self.codec_factory.create_codec()
                sub_image = Rect(x, y, codec_shape[0], codec_shape[1]).sub_image(resized_image)
                codec.compress(sub_image)
                self.codecs.append(codec)

    # TODO Comment
    def uncompress(self) -> np.ndarray:
        codec_shape = self.codecs[0].get_codec_shape()
        new_size = (self.original_size // codec_shape) * codec_shape
        image = np.zeros((new_size[0], new_size[1]), dtype=np.uint16)
        index = 0
        for y in progressbar(range(0, new_size[0], codec_shape[0])):
            for x in range(0, new_size[1], codec_shape[1]):
                sub_image = Rect(x, y, codec_shape[0], codec_shape[1]).sub_image(image)
                self.codecs[index].uncompress(sub_image)
                index += 1
        image = cv2.resize(image, tuple([self.original_size[1], self.original_size[0]]), interpolation=cv2.INTER_NEAREST)
        #image = cv2.rectangle(image, (x,y), tuple(np.asarray([x,y])+codec_shape), np.max(image))
        #image = np.asarray(Image.fromarray(image).resize((self.original_size[1], self.original_size[0]), Image.NEAREST))
        return image

    # TODO Comment
    def encode(self, stream: BitStream):
        for codec in self.codecs:
            codec.encode(stream)
        return stream

    # TODO Comment
    def decode(self, stream: BitStream):
        for codec in self.codecs:
            codec.decode(stream)