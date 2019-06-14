# video-SnapCut
Advance in Computer Graphics Research final project

#### papers for selection

- [ ] [Video Object Cut and Paste](https://www.cs.cmu.edu/~efros/courses/AP06/Papers/li-siggraph-05.pdf) SIGGRAPH 2005

- [ ] [Improved Seam Carving for Video Retargeting](http://www.eng.tau.ac.il/~avidan/papers/vidret.pdf)

- [x] [Video SnapCut: Robust Video Object Cutout Using Localized Classifiers](http://juew.org/publication/VideoSnapCut_lr.pdf) SIGGRAPH 2009 (let's go with this)



## Roadmap

6.12 TODOs

- [ ] initialize first frame
- [x] feature detection on second, affine transformation
- [x] optical flow
- [ ] update shape model
- [ ] update color model
- [ ] merge window 
- [ ] cut out final foreground mask(graphcut)



## Critical Steps

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

2. update local models

   * color model
     * history color model
     * updated color model
       * how to combine?
     * use the one with the smaller foreground region
   * shape prior
     * shape model and shape confidence
   * color and shape integration
     * compute $\sigma_s$
     * use shape confidence $f_s(x)$ to perform a linear combination

3. object cutout

   * full-frame foreground probabilities computed from overlaping windows

   * pixel-level Graph Cut to generate $L^{t+1}(x)$

   * use this newly generated mask iteratively perform Step2: update local models and Step3: object cutout
