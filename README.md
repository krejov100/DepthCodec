# Depth Codec

## System Overview
There are two major components to this project, the compression methods and the evaluation framework.
The compression method is written to use a consistent interface allowing the evaluation framework to compare methods. 

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


    


