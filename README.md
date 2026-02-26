# Raytracer

A Raytracer written in C++ for a university course on Computer Graphics at the University of Vienna.

Basic features: Phong shading, basic reflection and refraction, shadows, texture mapping, camera transformations.

Special effects and post-processing: motion blur, animations scripts, super sampling

## Instructions

To compile on Linux run: "clang++ ray.cpp lodepng.cpp pugixml.cpp -o ray"

To run: "./ray <filename.xml> [flag]"
	
flags are: "-blur"  for motion blur
		       "-anim"  for gif animation
           "-super" for super-sampling	
-> for example: "./ray example5.xml -super"

## Example Scenes

The example scenes are formatted as described here: https://teaching.vda.univie.ac.at/graphics/24s/Labs/Lab3/lab2_file_specification.html 

<<<<<<< HEAD
<p>&nbsp;<img align="center" src="https://github-readme-stats.vercel.app/api?username=Cyronio&show_icons=true&theme=onedark&locale=en" alt="Cyronio" /></p>

To compile on Linux type: "clang++ ray.cpp lodepng.cpp pugixml.cpp -o ray"

To run type: "./ray <filename.xml> [flag]
	
-> flags are: "-blur"  for motion blur
		          "-anim"  for gif animation
              "-super" for super-sampling	

	for example: "./ray example5.xml -super"

look up the provided example.xml files for the layout necessary to be rendered by the Raytracer (documentation: https://teaching.vda.univie.ac.at/graphics/24s/Labs/Lab3/lab2_file_specification.html )



=======
>>>>>>> cb18dfd72bab496adfdceca7f04808db2badadb5
