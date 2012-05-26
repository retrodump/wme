@echo off

mkdir debug
mkdir release

copy ..\visualc\dep\ogre\lib\OgreMain_d.dll                  .\debug\
copy ..\visualc\dep\ogre\lib\Plugin_CgProgramManager_d.dll   .\debug\
copy ..\visualc\dep\ogre\lib\Plugin_OctreeSceneManager_d.dll .\debug\
copy ..\visualc\dep\ogre\lib\Plugin_ParticleFX_d.dll         .\debug\
copy ..\visualc\dep\ogre\lib\RenderSystem_Direct3D9_d.dll    .\debug\
copy ..\visualc\dep\ogre\lib\RenderSystem_GL_d.dll           .\debug\
copy ..\visualc\dep\ogre\lib\cg.dll                          .\debug\
copy ..\visualc\dep\vld\bin\vld_x86.dll                      .\debug\
copy ..\visualc\dep\vld\bin\dbghelp.dll                      .\debug\
copy ..\visualc\dep\freeimage\lib\FreeImage.dll              .\debug\
copy ..\visualc\dep\OIS\lib\OIS_d.dll                        .\debug\

copy ..\visualc\dep\ogre\lib\OgreMain.dll                    .\release\
copy ..\visualc\dep\ogre\lib\Plugin_CgProgramManager.dll     .\release\
copy ..\visualc\dep\ogre\lib\Plugin_OctreeSceneManager.dll   .\release\
copy ..\visualc\dep\ogre\lib\Plugin_ParticleFX.dll           .\release\
copy ..\visualc\dep\ogre\lib\RenderSystem_Direct3D9.dll      .\release\
copy ..\visualc\dep\ogre\lib\RenderSystem_GL.dll             .\release\
copy ..\visualc\dep\ogre\lib\cg.dll                          .\release\
copy ..\visualc\dep\freeimage\lib\FreeImage.dll              .\release\
copy ..\visualc\dep\OIS\lib\OIS.dll                          .\release\
