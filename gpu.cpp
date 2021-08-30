#include "gpu.h"
#include "Bus.h"
#include <cmath>
Bus* bus;

gpu::gpu() {
	// Initialize pixel array
	pixels = new uint32_t [480 * 640];
	debug = false;
}

void connectBus(Bus *_bus) {
	bus = _bus;
}


void gpu::debug_printf(const char* fmt, ...) {
	if (debug) {
		std::va_list args;
		va_start(args, fmt);
		std::vprintf(fmt, args);
		va_end(args);
	}
}

uint32_t gpu::get_status() {
	uint32_t status = 0;

	status |= page_base_x << 0;
	status |= page_base_y << 3;

	return status;
}

static const GLchar* VertexShaderSource =
R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 colour;
out vec3 frag_colour;

void main() {
	gl_Position = vec4(float(aPos.x) / 320 - 1, -(1 - float(aPos.y) / 240), 0.0, 1.0);
	frag_colour = vec3(float(colour.r) / 255, float(colour.g) / 255, float(colour.b) / 255);
}
)";
static const GLchar* FragmentShaderSource =
R"(
#version 330 core
in vec3 frag_colour;
out vec3 final_colour;

void main() {
	final_colour = frag_colour;
} 
)";

void gpu::InitGL() {
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
	glCompileShader(VertexShader);
	int success;
	char InfoLog[512];
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(VertexShader, 512, NULL, InfoLog);
		std::cout << "Vertex shader compilation failed\n" << InfoLog << std::endl;
	}
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, NULL);
	glCompileShader(FragmentShader);
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(FragmentShader, 512, NULL, InfoLog);
		std::cout << "Fragment shader compilation failed\n" << InfoLog << std::endl;
	}
	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(FragmentShader, 512, NULL, InfoLog);
		std::cout << "Linking shader program failed\n" << InfoLog << std::endl;
	}
	glUseProgram(ShaderProgram);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//
	//float Vertices[]{
	//	100, 200, 0.0,
	//	300, 200, 0.0,
	//	320, 400, 0.0
	//};
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	//glUseProgram(ShaderProgram);
	//glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glBindVertexArray(0);
	//
	//glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
}

gpu::point calc_tex_coords(int tx, int ty, int x, int y, int bpp) {
	gpu::point p;
	double r = 16 / bpp;
	double xc = (tx * r + x) / (1024.0 * r);
	double yc = (ty + y) / 512.0;
	p.x = xc;
	p.y = yc;
	return p;
}

void gpu::putpixel(point v1, uint32_t colour) {
	// TODO: OpenGL implementation
}


// Textures
uint16_t gpu::vram_read(int x, int y) {
	return vram[y * 1024 + x];
}

