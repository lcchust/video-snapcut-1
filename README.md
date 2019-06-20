# Video-SnapCut

Video-SnapCut is the course project for Advance in Computer Graphics, Zhejiang Unviersity. In this project, we have reimplemented the video object segmentation system describe in [Video SnapCut: Robust Video Object Cutout Using Localized Classifiers](http://juew.org/publication/VideoSnapCut_lr.pdf).

In this project, we have implemented all the following:
- A complete pipeline of Video-SnapCut (Section 2-3 of the paper)
- Alpha matting (Section 4 of the paper)
- UI for interactive refinement (Section 5 of the paper)
- Test and comparison against Adobe After Effects' Rotobrush, see report (Section 6 of the paper)
  
## Prerequisite
- OpenCV: 3.4.x
- QT: 5.x

## Build
You can build the project using QMake

## Results

![Cutout Results](images/results.png)

## Reference
1. Xue Bai, Jue Wang, David Simons, and Guillermo Sapiro. 2009. Video SnapCut: robust video object cutout using localized classifiers.
2. Carsten Rother, Vladimir Kolmogorov, and Andrew Blake. 2004. "GrabCut": interactive foreground extraction using iterated graph cuts.
3. Y. Y. Boykov and M. -. Jolly. 2001. "Interactive graph cuts for optimal boundary & region segmentation of objects in N-D images
