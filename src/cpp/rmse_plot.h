#pragma once
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <implot.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>

struct RmsePlot {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<float> rmseHistory;
    std::vector<float> iterHistory;
    std::string plotTitle;
    std::mutex dataMutex;
    std::atomic<bool> running{ false };
    std::atomic<bool> saveRequested{ false };
    std::atomic<bool> saveDone{ false };
    std::string saveFilename;
    std::thread renderThread;

    void push(float rmse, int iteration) {
        std::lock_guard<std::mutex> lock(dataMutex);
        rmseHistory.push_back(rmse);
        iterHistory.push_back((float)iteration);
    }

    void savePNG(const std::string& filename) {
        saveDone = false;
        saveFilename = filename;
        saveRequested = true;
        while (!saveDone) SDL_Delay(10);
    }

    void init(const std::string& title = "") {
        plotTitle = title;
        running = true;
        rmseHistory.clear();
        iterHistory.clear();
        renderThread = std::thread(&RmsePlot::renderLoop, this);
    }

    void renderLoop() {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow(plotTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            800, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGui::StyleColorsDark();
        ImPlot::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        ImPlot::GetStyle().FitPadding = ImVec2(0.05f, 0.1f);
        ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer2_Init(renderer);

        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                ImGui_ImplSDL2_ProcessEvent(&e);
                if (e.type == SDL_QUIT) running = false;
            }

            std::vector<float> localHistory;
            std::vector<float> localIter;
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                localHistory = rmseHistory;
                localIter = iterHistory;
            }

            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            int W, H;
            SDL_GetWindowSize(window, &W, &H);
            ImGui::SetNextWindowPos({ 0, 0 });
            ImGui::SetNextWindowSize({ (float)W, (float)H });
            ImGui::Begin(plotTitle.c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

            if (ImPlot::BeginPlot("##rmse", { -1, -1 })) {
                ImPlot::SetupAxes("Iteration", "RMSE", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
                ImPlot::SetNextLineStyle(ImVec4(1, 1, 1, 1), 2.0f);
                ImPlot::PlotLine("RMSE", localIter.data(), localHistory.data(), (int)localHistory.size());
                ImPlot::EndPlot();
            }

            ImGui::End();
            ImGui::Render();

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(renderer);

            if (saveRequested) {
                std::vector<unsigned char> pixels(W * H * 4);
                SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA32,
                    pixels.data(), W * 4);
                stbi_write_png(saveFilename.c_str(), W, H, 4, pixels.data(), W * 4);
                saveRequested = false;
                saveDone = true;
            }

            SDL_Delay(16);
        }

        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void destroy() {
        running = false;
        if (renderThread.joinable())
            renderThread.join();
    }
};