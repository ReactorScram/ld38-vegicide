// webgl.d.ts is from Rico Possienka https://github.com/RicoP/webgl.d.ts
// I modified it slightly as 'bool' is now 'boolean'
///<reference path="webgl.d.ts" />

// TSM is from Matthias Ferch https://github.com/matthiasferch/tsm
///<reference path="tsm-master/TSM/tsm-0.7.d.ts" />

declare var document: any;
declare var frame: number;
declare var gl: WebGLRenderingContext;
declare var window: any;

declare var textures: WebGLTexture [];
declare var defaultShader: any;

declare var ecs: any;
ecs = false;

declare var mesh_square: any;
declare var mesh_level: any;

declare class XMLHttpRequest {
	onload: any;
	readyState: number;
	responseText: string;
	
	open (verb: string, url: string, async: boolean): void;
	send (): void;
}

function start () {
	var canvas = <HTMLCanvasElement> document.getElementById ("glCanvas");
	
	frame = 0;
	
	initWebGl (canvas);
	
	if (gl) {
		gl.clearColor (1.0, 1.0, 1.0, 1.0);
		gl.disable (gl.DEPTH_TEST);
		gl.depthFunc (gl.LEQUAL);
		
		mesh_square = init_square ();
		initShaders ();
		initTextures ();
		
		draw ();
		
		// ms between frames
		window.setInterval (step, 1000.0 / 1.0);
	}
	
	var xhr = new XMLHttpRequest ();
	
	xhr.open ("GET", "graphics_ecs.json", false);
	xhr.send ();
	
	ecs = JSON.parse (xhr.responseText);
}

function step () {
	frame += 1.0;
	if (frame > 360.0) {
		frame -= 360.0;
	}
	
	draw ();
}

function draw_pass (ecs, pass) {
	for (var e in pass ["renderables"]) {
		var proj_view_mat = new TSM.mat4 (pass ["proj_view_mat"]);
		
		var model_mat = new TSM.mat4 (ecs ["rigid_mats"][e]);
		
		var mvpMatrix = proj_view_mat.multiply (model_mat);
		
		gl.uniformMatrix4fv (defaultShader.mvpUniform, false, new Float32Array (mvpMatrix.all ()));
		
		var color = ecs ["diffuse_colors"][e];
		gl.uniform4fv (defaultShader.colorUniform, color);
		
		gl.bindTexture (gl.TEXTURE_2D, textures [ecs ["textures"][e]]);
		
		gl.drawElements (gl.TRIANGLES, 6, gl.UNSIGNED_SHORT, 0 );
	}
}

