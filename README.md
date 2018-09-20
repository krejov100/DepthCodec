# Depth Codec
This repo looks to perform comparisons between differing depth codecs, on a variety of depth data captured using RealSense D415 and D435.

## Motivation
Depth is demonstrating a number of important usages, ranging from computer vision to viewpoint dependent rendering and light-field representation. Compressing depth efficiently offers a number of benefits including wireless cameras, enabling IoT devices and lightweight wireless HMD's.

## Depth and Quadtree Structure
This implementation focuses on per frame compression, ignoring savings that could come from temporal consistency. It based on a quadtrees representation that is designed to encode mid-level structure from an image. The Quadtree provides the mechanism to encode depth discontinuities, while its leaves represent the surface geometry within these discontinuities.  

We consider three main operations: at the highest level, we break the image up into a grid of patches, which is effectively the minimum depth of quadtree representations. The reason for this is discussed in more detail here /TODO link to reasons.

The mid-level representation is handled using a quadtree structure, one for each patch. The tree structure is optimized such that leaf nodes or cells can be represented using compact functions, with few parameters. Branching is determined by each functions ability to reproduce the underlying depth. When a functions ability to reproduce the depth is below a determined threshold that portion of the quadtree is subdivided.

The final depth representation is encoded using four compact functions that can effectively represent depth, in particular planer structure. The four functions: F0, F1, F2, F3 are based on platelet representations, with a carefully considered bit allocation for each. 

## bit encoding
This codec seeks to use the minimum required bit budget to represent both the tree structure and the parameters for F0, F1, F2, F3. There are also several bit flags used to indicate leaf/non-leaf and differentiate which F function is used.

### leaf/non-leaf flag
For a tree of know depth which is parsed in sequential order is t

# to test
16bit normal ie 8bit x grad  8bit y grad
adaptive snr for short and long range
fix flying pixels in python
high-speed decoder for practical use

## System Overview
There are two major components to this project, the compression methods, and the evaluation framework.
The compression method is written to use a consistent interface allowing the evaluation framework to compare methods. 

## Polymorphism
Runtime polymorphism is used at the image level of compression, reducing the overhead in runtime polymorphism.
Compile time polymorphism is handled using templated classes, allowing flexibility.

### Serialization
Serialization is handled using a combination of boost serialization for classes that are handled polymorphically.
Classes that are templated, implementing the interface:

```cpp
    virtual void writeCompressedData(std::ostream& out) = 0;
    virtual void readCompressedData(std::istream& in) = 0;
```

## Representations of 3D
There are a number of ways in which 3D data can be represented and impact on storage/transmission performance.
For example, when transmitting a planer surface it is much more compact to represent it using mesh or planer primitives then depth values.  

In order of the acquisition stack:
### Disparity
### DepthImage
### PointCloud 
### Mesh/Primatives

## Testing Framework
### Dataset for Evaluation
captured with extrinsic information, allowing comparative evaluation between 
point cloud and depth image methods

#### Preprocessing filters
Compression performance is again, highly correlated with the complexity and nature of the input data.
For example, run length encoding offers tremendous performance when most of the image is zero after background subtraction.
Another example is that bilateral filtering will provide smoother gradients, which favors wavelet methods.  

Examples of pre-processing
##### Back Ground Removal
##### bilateral filtering 
##### Resolution Upscaling
##### Hole Filling
 
### Evaluation    

#### Visualisation
    Qualitative evaluation of performance.
    
#### Performance Measures


    


