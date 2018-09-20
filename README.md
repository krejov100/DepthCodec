# Depth Codec
This repo looks to perform comparision between differing depth codecs, on a veriaty of depth data captured using RealSense D415 and D435.

## Motivation
Depth is demonstrating a number of improtant usages, ranging from computer vision to viewpoint dependent rendering and lightfield representation. Compressing depth efficiently offers a number of benifits including wireless cameras, enabeling IoT devices and lightweight wireless HMD's.

## Depth and Quadtree Structure
This implimentation focuses on per frame compression, ignoring savings that could come from temproal consistency. It based on a quadtrees representation that is designed to encode mid level structure from a image. The Quadtree provides the mechanisum to encode depth distontinuities, while its leafs represent the surface geometry within these discontinuities.  

We consider three main operations: at the highest level, we break the image up into a grid of patches, which is effectivly the minimum depth of a quadtree representations. The reason for this is discussed in more detail here /TODO link to reasons.

The mid level representation is handled using a quadtree structure, one for each patch. The tree structure is optimised such that leaf nodes or cells can be represented using compact functions, with few perameters. Branching is determined by each functions ability to reproduce the underlying depth. When a functions abuility to reproduce the depth is bellow a determined threshold that portion of the quadtree is subdivied.

The final depth representantion is encoded using four compact functions that can effectivly represent depth, in pertualar planer structure. The four functions: F0, F1, F2, F3 are based on platelett reprentations, with a carfully considered bit allocation for each. 

## bit encoding
This codec seeks to use the minimal required bit buget to represent both the tree structure and the parameters for F0, F1, F2, F3. There are also several bit flags used to indicate leaf/non-leaf and differentiate which F function is used.

### leaf/non-leaf flag
For a tree of know depth which is parsed in sequencial order is t

# to test
16bit normal ie 8bit x grad  8bit y grad
addaptive snr for close and long range
fix flying pixels in python
high speed decoder for practical use

## System Overview
There are two major components to this project, the compression methods and the evaluation framework.
The compression method is written to use a consistent interface allowing the evaluation framework to compare methods. 

## Polymorphism
Run time polymorphism is used at the image level of compression, reducing the overhead in runtime polymorphism.
Compile time polymorphism is handled using templated classes, allowing flexibility.

### Serialization
Serialization is handled using a combination of boost serialisation for classes that are handled polymorphicly.
Classes that are templated, implimenting the inteface:

```cpp
    virtual void writeCompressedData(std::ostream& out) = 0;
    virtual void readCompressedData(std::istream& in) = 0;
```

## Representations of 3D
There are a number of ways in which 3D data can be represented and impact on storage/transmission performance.
For example, when transmitting a planer surface its is much more compact to represent it using mesh or planer primitives then depth values.  

In order of the acquisition stack:
### Disparity
### DepthImage
### PointCloud 
### Mesh/Primatives

## Testing Framework
### Dataset for Evaluation
captured with extrinsic information, allowing comparitive evaluation between 
pointcloud and depth image methods

#### Preprocessing filters
Compression performance is again, highly correlated with the complexity and nature of the input data.
For example, run length encoding offers tremendous performance when most of the image is zero after back ground subtraction.
Another example is that bilateral filtering will provide smoother gradients, which favours wavelet methods.  

Examples of pre-processing
##### Back Ground Removal
##### bilateral filtering 
##### Resolution Upscaling
##### Hole Filling
 
### Evaluation    

#### Visualisation
    Qaulitative evaluation of performance.
    
#### Performance Measures


    


