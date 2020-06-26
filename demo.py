import pydispmanx, time, pygame
print(pydispmanx.getDisplaySize())
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
print(trials/(end-start))
time.sleep(2)
del(pygame_surface)
print("no surface")
time.sleep(2)
del(testlayer)
print("no layer")
time.sleep(2)
