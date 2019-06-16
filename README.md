## OpenTX NV14 Branch 

[<img align="left" width=50 src="TravisCI-Mascot-2.png" />](https://travis-ci.org/) [![Build Status](https://travis-ci.org/derdoktor667/Flysky-OpenTX-Test-Branch.svg?branch=matrix14)](https://travis-ci.org/derdoktor667/Flysky-OpenTX-Test-Branch)

Build command:

cmake -DGVARS=OFF -DLUA=ON -DLUA_COMPILER=ON -DPCB=NV14 /
      -DSIMU_AUDIO=OFF -DSIMU_LUA_COMPILER=OFF -DUSB_SERIAL=ON /
      -DHELI=OFF -DFONT=SQT5 -DMULTIMODULE=ON / 
      -DALLOW_NIGHTLY_BUILDS=ON -DDANGEROUS_MODULE_FUNCTIONS=ON / 
      -DAUTOSWITCH=ON -DAUTOSOURCE=ON -DBOOTLOADER=OFF -DGUI=YES ../

best regards

derdoktor667
