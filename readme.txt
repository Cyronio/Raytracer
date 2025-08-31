1. Claim
	T1, T2, T3, T4, T5, T6, ((T7)) / EF4, EF10 / B6
	
	ad T6: the cone is rendered darker than it should be. I could not find the reason for this
	ad T7: I've somehow implemented object transformations; mat3 matrix class should work properly, the parsing should be correct
		and the calculation of the transform matrix (but maybe the order is wrong)
		What doesn't seem right is the implementation
		Spheres appear to somehow translate and scale correctly; the rotation is around some other point, though, it seems
		Meshes only translate and rotate somewhat correctly; the scale doesn't work at all; weirdly, only the ratio seems to matter,
		for example scaling 1,2,1 at least gives a stretch in the y-axis. But uniform scaling doesn't change anything
		Also: I haven't calculated the shadwos accordingly
		Example9 is rendered as a black image sadly
		I've added an "exampleTransform.xml" to show that some transformations seem to work (kind of). Animation works for the file as well (see below)

	EF4: Motion blur should work for all example files (append "-blur")
	EF10: Animation used to work consistently for all but example5 (which throws an error from the gif.h-library; I couldn't find the reason)
		Unfortuantely, after trying to implement object transformation, examples 7 and 8 also throw errors (which is a pity, since example8 looked rather nice)
		append "-anim"
	B6: Super-Sampling should work for every sample (append "-super")
	
2. Tested Environments
	Developed, compiled and tested on Almighty. Have not been able to compile or run on Windows yet

3. Additional and general remarks
	
	To compile: "clang++ ray.cpp lodepng.cpp pugixml.cpp -o ray"
	To run: "./ray <filename.xml> [flag]
	
	flags are: "-blur"  for motion blur
		   "-anim"  for gif animation
                   "-super" for super-sampling	

	for example: "./ray example5.xml -super"

	
	
	


