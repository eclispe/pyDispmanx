import pydispmanx, time, pygame
print(pydispmanx.getDisplays())
print(pydispmanx.getDisplaySize())
print(str(pydispmanx.getFrameRate())+"Hz")
print(pydispmanx.getPixelAspectRatio())
testlayer = pydispmanx.dispmanxLayer(3);
pygame_surface = pygame.image.frombuffer(testlayer, testlayer.size, 'RGBA')
trials = 100
start=time.time()
for n in  range(int(trials/2)):
    pygame.draw.circle(pygame_surface, (255,0,0), (int(testlayer.size[0]/2), int(testlayer.size[1]/2)), int(min(testlayer.size)/4), 0)
    testlayer.updateLayer()
    pygame.draw.circle(pygame_surface, (0,0,255), (int(testlayer.size[0]/2), int(testlayer.size[1]/2)), int(min(testlayer.size)/4), 0)
    testlayer.updateLayer()
end=time.time()
print(str(round(trials/(end-start),2))+"fps")
framelag = (trials/(end-start))/pydispmanx.getFrameRate()
print(str(round(framelag*100,2))+"%")
if(framelag < 0.75):
    print("Tests under 75% max frame rate, try reducing resolution or frame rate")
time.sleep(0.5)
del(pygame_surface)
print("no surface")
time.sleep(0.5)
del(testlayer)
print("no layer")
time.sleep(0.5)
