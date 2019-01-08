-----------------------------------------------------------------------
--                                                                   --
-- Copyright (C) 2009,  AVIQ Bulgaria Ltd.                           --
--                                                                   --
-- Project:       Simple TrueType demo                               --
-- Filename:      simple.lua                                         --
--                                                                   --
-----------------------------------------------------------------------

require "luagl"
require "luaftgl"

NAME = 'Simple TrueType demo'

local fpstext
local fontfile = arg[1] or "arial.ttf"
local fonts
local infofont
local fontindex = 3
local h_win

function Init()
    gl.MatrixMode("PROJECTION")
    gl.LoadIdentity()
    glu.Perspective(90, 640.0 / 480.0, 1, 1000)
    gl.MatrixMode("MODELVIEW")
    gl.LoadIdentity()
    glu.LookAt(0.0, 0.0, 640.0 / 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0)

	fonts =
	{
		ftgl.new_font("BITMAP", fontfile),
		ftgl.new_font("PIXMAP", fontfile),
		ftgl.new_font("OUTLINE", fontfile),
		ftgl.new_font("POLYGON", fontfile),
		ftgl.new_font("EXTRUDE", fontfile),
		ftgl.new_font("TEXTURE", fontfile),
		ftgl.new_font("BUFFER", fontfile),
	}

    fonts[5]:Depth(10)
    fonts[5]:Outset(0, 3)
	for _, font in ipairs(fonts) do
		font:FaceSize(80)
		font:CharMap("ENCODING_UNICODE")
	end

	infofont = ftgl.new_font("PIXMAP", fontfile)
    infofont:FaceSize(18)

	w_win = glut.Get("WINDOW_WIDTH")
	h_win = glut.Get("WINDOW_HEIGHT")
end

function Done()
end

function Reshape(width, height)
	gl.Viewport(0, 0, width, height)
	gl.MatrixMode('PROJECTION')
	gl.LoadIdentity()
	gl.MatrixMode('MODELVIEW')
	gl.LoadIdentity()
end

function DrawFrame()
    local now = glut.Get("ELAPSED_TIME")

    local n = now / 20.
    local t1 = math.sin(n / 80)
    local t2 = math.sin(n / 50 + 1)
    local t3 = math.sin(n / 30 + 2)

    local ambient  = { (t1 + 2.0) / 3,
                       (t2 + 2.0) / 3,
                       (t3 + 2.0) / 3, 0.3 }
    local diffuse  = { 1.0, 0.9, 0.9, 1.0 }
    local specular = { 1.0, 0.7, 0.7, 1.0 }
    local position = { 100.0, 100.0, 0.0, 1.0 }

    local front_ambient = { 0.7, 0.7, 0.7, 0.0 }

    gl.Clear("COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT")

    gl.Enable("LIGHTING")
    gl.Enable("DEPTH_TEST")

    gl.PushMatrix()
        gl.Translate(-0.9, -0.2, -10.0)
        gl.Light("LIGHT1", "AMBIENT",  ambient)
        gl.Light("LIGHT1", "DIFFUSE",  diffuse)
        gl.Light("LIGHT1", "SPECULAR", specular)
        gl.Light("LIGHT1", "POSITION", position)
        gl.Enable("LIGHT1")
    gl.PopMatrix()

    gl.PushMatrix()
        gl.Material("FRONT", "AMBIENT", front_ambient)
        gl.ColorMaterial("FRONT", "DIFFUSE")
        gl.Translate(0.0, 10.0, 20.0)
        gl.Rotate(n / 1.11, 0.0, 1.0, 0.0)
        gl.Rotate(n / 2.23, 1.0, 0.0, 0.0)
        gl.Rotate(n / 3.17, 0.0, 0.0, 1.0)
        gl.Translate(-260.0, -0.2, 0.0)
        gl.Color(1.0, 1.0, 1.0)
		fonts[fontindex]:Render( "Lua OpenGL.")

		gl.PushMatrix()
		gl.Color(1, 0, 0)
		gl.Translate(480, 0, 0)

		local quad = glu.NewQuadric()
		quad:QuadricDrawStyle("FILL")
		quad:Sphere(5, 50, 50)
		gl.PopMatrix()
    gl.PopMatrix()

    gl.MatrixMode("PROJECTION")
    gl.LoadIdentity()
    glu.Ortho2D(0, w_win, 0, h_win);
    gl.MatrixMode("MODELVIEW")
    gl.LoadIdentity()
    gl.Disable("LIGHTING")
    gl.Color(1.0, 1.0, 1.0)
    gl.RasterPos(20.0 , h_win - (20.0 + infofont:Ascender()))
	infofont:Render("Font #"..fontindex)
	if fpstext then
		gl.RasterPos(20.0 , h_win - (40.0 + infofont:Ascender()))
		infofont:Render(fpstext)
	end
    gl.Enable("LIGHTING")

    gl.MatrixMode("PROJECTION")
    gl.LoadIdentity()
    glu.Perspective(90, 640.0 / 480.0, 1, 1000)
    gl.MatrixMode("MODELVIEW")
    gl.LoadIdentity()
    glu.LookAt(0.0, 0.0, 640.0 / 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0)


	glut.SwapBuffers()
	gl.Flush()

    frames = (frames or 0) + 1

	lastfps = lastfps or 0
    if now - lastfps > 5000 then
		fpstext = string.format("%i frames in 5.0 seconds = %g FPS",
			  frames, frames * 1000. / (now - lastfps))
		lastfps = lastfps + 5000
		frames = 0
    end



end

function Keyboard(key)
	if key == 27 then
		Done()
		os.exit()
	elseif key == 13 then
		fontindex = 1 + math.mod(fontindex, #fonts)
	end
end

glut.Init()
glut.InitDisplayMode("DEPTH,DOUBLE,RGBA")
glut.InitWindowPosition(100, 100)
glut.InitWindowSize(640, 480)
glut.CreateWindow(NAME)
glut.DisplayFunc('DrawFrame')
--glut.ReshapeFunc('Reshape')
glut.KeyboardFunc('Keyboard')
glut.IdleFunc('DrawFrame')
Init()
glut.MainLoop()
Done()
