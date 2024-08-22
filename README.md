# Autonomous Mini Drone

### Design of Mini Quadcopter used for autonomous tasks


## Dependencies
Install the pseyepy python library: [https://github.com/bensondaled/pseyepy](https://github.com/bensondaled/pseyepy)

This project requires the sfm (structure from motion) OpenCV module, which requires you to compile OpenCV from source[^1]. This is a bit of a pain, but these links should help you get started: [SFM dependencies](https://docs.opencv.org/4.x/db/db8/tutorial_sfm_installation.html) [OpenCV module installation guide](https://github.com/opencv/opencv_contrib/blob/master/README.md)

## Runing the code

From the computer_code directory Run `yarn install` to install node dependencies 

Then run `yarn run dev` to start the webserver. You will be given a url view the frontend interface.

In another terminal window, run `python3 api/index.py` to start the backend server. This is what receives the camera streams and does motion capture computations.

## Documentation
