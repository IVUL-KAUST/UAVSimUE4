# Scale and Motion Adaptive Correlation Filter Tracker (SMACF)

This is the Matlab code for SMACF which was submitted to VOT16 with integration to use it with our simulator.
 Our proposed tracker builds on top of the popular KCF (Kernelized Correlation Filter) tracker by adding
several improvements while maintaining real­time speed (>30fps). We integrate Colorname and HOG features. We make the cell size adaptive with regard to target size in order to ensure
sufficient resolution of the object being tracked. Moreover, we implement a first order motion model to improve robustness to camera motion. Lastly, we perform a weighted scale search at each iteration for more accurate bounding
boxes.

## Test UDP connection
Start the simulator and fly the UAV above the car (N to increase altitude, arrow keys to navigate, H to enable the view from the UAV).

When the UAV is positioned press T to start tracking using the groundtruth.

Now run the udp_test.m file in Matlab. You will see the framerate displayed in the command window. Optionally you can display the image feed. 


## Run Tracker
Start the simulator and fly the UAV above the car (N to increase altitude, arrow keys to navigate, H to enable the view from the UAV).

When the UAV is positioned press Y to enable feedback from a vision algorithm (This is important! If you don't do this step, the groundtruth will be used for tracking).

Now run the run_tracker.m file in Matlab. You will see messages on the screen, that the tracker is waiting for data from the simulator. 

Click on the window of the simulator to bring it into scope and press T to start tracking. 