# gl3dv - 3D mesh viewer v1.2

by George Litos (gl@cyberpunk.gr)

based on [Coin3D](https://github.com/coin3d)

> WARNING - old code base (last compiled on 2008) 

## Command Line Options

usage: gl3dv [filename] [options]

	--stereo
		enables stereo viewer at startup

	--stereooffset N
		set the offset between the two viewpoints when in stereo mode

	--background R G B
		color values in the range [0.0, 1.0]

	--size W H
		output window size in pixels WxH

	--numpasses N
		antialising number of passes in the range [1, 15]

	--anaglyph


filename:
        VRML file (*.wrl)
		VRML97 compatible or older VRML format files

        PTS pointset file (*.pts)
		point cloud files using the following format (each line):
		(float)x (float)y (float)z (int)R (int)G (int)B
		correct RGB values are in the range of 0 to 255

        Inventor file (*.iv)
		Inventor compatible files


## GUI Options

```
[i]		diplay information panel [OSD]
[s]		save snapshot in JPEG format (default filaneme: snapshot.jpg in current directory)
[p]		save snapshot in PNG format (supports transparency)
[e]		save snapshot in EPS format
[a]		toggle antialiasing
[t]		toggle transparency
[c]		toggle autocropping
[b/B]		increase/decrease crop border size (pixels)
[x|y|z]		rotate on the corresponding axis
[m]		toggle image saving status,
		saves image sequence when rotating (frame0xxx.png)

Right mouse button menu brings up various self explanatory display options.
```

## Feedback

For support, bugs, recommendations etc. check https://github.com/glls/gl3dv
