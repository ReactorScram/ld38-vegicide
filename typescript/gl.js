// webgl.d.ts is from Rico Possienka https://github.com/RicoP/webgl.d.ts
// I modified it slightly as 'bool' is now 'boolean'
///<reference path="webgl.d.ts" />
///<reference path="tsm-master/TSM/tsm-0.7.d.ts" />
function start() {
    var canvas = document.getElementById("glCanvas");
    frame = 0;
    initWebGl(canvas);
    if (gl) {
        gl.clearColor(0.0, 0.0, 0.0, 1.0);
        gl.enable(gl.DEPTH_TEST);
        gl.depthFunc(gl.LEQUAL);
        initBuffers();
        initShaders();
        initTextures();
        draw();
        // ms between frames
        window.setInterval(step, 1000.0 / 60.0);
    }
}
function step() {
    frame += 1.0;
    if (frame > 360.0) {
        frame -= 360.0;
    }
    draw();
}
function draw() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 5 * 4, 0);
    gl.vertexAttribPointer(texCoordAttribute, 2, gl.FLOAT, false, 5 * 4, 3 * 4);
    var pMatrix = TSM.mat4.perspective(3.1415926535 / 4.0, 800.0 / 480.0, 1, 1000.0);
    var mvMatrix = TSM.mat4.identity.copy();
    var object_pos = new TSM.vec3([0.0, 0.0, -80.0]);
    //mat4.rotateY (mvMatrix, mvMatrix, frame * 3.1415926535 / 180.0);
    var mvMatrix = TSM.mat4.identity.copy().translate(object_pos).rotate(frame * 3.1415926535 / 180.0, new TSM.vec3([0.0, 1.0, 0.0]));
    var mvpMatrix = pMatrix.multiply(mvMatrix);
    //var mvpMatrix = mvMatrix.multiply (pMatrix);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, new Float32Array(mvpMatrix.all()));
    gl.bindBuffer(gl.ARRAY_BUFFER, verticesBuffer);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    gl.bindTexture(gl.TEXTURE_2D, texture);
    //gl.drawArrays (gl.TRIANGLE_STRIP, 0, 4);
    gl.drawElements(gl.TRIANGLES, 6, gl.UNSIGNED_SHORT, 0);
}
function initWebGl(canvas) {
    gl = null;
    try {
        gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
    }
    catch (e) {
    }
    if (!gl) {
        //alert ("Can't initialize WebGL");
    }
}
function initBuffers() {
    verticesBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, verticesBuffer);
    var vertices = [
        -1.0, -1.0, 0.0, 0.0, 1.0,
        -1.0, 1.0, 0.0, 0.0, 0.0,
        1.0, 1.0, 0.0, 1.0, 0.0,
        1.0, -1.0, 0.0, 1.0, 1.0
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    indexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    var indices = [
        0, 1, 2,
        0, 2, 3
    ];
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);
}
function initShaders() {
    var fragmentShader = getShader(gl, "shaders/test.fs", gl.FRAGMENT_SHADER);
    var vertexShader = getShader(gl, "shaders/test.vs", gl.VERTEX_SHADER);
    if (fragmentShader && vertexShader) {
        shaderProgram = gl.createProgram();
        gl.attachShader(shaderProgram, fragmentShader);
        gl.attachShader(shaderProgram, vertexShader);
        gl.linkProgram(shaderProgram);
        if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
            //alert ("Couldn't compile shader program");
        }
        gl.useProgram(shaderProgram);
        vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "attribVertexPosition");
        gl.enableVertexAttribArray(vertexPositionAttribute);
        texCoordAttribute = gl.getAttribLocation(shaderProgram, "attribTexCoord");
        gl.enableVertexAttribArray(texCoordAttribute);
        mvpMatrixUniform = gl.getUniformLocation(shaderProgram, "mvpMatrix");
    }
    else {
        //alert ("Couldn't load shaders");
    }
}
function initTextures() {
    texture = gl.createTexture();
    textureImage = new Image();
    textureImage.onload = function () {
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, textureImage);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
        gl.generateMipmap(gl.TEXTURE_2D);
        draw();
    };
    textureImage.src = "textures/shibe.png";
}
function getShader(gl, name, type) {
    var shaderSource, shader;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", name, false);
    xhr.send();
    shaderSource = xhr.responseText;
    shader = gl.createShader(type);
    gl.shaderSource(shader, shaderSource);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        //alert ("Couldn't compile a shader: " + gl.getShaderInfoLog (shader));
        return null;
    }
    return shader;
}