void gpu::execute_gp0(uint32_t command) {
	uint8_t instr = (command >> 24) & 0xff;
	if (cmd_left == 0) {
		cmd_length = 0;
		switch (instr) {
		case(0):	// nop
			debug_printf("[GP0] NOP (0x%x)\n", command);
			break;
		case(0x01): {	// Clear Cache
			debug_printf("[GP0] Clear Cache\n");
			break;
		}
		case(0x02): {	// Fill rectangle in VRAM
			fifo[0] = command;
			cmd_length++;
			cmd_left = 2;
			break;
		}
		case(0x20): { // Monochrome three - point polygon, opaque
			fifo[0] = command;
			cmd_length++;
			cmd_left = 3;
			break;
		}
		case(0x22): { // Monochrome three-point polygon, semi-transparent
			fifo[0] = command;
			cmd_length++;
			cmd_left = 3;
			break;
		}
		case(0x28): { // Monochrome four-point polygon, opaque
			fifo[0] = command;
			cmd_length++;
			cmd_left = 4;
			break;
		}
		case(0x2A): { // Monochrome four-point polygon, semi-transparent
			fifo[0] = command;
			cmd_length++;
			cmd_left = 4;
			break;
		}
		case(0x2C): { // Textured four-point polygon, opaque, texture-blending
			fifo[0] = command;
			cmd_length++;
			cmd_left = 8;
			break;
		}
		case(0x30): {	// Shaded three-point polygon, opaque
			fifo[0] = command;
			cmd_length++;
			cmd_left = 5;
			break;
		}
		case(0x32): { // Shaded three-point polygon, semi-transparent
			fifo[0] = command;
			cmd_length++;
			cmd_left = 5;
			break;
		}
		case(0x38): { // Shaded four-point polygon, opaque
			fifo[0] = command;
			cmd_length++;
			cmd_left = 7;
			break;
		}
		case(0x40): { // Monochrome line, opaque
			fifo[0] = command;
			cmd_length++;
			cmd_left = 2;
			break;
		}
		case(0x3A): { // Shaded four-point polygon, semi-transparent
			fifo[0] = command;
			cmd_length++;
			cmd_left = 7;
			break;
		}
		case(0x68): {	// 1x1 Opaque Monochrome Rectangle
			fifo[0] = command;
			cmd_length++;
			cmd_left = 1;
			break;
		}
		case(0xA0): {	// Copy rectangle CPU to VRAM
			fifo[0] = command;
			cmd_length++;
			cmd_left = 2;
			break;
		}
		case(0xC0): {	// Copy rectangle VRAM to CPU
			// unimplemented
			debug_printf("[GP0] Copy Rectangle (VRAM to CPU)\n");
			break;
		}
		case(0xE1): {	// Draw Mode Setting
			debug_printf("[GP0] Draw Mode Setting\n");
			page_base_x = command & 0xf;
			page_base_y = (command >> 4) & 1;
			semi_transparency = (command >> 5) & 3;
			texture_depth = (command >> 7) & 3;
			dithering = ((command >> 9) & 1) != 0;
			allow_display_drawing = ((command >> 10) & 1) != 0;
			// rectangle x flip
			// rectangle y flip
			break;
		}
		case(0xE2): {	// Set Texture Window
			// unimplemented
			debug_printf("[GP0] Set Texture Window\n");
			break;
		}
		case(0xE3): {	// Set Drawing Area top left
			// unimplemented
			debug_printf("[GP0] Set Drawing Area top left\n");
			break;
		}
		case(0xE4): {	// Set Drawing Area bottom right
			// unimplemented
			debug_printf("[GP0] Set Drawing Area bottom right\n");
			break;
		}
		case(0xE5): {	// Set Drawing Area Offset
			// unimplemented
			debug_printf("[GP0] Set Drawing Area Offset\n");
			break;
		}
		case(0xE6): {	// Set Mask Bit Setting
			debug_printf("[GP0] Set Mask Bit Setting\n");
			mask_bit = (command & 1) != 0;
			disallow_masked_pixels_drawing = (command & 2) != 0;
			break;
		}
		default:
			printf("\n[GP0] Unknown GP0 command: 0x%x (0x%x)\n", instr, command);
			//exit(0);
		}
	}
	else {
		cmd_left--;
		switch (gp0_mode) {
		case 0: {	// command mode
			fifo[cmd_length++] = command;

			if (cmd_left == 0) {	// all the parameters are in, run command

				switch ((fifo[0] >> 24) & 0xff) {

				case(0x02): gpu::fill_rectangle(); break;
				case(0x20): gpu::monochrome_three_point_opaque_polygon(); break;
				case(0x22): gpu::monochrome_three_point_semi_transparent_polygon(); break;
				case(0x28): gpu::monochrome_four_point_opaque_polygon(); break;
				case(0x2A): gpu::monochrome_four_point_semi_transparent_polygon(); break;
				case(0x2C): gpu::texture_blending_four_point_opaque_polygon(); break;
				case(0x30): gpu::shaded_three_point_opaque_polygon(); break;
				case(0x32): gpu::shaded_three_point_semi_transparent_polygon(); break;
				case(0x38): gpu::shaded_four_point_opaque_polygon(); break;
				case(0x3A): gpu::shaded_four_point_semi_transparent_polygon(); break;
				case(0x40): gpu::monochrome_line_opaque();
				case(0x68): gpu::monochrome_rectangle_dot_opaque(); break;
				case(0xA0): gpu::cpu_to_vram(); break;
				
				}
			}
			break;
		}
		case 1: {	// load mode
			debug_printf("[CPU to VRAM transfer] Data: 0x%x\n", command);
			uint32_t resolution = fifo[2];
			uint32_t coords = fifo[1];
			auto width = resolution & 0xffff;
			auto height = resolution >> 16;
			if (width == 0) width = 1024;
			if (height == 0) height = 512;
			auto x = coords & 0xffff;
			auto y = coords >> 16;
			
			xpos %= 1024;
			ypos %= 512;
			
			uint32_t c = command & 0xffff;
			uint32_t a = (c >> 15) & 1;
			uint32_t b = (c >> 10) & 0b11111;
			uint32_t g = (c >> 5) & 0b11111;
			uint32_t r = c & 0b11111;
			uint32_t rgba = ((r << 3) << 24) | ((g << 3) << 16) | ((b << 3) << 8) | 0xff;
			vram[(y + ypos) * 1024 + (x+xpos)] = command & 0xffff;
			//vram_rgb[(y + ypos) * 1024 + (x + xpos)] = rgba;
			//writev(x + xpos, y + ypos, command & 0xffff);
			xpos++;

			if (xpos == width) {
				xpos = 0;
				ypos++;

				if (ypos == height) {
					gp0_mode = 0;
					//upload_to_gpu();
					break;
				}
			}

			c = command >> 16;
			a = (c >> 15) & 1;
			b = (c >> 10) & 0b11111;
			g = (c >> 5) & 0b11111;
			r = c & 0b11111;
			rgba = ((r << 3) << 24) | ((g << 3) << 16) | ((b << 3) << 8) | 0xff;
			vram[(y+ypos) * 1024 + (x + xpos)] = command >> 16;
			//vram_rgb[(y + ypos) * 1024 + (x + xpos)] = rgba;
			//writev(x + xpos, y + ypos, command >> 16);
			xpos++;

			if (xpos == width) {
				xpos = 0;
				ypos++;

				if (ypos == height) {
					gp0_mode = 0;
					//upload_to_gpu();
					break;
				}
			}

		}
		}
	}
}

