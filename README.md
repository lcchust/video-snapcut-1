# video-SnapCut
Advance in Computer Graphics Research final project

## papers for selection

- [ ] [Video Object Cut and Paste](https://www.cs.cmu.edu/~efros/courses/AP06/Papers/li-siggraph-05.pdf) SIGGRAPH 2005

- [ ] [Improved Seam Carving for Video Retargeting](http://www.eng.tau.ac.il/~avidan/papers/vidret.pdf)

- [x] [Video SnapCut: Robust Video Object Cutout Using Localized Classifiers](http://juew.org/publication/VideoSnapCut_lr.pdf) SIGGRAPH 2009 (let's go with this)

## types

frame(img): CV_8UC3
mask: CV_8UC1
frame_lab: CV_32FC3
boundry_distance: CV_64FC1

window:

foreground_gmm_mat_: CV_32FC1 
shape_confidence_: CV_64FC1

## Roadmap
6.12 TODOs
- [x] use command to extract key frames from original video
```bash
ffmpeg -skip_frame nokey -i book
.mov -vsync 0 -r 30 -f image2 keyframe-%02d.jpg
```
- [x] initialize first frame
- [ ] feature detection on second, affine transformation
- [ ] optical flow
- [ ] update shape model
- [ ] update color model
- [ ] merge window 
- [ ] cut out final foreground mask(graphcut)