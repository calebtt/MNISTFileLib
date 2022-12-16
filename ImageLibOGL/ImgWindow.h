#pragma once
// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <expected>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <iostream>
#include <format>
#include <optional>
#include <string>
#include <vector>

#include "../MNISTFileLib/Idx3HeaderData.hpp"
#include "../MNISTFileLib/Idx3ImageDataBuffer.hpp"

struct SmallImage1
{
	GLuint textureHandle{};

	int width{250};
	int height{250};

	int m_originalWidth{};
	int m_originalHeight{};

	std::size_t currentImageIndex{ 0 };
	Idx3Lib::Idx3ImageDataBuffer m_imageBytes;
	//SmallImage1(std::string_view pathToLoad)
	//{
	//	static const bool didLoad = LoadTextureFromFile(pathToLoad.data(), textureHandle, width, height);
	//	IM_ASSERT(didLoad);
	//	//ImGui::Begin("OpenGL Texture Text");
	//	//ImGui::Text("pointer = %p", textureHandle);
	//	ImGui::Text(std::format("pointer = {}", textureHandle).c_str());
	//	ImGui::Text("size = %d x %d", width, height);
	//	ImGui::Image((void*)(intptr_t)textureHandle, ImVec2(width, height));
	//	//ImGui::End();
	//}

	SmallImage1(Idx3Lib::Idx3ImageDataBuffer bytesToLoad) : m_imageBytes(std::move(bytesToLoad))
	{
		static const auto didLoad = LoadTextureFromMemory(m_imageBytes);
		IM_ASSERT(didLoad);
		const auto [texPtr, texWidth, texHeight] = didLoad.value();

		textureHandle = texPtr;
		m_originalHeight = texHeight;
		m_originalWidth = texWidth;
	}
	~SmallImage1()
	{
		if(textureHandle)
			glDeleteTextures(1, &textureHandle);
	}

	void operator()()
	{
		//const auto formatted1 = std::format("pointer = {}", textureHandle);
		//ImGui::Text(formatted1.c_str());
		ImGui::Text("original size = %d x %d scaled to %d x %d", m_originalWidth, m_originalHeight, width, height);
		ImGui::Image(
			(void*)(intptr_t)textureHandle,
			ImVec2(
				static_cast<float>(width),
				static_cast<float>(height)));
	}

	// Simple helper function to load an image into a OpenGL texture with common settings
	static bool LoadTextureFromFile(const char* filename, GLuint& out_texture, int& out_width, int& out_height)
	{
		// Load from file
		int image_width{};
		int image_height{};
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, nullptr, 4);
		if (image_data == nullptr)
			return false;

		// Create a OpenGL texture identifier
		GLuint image_texture;
		// glGenTextures generates a (usually) unique texture name,
		// should be deleted via glDeleteTextures(...) for re-use
		glGenTextures(1, &image_texture);
		// bind a named texture to a texturing target, 2d target chosen.
		glBindTexture(GL_TEXTURE_2D, image_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // Same

		// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		// set in-memory pixel storage modes
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		stbi_image_free(image_data);

		out_texture = image_texture;
		out_width = image_width;
		out_height = image_height;

		return true;
	}

	auto LoadTextureFromMemory(Idx3Lib::Idx3ImageDataBuffer imageBytes)
		-> std::expected<std::tuple<GLuint, int,int>, std::string>
	{
		// Some "returned" data from the C style function.
		int image_width{static_cast<int>(imageBytes.Col_Count)};
		int image_height{static_cast<int>(imageBytes.Row_Count)};
		int image_channels{ 1 };

		// Create a OpenGL texture identifier
		//GLuint image_texture{};
		// glGenTextures generates a (usually) unique texture name,
		// should be deleted via glDeleteTextures(...) for re-use
		glGenTextures(1, &textureHandle);
		// bind a named texture to a texturing target, 2d target chosen.
		glBindTexture(GL_TEXTURE_2D, textureHandle);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // Same

		// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		// set in-memory pixel storage modes
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image_width, image_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, imageBytes.ImageBuffer.data());
		//stbi_image_free(image_data);

		return std::make_tuple(textureHandle, image_width, image_height);
	}
};
struct SmallWindow1
{
	std::string m_imageIndexMessage;
	std::string m_fpsMessage;
	int m_imageIndex{};
	SmallImage1 si1;

	SmallWindow1(Idx3Lib::Idx3ImageDataBuffer imageBytes) : si1(std::move(imageBytes)) { }

	void operator()()
	{
		// Create a window and append into it.
		ImGui::Begin("OpenGL3 Window");
		// Display some text (you can use a format strings too)
		ImGui::Text("Below, see our image.");

		// call operator
		si1();

		auto indexCopy = m_imageIndex;
		m_imageIndexMessage = std::format("Image at index: {}", indexCopy);
		ImGui::SliderInt(m_imageIndexMessage.data(), &indexCopy, 0, 100);
		//ImGui::SliderFloat("Image at index: ", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);
		const auto frameRateSec = 1000.0f / ImGui::GetIO().Framerate;
		const auto frameRate = ImGui::GetIO().Framerate;
		m_fpsMessage = std::format("Application average {:.3f} ms/frame ({:.3f} FPS)", frameRateSec, frameRate);
		ImGui::Text(m_fpsMessage.data());
		ImGui::End();
		m_imageIndex = indexCopy;
	}
};

inline void glfw_error_callback(int error, const char* description)
{
	std::cerr << "Glfw Error " << error << " " << description << '\n';
}

inline
auto imgMainLoop(
	GLFWwindow* window,
	[[maybe_unused]]
	bool show_another_window, 
	ImVec4 clear_color,
	Idx3Lib::Idx3ImageDataBuffer imageBytes)
{
	SmallWindow1 imageWindow{ imageBytes };
	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if (show_demo_window)
		//ImGui::ShowDemoWindow(&show_demo_window);
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.

		imageWindow();
		

		// 3. Show another simple window.
		//if (show_another_window)
		//{
		//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//	ImGui::Text("Hello from another window!");
		//	if (ImGui::Button("Close Me"))
		//		show_another_window = false;
		//	ImGui::End();
		//}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}

inline
auto imgCleanup(GLFWwindow* window)
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

inline
auto imgLoadFont()
{
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
}

inline
auto imgSetup(GLFWwindow*& window) -> std::optional<std::string>
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		return "Call to glfwInit() failed!";
	}

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	window = { glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr) };
	if (window == nullptr)
	{
		return "Call to glfwCreateWindow() failed!";
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	[[maybe_unused]]
	ImGuiIO& io = ImGui::GetIO();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//Load font(s)
	//const auto fontHandle = imgLoadFont();

	return {};
}

inline int test_glfw_ogl3(const Idx3Lib::Idx3ImageDataBuffer& imageBytes)
{
	GLFWwindow* window;
	const auto setupResult = imgSetup(window);
	if (setupResult.has_value())
	{
		std::cerr << "Error setting up ImGui/GLFW! Message:" << setupResult.value() << '\n';
	}

	// Our state
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// **The main loop!
	imgMainLoop(window, show_another_window, clear_color, imageBytes);

	// Cleanup function.
	imgCleanup(window);

	return 0;
}