/* CS148 Summer 2014
 * Final Project
 * furDemo.vert
 *
 * Name: Tsao-Wei (Brad) Huang
 * ---------------------------
 * This pair of shaders (furDemo.vert and furDemo.frag) is used to present the 
 * effect of making fur segments closer to the skin darker.
 */

// varying variables
varying vec2 furCoord;			// fur texture coordinate of the vertex
varying float currLayer;		// the index of layer the current vertex is in

// unifrom variables
uniform sampler2D furImage;		// fur texture with random cyan dots 
uniform sampler2D colorTexture;	// tiger color texture 

void main()
{
	// texture coordinate 0 stores the coordinate for the fur texture
	furCoord = gl_MultiTexCoord0.st;

	// using fog coordinate to pass in the current layer's displacement
	currLayer = gl_FogCoord;

	gl_Position = ftransform();
}