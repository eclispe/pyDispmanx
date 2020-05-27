from distutils.core import setup, Extension

# define the pydispmanx extension module
pydispmanx = Extension('pydispmanx', sources=['pydispmanx.c', 'image.c', 'imageLayer.c'], library_dirs=['/opt/vc/lib'], libraries=['bcm_host'], include_dirs=['/opt/vc/include', '/opt/vc/include/interface/vcos/pthreads', '/opt/vc/includes/interface/vmcs_host/linnux'])

# run the setup
setup(
    name = "PyDispmanx",
    ext_modules=[pydispmanx]
)
