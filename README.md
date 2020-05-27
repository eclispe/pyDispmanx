# PyDispmanx

This python module provides a buffer interface to a Raspberry Pi GPU layer through the dispmanx API.

This was designed as a simple interface to allow the GPU layers of the Raspberry Pi to be used with existing graphics libraries that can interface with a provided buffer such as cairo and pyGame.

## Minimal example
```python
import pydispmanx, pygame
# Create new layer object for GPU layer 1
demoLayer = pydispmanx.dispmanxLayer(1)
# Create pyGame surface linked to the layer buffer
demoSurface = pygame.image.frombuffer(demoLayer, demolayer.size, 'RGBA')

# Use exsisting pyGame features to draw to the surface

# Trigger the redraw of the screen from the buffer
demoLayer.updateLayer()

# Do other things, redraw layers etc

# Delete surface before layer
del(demoSurface)
# Delete layer
del(demoLayer)
```

## Install

Install prerequisites:

```sudo apt-get install python3-dev```

Compile it:

```python3 setup.py build_ext --inplace```

Copy the .so file to the source folder of your project (I probably wouldn't install this system wide yet)

```cp pydispmanx.cpython-37m-arm-linux-gnueabihf.so ~/myproject/```

### Test

The demo script can be run by `python3 demo.py`. This should draw 10 circles on the GPU layer 3 alternating red and blue as fast as possible and then display the framerate. The script will then destroy the surface and the layer and 2 seconds apart to check proper cleanup.

You can view the currently active dispmanx layers by running `vcgencmd dispmanx_list`

## To Do
- Add some error handling, any error handling, there is currently none
- Add support for other colour modes
- Add support for non-fullscreen buffers
  - Add support for moving buffers around the screen
- Integrate to create surfaces directly for various libraries rather than just the buffer
- Possibly add support for python2 for legacy projects
- Investigate touch events from built in screens

## License
This project uses some common files from the https://github.com/AndrewFromMelbourne/raspidmx examples project and those files retain their original licence (see file headers for details).

All other files are licensed under GNU LGPL version 3. See file headers, the COPYING file and COPYING.LESSER file for more details.
