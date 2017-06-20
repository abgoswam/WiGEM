# WiGEM

A Learning-Based Approach for Indoor
Localization

> Work done as part of my M.S. degree from [The State Univ. of New York, Stony Brook](https://www.cs.stonybrook.edu/). We presented this at [ACM CoNEXT, Tokyo, Japan. 2011](http://conferences.sigcomm.org/co-next/2011/)

## Abstract

*We consider the problem of localizing a wireless client in
an indoor environment based on the signal strength of its
transmitted packets as received on stationary sniffers or access
points. Several state-of-the-art indoor localization techniques
have the drawback that they rely extensively on a
labor-intensive‘training’ phase that does not scale well. Use
of unmodeled hardware with heterogeneous power levels further
reduces the accuracy of these techniques.*

*We propose a ‘learning-based’ approach, WiGEM, where
the received signal strength is modeled as a Gaussian Mixture
Model (GMM). Expectation Maximization (EM) is used
to learn the maximum likelihood estimates of the model parameters.
This approach enables us to localize a transmitting
device based on the maximum a posteriori estimate.
The key insight is to use the physics of wireless propagation,
and exploit the signal strength constraints that exist for
different transmit power levels. The learning approach not
only avoids the labor-intensive training, but also makes the
location estimates considerably robust in the face of heterogeneity
and various time varying phenomena. We present
evaluations on two different indoor testbeds with multiple
WiFi devices. We demonstrate that WiGEM’s accuracy is at
par with or better than state-of-the-art techniques but without
requiring any training.*

The full paper is [here](wigem.pdf)
