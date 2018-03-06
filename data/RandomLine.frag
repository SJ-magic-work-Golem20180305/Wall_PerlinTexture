/************************************************************
************************************************************/
#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

// #define TIME_BASED

/************************************************************
************************************************************/
uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;
uniform vec4 BaseColor;

/************************************************************
************************************************************/

/******************************
******************************/
float random (in float x) {
    return fract(sin(x)*1e4);
}

/******************************
******************************/
float random (in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))* 43758.5453123);
}

/******************************
******************************/
float pattern(vec2 st, vec2 v, float t) {
    vec2 p = floor(st+v);
    return step(t, random(100.+p*.000001)+random(p.x)*0.5 );
}

/******************************
******************************/
// Value noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/lsf3WH
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    vec2 u = f*f*(3.0-2.0*f);
    return mix( mix( random( i + vec2(0.0,0.0) ),
                     random( i + vec2(1.0,0.0) ), u.x),
                mix( random( i + vec2(0.0,1.0) ),
                     random( i + vec2(1.0,1.0) ), u.x), u.y);
}

/******************************
******************************/
mat2 rotate2d(float angle){
    return mat2(cos(angle),-sin(angle),
                sin(angle),cos(angle));
}

/******************************
******************************/
void main() {
	/********************
	********************/
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

	/********************
	********************/
	st = rotate2d( 1.2 * u_mouse.y / u_resolution.y * noise(st) ) * st;
	
	/********************
	********************/
    vec2 grid = vec2(100.0, 20.);
    st *= grid;

    vec2 ipos = floor(st);  // integer
    vec2 fpos = fract(st);  // fraction

	/********************
	********************/
#ifdef TIME_BASED
	vec2 vel = vec2(u_time * 0.5/* speed */ * max(grid.x,grid.y)); // time
#else
	vec2 vel = vec2(u_mouse.x/u_resolution.x * 4. /* speed */ * max(grid.x,grid.y)); // mouse.x
#endif
    vel *= vec2(-1.,0.0) * random(1.0+ipos.y); // direction
	

	/********************
	********************/
	/*
    // Assign a random value base on the integer coord
	vec2 offset = vec2(0.1,0.);

	vec3 color = vec3(0.);
    color.r = pattern(st+offset,vel,0.5+u_mouse.x/u_resolution.x);
    color.g = pattern(st       ,vel,0.5+u_mouse.x/u_resolution.x);
    color.b = pattern(st-offset,vel,0.5+u_mouse.x/u_resolution.x);
	*/
	
	float ColorVal = 0.0;
#ifdef TIME_BASED
	ColorVal = pattern(st, vel, 0.4 + 0.6*(u_mouse.x/u_resolution.x) ); // mouse.x = 0の時、thresh = 0.4.  右端で1.0. の時.
#else
	ColorVal = pattern(st, vel, 0.5 ); // thresh固定 : mouse.xをAnimationに割り当て
#endif

	/********************
	Margins
	********************/
    // color *= step(0.2,fpos.y);
	ColorVal *= step(0.2, fpos.y);

	/********************
	********************/
	// gl_FragColor = vec4(1.0-color,1.0);
	ColorVal = 1.0 - ColorVal;
	gl_FragColor = vec4(ColorVal * BaseColor.r, ColorVal * BaseColor.g, ColorVal * BaseColor.b, ColorVal * BaseColor.a);
}