void gpu::execute_gp1(uint32_t command) {
	uint8_t instr = (command >> 24) & 0xff;

	switch (instr) {
	case(0x0):	// reset gpu
		debug_printf("[GP1] Reset Gpu\n");
		break;
	case(0x1): // reset command buffer
		debug_printf("[GP1] Reset Command Buffer\n");
		break;
	case(0x4): { // set dma direction
		debug_printf("[GP1] Set DMA Direction to %d\n", command & 0b11);
		dma_direction = command & 0b11;
		break;
	}
	default:
		debug_printf("[GP1] Unknown GP1 command: 0x%x\n", instr);
		//exit(0);
	}
}

// commands
void gpu::monochrome_four_point_opaque_polygon() {
	uint32_t colour = fifo[0] & 0xffffff;
	debug_printf("[GP0] Monochrome four-point polygon, opaque (colour: 0x%x)\n", colour);
	point v1, v2, v3, v4;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[2] & 0xffff;
	v2.y = fifo[2] >> 16;
	v3.x = fifo[3] & 0xffff;
	v3.y = fifo[3] >> 16;
	v4.x = fifo[4] & 0xffff;
	v4.y = fifo[4] >> 16;

	uint32_t Vertices1[]{
		v1.x, v1.y, 0,
		v2.x, v2.y, 0,
		v3.x, v3.y, 0,
	
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff)
	};
	glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices1), Vertices1, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)(9 * sizeof(uint32_t)));
	glEnableVertexAttribArray(1);
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	
	uint32_t Vertices2[]{
		v2.x, v2.y, 0.0,
		v3.x, v3.y, 0.0,
		v4.x, v4.y, 0.0,
	
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff)
	};
	glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2), Vertices2, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)(9 * sizeof(uint32_t)));
	glEnableVertexAttribArray(1);
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	return;
}

void gpu::texture_blending_four_point_opaque_polygon() {
	uint32_t colour = fifo[0] & 0xffffff;
	debug_printf("[GP0] Textured four-point polygon, opaque, texture blending (colour: 0x%x)\n", colour);
	point v1, v2, v3, v4, t1, t2, t3, t4;
	point page, clut;
	page.x = page_base_x;
	page.y = page_base_y;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	clut.x = fifo[2] & 0b111111;
	clut.y = (fifo[2] & 0b111111111) >> 5;
	page.x = fifo[5] & 0b111111;
	page.y = (fifo[5] & 0b111111111) >> 5;
	v2.x = fifo[3] & 0xffff;
	v2.y = fifo[3] >> 16;
	v3.x = fifo[5] & 0xffff;
	v3.y = fifo[5] >> 16;
	v4.x = fifo[7] & 0xffff;
	v4.y = fifo[7] >> 16;

	t1.x = fifo[2] & 0b11;
	t1.y = (fifo[2] & 0b1100) >> 2;
	t2.x = fifo[4] & 0b11;
	t2.y = (fifo[4] & 0b1100) >> 2;
	t3.x = fifo[6] & 0b11;
	t3.y = (fifo[6] & 0b1100) >> 2;
	t4.x = fifo[8] & 0b11;
	t4.y = (fifo[8] & 0b1100) >> 2;

	uint32_t Vertices1[]{
		v1.x, v1.y, 0.0,
		v2.x, v2.y, 0.0,
		v3.x, v3.y, 0.0,

		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff)
	};
	glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices1), Vertices1, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)(9 * sizeof(uint32_t)));
	glEnableVertexAttribArray(1);
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);

	uint32_t Vertices2[]{
		v2.x, v2.y, 0.0,
		v3.x, v3.y, 0.0,
		v4.x, v4.y, 0.0,

		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff)
	};
	glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2), Vertices2, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)(9 * sizeof(uint32_t)));
	glEnableVertexAttribArray(1);
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	return;
}

