// webgl.d.ts is from Rico Possienka https://github.com/RicoP/webgl.d.ts
// I modified it slightly as 'bool' is now 'boolean'
///<reference path="webgl.d.ts" />
///<reference path="tsm-master/TSM/tsm-0.7.d.ts" />
var bgm_music = "";
var key_map = (_a = {
        ArrowRight: 0,
        d: 0,
        l: 0,
        ArrowUp: 1,
        w: 1,
        i: 1,
        ArrowLeft: 2,
        a: 2,
        j: 2,
        ArrowDown: 3,
        s: 3,
        k: 3,
        Control: 4,
        Shift: 4
    },
    _a[" "] = 4,
    _a.r = 5,
    _a);
function process_key_event(down, code) {
    vegicide_process_input(down, code);
}
function vegicide_fullscreen() {
    var game_div = document.getElementById("game_div");
    game_div.requestFullscreen();
}
document.addEventListener('keydown', function (event) {
    var keyName = event.key;
    if (key_map[keyName]) {
        event.stopPropagation();
        event.preventDefault();
    }
    process_key_event(1, key_map[keyName]);
}, false);
document.addEventListener('keyup', function (event) {
    var keyName = event.key;
    if (key_map[keyName]) {
        event.stopPropagation();
        event.preventDefault();
    }
    process_key_event(0, key_map[keyName]);
}, false);
document.addEventListener('keypress', function (event) {
    var keyName = event.key;
    if (key_map[keyName]) {
        event.stopPropagation();
        event.preventDefault();
    }
}, false);
function animate_start() {
    vegicide_init = Module.cwrap("vegicide_init", "VgHandle", []);
    vegicide_step = Module.cwrap("vegicide_step", "", []);
    vegicide_get_graphics_json = Module.cwrap("vegicide_get_graphics_json", "string", []);
    vegicide_get_audio_json = Module.cwrap("vegicide_get_audio_json", "string", []);
    vg_handle = vegicide_init();
    vegicide_process_input = Module.cwrap("vegicide_process_input", "", ["number", "number"]);
}
function load_sound(name) {
    return new Howl({
        src: ["sounds/" + name + ".ogg", "sounds/" + name + ".webm", "sounds/" + name + ".mp3"]
    });
}
function sound_start() {
    sounds = new Map([
        ["ambient", load_sound("ambient")],
        ["attack", load_sound("attack")],
        ["bling", load_sound("bling")],
        ["crunch", load_sound("crunch")],
        ["gasp", load_sound("gasp")],
        ["king-laugh", load_sound("king-laugh")],
        ["king-pain", load_sound("king-pain")],
        ["king-roar", load_sound("king-roar")],
        ["king-you", load_sound("king-you")],
        ["respawn", load_sound("respawn")],
        ["swooce", load_sound("swooce")],
    ]);
    play_music("ambient");
}
function play_music(name) {
    if (name != bgm_music) {
        if (sounds.get(bgm_music)) {
            sounds.get(bgm_music).stop();
        }
        bgm_music = name;
        sounds.get(name).loop(true);
        sounds.get(name).play();
    }
}
function play_sound(name) {
    sounds.get(name).play();
}
function start() {
    var canvas = document.getElementById("glCanvas");
    frame = 0;
    initWebGl(canvas);
    if (gl) {
        gl.clearColor(1.0, 1.0, 1.0, 1.0);
        gl.disable(gl.DEPTH_TEST);
        mesh_square = init_square();
        mesh_level = init_map();
        initShaders();
        initTextures();
        //draw ();
        // ms between frames
        //window.setInterval (step, 1000.0 / 60.0);
        window.requestAnimationFrame(step);
    }
    scene_ecs = JSON.parse(sync_xhr("scene_ecs.json"));
    //ecs = JSON.parse (sync_xhr ("graphics_ecs.json"));
    animate_start();
    sound_start();
}
function step() {
    window.requestAnimationFrame(step);
    vegicide_step();
    frame += 1.0;
    var graphics_json = vegicide_get_graphics_json();
    var graphics_ecs = JSON.parse(graphics_json);
    draw(graphics_ecs);
    var audio_json = vegicide_get_audio_json();
    //console.log (audio_json);
    var audio_ecs = JSON.parse(audio_json);
    process_audio(audio_ecs);
}
function process_audio(ecs) {
    var audio_names = [
        "bling",
        "crunch",
        "gasp",
        "king-laugh",
        "king-pain",
        "king-roar",
        "king-you",
        "respawn",
        "swooce",
    ];
    ecs.sounds.forEach(function (sound_code) {
        play_sound(audio_names[sound_code]);
    });
    var music_names = [
        "ambient",
        "ambient",
        "attack",
    ];
    play_music(music_names[ecs.bgm]);
}
function set_shader(shader) {
    current_shader = shader;
    gl.useProgram(shader.program);
}
function draw_pass(ecs, pass, num_tris) {
    var proj_view_mat = new TSM.mat4(pass["proj_view_mat"]);
    for (var e in pass["renderables"]) {
        var model_mat = new TSM.mat4(ecs["rigid_mats"][e]);
        var mvpMatrix = proj_view_mat.copy().multiply(model_mat);
        gl.uniformMatrix4fv(current_shader.mvpUniform, false, new Float32Array(mvpMatrix.all()));
        var color = ecs["diffuse_colors"][e];
        gl.uniform4fv(current_shader.colorUniform, color);
        gl.bindTexture(gl.TEXTURE_2D, textures[ecs["textures"][e]]);
        gl.drawElements(gl.TRIANGLES, 3 * num_tris, gl.UNSIGNED_SHORT, 0);
    }
}
function set_vertex_attribs() {
    gl.vertexAttribPointer(current_shader.posAttr, 3, gl.FLOAT, false, 5 * 4, 0);
    gl.vertexAttribPointer(current_shader.texCoordAttr, 2, gl.FLOAT, false, 5 * 4, 3 * 4);
}
function draw(ecs) {
    //window.requestAnimationFrame (draw);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    if (ecs && ecs["passes"]) {
        set_shader(defaultShader);
        var opaque_pass = ecs["passes"][0];
        gl.disable(gl.BLEND);
        gl.bindBuffer(gl.ARRAY_BUFFER, mesh_level.vert_buffer);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh_level.index_buffer);
        set_vertex_attribs();
        draw_pass(ecs, opaque_pass, 2 * 64 * 32);
        // Everything uses the square mesh
        gl.bindBuffer(gl.ARRAY_BUFFER, mesh_square.vertices);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh_square.indices);
        set_vertex_attribs();
        var shadow_pass = ecs["passes"][1];
        set_shader(shadow_shader);
        // Idk why but WebGL doesn't like SRC_ALPHA? shrug
        gl.blendFunc(gl.DST_COLOR, gl.ZERO);
        gl.enable(gl.BLEND);
        draw_pass(ecs, shadow_pass, 2);
        set_shader(defaultShader);
        var transparent_pass = ecs["passes"][2];
        gl.blendFunc(gl.ONE, gl.ONE_MINUS_SRC_ALPHA);
        draw_pass(ecs, transparent_pass, 2);
    }
}
function initWebGl(canvas) {
    gl = null;
    try {
        gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
    }
    catch (e) {
    }
    if (!gl) {
        window.alert("Can't initialize WebGL");
    }
}
function init_square() {
    var vs = [
        -1.0, -1.0, 0.0, 0.0, 0.0,
        -1.0, 1.0, 0.0, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0, 1.0,
        1.0, -1.0, 0.0, 1.0, 0.0
    ];
    var vertices = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertices);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vs), gl.STATIC_DRAW);
    var is = [
        0, 1, 2,
        0, 2, 3
    ];
    var indices = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indices);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(is), gl.STATIC_DRAW);
    return {
        vertices: vertices,
        indices: indices
    };
}
// Yeah, I know.
function sync_xhr(name) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", name, false);
    xhr.send();
    return xhr.responseText;
}
function init_map() {
    var level = JSON.parse(sync_xhr("maps/demo.json"));
    var verts = [];
    // The tilemap for Vegicide is 8x8
    var tex_size = 1.0 / 8.0;
    for (var ty = 0; ty < level.height; ty++) {
        for (var tx = 0; tx < level.width; tx++) {
            var tex_i = level.data[tx + ty * level.width];
            var tex_x = (tex_i % 8) * tex_size;
            var tex_y = 1.0 - Math.floor(tex_i / 8) * tex_size;
            verts.push([tx, ty, 0.0, tex_x, tex_y]);
            verts.push([tx + 1, ty, 0.0, tex_x + tex_size, tex_y]);
            verts.push([tx + 1, ty + 1, 0.0, tex_x + tex_size, tex_y - tex_size]);
            verts.push([tx, ty + 1, 0.0, tex_x, tex_y - tex_size]);
        }
    }
    var vs = [];
    verts.forEach(function (vert) {
        for (var i = 0; i < 5; i++) {
            vs.push(vert[i]);
        }
    });
    var is = [];
    for (var i = 0; i < level.width * level.height; i++) {
        is.push(i * 4 + 0);
        is.push(i * 4 + 1);
        is.push(i * 4 + 2);
        is.push(i * 4 + 0);
        is.push(i * 4 + 2);
        is.push(i * 4 + 3);
    }
    var vert_buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vert_buffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vs), gl.STATIC_DRAW);
    var index_buffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, index_buffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(is), gl.STATIC_DRAW);
    return {
        vert_buffer: vert_buffer,
        index_buffer: index_buffer
    };
}
function getShader(gl, name, type) {
    var shaderSource, shader;
    shaderSource = sync_xhr(name);
    shader = gl.createShader(type);
    gl.shaderSource(shader, shaderSource);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        window.alert("Couldn't compile a shader: " + gl.getShaderInfoLog(shader));
        return null;
    }
    return shader;
}
function getShaderProgram(vert, frag) {
    var program = gl.createProgram();
    gl.attachShader(program, frag);
    gl.attachShader(program, vert);
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        //alert ("Couldn't compile shader program");
    }
    gl.useProgram(program);
    var posAttr = gl.getAttribLocation(program, "attribPos");
    gl.enableVertexAttribArray(posAttr);
    var texCoordAttr = gl.getAttribLocation(program, "attribTexCoord");
    gl.enableVertexAttribArray(texCoordAttr);
    var mvpUniform = gl.getUniformLocation(program, "mvpMatrix");
    var colorUniform = gl.getUniformLocation(program, "color");
    return {
        program: program,
        posAttr: posAttr,
        texCoordAttr: texCoordAttr,
        mvpUniform: mvpUniform,
        colorUniform: colorUniform
    };
}
function initShaders() {
    var defaultVert = getShader(gl, "shaders/shader.vert", gl.VERTEX_SHADER);
    var defaultFrag = getShader(gl, "shaders/shader.frag", gl.FRAGMENT_SHADER);
    defaultShader = getShaderProgram(defaultVert, defaultFrag);
    var shadowFrag = getShader(gl, "shaders/shadow.frag", gl.FRAGMENT_SHADER);
    shadow_shader = getShaderProgram(defaultVert, shadowFrag);
}
function initTextures() {
    var texture_files = [
        "beet",
        "beet-dead",
        "blood",
        "carrot",
        "carrot-dead",
        "crab-apple",
        "crab-apple-dead",
        "egg",
        "white",
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
    var _loop_1 = function (i) {
        textures[i] = gl.createTexture();
        var textureImage = new Image();
        textureImage.onload = function () {
            gl.bindTexture(gl.TEXTURE_2D, textures[i]);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, textureImage);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
            gl.generateMipmap(gl.TEXTURE_2D);
            //draw ();
        };
        textureImage.src = "textures/" + texture_files[i] + ".png";
    };
    for (var i = 0; i < texture_files.length; i++) {
        _loop_1(i);
    }
}
var _a;