function draw () {
	gl.clear (gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.enable (gl.BLEND);
	
	// Everything uses the square mesh
	gl.bindBuffer (gl.ARRAY_BUFFER, mesh_square.vertices);
	gl.bindBuffer (gl.ELEMENT_ARRAY_BUFFER, mesh_square.indices);
	
	if (ecs) {
	gl.blendFunc (gl.DST_COLOR, gl.ZERO);
	
	gl.useProgram (defaultShader.program);
	gl.vertexAttribPointer (defaultShader.posAttr, 3, gl.FLOAT, false, 5 * 4, 0);
	gl.vertexAttribPointer (defaultShader.texCoordAttr, 2, gl.FLOAT, false, 5 * 4, 3 * 4);
	
	var shadow_pass = ecs ["passes"][1];
	// Idk why but WebGL doesn't like SRC_ALPHA? shrug
	gl.blendFunc (gl.ONE, gl.ONE_MINUS_SRC_ALPHA);
	draw_pass (ecs, shadow_pass);
	
	var transparent_pass = ecs ["passes"][2];
	gl.blendFunc (gl.ONE, gl.ONE_MINUS_SRC_ALPHA);
	draw_pass (ecs, transparent_pass);
	}
}

function initWebGl (canvas) {
	gl = null;
	
	try {
		gl = canvas.getContext ("webgl") || canvas.getContext ("experimental-webgl");
	}
	catch (e) {
		
	}
	
	if (! gl) {
		//alert ("Can't initialize WebGL");
	}
}

function init_square () {
	var vertices = gl.createBuffer ();
	gl.bindBuffer (gl.ARRAY_BUFFER, vertices);
	
	var vs = [
		-1.0, -1.0, 0.0, 0.0, 0.0,
		-1.0, 1.0,  0.0, 0.0, 1.0,
		1.0,  1.0,  0.0, 1.0, 1.0,
		1.0,  -1.0, 0.0, 1.0, 0.0
	];
	
	gl.bufferData (gl.ARRAY_BUFFER, new Float32Array (vs), gl.STATIC_DRAW);
	
	var indices = gl.createBuffer ();
	gl.bindBuffer (gl.ELEMENT_ARRAY_BUFFER, indices);
	
	var is = [
		0, 1, 2, 
		0, 2, 3
	];
	
	gl.bufferData (gl.ELEMENT_ARRAY_BUFFER, new Uint16Array (is), gl.STATIC_DRAW);
	
	return {
		vertices,
		indices
	};
}

function getShader (gl, name, type) {
	var shaderSource,  shader;
	
	var xhr = new XMLHttpRequest ();
	
	xhr.open ("GET", name, false);
	xhr.send ();
	
	shaderSource = xhr.responseText;
	
	shader = gl.createShader (type);
	
	gl.shaderSource (shader, shaderSource);
	
	gl.compileShader (shader);
	
	if (! gl.getShaderParameter (shader, gl.COMPILE_STATUS)) {
		alert ("Couldn't compile a shader: " + gl.getShaderInfoLog (shader));
		
		return null;
	}
	
	return shader;
}

function getShaderProgram (vert, frag) {
	var program = gl.createProgram ();
	
	gl.attachShader (program, frag);
	gl.attachShader (program, vert);
	gl.linkProgram (program);
	
	if (! gl.getProgramParameter (program, gl.LINK_STATUS)) {
		//alert ("Couldn't compile shader program");
	}
	
	gl.useProgram (program);
	
	var posAttr = gl.getAttribLocation (program, "attribPos");
	gl.enableVertexAttribArray (posAttr);
	
	var texCoordAttr = gl.getAttribLocation (program, "attribTexCoord");
	gl.enableVertexAttribArray (texCoordAttr);
	
	var mvpUniform = gl.getUniformLocation (program, "mvpMatrix");
	
	var colorUniform = gl.getUniformLocation (program, "color");
	
	return {
		program,
		posAttr,
		texCoordAttr,
		mvpUniform,
		colorUniform
	};
}

function initShaders () {
	var defaultVert = getShader (gl, "shaders/shader.vert", gl.VERTEX_SHADER);
	var defaultFrag = getShader (gl, "shaders/shader.frag", gl.FRAGMENT_SHADER);
	
	defaultShader = getShaderProgram (defaultVert, defaultFrag);
}

function initTextures () {
	var texture_files: string [] = [
		"beet",
		"beet-dead",
		"blood",
		"carrot",
		"carrot-dead",
		"crab-apple",
		"crab-apple-dead",
		"egg",
		"white", // Farm
		"pumpking",
		"pumpking-dead",
		"shadow",
		"tiles",
		"title",
		"venus",
		"venus-dead",
		"victory",
		"vignette",
		"white"
	];
	
	textures = [];
	
	for (let i = 0; i < texture_files.length; i++) {
		textures [i] = gl.createTexture ();
		let textureImage = new Image ();
		
		textureImage.onload = function () {
			gl.bindTexture (gl.TEXTURE_2D, textures [i]);
			gl.texImage2D (gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, textureImage);
			gl.texParameteri (gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
			gl.texParameteri (gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
			gl.generateMipmap (gl.TEXTURE_2D);
			
			draw ();
		}
		
		textureImage.src = "textures/" + texture_files [i] + ".png";
	}
}
