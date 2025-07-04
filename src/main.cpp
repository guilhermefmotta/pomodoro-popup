#include <iostream>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glad/glad.h>
#include "../vendor/imgui_impl_glfw.h"
#include "../vendor/imgui_impl_opengl3.h"
#include <iostream>
#include <chrono>

#define GLSL_VERSION "#version 330"

enum class TimerState {
    Working,
    Paused
};



struct PomodoroTimer {
    int workMinutes = 25;
    int pauseMinutes = 5;
    TimerState state = TimerState::Working;
    std::chrono::steady_clock::time_point lastTick;
    int remainingSeconds = 0;
    bool running = false;

    void start() {
        running = true;
        state = TimerState::Working;
        remainingSeconds = workMinutes * 60;
        lastTick = std::chrono::steady_clock::now();

    }


    void stop() {
        running = false;
        remainingSeconds = 0;

    }


    void update() {
        if (!running) return;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastTick).count();
        if (elapsed >= 1) {
            remainingSeconds -= static_cast<int>(elapsed);
            lastTick = now;
            if (remainingSeconds <= 0) {
                if (state == TimerState::Working) {

                    state = TimerState::Paused;

                    remainingSeconds = pauseMinutes * 60;
                } else {
                    state = TimerState::Working;
                    remainingSeconds = workMinutes * 60;

                }

            }

        }

    }


    std::string getTimeString() const {
        int mins = remainingSeconds / 60;
        int secs = remainingSeconds % 60;
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", mins, secs);
        return std::string(buffer);
    }


};







static void glfw_error_callback(int error, const char* description)

{

    fprintf(stderr, "Glfw Error %d: %s\n", error, description);

}





int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(600, 600, "Hello, ImGUI!",

                                          nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Could not create GLFW window" << std::endl;
        glfwTerminate();
        return -1;

    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Could not initialize GLAD" << std::endl;
        return -1;

    }


    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = "imgui.ini";

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    PomodoroTimer timer;

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();



        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Pomodoro Timer");
        ImGui::InputInt("Work minutes", &timer.workMinutes);

        ImGui::InputInt("Pause minutes", &timer.pauseMinutes);

        if (timer.workMinutes < 1) timer.workMinutes = 1;

        if (timer.pauseMinutes < 1) timer.pauseMinutes = 1;

        // Start/Stop buttons


        if (!timer.running) {
            if (ImGui::Button("Start")) {
                timer.start();
            }

        } else {
            if (ImGui::Button("Stop")) {
                timer.stop();

            }
        }



        // Update timer



        timer.update();

	// Display state and time remaining

        const char* state_str = timer.state == TimerState::Working ? "Working" : "Paused";
        ImGui::Text("State: %s", state_str);
        ImGui::Text("Time remaining: %s", timer.getTimeString().c_str());

        // Notification color if time is up (just switched)
        if (timer.remainingSeconds == 0) {
            ImGui::TextColored(ImVec4(1,0,0,1), "Time's up! Switch now!");

        }



        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}







