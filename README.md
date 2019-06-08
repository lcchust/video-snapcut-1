# video-SnapCut
Adance in Computer Graphics Research final project

## papers for selection

- [ ] [Video Object Cut and Paste](https://www.cs.cmu.edu/~efros/courses/AP06/Papers/li-siggraph-05.pdf) SIGGRAPH 2005

- [ ] [Improved Seam Carving for Video Retargeting](http://www.eng.tau.ac.il/~avidan/papers/vidret.pdf)

- [x] [Video SnapCut: Robust Video Object Cutout Using Localized Classifiers](http://juew.org/publication/VideoSnapCut_lr.pdf) SIGGRAPH 2009 (let's go with this)



### Critical Steps

#### localized classifiers

For frame $I_t$, we have:

* mask $L^t(x)$
  * segmentation label：$\mathcal{F}=1$ and $\mathcal{B} = 0$
  * generated from previous operations, either user initialization or previous propagation
  * the border line of the mask $L^t(x)$ is the contour $C_t$

* local windows $W_1^t, \cdots, W_n^t$
  * **window size**: from $30\times30$ to $80\times80$
  * **windows overlap**: 1/3rd of the sindow size
  * classifier: assign to every pixel a foreground probability
    * local color model $M_c$
      * color model confidence $f_c$
    * local shape model $M_s$
      * contains the existing segmentatinon mask $L^t(x)$
      * shape confidence mask $f_s(x)$
        * **important parameter** $\sigma_s$ : adaptively and automatically adjusted
    * foreground & background GMM
      * **color space**: Lab
      * **GMM training data**: 5 pixels threshold to **segmented** boundary
      * **number of components**: each GMM is set to 3
      * **foreground probability** $p_c(x)=\frac{p_c(x|\mathcal{F})}{p_c(x|\mathcal{F})+p_c(x|\mathcal{B})}$

#### single-frame propagation

1. motion estimation
   * global affine transform using SIFT features inside foreground
     * align $I_t$ to $I_{t+1}$, get new image $I_{t+1}'$
   * Optical flow between $I_{t+1}'$ and $I_{t+1}$
     * only focus on pixels inside the foreground of $I_{t+1}'$
     * for each window $W_i$, the movement of window center is the **average flow vector** inside the region of foreground of this window
     * (does every pixel on contour need to move along flow vertor?)

2. update local models