void gpu::monochrome_four_point_semi_transparent_polygon() {
	uint32_t colour = fifo[0] & 0xffffff;
	debug_printf("[GP0] Monochrome four-point polygon, semi-transparent (colour: 0x%x)\n", colour);
	point v1, v2, v3, v4;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[2] & 0xffff;
	v2.y = fifo[2] >> 16;
	v3.x = fifo[3] & 0xffff;
	v3.y = fifo[3] >> 16;
	v4.x = fifo[4] & 0xffff;
	v4.y = fifo[4] >> 16;
	
	// TODO: OpenGL implementation
	return;
}

void gpu::monochrome_three_point_opaque_polygon() {
	debug_printf("[GP0] Monochrome three-point polygon, opaque\n");
	point v1, v2, v3;
	uint32_t colour = fifo[0] & 0xffffff;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[2] & 0xffff;
	v2.y = fifo[2] >> 16;
	v3.x = fifo[3] & 0xffff;
	v3.y = fifo[3] >> 16;

	uint32_t Vertices1[]{
		v1.x, v1.y, 0.0,
		v2.x, v2.y, 0.0,
		v3.x, v3.y, 0.0,

		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff),
		(((colour) >> 0) & 0xff), (((colour) >> 8) & 0xff), (((colour) >> 16) & 0xff)
	};
	glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices1), Vertices1, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)(9 * sizeof(uint32_t)));
	glEnableVertexAttribArray(1);
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	return;
}

void gpu::monochrome_three_point_semi_transparent_polygon() {
	debug_printf("[GP0] Monochrome three-point polygon, semi-transparent\n");
	point v1, v2, v3;
	uint32_t colour = fifo[0] & 0xffffff;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[2] & 0xffff;
	v2.y = fifo[2] >> 16;
	v3.x = fifo[3] & 0xffff;
	v3.y = fifo[3] >> 16;
	
	// TODO: OpenGL implementation
	return;
}

void gpu::shaded_three_point_opaque_polygon() {
	debug_printf("[GP0] Shaded three-point polygon, opaque\n");
	point v1, v2, v3;
	v1.c = fifo[0] & 0xffffff;
	v2.c = fifo[2] & 0xffffff;
	v3.c = fifo[4] & 0xffffff;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[3] & 0xffff;
	v2.y = fifo[3] >> 16;
	v3.x = fifo[5] & 0xffff;
	v3.y = fifo[5] >> 16;

	uint32_t Vertices[]{
		v1.x, v1.y, 0.0, 
		v2.x, v2.y, 0.0, 
		v3.x, v3.y, 0.0, 

		(((v1.c) >> 0) & 0xff), (((v1.c) >> 8) & 0xff), (((v1.c) >> 16) & 0xff),
		(((v2.c) >> 0) & 0xff), (((v2.c) >> 8) & 0xff), (((v2.c) >> 16) & 0xff),
		(((v3.c) >> 0) & 0xff), (((v3.c) >> 8) & 0xff), (((v3.c) >> 16) & 0xff)
	};
	glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)(9*sizeof(uint32_t)));
	glEnableVertexAttribArray(1);
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	return;
}

void gpu::shaded_three_point_semi_transparent_polygon() {
	debug_printf("[GP0] Shaded three-point polygon, semi-transparent\n");
	point v1, v2, v3;
	v1.c = fifo[0] & 0xffffff;
	v2.c = fifo[2] & 0xffffff;
	v3.c = fifo[4] & 0xffffff;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[3] & 0xffff;
	v2.y = fifo[3] >> 16;
	v3.x = fifo[5] & 0xffff;
	v3.y = fifo[5] >> 16;

	// TODO: OpenGL implementation
	return;
}

