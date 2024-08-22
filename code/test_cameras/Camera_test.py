from pseyepy import Camera, Display

c = Camera([0,1], fps=[60, 60], resolution=Camera.RES_LARGE, colour=True)
d = Display(c) # begin the display


# read from the camera/s
frame, timestamp = c.read()

# when finished, close the camera
c.end()