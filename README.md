# PicProcess

Image Processing software based on Qt5. Assignment of lesson `Image Process`.

## Functions

+ Arithmetic Operations.
+ Geometry Operations: 
    * translation
    * scale (up, down)
    * rotation
    * reflect
+ Non-Geometry Operations:
    * Exponential Gray-Scale Transformation.
    * Histogram Equalization.
+ FFT
+ Filter:
    * Median Filter, Mean Filter, Gaussian Filter.
+ High-level process；
    * Edge-Detection: based on `Roberts`, `Sobel`, `Prewitt`, `Laplace`.
    * Clustering color of pixels by training KMeans.
    * Image Sharpening: based on `Laplace` operator.
+ Examples:
    * An example of refactoring number of terms of boundary using FFT and inverse FFT.     

For more implementing details and usage, refer to the codes or a [simple design description in Chinese](docs/README.md). 

## Further Notice

+ Related image process functions are **hand-crafted** **without support** of other library like **`opencv`**.  So it would **take longer time** to do operation needing massive matrix calculation. 

## Plans

- [ ] More Language Supported.
- [ ] Code Cleaning and Refactoring.
- [ ] Matrix Operation Speed Up with library like MKL.
- [ ] Add more image process function.