void gpu::shaded_four_point_opaque_polygon() {
	point v1, v2, v3, v4;
	v1.c = fifo[0] & 0xffffff;
	v2.c = fifo[2] & 0xffffff;
	v3.c = fifo[4] & 0xffffff;
	v4.c = fifo[6] & 0xffffff;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[3] & 0xffff;
	v2.y = fifo[3] >> 16;
	v3.x = fifo[5] & 0xffff;
	v3.y = fifo[5] >> 16;
	v4.x = fifo[7] & 0xffff;
	v4.y = fifo[7] >> 16;
	debug_printf("[GP0] Shaded four-point polygon, opaque\n");

	uint32_t Vertices1[]{
		v1.x, v1.y, 0.0,
		v2.x, v2.y, 0.0,
		v3.x, v3.y, 0.0,
	
		(((v1.c) >> 0) & 0xff), (((v1.c) >> 8) & 0xff), (((v1.c) >> 16) & 0xff),
		(((v2.c) >> 0) & 0xff), (((v2.c) >> 8) & 0xff), (((v2.c) >> 16) & 0xff),
		(((v3.c) >> 0) & 0xff), (((v3.c) >> 8) & 0xff), (((v3.c) >> 16) & 0xff)
	};
	glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices1), Vertices1, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)(9 * sizeof(uint32_t)));
	glEnableVertexAttribArray(1);
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);

	uint32_t Vertices2[]{
		v2.x, v2.y, 0.0,
		v3.x, v3.y, 0.0,
		v4.x, v4.y, 0.0,

		(((v2.c) >> 0) & 0xff), (((v2.c) >> 8) & 0xff), (((v2.c) >> 16) & 0xff),
		(((v3.c) >> 0) & 0xff), (((v3.c) >> 8) & 0xff), (((v3.c) >> 16) & 0xff),
		(((v4.c) >> 0) & 0xff), (((v4.c) >> 8) & 0xff), (((v4.c) >> 16) & 0xff)
	};
	glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2), Vertices2, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)(9 * sizeof(uint32_t)));
	glEnableVertexAttribArray(1);
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
	return;
}

void gpu::shaded_four_point_semi_transparent_polygon() {
	point v1, v2, v3, v4;
	v1.c = fifo[0] & 0xffffff;
	v2.c = fifo[2] & 0xffffff;
	v3.c = fifo[4] & 0xffffff;
	v4.c = fifo[6] & 0xffffff;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[3] & 0xffff;
	v2.y = fifo[3] >> 16;
	v3.x = fifo[5] & 0xffff;
	v3.y = fifo[5] >> 16;
	v4.x = fifo[7] & 0xffff;
	v4.y = fifo[7] >> 16;
	debug_printf("[GP0] Shaded four-point polygon, semi-transparent\n");

	// TODO: OpenGL implementation
	return;
}

void gpu::monochrome_line_opaque() {
	point v1, v2;
	v1.c = fifo[0] & 0xffffff;
	v2.c = fifo[0] & 0xffffff;
	v1.x = fifo[1] & 0xffff;
	v1.y = fifo[1] >> 16;
	v2.x = fifo[2] & 0xffff;
	v2.y = fifo[2] >> 16;

	// TODO: OpenGL implementation
	return;
}

void gpu::monochrome_rectangle_dot_opaque() {
	uint16_t x = fifo[1] & 0xffff;
	uint16_t y = fifo[1] >> 16;
	uint32_t colour = fifo[0] & 0xffffff;
	debug_printf("[GP0] 1x1 Draw Opaque Monochrome Rectangle (coords: %d;%d colour: 0x%x)\n", x, y, colour);
	if (x >= 640 || y >= 480)
		return;

	// TODO: OpenGL implementation
	return;
}

void gpu::fill_rectangle() {
	debug_printf("[GP0] Fill Rectangle\n");
	return;
}

void gpu::cpu_to_vram() {
	debug_printf("[GP0] Copy Rectangle (CPU to VRAM)\n");
	uint32_t resolution = fifo[2];
	uint32_t coords = fifo[1];
	auto width = resolution & 0xffff;
	auto height = resolution >> 16;
	xpos = 0;
	ypos = 0;
	uint32_t size = width * height;
	size += 1;
	size &= ~1;
	cmd_left = size / 2;
	gp0_mode = 1;
	return;
}

void gpu::vram_to_cpu() {
	uint32_t resolution = fifo[2];
	auto width = resolution & 0xffff;
	auto height = resolution >> 16;
	return;
}