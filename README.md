Face detection in 2D images
===========================

This repository contains an implementation of a facial recognition system that (hopefully) satisfies the requirements for a project in the 02238 Biometric Systems course at the Technical University of Denmark. The implementation is written in the OpenFrameworks C++ framework which is available from http://openframeworks.cc/

The text following this paragraph has been copy-pasted from the requirements PDF file, written by Christoph Busch.

Summary
-------
Implementation of the viola jones face detection algorithm in C/C++.

Background
----------
Face detection is an important processing step for surveillance or 2D face recognition applications based on video images. The viola jones method for object detection and the adaption to face detection introduced 2001/2004 is the standard approach to identify the presence and location of faces in 2D images.

Task
----
Analyze the CVPR 2001 and IJCV04 papers by Viola and Jones. Provide an own implementation based on open source libraries and do some experiments with provided data. Investigate the requirements with respect to image quality considering resolution, contrast and perspective.

Expected Outcome
----------------
- C/C++ implementation based on available open source frameworks.
- Demonstrator and test report.

Reading and other Material
--------------------------
- Viola and Jones, 2001: Rapid Object Detection using a Boosted Cascade of Simple Features, violajones_cvpr2001.pdf
- Viola and Jones, 2004: Robust Real-Time Face Detection, viola-ijcv04.pdf
- Sample matlab implementation and data: http://www.cmucam.org/projects/cmucam3/wiki/Viola-jones
- Vigra computer vision library: http://hci.iwr.uni-heidelberg.de/vigra/
- Numerical recipes in C/C++
