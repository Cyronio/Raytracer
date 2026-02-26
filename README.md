# Raytracer

A Raytracer written in C++ for a university course on Computer Graphics at the University of Vienna.

Basic features: Phong shading, basic reflection and refraction, shadows, texture mapping, camera transformations.

Special effects and post-processing: motion blur, animations scripts, super sampling

## Instructions

To compile on Linux run: "clang++ ray.cpp lodepng.cpp pugixml.cpp -o ray"

To run: "./ray <filename.xml> [flag]"
	
flags are: 
			
		   "-blur"  for motion blur

		   "-anim"  for gif animation
			   
           "-super" for super-sampling	
		   
-> for example: "./ray example5.xml -super"

## XML Formatting

The example scenes are formatted as described here: https://teaching.vda.univie.ac.at/graphics/24s/Labs/Lab3/lab2_file_specification.html 

## Example Scenes

<img src="https://github.com/Cyronio/Raytracer/blob/main/ExamplePictures/example7_super.png">

<img src="https://github.com/Cyronio/Raytracer/blob/main/ExamplePictures/example8_super.png">

<img src="https://github.com/Cyronio/Raytracer/blob/main/ExamplePictures/example6_gif.gif">

## External Libraries

- PugiXML: https://github.com/zeux/pugixml?tab=readme-ov-file
- LodePNG: https://github.com/lvandeve/lodepng 

